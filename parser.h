#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "memory"


class ParserException :public std::runtime_error{public:
    enum Reasons{
        ExpectedExpression,
        ExpectedTypeIdentifier,
        ExpectedComma,
        ExpectedId,
        ExpectedClosingBracket,
        ExpectedExpressionOrClosingBracket,
        ExpectedClosingSquareBracket,
        ExpectedPrimaryExpression,
        ExpectedUnaryExpression,
        ExpectedAdditiveExpression,
        ExpectedMultiplicativeExpression
    };

    static const std::map<Reasons, std::string> reasonsToStr;
    ParserException(Reasons reason, int line, int number) :std::runtime_error("ParserException"){
        this->reason=reason;
        this->line=line;
        this->number=number;
        what_str=(std::to_string(line)+":"+std::to_string(number)+": "+reasonsToStr.at(reason));
    }
    ParserException(Reasons reason, std::pair<int, int> pos) :ParserException(reason, pos.first, pos.second){}

    bool operator==(const ParserException in) const{return reason==in.reason && in.line==line && in.number==number;}

    Reasons reason;
    std::string what_str;
    int line, number;
    const char* what() const noexcept {return what_str.c_str();}
};

struct Type{public:
    enum TypeNum{Bool, Int, Float, Str, Void};
    Type(TypeNum type, bool mutability=false):type(type), mutability(mutability){}
    Type(std::string type, bool mutability=false):mutability(mutability){
        if(     type=="bool")  this->type=TypeNum::Bool;
        else if(type=="int")   this->type=TypeNum::Int;
        else if(type=="float") this->type=TypeNum::Float;
        else if(type=="str")   this->type=TypeNum::Str;
        else if(type=="void")  this->type=TypeNum::Void;
        else throw std::runtime_error("wrong type string passed to constructor: "+type);
    }
    Type(Token::Type tokType, bool mutability=false):mutability(mutability){
        if(     tokType==Token::Type::Literal_bool)  type=TypeNum::Bool;
        else if(tokType==Token::Type::Literal_int)   type=TypeNum::Int;
        else if(tokType==Token::Type::Literal_float) type=TypeNum::Float;
        else if(tokType==Token::Type::Literal_str)   type=TypeNum::Str;
        else throw std::runtime_error("wrong type token passed to constructor: "+std::to_string(tokType));
    }

    bool operator!=(const TypeNum b) const{return type!=b;}
    bool operator==(const TypeNum b) const{return type==b;}

    TypeNum type;
    bool mutability;
};


class Literal{
    public:


    Literal(Type type, const std::variant<std::string, int, float> value):type_(type), val(value){}

    operator bool() const{return type_!=Type::Void;}

    Type type(){return type_;};
    bool isValue(std::string str);
    std::string getStrVal();
    std::pair<int, int> pos;

    protected:
    Type type_;
    std::variant<std::string, int, float> val;
};

class Value{

};

class BasicExpression{public:

    virtual operator bool() const=0;
    bool operator!=(const std::unique_ptr<BasicExpression> in) const{return (*this==(*in));}
    bool operator==(const std::unique_ptr<BasicExpression> in) const{return *this==in.get();}
    virtual bool operator==(const BasicExpression *in) const=0;

    virtual Value eval(){throw std::runtime_error("eval not ready");}
};

class Expression :public BasicExpression{public:
    Expression(){}

    operator bool() const{return false;}
    bool operator==(const std::unique_ptr<BasicExpression> in) const{return *this==in.get();}
    bool operator==(const BasicExpression *in) const{
        const Expression *ptr=dynamic_cast<const Expression*>(in); return ptr;
    }


};
class IdDeclaration{public:
    IdDeclaration(const std::string id="");
    IdDeclaration(const std::string id, Expression val);
    IdDeclaration(const IdDeclaration &in):id(in.id), val(in.val){}


    operator bool() const{return id.size()>0;}
    std::string id;
    Expression val;
};
class VariableDeclaration{public:
    VariableDeclaration():type(Type::TypeNum::Bool){}
    VariableDeclaration(Type::TypeNum type, IdDeclaration idd):type(type), idDeclaration(idd){}
    VariableDeclaration(std::string type, IdDeclaration idd):type(type), idDeclaration(idd){}


    operator bool() const{return idDeclaration;}

    Type type;
    IdDeclaration idDeclaration;
};
class VariablesDeclaration{public:
    VariablesDeclaration():type(Type::TypeNum::Bool){}
    VariablesDeclaration(VariableDeclaration vd):type(vd.type){idDeclarations={vd.idDeclaration};}
    VariablesDeclaration(Type::TypeNum type, IdDeclaration idd):type(type){idDeclarations={idd};}
    VariablesDeclaration(std::string type, IdDeclaration idd):type(type){idDeclarations={idd};}


    operator bool() const{return idDeclarations.size()>0;}

    Type type;
    std::vector<IdDeclaration> idDeclarations={};
};
class Block{public:

    Block(){}

    operator bool() const{return false;}


};
class FunctionHeaderDeclaration{public:

    FunctionHeaderDeclaration():retType(Type::Void), id(""), params({}){}
    FunctionHeaderDeclaration(Type retType, std::string id):
        retType(retType), id(id), params({}){}
    FunctionHeaderDeclaration(Type retType, std::string id, std::vector<VariableDeclaration> params):
        retType(retType), id(id), params(params){}

    operator bool() const{return id.size()>0;}

    Type retType;
    std::string id;
    std::vector<VariableDeclaration> params;
};
class FunctionDeclaration{public:

    FunctionDeclaration():id(""), params({}), body(Block()){}
    FunctionDeclaration(FunctionHeaderDeclaration fhd, Block body):
        id(fhd.id), params(fhd.params), body(body){}
    FunctionDeclaration(std::string id, std::vector<VariableDeclaration> params, Block body):
        id(id), params(params), body(body){}

    operator bool() const{return id.size()>0 && body;}

    std::string id;
    std::vector<VariableDeclaration> params;
    Block body;
};



class FunctionCall :public BasicExpression{public:
    FunctionCall(std::string id=""):id(id){}

    operator bool() const{return id.size();};
    bool operator==(const std::unique_ptr<BasicExpression> in) const{return *this==in.get();}
    bool operator==(const BasicExpression *in) const{
        const FunctionCall *ptr=dynamic_cast<const FunctionCall*>(in);
        return ptr && id==ptr->id && arguments==ptr->arguments;
    }
    void push_back(Expression expr){arguments.push_back(expr);}

    std::string id;
    std::vector<Expression> arguments;
};

class PrimaryExpression :public BasicExpression{public:
    PrimaryExpression():val(""){}
    PrimaryExpression(Expression exp):val(exp){}
    PrimaryExpression(Token tok);

    operator bool() const;
    bool operator==(const std::unique_ptr<BasicExpression> in) const{return *this==in.get();}
    bool operator==(const BasicExpression *in) const{
        const PrimaryExpression *ptr=dynamic_cast<const PrimaryExpression*>(in);
        return ptr && val==ptr->val;
    }

    //             \\ id //
    std::variant<std::string, Literal, Expression> val;
};
class AccessExpression :public BasicExpression{public:
    AccessExpression(){}
    AccessExpression(std::unique_ptr<BasicExpression> str, std::unique_ptr<BasicExpression> accessExpr):expr(std::move(str)), accessExpr(std::move(accessExpr)){}

    operator bool() const{return expr!=nullptr && (*expr) && accessExpr!=nullptr && (*accessExpr);}
    bool operator==(const std::unique_ptr<BasicExpression> in) const{return *this==in.get();}
    bool operator==(const BasicExpression *in) const{
        const AccessExpression *ptr=dynamic_cast<const AccessExpression*>(in);
        return ptr && (*expr)==(*(ptr->expr)) && (*accessExpr)==(*(ptr->accessExpr));
    }

    std::unique_ptr<BasicExpression> expr=nullptr;
    std::unique_ptr<BasicExpression> accessExpr=nullptr;

};
class CrementationExpression :public BasicExpression{public:
    CrementationExpression():id(""){}
    CrementationExpression(std::string oper, std::string id, bool post):post(post), id(id){
        if(     oper=="++") in=true;
        else if(oper=="--") in=false;
        else throw std::runtime_error(oper+" is not a crementation operator");
    }

    operator bool() const{return id.size();}
    bool operator==(const std::unique_ptr<BasicExpression> in) const{return *this==in.get();}
    bool operator==(const BasicExpression *in) const{
        const CrementationExpression *ptr=dynamic_cast<const CrementationExpression*>(in);
        return ptr && post==ptr->post && this->in==ptr->in && id==ptr->id;
    }

    bool post;
    bool in;
    std::string id;
};
class UnaryExpression :public BasicExpression{public:
    UnaryExpression(){}
    UnaryExpression(std::string oper, std::unique_ptr<BasicExpression> expr):oper(oper), expr(std::move(expr)){}

    operator bool() const{return oper.size() && expr!=nullptr && (*expr);}
    bool operator==(const std::unique_ptr<BasicExpression> in) const{return *this==in.get();}
    bool operator==(const BasicExpression *in) const{
        const UnaryExpression *ptr=dynamic_cast<const UnaryExpression*>(in);
        return ptr && oper==ptr->oper && (*expr)==(*(ptr->expr));
    }

    std::string oper="";
    std::unique_ptr<BasicExpression> expr;
};
class BinaryExpression :public BasicExpression{public:
    BinaryExpression(){}
    BinaryExpression(std::string oper, std::unique_ptr<BasicExpression> lexpr, std::unique_ptr<BasicExpression> rexpr):oper(oper), lexpr(std::move(lexpr)), rexpr(std::move(rexpr)){}

    operator bool() const{return oper.size() && lexpr!=nullptr && (*lexpr) && rexpr!=nullptr && (*rexpr);}
    bool operator==(const std::unique_ptr<BasicExpression> in) const{return *this==in.get();}
    bool operator==(const BasicExpression *in) const{
        const BinaryExpression *ptr=dynamic_cast<const BinaryExpression*>(in);
        return ptr && oper==ptr->oper && (*lexpr)==(*(ptr->lexpr)) && (*rexpr)==(*(ptr->rexpr));
    }

    std::string oper;
    std::unique_ptr<BasicExpression> lexpr, rexpr;
};
class SpecialInstruction{};
class Instruction{};
class Instructions{};
class While{};
class If{};
class Switch{};

class Parser{public:
    Parser(Lexer &lex);

    IdDeclaration tryGetIdDeclaration();
    VariableDeclaration tryGetVariableDeclaration(); // call AFTER checking for function declaration
    VariablesDeclaration tryGetVariablesDeclaration();
    FunctionHeaderDeclaration tryGetFunctionHeaderDeclaration();  // still TODO
    FunctionDeclaration tryGetFunctionDeclaration();              // TODO
    //           \\ guaranteed not to return nullptr //
    std::unique_ptr<BasicExpression> tryGetFunctionCall();
    std::unique_ptr<BasicExpression> tryGetPrimaryExpression();
    std::unique_ptr<BasicExpression> tryGetAccessExpression();
    std::unique_ptr<BasicExpression> tryGetCrementationExpression();
    std::unique_ptr<BasicExpression> tryGetUnaryExpression();
    std::unique_ptr<BasicExpression> tryGetMultExpression();
    std::unique_ptr<BasicExpression> tryGetAddExpression();
    std::unique_ptr<BasicExpression> tryGetShiftExpression();     // TODO
    std::unique_ptr<BasicExpression> tryGetRelationalExpression();// TODO
    std::unique_ptr<BasicExpression> tryGetEqualityExpression();  // TODO
    std::unique_ptr<BasicExpression> tryGetAndExpression();       // TODO
    std::unique_ptr<BasicExpression> tryGetXorExpression();       // TODO
    std::unique_ptr<BasicExpression> tryGetOrExpression();        // TODO
    std::unique_ptr<BasicExpression> tryGetLAndExpression();      // TODO
    std::unique_ptr<BasicExpression> tryGetLOrExpression();       // TODO
    std::unique_ptr<BasicExpression> tryGetAssignmentExpression();// TODO
    Expression tryGetExpression();                                // TODO
    SpecialInstruction tryGetSpecialInstruction();                // TODO
    Instruction tryGetInstruction();                              // TODO
    Instructions tryGetInstructions();                            // TODO
    Block tryGetBlock();                                          // TODO
    While tryGetWhile();                                          // TODO
    If tryGetIf();                                                // TODO
    Switch tryGetSwitch();                                        // TODO

    bool checkForOperator(std::string op, int n=0){return lex.peekToken(n).type()==Token::Type::Operator && lex.peekToken(n).isValue(op);}
    bool checkForOperators(std::vector<std::string> op, int n=0);
    Lexer lex;
    std::vector<ParserException> postponableExceptions;
};

#endif // PARSER_H
