#include <iostream>
#include <variant>
#include <string>
#include <sstream>

class Token{public:

    enum Type{
        None, // failed to construct
        Type_identifier,
        Literal_bool,
        Literal_int,
        Literal_float,
        Literal_str,
        Operator,
        Id,
        Comment,
    };

    friend std::ostream& operator<< (std::ostream& os, Token val);

    Token(Type type, std::variant<std::string, int, float> value="", int line=-1, int character=-1){
        this->type=type;
        val=value;
        this->line=line;
        this->character=character;
    }

    std::string toString(){
        std::string out="Token(";
        switch(type){
            case None:
                out+="None";
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


    Type type;
    std::variant<std::string, int, float> val;
    int line=0, character=0;
};

std::ostream& operator<< (std::ostream& os, Token val){
    os<<val.toString();
    return os;
}

class Lexer{public:


    Token getToken(std::istream &in){

        while(true){// skip whites
            int test=in.peek();
            if(test==std::char_traits<char>::eof()){
                return Token(Token::Type::None);
            }

            if(isspace(test)){
                if(test=='\n'){
                    line++;
                    character=0;
                }else
                    character++;
                in.get();
                continue;
            }
            break;
        }
        int start_pos=in.tellg();
        int first_char=in.get();
        character++;

        switch(first_char){
            case'-': // operators
                if(in.peek()=='>'){
                    character++;
                    in.get();
                    return Token(Token::Type::Operator, std::string(1, char(first_char))+'>', line, character-2);
                }
            case'+':
            case'|':
            case'&':
            case'<':
            case'>':
            case'=':
                if(in.peek()==first_char){
                    character++;
                    in.get();
                    return Token(Token::Type::Operator, std::string(2, char(first_char)), line, character-2);
                }
            case'*':
            case'^':
            case'!':
                if(in.peek()=='='){
                    character++;
                    in.get();
                    return Token(Token::Type::Operator, std::string(1, char(first_char))+'=', line, character-2);
                }
            case',':
            case'(':
            case')':
            case'{':
            case'}':
            case'[':
            case']':
            return Token(Token::Type::Operator, std::string(1, char(first_char)), line, character-1);
            case'/':
                if(in.peek()==first_char){
                    int pos=character+1;
                    character=0;
                    in.get();
                    std::string comment="";
                    int next_char=0;
                    while(true){
                        next_char=in.get();
                        if(next_char==std::char_traits<char>::eof() || next_char=='\n')
                            break;
                        else
                            comment+=next_char;
                    }
                    return Token(Token::Type::Comment, comment, line++, pos);
                }else
                    return Token(Token::Type::Operator, std::string(1, char(first_char)), line, character-1);
            break;
        }
    }

    void reset_position(){
        line=1;
        character=0;
    }

    int line=1;
    int character=0;
};


int main()
{
    Lexer lexer;
    std::stringstream dsa("     \n++=//asdfghjkl\n//hehe");
    std::cout<<lexer.getToken(dsa)<<"\n";
    std::cout<<lexer.getToken(dsa)<<"\n";
    std::cout<<lexer.getToken(dsa)<<"\n";
    std::cout<<lexer.getToken(dsa)<<"\n";
    std::cout<<lexer.getToken(dsa)<<"\n";

    return 0;
}
