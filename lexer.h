#ifndef LEXER_H
#define LEXER_H
#include <variant>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <iostream>



class LexerException :public std::runtime_error{
    public:
    LexerException(std::string reason, int line, int number) :std::runtime_error(reason){

        this->reason=reason;
        this->line=line;
        this->number=number;
        what_str=(std::to_string(line)+":"+std::to_string(number)+": "+reason);
    }

    bool operator==(const LexerException in) const{return reason==in.reason && in.line==line && in.number==number;}

    std::string reason, what_str;
    int line, number;
    const char* what() const noexcept {return what_str.c_str();}
};

class Token{
    public:

    enum Type{
        None, // failed to construct
        Invalid,
        identifier_bool,
        identifier_int,
        identifier_float,
        identifier_str,
        Literal_bool,
        Literal_int,
        Literal_float,
        Literal_str,

        At,
        Minus,
        Arrow,
        Plus,
        Or,
        And,
        Less,
        More,
        Eq,
        PlusPlus,
        OrOr,
        AndAnd,
        LessLess,
        MoreMore,
        EqEq,
        Star,
        Percent,
        Xor,
        Exclam,
        MinusEq,
        PlusEq,
        OrEq,
        AndEq,
        LessEq,
        MoreEq,
        StarEq,
        PercentEq,
        XorEq,
        ExclamEq,
        Comma,
        Bracket,
        BracketEnd,
        BlockBracket,
        BlockBracketEnd,
        SqBracket,
        SqBracketEnd,
        RSlash,
        RSlashEq,

        EndOfInstruction,
        Id,
        Comment,
        If,
        Switch,
        While,
        Continue,
        Break,
        Return,
        Warning,
    };
    static const std::map<Type, std::string> typeToStr;

    friend std::ostream& operator<< (std::ostream &os, const Token &val);

    Token(Type type=Type::None, std::variant<std::string, int, float> value="", int line=-1, int character=-1);
    Token(int line, int character, Type type, std::string str);

    std::string toString() const;

    bool operator!=(const Token in) const{return !(*this==in);}
    bool operator==(const Token in) const{
#ifdef TESTS
        bool out= in.type_==type_ && in.pos.first==pos.first && in.pos.second==pos.second && in.pos.first==pos.first;
        if(!out)
            std::cout<<(in.type_==type_)<<(in.val==val)<<(in.pos.first==pos.first)<<(in.pos.second==pos.second)<<" "<<in.toString()<<"\n     "<<toString()<<"\n";
        return out;
#else
        return in.type_==type_ && in.val==val && in.pos.first==pos.first && in.pos.second==pos.second;
#endif
    }
    operator bool() const{return type_!=Type::None;}

    Type type(){return type_;};
    bool isValue(std::string str);
    std::string getStrVal();
    std::pair<int, int> pos;
    const std::variant<std::string, int, float> value(){return val;};

    private:
    Type type_;
    std::variant<std::string, int, float> val;
};


std::ostream& operator<< (std::ostream &os, const Token &val);

class istreamProxy{
public:

    explicit istreamProxy(std::istream &in):_in(&in){}
    istreamProxy(istreamProxy &in):_line(in._line), _character(in._character), _in(in._in), buffer(in.buffer){}

    int get();

    int peek(unsigned int n=0);
    std::istream& putback(char c);
    std::istream& putback_buffed(char c);
    std::streampos tellg() const{return _in->tellg();}
    std::istream& seekg(std::streampos pos){return _in->seekg(pos);}

    int line() const{return _line;}
    int character() const{return _character;}
    int decrement_line(){return --_line;}


    private:
    int advancePeek();

    int _line=1, _character=0;
    std::istream *_in;
    std::list<char> buffer;
};

class Lexer{
    public:
    Lexer(std::istream &in):is(in){}
    Lexer(Lexer &in):is(in.is), warnings(in.warnings), peeked(in.peeked){}

    Token peekToken(unsigned int n=0);
    Token getToken();

    istreamProxy is;
    std::vector<Token> warnings;

    protected:
    virtual Token advancePeek();
    Token getOperatorToken();
    Token getStringToken();
    Token getCommentToken();
    Token getNumberToken();
    Token getIdToken();

    void skipWhites();

    std::list<Token> peeked;
};

class CommentlessLexer :public Lexer{
public:
    CommentlessLexer(Lexer &in):Lexer(in){}
    CommentlessLexer(CommentlessLexer &in):Lexer(in){}
    CommentlessLexer(std::istream &in):Lexer(in){}

private:
    Token advancePeek(){
        Token tok=Lexer::advancePeek();
        while(tok.type()==Token::Type::Comment)
            tok=Lexer::advancePeek();
        if(tok.type()==Token::Type::Comment)
        return tok;
    }

};


#endif // LEXER_H
