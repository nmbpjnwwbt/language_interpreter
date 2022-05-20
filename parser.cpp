#include "parser.h"

const std::map<ParError::Reasons, std::string> ParError::reasonsToStr={
    {ParError::ExpectedExpression, "expected expression"},
    {ParError::ExpectedTypeIdentifier, "expected type identifier"},
    {ParError::ExpectedComma, "expected ','"},
    {ParError::ExpectedId, "expected identifier"},
    {ParError::ExpectedClosingBracket, "expected ')'"},
    {ParError::ExpectedExpressionOrClosingBracket, "expected expression or ')'"},
    {ParError::ExpectedClosingSquareBracket, "expected ']'"},
    {ParError::ExpectedPrimaryExpression, "expected primary expression"},
    {ParError::ExpectedUnaryExpression, "expected unary expression"},
    {ParError::ExpectedAdditiveExpression, "expected additive expression"},
    {ParError::ExpectedMultiplicativeExpression, "expected multiplicative expression"},
    {ParError::ExpectedEOI, "expected ';'"},
    {ParError::ExpectedBracket, "expected '('"},
    {ParError::ExpectedBlock, "expected block"},
    {ParError::ExpectedInstruction, "expected instruction"},
    {ParError::ExpectedEndOfBlock, "expected '}'"},
    {ParError::ExpectedVariableDeclaration, "expected variable declaration"},
    {ParError::UnexpectedKeyword, "unexpected keyword: "},
    {ParError::UnexpectedOperator, "unexpected operator: "},
    {ParError::ExpectedSwitchBlock, "expected switch block"},
    {ParError::ExpectedCaseOperator, "expected '->'"}

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


std::string FunctionCall::str(){std::string out="FunctionCall("+id+", {";for(Expression arg:arguments)out+="\n"+arg.str();return out+"\n}";}
std::string PrimaryExpression::str(){
    std::string out="PrimaryExpression(";
    if(std::holds_alternative<std::string>(val))
        out+=std::get<std::string>(val);
    else if(std::holds_alternative<Literal>(val))
        out+=std::get<Literal>(val).str();
    else if(std::holds_alternative<Expression>(val))
        return "\n["+std::get<Expression>(val).str()+"]\n";
    else throw std::runtime_error("fix PrimaryExpression::str(): "+std::to_string(val.index()));
    return out+")";
}
std::string AccessExpression::str(){
    std::string out="AccessExpression(";
    if(expr) out+="\n"+expr->str(); else out+="\nnullptr";
    if(accessExpr) out+="\n"+accessExpr->str(); else out+="\nnullptr";
    return out+"\n)";
}
std::string CrementationExpression::str(){
    std::string out="CrementationExpression(", op=in?"++":"--";
    return out+(post?(id+op):(op+id))+")";
}
std::string UnaryExpression::str(){
    return "UnaryOperator("+oper+",\n"+(expr?expr->str():"nullptr")+"\n)";
}
std::string BinaryExpression::str(){
    return "BinaryExpression(\n"+(lexpr?lexpr->str():"nullptr")+",\n"+oper+",\n"+(rexpr?rexpr->str():"nullptr")+"\n)";
}


FunctionDeclaration::FunctionDeclaration(FunctionHeaderDeclaration fhd, std::unique_ptr<InstructionBase> body):id(fhd.id), params(fhd.params){
    Block *ptr=dynamic_cast<Block*>(body.get());
}

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


Parser::Parser(CommentlessLexer &lex):lex(lex){
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
//equality_operator = "==" | "!=";
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
    lex.getToken();
    if(std::unique_ptr<Expression> exp=tryGetExpression())
        return IdDeclaration(tok.getStrVal(), *exp.release());
    errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
    return IdDeclaration(tok.getStrVal());
}
VariableDeclaration Parser::tryGetVariableDeclaration(){ // would eat a type identifier
    //variable_declaration = ["@"], type_identifier, id_declaration;
    bool mutab=false;
    if(checkForKeyword("@")){
        mutab=true;
        lex.getToken();
    }
    Token tok=lex.peekToken();
    if(tok.type()!=Token::Type::Type_identifier || tok.isValue("void")){
        if(mutab)
            errors.push_back(ParError(ParError::ExpectedTypeIdentifier, tok.pos));
        return VariableDeclaration();
    }
    lex.getToken();
    if(IdDeclaration idd=tryGetIdDeclaration()){
        return VariableDeclaration(tok.getStrVal(), idd);
    }
    return VariableDeclaration();

}
std::unique_ptr<InstructionBase> Parser::tryGetVariablesDeclaration(){
    //variables_declaration = variable_declaration, {",", id_declaration}, ";";
    if(VariableDeclaration vardec=tryGetVariableDeclaration()){
        std::unique_ptr<VariablesDeclaration> vardecs=std::make_unique<VariablesDeclaration>(vardec);
        while(lex.peekToken().type()!=Token::Type::EndOfInstruction && lex.peekToken()){
            if(checkForOperator(","))
                lex.getToken();
            else{
                errors.push_back(ParError(ParError::ExpectedComma, lex.peekToken().pos));
                break;
            }
            if(IdDeclaration idd=tryGetIdDeclaration()){
                vardecs->push_back(idd);
            }else{
                errors.push_back(ParError(ParError::ExpectedId, lex.peekToken().pos));
                break;
            }
        }
        return vardecs;
    }
    return nullptr;
}
FunctionHeaderDeclaration Parser::tryGetFunctionHeaderDeclaration(){
    //function_header = function_type_identifier, id, "(", {variable_declaration}, ")";
    if(lex.peekToken().type()!=Token::Type::Type_identifier)
        return FunctionHeaderDeclaration();
    if(lex.peekToken(1).type()!=Token::Type::Id)
        return FunctionHeaderDeclaration();
    if(!checkForOperator("(", 2))
        return FunctionHeaderDeclaration();

    FunctionHeaderDeclaration fhd(lex.peekToken().getStrVal(), lex.peekToken(1).getStrVal());
    lex.getToken();
    lex.getToken();
    lex.getToken();
    if(VariableDeclaration vardec=tryGetVariableDeclaration()){
        fhd.push_back(vardec);
        while(checkForOperator(",")){
            lex.getToken();
            if((vardec=tryGetVariableDeclaration())){
                fhd.push_back(vardec);
            }else{
                errors.push_back(ParError(ParError::ExpectedVariableDeclaration, lex.peekToken().pos));
                if(checkForOperator(")"))
                    lex.getToken();
                break;
            }
        }
    }
    if(checkForOperator(")"))
        lex.getToken();
    else
        errors.push_back(ParError(ParError::ExpectedClosingBracket, lex.peekToken().pos));
    return fhd;
}
std::unique_ptr<InstructionBase> Parser::tryGetFunctionDeclaration(){
    //function_declaration = function_header, block;
    if(FunctionHeaderDeclaration fhd=tryGetFunctionHeaderDeclaration()){
        if(std::unique_ptr<InstructionBase> body=tryGetBlock()){
            return std::make_unique<FunctionDeclaration>(fhd, std::move(body));
        }
    }
    return nullptr;
}
std::unique_ptr<BasicExpression> Parser::tryGetFunctionCall(){
    //function_call = id, "(", [expression, {",", expression}], ")";
    if(lex.peekToken().type()!=Token::Type::Id)
        return nullptr;
    if(!checkForOperator("(", 1)){
        return nullptr;
    }
    std::unique_ptr<FunctionCall> fc=std::make_unique<FunctionCall>(lex.getToken().getStrVal());
    lex.getToken();
    if(checkForOperator(")")){
        lex.getToken();
        return fc;
    }
    if(std::unique_ptr<BasicExpression> expr=tryGetExpression()){
        fc->push_back(std::move(expr));
        while(!checkForOperator(")") && lex.peekToken()){
            if(!checkForOperator(",")){
                errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
                break;
            }
            lex.getToken();
            if(!(expr=tryGetExpression())){
                errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
                break;
            }
            fc->push_back(std::move(expr));
        }
        lex.getToken();
        return fc;
    }else
        errors.push_back(ParError(ParError::ExpectedExpressionOrClosingBracket, lex.peekToken().pos));
    return nullptr;
}
std::unique_ptr<BasicExpression> Parser::tryGetPrimaryExpression(){
    //primary_expression = id
    //                   | literal
    //                   | "(", expression, ")";
    std::unique_ptr<BasicExpression> out=std::make_unique<PrimaryExpression>(lex.peekToken());
    if(*(PrimaryExpression*)(out.get())){ // checking for literals
        lex.getToken();
        return out;
    }
    if(checkForOperator("(")){
        lex.getToken();
        out=tryGetExpression();
        if(!out)
            errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
        if(checkForOperator(")")){
            lex.getToken();
        }else
            errors.push_back(ParError(ParError::ExpectedClosingBracket, lex.peekToken().pos));
        return out;
    }
    return nullptr;
}
std::unique_ptr<BasicExpression> Parser::tryGetAccessExpression(){
    //call_expression = primary_expression
    //                | function_call;
    //access_expression = call_expression
    //                  | call_expression, "[", expression, "]";
    std::unique_ptr<BasicExpression> cexpr=tryGetFunctionCall();
    if(!cexpr)
        cexpr=tryGetPrimaryExpression();
    if(cexpr)
        if(checkForOperator("[")){
            lex.getToken();
            if(std::unique_ptr<Expression> expr=tryGetExpression()){
                if(checkForOperator("]")){
                    lex.getToken();
                    return std::make_unique<AccessExpression>(std::move(cexpr), std::move(expr));
                }
                errors.push_back(ParError(ParError::ExpectedClosingSquareBracket, lex.peekToken().pos));
            }else{
                errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
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
        errors.push_back(ParError(ParError::ExpectedId, lex.peekToken().pos));
        return nullptr;
    }
    if(lex.peekToken().type()==Token::Type::Id){
        if(checkForOperators({inc_oper, dec_oper}, 1)){
            std::unique_ptr<CrementationExpression> cremexpr=std::make_unique<CrementationExpression>(lex.peekToken(1).getStrVal(), lex.peekToken(0).getStrVal(), true);
            lex.getToken();
            lex.getToken();
            return cremexpr;
        }
    }
    return nullptr;
}
std::unique_ptr<BasicExpression> Parser::tryGetUnaryExpression(){
    //unary_expression = (unary_operator, primary_expression)
    //                 | crementation_expression
    //                 | access_expression;
    if(checkForOperators({neg_oper, sub_oper})){
        std::string oper=lex.getToken().getStrVal();
        std::unique_ptr<BasicExpression> primexpr=tryGetPrimaryExpression();
        if(primexpr)
            return std::make_unique<UnaryExpression>(oper, std::move(primexpr));
        errors.push_back(ParError(ParError::ExpectedPrimaryExpression, lex.peekToken().pos));
        return nullptr;
    }
    std::unique_ptr<BasicExpression> cremexpr=tryGetCrementationExpression();
    if(cremexpr)
        return cremexpr;
    cremexpr=tryGetAccessExpression();
    return cremexpr;
}
std::unique_ptr<BasicExpression> Parser::tryGetBinaryExpression(std::vector<std::string> opers, std::unique_ptr<BasicExpression, std::default_delete<BasicExpression>>(*func)(Parser &par)){
    //multiplicative_expression = unary_expression
    //                          | multiplicative_expression, multiplicative_operator, unary_expression;
    std::unique_ptr<BasicExpression> expr=(*func)(*this);
    if(!expr)
        return expr;
    while(checkForOperators(opers)){
        std::string oper=lex.getToken().getStrVal();
        std::unique_ptr<BasicExpression> expr2=(*func)(*this);
        if(!expr2){
            errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
            return expr;
        }
        expr=std::make_unique<BinaryExpression>(oper, std::move(expr), std::move(expr2));
    }
    return expr;
}
std::unique_ptr<BasicExpression> Parser::tryGetMultExpression(){
    //multiplicative_expression = unary_expression
    //                          | multiplicative_expression, multiplicative_operator, unary_expression;
    return tryGetBinaryExpression({mul_oper, div_oper, mod_oper}, [](Parser &par){return par.tryGetUnaryExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetAddExpression(){
    //additive_expression = multiplicative_expression
    //                    | additive_expression, additive_operator, multiplicative_expression;
    return tryGetBinaryExpression({add_oper, sub_oper}, [](Parser &par){return par.tryGetMultExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetShiftExpression(){
    //shift_expression = additive_expression
    //                 | shift_expression, shift_operator, additive_expression;
    return tryGetBinaryExpression({lsh_oper, rsh_oper}, [](Parser &par){return par.tryGetAddExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetRelationalExpression(){
    //relational_expression = shift_expression
    //                      | relational_expression, relational_operator, shift_expression;
    return tryGetBinaryExpression({lth_oper, mth_oper, lth_oper, mth_oper}, [](Parser &par){return par.tryGetShiftExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetEqualityExpression(){
    //equality_expression = relational_expression
    //                    | equality_expression, equality_operator, relational_expression;
    return tryGetBinaryExpression({equ_oper, neq_oper}, [](Parser &par){return par.tryGetRelationalExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetAndExpression(){
    //and_expression = equality_expression
    //               | and_expression, and_operator, equality_expression;
    return tryGetBinaryExpression({and_oper}, [](Parser &par){return par.tryGetEqualityExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetXorExpression(){
    //xor_expression = and_expression
    //               | xor_expression, xor_operator, and_expression;
    return tryGetBinaryExpression({or_oper}, [](Parser &par){return par.tryGetAndExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetOrExpression(){
    //or_expression = xor_expression
    //              | or_expression, or_operator, xor_expression;
    return tryGetBinaryExpression({xor_oper}, [](Parser &par){return par.tryGetXorExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetLAndExpression(){
    //log_and_expression = or_expression
    //                   | log_and_expression, log_and_operator, or_expression;
    return tryGetBinaryExpression({land_oper}, [](Parser &par){return par.tryGetOrExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetLOrExpression(){
    //log_or_expression = log_and_expression
    //                  | log_or_expression, log_or_operator, log_and_expression;
    return tryGetBinaryExpression({lor_oper}, [](Parser &par){return par.tryGetLAndExpression();});
}
std::unique_ptr<BasicExpression> Parser::tryGetAssignmentExpression(){
    //assignment_expression = log_or_expression
    //                      | id, assignment_operator, assignment_expression;
    return tryGetBinaryExpression({ass_oper, add_ass, sub_ass, mul_ass, div_ass, mod_ass, and_ass, or_ass, lsh_ass, rsh_ass, xor_ass},
                                  [](Parser &par){return par.tryGetLOrExpression();});
}
std::unique_ptr<Expression> Parser::tryGetExpression(){
    //expression = assignment_expression;
    std::unique_ptr<BasicExpression> expr=tryGetAssignmentExpression();
    if(expr){
        return std::make_unique<Expression>(std::move(expr));
    }else{
        return nullptr;
    }
}
std::unique_ptr<CaseExpression> Parser::tryGetCaseExpression(){
    //expression = assignment_expression;
    std::string oper="";
    if(checkForKeyword("else")){
        lex.getToken();
        return std::make_unique<CaseExpression>(nullptr, oper);
    }
    if(checkForOperators({lth_oper, mth_oper, lth_oper, mth_oper})){
        oper=lex.getToken().getStrVal();
    }
    std::unique_ptr<BasicExpression> expr=tryGetAssignmentExpression();
    if(expr){
        return std::make_unique<CaseExpression>(std::move(expr), oper);
    }else{
        return nullptr;
    }
}

//
//special_instruction = "continue"
//                    | "break"
//                    | ("return", [expression]);
std::unique_ptr<InstructionBase> Parser::tryGetSpecialInstruction(){
    std::unique_ptr<SpecialInstruction> spi=nullptr;
    if(lex.peekToken().type()==Token::Type::Keyword){
        std::string str=lex.peekToken().getStrVal();
        if(str=="break")
            spi=std::make_unique<SpecialInstruction>(SpecialInstruction::Type::Break);
        else if(str=="continue")
            spi=std::make_unique<SpecialInstruction>(SpecialInstruction::Type::Continue);

        if(spi){
            lex.getToken();
            return spi;
        }
        if(str=="return"){
            lex.getToken();
            std::unique_ptr<Expression> expr=tryGetExpression();
            if(expr==nullptr)
                errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
            spi=std::make_unique<SpecialInstruction>(SpecialInstruction::Type::Return, std::move(expr));
        }
    }
    return spi;
}

//instruction = ([expression], ";")
//            | (special_instruction, ";")
//            | variables_declaration
//            | function_declaration
//            | while_loop
//            | if_statement
//            | switch_statement
std::unique_ptr<InstructionBase> Parser::tryGetInstruction(){
    if(lex.peekToken().type()==Token::Type::EndOfInstruction){
        lex.getToken();
        return std::make_unique<ExpressionWrapper>(std::make_unique<Expression>());
    }
    std::unique_ptr<Expression> expr=tryGetExpression();
    std::unique_ptr<InstructionBase> inst=expr?std::make_unique<ExpressionWrapper>(std::move(expr)):nullptr;
    if(inst!=nullptr){
        if(lex.peekToken().type()!=Token::Type::EndOfInstruction){
            errors.push_back(ParError(ParError::ExpectedEOI, lex.peekToken().pos));
            return tryGetInstruction();
        }
        lex.getToken();
        return inst;
    }
    inst=tryGetSpecialInstruction();
    if(inst!=nullptr){
        if(lex.peekToken().type()!=Token::Type::EndOfInstruction){
            errors.push_back(ParError(ParError::ExpectedEOI, lex.getToken().pos));
            return nullptr;
        }
        lex.getToken();
        return inst;
    }
    inst=tryGetFunctionDeclaration();
    if(inst==nullptr) inst=tryGetVariablesDeclaration();
    if(inst==nullptr) inst=tryGetWhile();
    if(inst==nullptr) inst=tryGetIf();
    if(inst==nullptr) inst=tryGetSwitch();
    if(inst==nullptr) inst=tryGetBlock(false);
    if(inst==nullptr){
        if(lex.peekToken().type()==Token::Type::Keyword){
            errors.push_back(ParError(ParError::UnexpectedKeyword, lex.peekToken().getStrVal(), lex.peekToken().pos));
            lex.getToken();
            inst=tryGetInstruction();
        }else if(lex.peekToken().type()==Token::Type::Operator){
            if(!checkForOperator("}")){
                errors.push_back(ParError(ParError::UnexpectedOperator, lex.peekToken().getStrVal(), lex.peekToken().pos));
                lex.getToken();
                inst=tryGetInstruction();
            }
        }
    }
    return inst;
}

//instructions = instruction, {instruction}
Instructions Parser::tryGetInstructions(){
    if(std::unique_ptr<InstructionBase> inst=tryGetInstruction()){
        Instructions insts(std::move(inst));
        while((inst=tryGetInstruction())){
                insts.push_back(std::move(inst));
        }
        return insts;
    }
    return Instructions();
}

//block = ("{", instructions, "}")
//      | instruction;
std::unique_ptr<InstructionBase> Parser::tryGetBlock(bool singleLineToo){
    if(checkForOperator("{")){
        lex.getToken();
        std::unique_ptr<Instructions> insts=std::make_unique<Instructions>(tryGetInstructions());
        if(checkForOperator("}"))
            lex.getToken();
        else
            errors.push_back(ParError(ParError::ExpectedEndOfBlock, lex.peekToken().pos));
        return insts;
    }else if(singleLineToo){
        return tryGetInstruction();
    }
    return nullptr;
}


std::unique_ptr<InstructionBase> Parser::tryGetConditionedInstruction(std::string keyw){
    if(!checkForKeyword(keyw))
        return nullptr;
    lex.getToken();
    if(!checkForOperator("(")){
        errors.push_back(ParError(ParError::ExpectedBracket, lex.peekToken().pos));
        return nullptr;
    }
    lex.getToken();
    std::unique_ptr<BasicExpression> expr=tryGetExpression();
    if(expr==nullptr){
        errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
        if(checkForOperator(")"))
            lex.getToken();
        return nullptr;
    }
    if(!checkForOperator(")")){
        errors.push_back(ParError(ParError::ExpectedClosingBracket, lex.peekToken().pos));
        return nullptr;
    }
    lex.getToken();
    if(std::unique_ptr<InstructionBase> body=tryGetBlock()){
        if(keyw=="while")
            return std::make_unique<While>(std::move(expr), std::move(body));
        if(keyw=="if")
            return std::make_unique<If>(std::move(expr), std::move(body));
        else
            throw std::runtime_error("unexpected condition instruction keyword: "+keyw);
    }
    errors.push_back(ParError(ParError::ExpectedBlock, lex.peekToken().pos));
    return nullptr;
}
//while_loop = "while", "(", expression, ")",
//           block;
std::unique_ptr<InstructionBase> Parser::tryGetWhile(){
    return tryGetConditionedInstruction("while");
}

//if_statement = "if", "(", expression, ")",
//             block,
//             ["else"], (instruction | (block));
std::unique_ptr<InstructionBase> Parser::tryGetIf(){
    std::unique_ptr<InstructionBase> inst=tryGetConditionedInstruction("if");
    if(inst==nullptr)
        return inst;
    std::unique_ptr<If> if_inst;
    if_inst.reset(dynamic_cast<If*>(inst.release()));
    if(if_inst==nullptr)
        throw std::runtime_error("tryGetConditionedInstr(\"if\") did not return a pointer to If");
    if(!checkForKeyword("else"))
        return if_inst;
    lex.getToken();
    if_inst->else_body=tryGetBlock();
    return if_inst;
}

std::unique_ptr<Case> Parser::tryGetCase(){
    if(std::unique_ptr<CaseExpression> cexpr=tryGetCaseExpression()){
        if(!checkForOperator("->")){
            errors.push_back(ParError(ParError::ExpectedCaseOperator, lex.peekToken().pos));
            return nullptr;
        }
        lex.getToken();
        if(std::unique_ptr<InstructionBase> body=tryGetBlock()){
            return std::make_unique<Case>(std::move(cexpr), std::move(body));
        }else
            errors.push_back(ParError(ParError::ExpectedBlock, lex.peekToken().pos));
    }
    return nullptr;
}

//switch_statement = "switch", "(", expression, ")",
//                "{", {(expression | "else"), "->", block}, "}";
std::unique_ptr<InstructionBase> Parser::tryGetSwitch(){
    if(!checkForKeyword("switch"))
        return nullptr;
    lex.getToken();
    if(!checkForOperator("(")){
        errors.push_back(ParError(ParError::ExpectedBracket, lex.peekToken().pos));
        return nullptr;
    }
    lex.getToken();
    std::unique_ptr<Expression> expr=tryGetExpression();
    if(expr==nullptr){
        errors.push_back(ParError(ParError::ExpectedExpression, lex.peekToken().pos));
        if(checkForOperator(")"))
            lex.getToken();
        return nullptr;
    }
    std::unique_ptr<Switch> swit=std::make_unique<Switch>(std::move(expr));
    if(!checkForOperator(")")){
        errors.push_back(ParError(ParError::ExpectedClosingBracket, lex.peekToken().pos));
        return nullptr;
    }
    lex.getToken();


    if(!checkForOperator("{")){
        errors.push_back(ParError(ParError::ExpectedSwitchBlock, lex.peekToken().pos));
        return swit;
    }
    lex.getToken();
    while(std::unique_ptr<Case> cas=tryGetCase()){
        swit->push_back(std::move(cas));
    }
    if(!checkForOperator("}")){
        errors.push_back(ParError(ParError::ExpectedEndOfBlock, lex.peekToken().pos));
        return swit;
    }
    lex.getToken();
    return swit;
}

//
//
//start = instructions;


bool Parser::checkForOperators(std::vector<std::string> op, int n) const{
    if(lex.peekToken(n).type()==Token::Type::Operator)
        for(std::string i:op)
            if(lex.peekToken(n).isValue(i))
                return true;
    return false;
}
