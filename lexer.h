#ifndef LEXER_H
#define LEXER_H
#include <variant>
#include <string>
#include <sstream>
#include <vector>


class Token{public:

    enum Type{
        None, // failed to construct
        Invalid,
        Type_identifier,
        Literal_bool,
        Literal_int,
        Literal_float,
        Literal_str, // TODO
        Operator,
        Id,
        Comment,
        Keyword,
        Warning,
    };

    friend std::ostream& operator<< (std::ostream& os, Token val);

    Token(Type type, std::variant<std::string, int, float> value="", int line=-1, int character=-1);

    std::string toString();

    bool operator!=(Token in){return !(*this==in);}
    bool operator==(Token in){return in.type==type && in.val==val && in.line==line && in.character==character;}

    Type type;
    std::variant<std::string, int, float> val;
    int line=0, character=0;
};

std::ostream& operator<< (std::ostream& os, Token val);

class istreamProxy{
public:

    istreamProxy(std::istream *in, int *line, int *character){
        _in=in;
        _line=line;
        _character=character;
    }

    int get();

    int peek(){return _in->peek();}
    std::istream& putback(char c);
    std::streampos tellg(){return _in->tellg();}

    std::istream *_in;
    int *_line, *_character;
};

class Lexer{
public:

    Token getToken(std::istream &in);
    Token getStringToken(istreamProxy &is);
    Token getNumberToken(istreamProxy &is);
    Token getIdToken(istreamProxy &is);

    bool skipWhites(istreamProxy &is);
    bool isLetter(int ch){return (ch>='a' &&ch<='z') || (ch>='A' &&ch<='Z') || ch=='_';}
    bool isNumber(int ch){return (ch>='0' &&ch<='9');}

    void reset_position(){
        line=1;
        character=0;
    }

    int line=1;
    int character=0;
    std::vector<Token> warnings;
};


#endif // LEXER_H
