#include "lexer.h"
#include <type_traits>
#include <exception>
#include <cmath>

namespace {
bool isLetter(int ch){return (ch>='a' &&ch<='z') || (ch>='A' &&ch<='Z') || ch=='_';}
bool isNumber(int ch){return (ch>='0' &&ch<='9');}
const std::map<Token::Type, std::string> tokenToStr={
    {Token::Type::None, "None"},
    {Token::Type::Invalid, "Invalid"},
    {Token::Type::Type_identifier, "Type_identifier"},
    {Token::Type::Literal_bool, "Literal_bool"},
    {Token::Type::Literal_int, "Literal_int"},
    {Token::Type::Literal_float, "Literal_float"},
    {Token::Type::Literal_str, "Literal_str"},
    {Token::Type::Operator, "Operator"},
    {Token::Type::EndOfInstruction, "EOI"},
    {Token::Type::Id, "Id"},
    {Token::Type::Comment, "Comment"},
    {Token::Type::Keyword, "Keyword"},
    {Token::Type::Warning, "Warning"}
};
} //namespace

Token::Token(Type type, std::variant<std::string, int, float> value, int line, int character):
    type_(type),
    val(std::move(value)),
    pos(line, character){}
Token::Token(int line, int character, Type type, std::string str):
    type_(type),
    val(str),
    pos(line, character){}

std::string Token::toString() const{
    std::string out="Token("+tokenToStr.at(type_)+", ";


    if(std::holds_alternative<std::string>(val))
        out+='"'+std::get<std::string>(val)+'"';
    else if(std::holds_alternative<int>(val))
        out+=std::to_string(std::get<int>(val));
    else if(std::holds_alternative<float>(val))
        out+=std::to_string(std::get<float>(val));

    out+=", "+std::to_string(pos.first)+", "+std::to_string(pos.second)+")";
    return out;
}

bool Token::isValue(std::string str){
    if(std::holds_alternative<std::string>(val))
        if(std::get<std::string>(val)==str)
            return true;
    return false;
}
std::string Token::getStrVal(){
    if(std::holds_alternative<std::string>(val))
        return std::get<std::string>(val);
    return "";
}

std::ostream& operator<< (std::ostream& os, const Token &val){
    os<<val.toString();
    return os;
}

int istreamProxy::get(){
    int ch;
    if(buffer.empty())
        ch=_in->get();
    else{
        ch=buffer.top();
        buffer.pop();
    }
    if(ch==std::char_traits<char>::eof())
        return ch;
    _character++;
    if(ch=='\n'){
        _line++;
        _character=0;
    }
    return ch;
}

int istreamProxy::peek() const{
    if(buffer.empty())
        return _in->peek();
    else
        return buffer.top();
}

std::istream& istreamProxy::putback(char c){
    if(_character)
        _character--;
    if(c=='\n')
        _line--;
    return _in->putback(c);
}

std::istream& istreamProxy::putback_buffed(char c){
    if(_character)
        _character--;
    if(c=='\n')
        _line--;
    buffer.push(c);
    return *_in;
}

Token Lexer::peekToken(unsigned int n){
    if(peeked.size()>n){
        auto it=peeked.begin();
        std::advance(it, n);
        return *it;
    }
    while(peeked.size()<=n){
        peeked.push_back(advancePeek());
    }
    return peeked.back();
}
Token Lexer::getToken(){
    if(peeked.size()){
        Token out=peeked.front();
        peeked.pop_front();
        return out;
    }
    return advancePeek();
}

Token Lexer::advancePeek(){
    skipWhites();

    if(is.peek()==std::char_traits<char>::eof())
        return Token(Token::Type::None, "", is.line(), is.character());
    Token token;

    if(token=getOperatorToken())
        return token;
    if(token=getNumberToken())
        return token;
    if(token=getIdToken())
        return token;
    return Token(Token::Type::Invalid, "", is.line(), is.character());
}

Token Lexer::getOperatorToken(){
    const int l=is.line(), c=is.character();
    int first_char=is.get();

    auto makeToken=[=](Token::Type tokType, std::string str){
        return Token(l, c, tokType, str);
    };
    switch(first_char){
        case'@':
        return makeToken(Token::Type::Keyword, std::string(1, first_char));
        case'\"':
        return getStringToken();
        case'-':
            if(is.peek()=='>'){
                is.get();
                return makeToken(Token::Type::Operator, std::string(1, first_char)+'>');
            }
        case'+':
        case'|':
        case'&':
        case'<':
        case'>':
        case'=':
            if(is.peek()==first_char){
                is.get();
                return makeToken(Token::Type::Operator, std::string(2, first_char));
            }
        case'*':
        case'%':
        case'^':
        case'!':
            if(is.peek()=='='){
                is.get();
                return makeToken(Token::Type::Operator, std::string(1, first_char)+'=');
            }
        case',':
        case'(':
        case')':
        case'{':
        case'}':
        case'[':
        case']':
        return makeToken(Token::Type::Operator, std::string(1, first_char));
        case';':
        return makeToken(Token::Type::EndOfInstruction, ";");
        case'/':
            if(is.peek()==first_char){
                is.get();
                return getCommentToken();
            }else if(is.peek()=='='){
                is.get();
                return makeToken(Token::Type::Operator, std::string(1, first_char)+'=');
            }else
                return makeToken(Token::Type::Operator, std::string(1, first_char));
    }
    is.putback_buffed(first_char);
    return Token();
}

Token Lexer::getStringToken(){
    std::string str="";
    bool escaped=false;
    int ch, char_pos=is.character(), line_pos=is.line();
    std::streampos pos=is.tellg();

    ch=is.get();
    while(escaped || ch!='"'){
        if(!escaped){
            if(ch==std::char_traits<char>::eof())
                throw LexerException("stream ended on unfinished string", line_pos, char_pos);
            if(ch=='\n'){
                is.seekg(pos);
                is.decrement_line();
                throw LexerException("multiline string literal", line_pos, char_pos);
            }
            if(ch=='\\')
                escaped=true;
            else
                str+=ch;
        }else{
            switch(ch){
                case'n':
                    str+='\n';
                break; case'b':
                    str+='\b';
                break; case't':
                    str+='\t';
                break; case'v':
                    str+='\v';
                break; case'0':
                    str+='\0';
                break; case'r':
                    str+='\r';
                break; case'f':
                    str+='\f';
                break; case'a':
                    str+='\a';
                break; case'?':
                    str+="\?";
                break; default:
                    str+=ch;
            }
            escaped=false;
        }
        ch=is.get();
    }
    return Token(Token::Type::Literal_str, str, line_pos, char_pos);
}

Token Lexer::getCommentToken(){
    int pos=is.character(), l=is.line();
    std::string comment="";

    int next_char;
    for(next_char=is.get(); is.character() && next_char!=std::char_traits<char>::eof(); next_char=is.get()){
        comment+=next_char;
    }
    if(next_char==std::char_traits<char>::eof())
        return Token(Token::Type::Comment, comment, l, pos);
    return Token(Token::Type::Comment, comment, l, pos);
}

Token Lexer::getNumberToken(){
    uint64_t intbuf=0;
    int float_digits=0;
    float floatbuf=0;
    bool overflow_hit=false;
    bool returnNumber=false;

    int chnum=is.character();
    int ch=is.get();
    while(isNumber(ch)){
        returnNumber=true;
        ch-='0';
        intbuf*=10;
        intbuf+=ch;
        if(intbuf>0x7fffffff)
            overflow_hit=true;

        floatbuf*=10;
        floatbuf+=ch;

        ch=is.get();
    }
    if(ch=='.' && returnNumber){
        ch=is.get();
        int64_t multiplier=1, floatintbuf=0;
        float_digits=0;
        while(isNumber(ch)){
            ch-='0';
            floatintbuf*=10;
            floatintbuf+=ch;
            float_digits++;
            if(float_digits==18){
                if(isNumber(is.peek())){
                    warnings.push_back(Token(Token::Type::Warning, std::string("float literals with precision more than 10**-18 unsupported"), is.line(), is.character()));
                }
                ch+='0';
                while(isNumber(ch)) // skip rest of the digits to continue tokenizing
                    ch=is.get();
                break;
            }
            ch=is.get();
        }
        multiplier=std::pow(10, float_digits);
        floatbuf+=double(floatintbuf)/multiplier;
        is.putback_buffed(ch);
        return Token(Token::Type::Literal_float, floatbuf, is.line(), chnum);
    }else{
        is.putback_buffed(ch);
        if(overflow_hit)
            warnings.push_back(Token(Token::Type::Warning, std::string("integer overflow"), is.line(), chnum));
        else if(!returnNumber){
            return Token();
        }
        return Token(Token::Type::Literal_int, int(intbuf&0x7fffffff), is.line(), chnum);
    }
}

Token Lexer::getIdToken(){
    std::string id="";
    static const std::map<std::string, Token::Type> map={
        {"switch", Token::Type::Keyword},
        {"if", Token::Type::Keyword},
        {"else", Token::Type::Keyword},
        {"while", Token::Type::Keyword},
        {"break", Token::Type::Keyword},
        {"continue", Token::Type::Keyword},
        {"bool", Token::Type::Type_identifier},
        {"int", Token::Type::Type_identifier},
        {"float", Token::Type::Type_identifier},
        {"str", Token::Type::Type_identifier},
        {"void", Token::Type::Type_identifier}
    };

    int ch, pos=is.character();
    for(ch=is.get(); isLetter(ch)||isNumber(ch); ch=is.get()){
        id+=char(ch);
    }
    is.putback(ch);
    if(id=="true")
        return Token(Token::Type::Literal_bool, 1, is.line(), pos);
    if(id=="false")
        return Token(Token::Type::Literal_bool, 0, is.line(), pos);
    if(id.empty())
        return Token();

    const auto iter=map.find(id);
    if(iter!=map.end())
        return Token(iter->second, id, is.line(), pos);
    return Token(Token::Type::Id, id, is.line(), pos);
}

void Lexer::skipWhites(){
    while(isspace(is.peek())){
        is.get();
    }
}
