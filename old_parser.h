#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "memory"


class ParError{public:
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
        ExpectedMultiplicativeExpression,
        ExpectedEOI,
        ExpectedBracket,
        ExpectedBlock,
        ExpectedInstruction,
        ExpectedEndOfBlock,
        UnexpectedKeyword,
        UnexpectedOperator,
        ExpectedVariableDeclaration,
        ExpectedSwitchBlock,
        ExpectedCaseOperator
    };

    static const std::map<Reasons, std::string> reasonsToStr;
    ParError(Reasons reason, int line, int number){
        this->reason=reason;
        this->line=line;
        this->number=number;
        what_str=(std::to_string(line)+":"+std::to_string(number)+": "+reasonsToStr.at(reason));
    }
    ParError(Reasons reason, std::pair<int, int> pos) :ParError(reason, pos.first, pos.second){}
    ParError(Reasons reason, std::string str, std::pair<int, int> pos) :ParError(reason, pos.first, pos.second){what_str+=str;}

    bool operator==(const ParError in) const{return reason==in.reason && in.line==line && in.number==number;}

    Reasons reason;
    std::string what_str;
    int line, number;
    std::string what() const noexcept {return what_str.c_str();}
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
    bool operator==(const Type &b) const{return type==b.type && mutability==b.mutability;}
    bool operator!=(const Type &b) const{return !(*this==b);}

    TypeNum type;
    bool mutability;
};


class Literal{
    public:


    Literal(Type type, const std::variant<std::string, int, float> value):type_(type), val(value){}

    operator bool() const{return type_!=Type::Void;}

    Type type(){return type_;};
    bool isValue(std::string str);
    std::string getStrVal(){
        if(std::holds_alternative<std::string>(val))
            return std::get<std::string>(val);
        else
            return "";
    }
    std::string str(){
        if(std::holds_alternative<std::string>(val))
            return std::get<std::string>(val);
        else if(std::holds_alternative<int>(val))
            return std::to_string(std::get<int>(val));
        else if(std::holds_alternative<float>(val))
            return std::to_string(std::get<float>(val));
        else throw std::runtime_error("fix Literal::str(): "+std::to_string(val.index()));
    }
    std::pair<int, int> pos;

    protected:
    Type type_;
    std::variant<std::string, int, float> val;
};

class Value{

};
class InstructionBase{
public:

    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const=0;
};
class BasicExpression{public:
    virtual std::string str()=0;
    virtual Value eval(){throw std::runtime_error("eval not ready");}
};

class Expression :public BasicExpression{public:
    Expression(){}
    Expression(std::unique_ptr<BasicExpression> in){expr=std::move(in);}
    Expression(const Expression &in){expr=std::move(in.expr);}// multiple instances of the same Expression can exist

    operator bool() const{return expr!=nullptr;}
    Expression operator=(const std::unique_ptr<BasicExpression>)=delete;
    virtual std::string str(){return "{"+(expr?expr->str():"nullptr")+"}";}

    std::shared_ptr<BasicExpression> expr=nullptr;
};
class ExpressionWrapper :public InstructionBase{
public:
    ExpressionWrapper(std::unique_ptr<BasicExpression> in){
        Expression *ptr=dynamic_cast<Expression*>(in.get());
        if(in.get() && !ptr) throw std::runtime_error("Expression wrapper takes only Expression");
        in.release();
        expr.reset(ptr);
    }
    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const ExpressionWrapper *ptr=dynamic_cast<const ExpressionWrapper*>(&in); return expr && expr==ptr->expr;
    }

    std::unique_ptr<Expression> expr;
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
class VariablesDeclaration :public InstructionBase{public:
    VariablesDeclaration():type(Type::TypeNum::Bool){}
    VariablesDeclaration(VariableDeclaration vd):type(vd.type){idDeclarations={vd.idDeclaration};}
    VariablesDeclaration(Type::TypeNum type, IdDeclaration idd):type(type){idDeclarations={idd};}
    VariablesDeclaration(std::string type, IdDeclaration idd):type(type){idDeclarations={idd};}

    void push_back(const IdDeclaration &idd){idDeclarations.push_back(idd);}

    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const VariablesDeclaration *ptr=dynamic_cast<const VariablesDeclaration*>(&in); return ptr && type==ptr->type && idDeclarations==ptr->idDeclarations;
    }

    Type type;
    std::vector<IdDeclaration> idDeclarations={};
};
class Block :public InstructionBase{public:

    Block(){}

    operator bool() const{return false;}
    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const VariablesDeclaration *ptr=dynamic_cast<const VariablesDeclaration*>(&in);
        return ptr;
    }

};
class FunctionHeaderDeclaration{public:

    FunctionHeaderDeclaration():retType(Type::Void), id(""), params({}){}
    FunctionHeaderDeclaration(Type retType, std::string id):
        retType(retType), id(id), params({}){}
    FunctionHeaderDeclaration(Type retType, std::string id, std::vector<VariableDeclaration> params):
        retType(retType), id(id), params(params){}

    void push_back(const VariableDeclaration &vd){params.push_back(vd);}
    operator bool() const{return id.size()>0;}

    Type retType;
    std::string id;
    std::vector<VariableDeclaration> params;
};
class FunctionDeclaration :public InstructionBase{public:

    FunctionDeclaration(FunctionHeaderDeclaration fhd, std::unique_ptr<InstructionBase> body);

    operator bool() const{return id.size()>0 && body;}
    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const FunctionDeclaration *ptr=dynamic_cast<const FunctionDeclaration*>(&in);
        return ptr && id==ptr->id && params==ptr->params && ((body==nullptr && ptr->body==nullptr) || (body!=nullptr && ptr->body!=nullptr && (*body)==*(ptr->body)));
    }

    std::string id;
    std::vector<VariableDeclaration> params;
    std::unique_ptr<Block> body;
};

class FunctionCall :public BasicExpression{public:
    FunctionCall(std::string id=""):id(id){}

    void push_back(Expression expr){arguments.push_back(expr);}
    void push_back(std::unique_ptr<BasicExpression> expr){
        Expression *ptr=dynamic_cast<Expression*>(expr.get());
        if(ptr!=nullptr)
            arguments.push_back(Expression(*ptr)); // copy constructor, safe
    }
    std::string str();

    std::string id;
    std::vector<Expression> arguments;
};

class PrimaryExpression :public BasicExpression{public:
    PrimaryExpression():val(""){}
    PrimaryExpression(Expression exp):val(exp){}
    PrimaryExpression(Token tok);

    operator bool() const;
    std::string str();

    //             \\ id //
    std::variant<std::string, Literal, Expression> val;
};
class AccessExpression :public BasicExpression{public:
    AccessExpression(){}
    AccessExpression(std::unique_ptr<BasicExpression> str, std::unique_ptr<BasicExpression> accessExpr):expr(std::move(str)), accessExpr(std::move(accessExpr)){}

    std::string str();

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

    std::string str();

    bool post;
    bool in;
    std::string id;
};
class UnaryExpression :public BasicExpression{public:
    UnaryExpression(){}
    UnaryExpression(std::string oper, std::unique_ptr<BasicExpression> expr):oper(oper), expr(std::move(expr)){}

    std::string str();

    std::string oper="";
    std::unique_ptr<BasicExpression> expr;
};
class BinaryExpression :public BasicExpression{public:
    BinaryExpression(std::string oper, std::unique_ptr<BasicExpression> lexpr, std::unique_ptr<BasicExpression> rexpr):oper(oper), lexpr(std::move(lexpr)), rexpr(std::move(rexpr)){}

    std::string str();

    std::string oper;
    std::unique_ptr<BasicExpression> lexpr, rexpr;
};
class CaseExpression :public Expression{public:
    CaseExpression(std::unique_ptr<BasicExpression> in, std::string oper=""):oper(oper){expr=std::move(in);}
    CaseExpression(const CaseExpression &in){expr=std::move(in.expr); oper=in.oper;}

    operator bool() const{return expr!=nullptr || oper=="else";}
    CaseExpression operator=(const std::unique_ptr<BasicExpression>)=delete;
    virtual std::string str(){return "{"+oper+(expr?expr->str():"nullptr")+"}";}

    std::string oper;
};
class SpecialInstruction :public InstructionBase{
public:
    enum Type{
        Break,
        Continue,
        Return
    };
    SpecialInstruction(Type type, std::unique_ptr<Expression> expr=nullptr):type(type), expr(std::move(expr)){}

    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const SpecialInstruction *ptr=dynamic_cast<const SpecialInstruction*>(&in);
        return ptr && type==ptr->type && expr==ptr->expr;
    }

    Type type;
    std::unique_ptr<Expression> expr;
};
class While;
class If;
class Switch;
class Instructions :public InstructionBase{public:
    Instructions(){}
    Instructions(std::unique_ptr<InstructionBase> in):val({std::move(in)}){}

    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const Instructions *ptr=dynamic_cast<const Instructions*>(&in);
        return ptr && val==ptr->val;
    }
    void push_back(std::unique_ptr<InstructionBase> in){val.push_back(std::move(in));}

    std::vector<std::shared_ptr<InstructionBase>> val;
};
class While :public InstructionBase{public:
    While(std::unique_ptr<BasicExpression> condition, std::unique_ptr<InstructionBase> body):condition(std::move(condition)), body(std::move(body)){}

    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const While *ptr=dynamic_cast<const While*>(&in);
        return ptr && condition==ptr->condition && body==ptr->body;
    }

    std::unique_ptr<BasicExpression> condition;
    std::unique_ptr<InstructionBase> body;
};
class If :public InstructionBase{public:
    If(std::unique_ptr<BasicExpression> condition, std::unique_ptr<InstructionBase> body):condition(std::move(condition)), body(std::move(body)){}

    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const If *ptr=dynamic_cast<const If*>(&in);
        return ptr && condition==ptr->condition && body==ptr->body;
    }

    std::unique_ptr<BasicExpression> condition;
    std::unique_ptr<InstructionBase> body;
    std::unique_ptr<InstructionBase> else_body=nullptr;
};
class Case :public InstructionBase{
public:
    Case(std::unique_ptr<CaseExpression> expr, std::unique_ptr<InstructionBase> body):expr(std::move(expr)), body(std::move(body)){}

    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const Case *ptr=dynamic_cast<const Case*>(&in);
        return ptr && expr==ptr->expr && body==ptr->body;
    }

    std::unique_ptr<CaseExpression> expr;
    std::unique_ptr<InstructionBase> body;
};
class Switch :public InstructionBase{
public:
    Switch(std::unique_ptr<Expression> expr):expr(std::move(expr)){}

    bool operator==(const std::unique_ptr<InstructionBase> in) const{return in && (*this)==(*in.get());}
    virtual bool operator==(const InstructionBase &in) const{
        const Switch *ptr=dynamic_cast<const Switch*>(&in);
        return ptr && expr==ptr->expr && cases==ptr->cases;
    }

    void push_back(std::unique_ptr<Case> in){if(in!=nullptr) cases.push_back(std::move(in));}

    std::unique_ptr<Expression> expr;
    std::vector<std::shared_ptr<Case>> cases;
};

class Parser{public:
    Parser(CommentlessLexer &lex);

    IdDeclaration tryGetIdDeclaration();
    VariableDeclaration tryGetVariableDeclaration(); // call AFTER checking for function declaration
    std::unique_ptr<InstructionBase> tryGetVariablesDeclaration();
    FunctionHeaderDeclaration tryGetFunctionHeaderDeclaration();
    std::unique_ptr<InstructionBase> tryGetFunctionDeclaration();
    std::unique_ptr<BasicExpression> tryGetFunctionCall();
    std::unique_ptr<BasicExpression> tryGetPrimaryExpression();
    std::unique_ptr<BasicExpression> tryGetAccessExpression();
    std::unique_ptr<BasicExpression> tryGetCrementationExpression();
    std::unique_ptr<BasicExpression> tryGetUnaryExpression();
    std::unique_ptr<BasicExpression> tryGetBinaryExpression(std::vector<std::string> opers, std::unique_ptr<BasicExpression, std::default_delete<BasicExpression>>(*func)(Parser &par));
    std::unique_ptr<BasicExpression> tryGetMultExpression();
    std::unique_ptr<BasicExpression> tryGetAddExpression();
    std::unique_ptr<BasicExpression> tryGetShiftExpression();
    std::unique_ptr<BasicExpression> tryGetRelationalExpression();
    std::unique_ptr<BasicExpression> tryGetEqualityExpression();
    std::unique_ptr<BasicExpression> tryGetAndExpression();
    std::unique_ptr<BasicExpression> tryGetXorExpression();
    std::unique_ptr<BasicExpression> tryGetOrExpression();
    std::unique_ptr<BasicExpression> tryGetLAndExpression();
    std::unique_ptr<BasicExpression> tryGetLOrExpression();
    std::unique_ptr<BasicExpression> tryGetAssignmentExpression();
    std::unique_ptr<Expression> tryGetExpression();
    std::unique_ptr<CaseExpression> tryGetCaseExpression();
    std::unique_ptr<InstructionBase> tryGetSpecialInstruction();
    std::unique_ptr<InstructionBase> tryGetInstruction();
    Instructions tryGetInstructions();
    std::unique_ptr<InstructionBase> tryGetBlock(bool singleLineToo=true);
    std::unique_ptr<InstructionBase> tryGetConditionedInstruction(std::string keyw);
    std::unique_ptr<InstructionBase> tryGetWhile();
    std::unique_ptr<InstructionBase> tryGetIf();
    std::unique_ptr<Case> tryGetCase();
    std::unique_ptr<InstructionBase> tryGetSwitch();

    bool checkForKeyword(std::string keyw, int n=0) const{return lex.peekToken(n).type()==Token::Type::Keyword && lex.peekToken(n).isValue(keyw);}
    bool checkForOperator(std::string op,  int n=0) const{return lex.peekToken(n).type()==Token::Type::Operator && lex.peekToken(n).isValue(op);}
    bool checkForOperators(std::vector<std::string> op, int n=0) const;
    CommentlessLexer &lex;
    std::vector<ParError> errors;
};

#endif // PARSER_H
