
#include <iostream>
#include <fstream>
#define TESTS
#include "lexer.h"
#include <QTest>
#include <QtTest/QtTest>

class Tests: public QObject{
    Q_OBJECT
public:
    Tests(){}
    ~Tests(){}

private slots:
    void initTestCase(){}

    void operatorTest1(){
        std::stringstream ss("+"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "+", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest2(){
        std::stringstream ss("-"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "-", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest3(){
        std::stringstream ss("@"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Keyword, "@", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest4(){
        std::stringstream ss("|"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "|", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest5(){
        std::stringstream ss("++"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "++", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest6(){
        std::stringstream ss("--"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "--", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest8(){
        std::stringstream ss("||"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "||", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest9(){
        std::stringstream ss("&"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "&", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest10(){
        std::stringstream ss("&&"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "&&", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest11(){
        std::stringstream ss("-="); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "-=", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest12(){
        std::stringstream ss("+="); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "+=", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest13(){
        std::stringstream ss("<"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "<", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest14(){
        std::stringstream ss(">"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, ">", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest15(){
        std::stringstream ss("<<"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "<<", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest16(){
        std::stringstream ss(">>"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, ">>", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest17(){
        std::stringstream ss("<="); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "<=", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest18(){
        std::stringstream ss(">="); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, ">=", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest19(){
        std::stringstream ss("-<"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "-", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "<", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest20(){
        std::stringstream ss("->"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "->", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest21(){
        std::stringstream ss("   *"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "*", 1, 3));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void operatorTest22(){
        std::stringstream ss("\n/"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "/", 2, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 2, 1));
    }
    void operatorTest23(){
        std::stringstream ss("< - >"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "<", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "-", 1, 2));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, ">", 1, 4));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 5));
    }
    void operatorTest24(){
        std::stringstream ss("!"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "!", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest25(){
        std::stringstream ss("^^"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "^", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "^", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void operatorTest26(){
        std::stringstream ss("=-="); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "=", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "-=", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 3));
    }
    void operatorTest27(){
        std::stringstream ss("(!)"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "(", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "!", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, ")", 1, 2));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 3));
    }
    void operatorTest28(){
        std::stringstream ss(";"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, ";", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }
    void operatorTest29(){
        std::stringstream ss("//;"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Comment, ";", 1, 2));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 3));
    }
    void operatorTest30(){
        std::stringstream ss("//atr56-='/≥.\n"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Comment, "atr56-='/≥.", 1, 2));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 2, 0));
    }
    void operatorTest31(){
        std::stringstream ss("][]"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "]", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "[", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "]", 1, 2));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 3));
    }
    void operatorTest32(){
        std::stringstream ss("{*}()"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "{", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "*", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "}", 1, 2));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "(", 1, 3));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, ")", 1, 4));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 5));
    }
    void strTest1(){
        std::stringstream ss("\"\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void strTest2(){
        std::stringstream ss("\"asd\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "asd", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 5));
    }
    void strTest3(){
        std::stringstream ss("\"p4ye's[eg-4y6\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "p4ye's[eg-4y6", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 15));
    }
    void strTest4(){
        std::stringstream ss("\"\\n\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "\n", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest5(){
        std::stringstream ss("\"\\b\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "\b", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest6(){
        std::stringstream ss("\"\\t\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "\t", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest7(){
        std::stringstream ss("\"\\v\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "\v", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest8(){
        std::stringstream ss("\"\\0\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, std::string("\0", 1), 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest9(){
        std::stringstream ss("\"\\r\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "\r", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest10(){
        std::stringstream ss("\"\\f\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "\f", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest11(){
        std::stringstream ss("\"\\a\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "\a", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest12(){
        std::stringstream ss("\"\\?\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, "\?", 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void strTest13(){
        std::stringstream ss("\"\\?\\b\\t\\v\\0\\r\\a\\f\\t\\\\\""); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_str, std::string("\?\b\t\v\0\r\a\f\t\\", 10), 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 22));
    }
    void strTest14(){
        std::stringstream ss(" \"\n\""); Lexer lex(ss);
        try{
            std::cout<<lex.getToken()<<"\n";
            QCOMPARE("multiline string unhandled", "");
        }catch(LexerException exc){
            QCOMPARE(exc, LexerException("multiline string literal", 1, 2));
        }
        try{
            std::cout<<lex.getToken()<<"\n";
            QCOMPARE("stream ended on unfinished string unhandled", "");
        }catch(LexerException exc){
            QCOMPARE(exc, LexerException("stream ended on unfinished string", 2, 1));
        }
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 2, 1));
    }
    void numberTest1(){
        std::stringstream ss("123"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_int, 123, 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void numberTest2(){
        std::stringstream ss("123*5"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_int, 123, 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "*", 1, 3));
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_int, 5, 1, 4));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void numberTest3(){
        std::stringstream ss("4294967295"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_int, int(4294967295), 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 9));
    }
    void numberTest4(){
        std::stringstream ss("-123"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Operator, "-", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_int, 123, 1, 1));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 3));
    }
    void numberTest5(){
        std::stringstream ss("248798435692384569285698274369875692384756928374"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_int, int(248798435692384569285698274369875692384756928374), 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 47));
    }
    void numberTest6(){
        std::stringstream ss("0.5"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_float, 0.5f, 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void numberTest7(){
        std::stringstream ss("0.24565372453648"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_float, 0.24565372453648f, 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 15));
    }
    void numberTest8(){
        std::stringstream ss("37469258.0"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Literal_float, 37469258.0f, 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 9));
    }
    void idTest1(){
        std::stringstream ss("test"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Id, "test", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 3));
    }
    void idTest2(){
        std::stringstream ss("whiles"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Id, "whiles", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 5));
    }
    void idTest3(){
        std::stringstream ss("int"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Type_identifier, "int", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void idTest4(){
        std::stringstream ss("int"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Type_identifier, "int", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void idTest5(){
        std::stringstream ss("float"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Type_identifier, "float", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void idTest6(){
        std::stringstream ss("bool"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Type_identifier, "bool", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 3));
    }
    void idTest7(){
        std::stringstream ss("str"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Type_identifier, "str", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 2));
    }
    void idTest8(){
        std::stringstream ss("string"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Id, "string", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 5));
    }
    void idTest9(){
        std::stringstream ss("else"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Keyword, "else", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 3));
    }
    void idTest10(){
        std::stringstream ss("switch"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Keyword, "switch", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 5));
    }
    void idTest11(){
        std::stringstream ss("break"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Keyword, "break", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 4));
    }
    void idTest12(){
        std::stringstream ss("continue"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Keyword, "continue", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 7));
    }
    void idTest13(){
        std::stringstream ss("if"); Lexer lex(ss);
        QCOMPARE(lex.getToken(), Token(Token::Type::Keyword, "if", 1, 0));
        QCOMPARE(lex.getToken(), Token(Token::Type::None, "", 1, 1));
    }

    void cleanupTestCase(){}
};

Token tokenbuf(Token::Type::None, "");
void checkToken(Lexer &lexer, Token token){
    if((tokenbuf=lexer.getToken())!=token)
        std::cout<<tokenbuf<<"\n";
}
QTEST_MAIN(Tests)
#include "main.moc"
