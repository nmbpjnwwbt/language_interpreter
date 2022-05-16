#include "parser.h"

const std::map<ParserException::Reasons, std::string> ParserException::reasonsToStr={
    {ParserException::ExpectedExpression, "expected expression"},
    {ParserException::ExpectedTypeIdentifier, "expected type identifier"},
    {ParserException::ExpectedComma, "expected ','"},
    {ParserException::ExpectedId, "expected identifier"},
    {ParserException::ExpectedClosingBracket, "expected ')'"},
    {ParserException::ExpectedExpressionOrClosingBracket, "expected expression or ')'"},
    {ParserException::ExpectedClosingSquareBracket, "expected ']'"},
    {ParserException::ExpectedPrimaryExpression, "expected primary expression"},
    {ParserException::ExpectedUnaryExpression, "expected unary expression"},
    {ParserException::ExpectedAdditiveExpression, "expected additive expression"},
    {ParserException::ExpectedMultiplicativeExpression, "expected multiplicative expression"}
};

namespace{
static const std::string inc_oper="++";
static const std::string dec_oper="--";
static const std::string neg_oper="!";
static const std::string add_oper="+";
static const std::string sub_oper="-";
static const std::string mul_oper="*";
static const std::string div_oper="/";
static const std::string mod_oper="%";
static const std::string rsh_oper=">>";
static const std::string lsh_oper="<<";
static const std::string lth_oper="<";
static const std::string mth_oper=">";
static const std::string leq_oper="<=";
static const std::string meq_oper=">=";
static const std::string equ_oper="==";
static const std::string neq_oper="!=";
static const std::string and_oper="&";
static const std::string or_oper="|";
static const std::string xor_oper="^";
static const std::string land_oper="&&";
static const std::string lor_oper="||";
static const std::string ass_oper="=";
static const std::string add_ass="+=";
static const std::string sub_ass="-=";
static const std::string mul_ass="*=";
static const std::string div_ass="/=";
static const std::string mod_ass="%=";
static const std::string and_ass="&=";
static const std::string or_ass="|=";
static const std::string rsh_ass=">>=";
static const std::string lsh_ass="<<=";
static const std::string xor_ass="^=";
} //namespace

PrimaryExpression::PrimaryExpression(Token tok){
    switch(tok.type()){
        case Token::Type::Id:
            val=tok.getStrVal();
        break;
        case Token::Type::Literal_bool:
        case Token::Type::Literal_int:
        case Token::Type::Literal_float:
        case Token::Type::Literal_str:
            val=Literal(Type(tok.type()), tok.value());
        break;
        default:
            val="";
    }
}

PrimaryExpression::operator bool() const{
    if(std::holds_alternative<std::string>(val))
        return std::get<std::string>(val).size();
    else if(std::holds_alternative<Literal>(val))
        return std::get<Literal>(val);
    else if(std::holds_alternative<Expression>(val))
        return std::get<Expression>(val);
    else throw std::runtime_error("fix PrimaryExpression::operator bool(): "+std::to_string(val.index()));
}


Parser::Parser(Lexer &lex):lex(lex){

}




IdDeclaration::IdDeclaration(std::string id):id(id), val(Expression()){}
IdDeclaration::IdDeclaration(std::string id, Expression val):id(id), val(val){}

//function_type_identifier = type_identifier | "void";
//bool = "True" | "False";
//int = "0" | (digit_without_zero, {digit});
//float = int, ".", {digit};
//str = "\"", {(? all except \n and "\"" ?) | escape}, "\"";
//
//crementation_operator = "++" | "--";
//unary_operator = "-" | "!" | crementation_operator;
//multiplicative_operator = "*" | "/" | "%";
//additive_operator = "+" | "-";
//shift_operator = "<<" | ">>";
//relational_operator = ">=" | "<="| "<" | ">";
//eqiality_operator = "==" | "!=";
//and_operator = "&";
//xor_operator = "^";
//or_operator = "|";
//log_and_operator = "&&";
//log_or_operator = "||";
//
//assignment_operator = "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "<<=" | ">>=" | "^=";
//id = (letter | "_"), {name_char};


IdDeclaration Parser::tryGetIdDeclaration(){
    //literal = bool
    //        | int
    //        | float
    //        | str;

    //id_declaration = id, ["=", expression];
    if(lex.peekToken().type()!=Token::Type::Id)
        return IdDeclaration();
    Token tok=lex.getToken();
    if(!checkForOperator(ass_oper))
        return IdDeclaration(tok.getStrVal());
    if(Expression exp=tryGetExpression())
        return IdDeclaration(tok.getStrVal(), exp);
    postponableExceptions.push_back(ParserException(ParserException::ExpectedExpression, lex.peekToken().pos));
    return IdDeclaration(tok.getStrVal());
}
VariableDeclaration Parser::tryGetVariableDeclaration(){ // would eat a type identifier
    //variable_declaration = ["@"], type_identifier, id_declaration;
    Token tok=lex.peekToken();
    bool mutab=false;
    if(tok.type()==Token::Type::Keyword && tok.getStrVal()=="@"){
        mutab=true;
        lex.getToken();
    }
    tok=lex.peekToken();
    if(tok.type()!=Token::Type::Type_identifier || tok.isValue("void")){
        if(mutab)
            postponableExceptions.push_back(ParserException(ParserException::ExpectedTypeIdentifier, tok.pos));
        return VariableDeclaration();
    }
    lex.getToken();
    if(IdDeclaration idd=tryGetIdDeclaration()){
        return VariableDeclaration(tok.getStrVal(), idd);
    }
    return VariableDeclaration();

}
VariablesDeclaration Parser::tryGetVariablesDeclaration(){
    //variables_declaration = variable_declaration, {",", id_declaration}, ";";
    VariablesDeclaration vardec(tryGetVariableDeclaration());
    if(vardec){
        while(lex.peekToken().type()!=Token::Type::EndOfInstruction){
            if(checkForOperator(","))
                lex.getToken();
            else{
                postponableExceptions.push_back(ParserException(ParserException::ExpectedComma, lex.peekToken().pos));
                break;
            }
            if(IdDeclaration idd=tryGetIdDeclaration()){
                vardec.idDeclarations.push_back(idd);
            }else{
                postponableExceptions.push_back(ParserException(ParserException::ExpectedId, lex.peekToken().pos));
                break;
            }
        }
        return vardec;
    }
    return VariablesDeclaration();
}
FunctionHeaderDeclaration Parser::tryGetFunctionHeaderDeclaration(){
    //function_header = function_type_identifier, id, "(", {variable_declaration}, ")";
    if(lex.peekToken().type()!=Token::Type::Type_identifier)
        return FunctionHeaderDeclaration();
    if(lex.peekToken(1).type()!=Token::Type::Id)
        return FunctionHeaderDeclaration();
    if(checkForOperator("(", 2))
        return FunctionHeaderDeclaration();

    FunctionHeaderDeclaration fhd(lex.peekToken().getStrVal(), lex.peekToken(1).getStrVal());
    lex.getToken();
    lex.getToken();
    lex.getToken();
    if(VariableDeclaration vardec=tryGetVariableDeclaration()){ // TODO rest from here
        fhd.params.push_back(vardec);
    }
    while(!checkForOperator(")")){
        if(!checkForOperator(",")){
            postponableExceptions.push_back(ParserException(ParserException::ExpectedComma, lex.peekToken().pos));

        }
        lex.getToken();
        if(VariableDeclaration vardec=tryGetVariableDeclaration()){
            fhd.params.push_back(vardec);
        }
    }
}
FunctionDeclaration Parser::tryGetFunctionDeclaration(){
    //function_declaration = function_header, block;
}
std::unique_ptr<BasicExpression> Parser::tryGetFunctionCall(){
    //function_call = id, "(", [expression, {",", expression}], ")";
    if(lex.peekToken().type()!=Token::Type::Id)
        return std::make_unique<FunctionCall>();
    if(!checkForOperator("(", 1))
        return std::make_unique<FunctionCall>();
    std::unique_ptr<FunctionCall> fc=std::make_unique<FunctionCall>(lex.getToken().getStrVal());
    lex.getToken();
    if(checkForOperator(")"))
        return fc;
    if(Expression expr=tryGetExpression()){
        fc->push_back(expr);
        while(!checkForOperator(")")){
            if(!checkForOperator(",")){
                postponableExceptions.push_back(ParserException(ParserException::ExpectedExpression, lex.peekToken().pos));
                break;
            }
            lex.getToken();
            if(!(expr=tryGetExpression())){
                postponableExceptions.push_back(ParserException(ParserException::ExpectedExpression, lex.peekToken().pos));
                break;
            }
            fc->push_back(expr);
        }
        lex.getToken();
        return fc;
    }else
        postponableExceptions.push_back(ParserException(ParserException::ExpectedExpressionOrClosingBracket, lex.peekToken().pos));
    return std::make_unique<FunctionCall>();
}
std::unique_ptr<BasicExpression> Parser::tryGetPrimaryExpression(){
    //primary_expression = id
    //                   | literal
    //                   | "(", expression, ")";
    std::unique_ptr<PrimaryExpression> out=std::make_unique<PrimaryExpression>();
    if(*(out=std::make_unique<PrimaryExpression>(lex.peekToken()))){ // checking for literals
        lex.getToken();
        return out;
    }
    if(checkForOperator("(")){
        Expression buf=tryGetExpression();
        out=std::make_unique<PrimaryExpression>(buf);
        if(!(*out))
            postponableExceptions.push_back(ParserException(ParserException::ExpectedExpression, lex.peekToken().pos));
        if(checkForOperator(")")){
            lex.getToken();
        }else
            postponableExceptions.push_back(ParserException(ParserException::ExpectedClosingBracket, lex.peekToken().pos));

        return out;
    }
    return std::make_unique<PrimaryExpression>();
}
std::unique_ptr<BasicExpression> Parser::tryGetAccessExpression(){
    //call_expression = primary_expression
    //                | function_call;
    //access_expression = call_expression
    //                  | call_expression, "[", expression, "]";
    std::unique_ptr<BasicExpression> cexpr=tryGetFunctionCall();
    if(!*cexpr)
        cexpr=tryGetPrimaryExpression();
    if((*cexpr))
        if(checkForOperator("[")){
            lex.getToken();
            if(Expression expr=tryGetExpression()){
                if(checkForOperator("]"))
                    return std::make_unique<AccessExpression>(std::move(cexpr), std::make_unique<Expression>(expr));
                postponableExceptions.push_back(ParserException(ParserException::ExpectedClosingSquareBracket, lex.peekToken().pos));
            }else{
                postponableExceptions.push_back(ParserException(ParserException::ExpectedExpression, lex.peekToken().pos));
                if(checkForOperator("]"))
                    lex.getToken();
            }
        }
    return cexpr;
}
std::unique_ptr<BasicExpression> Parser::tryGetCrementationExpression(){
    //crementation_expression = (id, crementation_operator)
    //                        | (crementation_operator, id);
    if(checkForOperators({inc_oper, dec_oper})){
        if(lex.peekToken(1).type()==Token::Type::Id){
            std::unique_ptr<CrementationExpression> cremexpr=std::make_unique<CrementationExpression>(lex.peekToken().getStrVal(), lex.peekToken(1).getStrVal(), false);
            lex.getToken();
            lex.getToken();
            return cremexpr;
        }
        lex.getToken();
        postponableExceptions.push_back(ParserException(ParserException::ExpectedId, lex.peekToken().pos));
        return std::make_unique<CrementationExpression>();
    }
    if(lex.peekToken().type()==Token::Type::Id){
        if(checkForOperators({inc_oper, dec_oper}, 1)){
            std::unique_ptr<CrementationExpression> cremexpr=std::make_unique<CrementationExpression>(lex.peekToken(1).getStrVal(), lex.peekToken(0).getStrVal(), true);
            lex.getToken();
            lex.getToken();
            return cremexpr;
        }
    }
    return std::make_unique<CrementationExpression>();
}
std::unique_ptr<BasicExpression> Parser::tryGetUnaryExpression(){
    //unary_expression = (unary_operator, primary_expression)
    //                 | crementation_expression
    //                 | access_expression;
    if(checkForOperators({neg_oper, sub_oper})){
        std::string oper=lex.getToken().getStrVal();
        std::unique_ptr<BasicExpression> primexpr=tryGetPrimaryExpression();
        if(*primexpr)
            return std::make_unique<UnaryExpression>(oper, std::move(primexpr));
        postponableExceptions.push_back(ParserException(ParserException::ExpectedPrimaryExpression, lex.peekToken().pos));
        return std::make_unique<UnaryExpression>();
    }
    std::unique_ptr<BasicExpression> cremexpr=tryGetCrementationExpression();
    if(*cremexpr)
        return cremexpr;
    cremexpr=tryGetAccessExpression();
    if(*cremexpr)
            return cremexpr;
    return std::make_unique<UnaryExpression>();
}
std::unique_ptr<BasicExpression> Parser::tryGetMultExpression(){
    //multiplicative_expression = unary_expression
    //                          | multiplicative_expression, multiplicative_operator, unary_expression;
    std::unique_ptr<BasicExpression> expr=tryGetUnaryExpression();
    if(!(*expr))
        return expr;
    if(checkForOperators({mul_oper, div_oper, mod_oper})){
        std::string oper=lex.getToken().getStrVal();
        std::unique_ptr<BasicExpression> expr2=tryGetUnaryExpression();
        if(!(*expr)){
            postponableExceptions.push_back(ParserException(ParserException::ExpectedExpression, lex.peekToken().pos));
            return expr;
        }
        return std::make_unique<BinaryExpression>(oper, std::move(expr), std::move(expr2));
    }
    return expr;
}
std::unique_ptr<BasicExpression> Parser::tryGetAddExpression(){
    //additive_expression = multiplicative_expression
    //                    | additive_expression, additive_operator, multiplicative_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetShiftExpression(){
    //shift_expression = additive_expression
    //                 | shift_expression, shift_operator, additive_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetRelationalExpression(){
    //relational_expression = shift_expression
    //                      | relational_expression, relational_operator, shift_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetEqualityExpression(){
    //equality_expression = relational_expression
    //                    | equality_expression, equality_expression, relational_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetAndExpression(){
    //and_expression = equality_expression
    //               | and_expression, and_operator, equality_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetXorExpression(){
    //xor_expression = and_expression
    //               | xor_expression, xor_operator, and_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetOrExpression(){
    //or_expression = xor_expression
    //              | or_expression, or_operator, xor_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetLAndExpression(){
    //log_and_expression = or_expression
    //                   | log_and_expression, log_and_operator, or_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetLOrExpression(){
    //log_or_expression = log_and_expression
    //                  | log_or_expression, log_or_operator, log_and_expression;
}
std::unique_ptr<BasicExpression> Parser::tryGetAssignmentExpression(){
    //assignment_expression = log_or_expression
    //                      | id, assignment_operator, assignment_expression;
}

Expression Parser::tryGetExpression(){
    //expression = assignment_expression;
}

//
//special_instruction = "continue"
//                    | "break"
//                    | ("return", [expression]);
SpecialInstruction Parser::tryGetSpecialInstruction(){}

//instruction = ([expression], ";")
//            | (special_instruction, ";")
//            | variables_declaration
//            | function_declaration
//            | while_loop
//            | if_statement
//            | switch_statement;
Instruction Parser::tryGetInstruction(){}

//instructions = instruction, {instruction};
Instructions Parser::tryGetInstructions(){}

//block = ("{", instructions, "}")
//      | instruction;
Block Parser::tryGetBlock(){}

//while_loop = "while", "(", expression, ")",
//           block;
While Parser::tryGetWhile(){}

//if_statement = "if", "(", expression, ")",
//             block,
//             ["else"], (instruction | (block));
If Parser::tryGetIf(){}

//switch_statement = "switch", "(", expression, ")",
//                "{", {(expression | "else"), "->", block}, "}";
Switch Parser::tryGetSwitch(){}

//
//
//start = instructions;


bool Parser::checkForOperators(std::vector<std::string> op, int n){
    if(lex.peekToken(n).type()==Token::Type::Operator)
        for(std::string i:op)
            if(lex.peekToken(n).isValue(i))
                return true;
    return false;
}
