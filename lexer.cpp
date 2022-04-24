#include "lexer.h"

Token::Token(Type type, std::variant<std::string, int, float> value, int line, int character){
    this->type=type;
    val=value;
    this->line=line;
    this->character=character;
}

std::string Token::toString(){
    std::string out="Token(";
    switch(type){
        case None:
            out+="None";
        break;
        case Invalid:
            out+="Invalid";
        break;
        case Type_identifier:
            out+="Type_identifier";
        break;
        case Literal_bool:
            out+="Literal_bool";
        break;
        case Literal_int:
            out+="Literal_int";
        break;
        case Literal_float:
            out+="Literal_float";
        break;
        case Literal_str:
            out+="Literal_str";
        break;
        case Operator:
            out+="Operator";
        break;
        case Id:
            out+="Id";
        break;
        case Comment:
            out+="Comment";
        break;
        case Warning:
            out+="Warning";
        break;
        case Keyword:
            out+="Keyword";
        break;
    }
    out+=", ";
    switch(val.index()){
        case 0:
            out+='"'+std::get<std::string>(val)+'"';
        break;
        case 1:
            out+=std::to_string(std::get<int>(val));
        break;
        case 2:
            out+=std::to_string(std::get<float>(val));
        break;
    }
    out+=", "+std::to_string(line)+", "+std::to_string(character)+")";
    return out;
}

std::ostream& operator<< (std::ostream& os, Token val){
    os<<val.toString();
    return os;
}

int istreamProxy::get(){
    int ch=_in->get();
    if(ch==std::char_traits<char>::eof())
        return ch;
    (*_character)++;
    if(ch=='\n'){
        (*_line)++;
        (*_character)=0;
    }
    return ch;
}

std::istream& istreamProxy::putback(char c){
    (*_character)--;
    if(c=='\n')
        (*_line)--;
    return _in->putback(c);
}

Token Lexer::getToken(std::istream &in){
    istreamProxy is(&in, &line, &character);
    if(skipWhites(is))
        return Token(Token::Type::None);

    int first_char=is.get();

    switch(first_char){
        case std::char_traits<char>::eof():
        return   Token(Token::Type::None, "", line, character);
        case'@':
        return   Token(Token::Type::Keyword, std::string(1, char(first_char)), line, character-1);
        case'\"':
        return getStringToken(is);
        case'-':
            if(is.peek()=='>'){
                is.get();
                return Token(Token::Type::Operator, std::string(1, char(first_char))+'>', line, character-2);
            }
        case'+':
        case'|':
        case'&':
        case'<':
        case'>':
        case'=':
            if(is.peek()==first_char){
                is.get();
                return Token(Token::Type::Operator, std::string(2, char(first_char)), line, character-2);
            }
        case'*':
        case'^':
        case'!':
            if(is.peek()=='='){
                is.get();
                return Token(Token::Type::Operator, std::string(1, char(first_char))+'=', line, character-2);
            }
        case',':
        case';':
        case'(':
        case')':
        case'{':
        case'}':
        case'[':
        case']':
        return Token(Token::Type::Operator, std::string(1, char(first_char)), line, character-1);
        case'/':
            if(is.peek()==first_char){
                is.get();
                int pos=character;
                std::string comment="";

                for(int next_char=is.get(); character; next_char=is.get()){
                    comment+=next_char;
                }
                return Token(Token::Type::Comment, comment, line-1, pos);
            }else
                return Token(Token::Type::Operator, std::string(1, char(first_char)), line, character-1);
    }

    is.putback(first_char);
    if(isNumber(first_char))
        return getNumberToken(is);
    if(isLetter(first_char))
        return getIdToken(is);
    return Token(Token::Type::Invalid, "", line, character);
}

Token Lexer::getStringToken(istreamProxy &is){
    std::string str="";
    bool escaped=false;
    int ch, pos=character, line_pos=line;

    while((ch=is.get())!=std::char_traits<char>::eof()){
        if(!escaped){
            if(ch=='"')
                break;
            if(ch=='\n')
                return Token(Token::Type::Invalid, "multiline string literal", line, character);
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
                    str+='\?';
                default:
                    str+=ch;
            }
        }
    }
    return Token(Token::Type::Literal_str, str, line_pos, pos);
}

Token Lexer::getNumberToken(istreamProxy &is){
    int32_t intbuf=0, float_digits=0;
    uint64_t floatintbuf=0;
    float floatbuf=0;
    bool overflow_hit=false;

    int chnum=character;
    int ch=is.get();
    while(isNumber(ch)){
        ch-='0';
        intbuf*=10;
        intbuf+=ch;
        if(intbuf<0)
            overflow_hit=true;

        floatintbuf*=10;
        floatintbuf+=ch;
        float_digits++;
        if(float_digits==18){
            floatbuf*=1000000000000000000;
            floatbuf+=floatintbuf;
            float_digits=0;
        }

        ch=is.get();
    }
    if(ch=='.'){
        ch=is.get();
        int64_t multiplier=1;
        for(int i=0; i<float_digits; i++){
            multiplier*=10;
        }
        floatbuf*=multiplier;
        floatbuf+=floatintbuf;

        floatintbuf=float_digits=0;
        while(isNumber(ch)){
            ch-='0';
            floatintbuf*=10;
            floatintbuf+=ch;
            float_digits++;
            if(float_digits==18){
                if(isNumber(is.peek())){
                    warnings.push_back(Token(Token::Type::Warning, std::string("float literals with precision more than 10**-18 unsupported"), line, character));
                }
                ch+='0';
                while(isNumber(ch)) // skip rest of the digits to continue tokenizing
                    ch=is.get();
                break;
            }
            ch=is.get();
        }
        multiplier=1;
        for(int i=0; i<float_digits; i++){
            multiplier*=10;
        }
        floatbuf+=double(floatintbuf)/multiplier;
        is.putback(ch);
        return Token(Token::Type::Literal_float, floatbuf, line, chnum);
    }else{
        is.putback(ch);
        if(overflow_hit)
            warnings.push_back(Token(Token::Type::Warning, std::string("integer overflow"), line, chnum));
        return Token(Token::Type::Literal_int, intbuf, line, chnum);
    }
}


Token Lexer::getIdToken(istreamProxy &is){
    std::string id="";

    int ch, pos=character;
    for(ch=is.get(); isLetter(ch)||isNumber(ch); ch=is.get()){
        id+=char(ch);
    }
    is.putback(ch);
    if(id=="true")
        return Token(Token::Type::Literal_bool, 1, line, pos);
    if(id=="false")
        return Token(Token::Type::Literal_bool, 0, line, pos);
    if(id=="switch")
        return Token(Token::Type::Keyword, id, line, pos);
    if(id=="if")
        return Token(Token::Type::Keyword, id, line, pos);
    if(id=="else")
        return Token(Token::Type::Keyword, id, line, pos);
    if(id=="while")
        return Token(Token::Type::Keyword, id, line, pos);
    if(id=="break")
        return Token(Token::Type::Keyword, id, line, pos);
    if(id=="continue")
        return Token(Token::Type::Keyword, id, line, pos);
    if(id=="bool")
        return Token(Token::Type::Type_identifier, id, line, pos);
    if(id=="int")
        return Token(Token::Type::Type_identifier, id, line, pos);
    if(id=="float")
        return Token(Token::Type::Type_identifier, id, line, pos);
    if(id=="str")
        return Token(Token::Type::Type_identifier, id, line, pos);
    return Token(Token::Type::Id, id, line, pos);
}

bool Lexer::skipWhites(istreamProxy &is){
    int test;
    while(isspace(test=is.peek())){
        if(test==std::char_traits<char>::eof()){
            return true;
        }

        is.get();
    }
    if(test==std::char_traits<char>::eof()){
        return true;
    }
    return false;
}
