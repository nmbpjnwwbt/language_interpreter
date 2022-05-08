#ifndef LEXER_H
#define LEXER_H
#include <variant>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include <iostream>


bool isLetter(int ch);
bool isNumber(int ch);

class LexerException : std::exception{
    public:
    LexerException(std::string reason, int line, int number){

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

class Token{public:

    enum Type{
        None, // failed to construct
        Invalid,
        Type_identifier,
        Literal_bool,
        Literal_int,
        Literal_float,
        Literal_str,
        Operator,
        Id,
        Comment,
        Keyword,
        Warning,
    };

    friend std::ostream& operator<< (std::ostream& os, Token val);

    Token(Type type=Type::None, std::variant<std::string, int, float> value="", int line=-1, int character=-1);
    Token(int line, int character, Type type, char ch);
    Token(int line, int character, Type type, char ch1, char ch2);

    std::string toString() const;

    bool operator!=(const Token in) const{return !(*this==in);}
    bool operator==(const Token in) const{
#ifdef TESTS
        bool out= in.type==type && in.val==val && in.line==line && in.character==character;
        if(!out)
            std::cout<<(in.type==type)<<(in.val==val)<<(in.line==line)<<(in.character==character)<<" "<<in.toString()<<"\n     "<<toString()<<"\n";
        return out;
#else
        return in.type==type && in.val==val && in.line==line && in.character==character;
#endif
    }
    explicit operator bool() const{return type!=Type::None;}

    private:
    Type type;
    std::variant<std::string, int, float> val;
    int line=0, character=0;
};
extern const std::map<Token::Type, std::string> tokenToStr;


std::ostream& operator<< (std::ostream& os, const Token val);

class istreamProxy{
public:

    explicit istreamProxy(std::istream &in){
        _in=&in;
    }

    int get();

    int peek() const;
    std::istream& putback(char c);
    std::istream& putback_buffed(char c);
    std::streampos tellg() const{return _in->tellg();}
    std::istream& seekg(std::streampos pos){return _in->seekg(pos);}

    int line(){return _line;}
    int character(){return _character;}

    int _line=1, _character=0;

    private:
    std::istream *_in;
    std::stack<char> buffer;
};

class Lexer{
public:
    Lexer(std::istream &in):is(in){}

    Token getToken();

    istreamProxy is;
    std::vector<Token> warnings;
    private:

    Token getOperatorToken();
    Token getStringToken();
    Token getCommentToken();
    Token getNumberToken();
    Token getIdToken();

    void skipWhites();

};


#endif // LEXER_H
