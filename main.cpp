#include <iostream>
#include <fstream>
#include "lexer.h"

Lexer lexer;
Token tokenbuf(Token::Type::None, "");

void checkToken(std::istream &is, Token token){
    if((tokenbuf=lexer.getToken(is))!=token)
        std::cout<<tokenbuf<<"\n";
}

int main()
{
    std::fstream dsa;
    dsa.open("test.int", std::ios::in|std::ios::binary);

    checkToken(dsa, Token(Token::Type::Operator, "++", 2, 0));
    checkToken(dsa, Token(Token::Type::Operator, "=",  2, 2));
    checkToken(dsa, Token(Token::Type::Comment, "asdfghjkl1234", 2, 9));
    checkToken(dsa, Token(Token::Type::Literal_int, 1234, 3, 0));
    checkToken(dsa, Token(Token::Type::Comment, "hehe", 3, 6));
    checkToken(dsa, Token(Token::Type::Literal_int, 1, 4, 0));
    checkToken(dsa, Token(Token::Type::Operator, "/", 4, 1));
    checkToken(dsa, Token(Token::Type::Literal_int, 2, 4, 2));
    checkToken(dsa, Token(Token::Type::Literal_float, 123.5423f, 5, 0));
    checkToken(dsa, Token(Token::Type::Literal_int, 123, 6, 0));
    checkToken(dsa, Token(Token::Type::Literal_int, int(52983568273456298375692835692836598236592834659823456928357692835692835769837452), 6, 4));
    if(lexer.warnings.size()!=1)
        std::cout<<"warnings after line 6:"<<lexer.warnings.size();
    else{
        if(lexer.warnings[0]!=Token(Token::Type::Warning, "integer overflow", 6, 4))
            std::cout<<lexer.warnings[0]<<"\n";
    }
    lexer.warnings.clear();

    checkToken(dsa, Token(Token::Type::Literal_float, 123.89172f, 7, 0));
    if(lexer.warnings.size()!=1)
        std::cout<<"warnings after line 6:"<<lexer.warnings.size();
    else{
        if(lexer.warnings[0]!=Token(Token::Type::Warning, "float literals with precision more than 10**-18 unsupported", 7, 22))
            std::cout<<lexer.warnings[0]<<"\n";
    }
    lexer.warnings.clear();

    checkToken(dsa, Token(Token::Type::Id, "test", 8, 0));
    checkToken(dsa, Token(Token::Type::Literal_bool, 0, 8, 5));
    checkToken(dsa, Token(Token::Type::Id, "trye", 8, 11));
    checkToken(dsa, Token(Token::Type::Literal_bool, 1, 8, 16));
    checkToken(dsa, Token(Token::Type::Id, "whilez", 8, 21));
    checkToken(dsa, Token(Token::Type::Literal_str, "aaa", 8, 29));
    checkToken(dsa, Token(Token::Type::Type_identifier, "int", 9, 0));
    checkToken(dsa, Token(Token::Type::Id, "ints", 9, 4));
    checkToken(dsa, Token(Token::Type::Id, "in", 9, 9));
    checkToken(dsa, Token(Token::Type::Type_identifier, "str", 9, 12));
    checkToken(dsa, Token(Token::Type::Id, "string", 9, 16));
    checkToken(dsa, Token(Token::Type::Type_identifier, "bool", 10, 0));
    checkToken(dsa, Token(Token::Type::Keyword, "@", 10, 5));
    checkToken(dsa, Token(Token::Type::Operator, "{", 10, 6));
    checkToken(dsa, Token(Token::Type::Type_identifier, "float", 10, 7));
    checkToken(dsa, Token(Token::Type::Keyword, "if", 10, 13));
    checkToken(dsa, Token(Token::Type::Operator, "(", 10, 15));
    checkToken(dsa, Token(Token::Type::Operator, ")", 10, 16));
    checkToken(dsa, Token(Token::Type::Operator, "}", 10, 18));
    checkToken(dsa, Token(Token::Type::Keyword, "else", 10, 19));
    checkToken(dsa, Token(Token::Type::Keyword, "switch", 10, 24));
    checkToken(dsa, Token(Token::Type::Id, "switcher", 11, 1));
    checkToken(dsa, Token(Token::Type::Keyword, "break", 11, 10));
    checkToken(dsa, Token(Token::Type::Keyword, "continue", 11, 16));
    checkToken(dsa, Token(Token::Type::Id, "switcher", 11, 1));
    return 0;
}
