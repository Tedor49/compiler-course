#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <sstream>
#import <queue>
#include <typeinfo>

#include "token_data.hpp"

enum NodeType {
    Program,
    Statement,
    Declaration,
    VariableDefinition,
    Expression,
    IDENT,
    Assignment,
    If,
    For,
    While,
    Return,
    Print,
    Relation,
    Factor,
    Term,
    Unary,
    TypeIndicator,
    Primary,
    IntegerLiteral,
    RealLiteral,
    BooleanLiteral,
    StringLiteral,
    ArrayLiteral,
    TupleLiteral,
    Reader,
    FunctionLiteral,
    Body,
    Reference,
    Array,
    FunBody,
    Parameters,
};


class Node {
    public:
        virtual void find_nodes(std::vector<void*>& out, std::type_info& q) = 0;
        virtual void print(std::ostream& out, int indent=4, int acc_indent=0) = 0;
};

std::ostream& operator<<(std::ostream& out, Node* node) {
     node->print(out, 2);
     return out;
}

class ProgramNode;
class StatementNode;
class DeclarationNode;
class VariableDefinitionNode;
class ExpressionNode;
class RelationNode;
class FactorNode;
class TermNode;
class UnaryNode;
class PrimaryNode;
class TailNode;
class AssignmentNode;
class PrintNode;
class ReturnNode;
class IfNode;
class ForNode;
class WhileNode;
class BodyNode;
class TypeIndicatorNode;
class LiteralNode;
class ArrayLiteralNode;
class TupleLiteralNode;
class FunctionNode;
class RangeNode;

Node* createNode(const std::string& t, std::vector<Token>& tokens, int& y);

class ProgramNode: public Node {
    private:
        std::vector<Node*> statements;
    public:
        ProgramNode (std::vector<Token>& tokens, int& y) {
            while (!tokens.empty()) {

                while (tokens[y].type == TokenCode::tkLineEnd){
                    ++y;
                    if (tokens.size() == y) break;

                }
                if (tokens.size() == y) break;
                this->statements.push_back(createNode("Statement", tokens, y));
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            for(auto i: statements) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Programm{\n";
            for(auto i: statements) {
                i->print(out, indent, acc_indent+indent);
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class StatementNode: public Node {
    private:
        char type;
        Node* declare_node;
        Node* assign_node;
        Node* if_node;
        Node* for_node;
        Node* while_node;
        Node* return_node;
        Node* print_node;
        Node* expression_node;
    public:
        StatementNode (std::vector<Token>& tokens, int& y) {

            switch (tokens[y].type) {
                case TokenCode::tkVar:
                    this->type = 'd';
                    ++y;
                    this->declare_node = createNode("Declaration", tokens, y);
                    break;
                case TokenCode::tkIdentifier:
                    this->type = 'a';
                    this->assign_node = createNode("Assignment", tokens, y);
                    break;
                case TokenCode::tkIf:
                    this->type = 'i';
                    ++y;
                    this->if_node = createNode("If", tokens, y);
                    break;
                case TokenCode::tkFor:
                    this->type = 'f';
                    ++y;
                    this->for_node = createNode("For", tokens, y);
                    break;
                case TokenCode::tkWhile:
                    this->type = 'w';
                    ++y;
                    this->while_node = createNode("While", tokens, y);
                    break;
                case TokenCode::tkReturn:
                    this->type = 'r';
                    ++y;
                    this->return_node = createNode("Return", tokens, y);
                    break;
                case TokenCode::tkPrint:
                    this->type = 'p';
                    ++y;
                    this->print_node = createNode("Print", tokens, y);
                    break;
                default:
                    throw std::invalid_argument("Not a statement");
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            switch (this->type) {
                case 'd':
                    this->declare_node->find_nodes(out, q);
                    break;
                case 'a':
                    this->assign_node->find_nodes(out, q);
                    break;
                case 'e':
                    this->expression_node->find_nodes(out, q);
                    break;
                case 'i':
                    this->if_node->find_nodes(out, q);
                    break;
                case 'f':
                    this->for_node->find_nodes(out, q);
                    break;
                case 'w':
                    this->while_node->find_nodes(out, q);
                    break;
                case 'r':
                    this->return_node->find_nodes(out, q);
                    break;
                case 'p':
                    this->print_node->find_nodes(out, q);
                    break;
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Statement{\n";
            switch (this->type) {
                case 'd':
                    this->declare_node->print(out, indent, acc_indent+indent);
                    break;
                case 'a':
                    this->assign_node->print(out, indent, acc_indent+indent);
                    break;
                case 'e':
                    this->expression_node->print(out, indent, acc_indent+indent);
                    break;
                case 'i':
                    this->if_node->print(out, indent, acc_indent+indent);
                    break;
                case 'f':
                    this->for_node->print(out, indent, acc_indent+indent);
                    break;
                case 'w':
                    this->while_node->print(out, indent, acc_indent+indent);
                    break;
                case 'r':
                    this->return_node->print(out, indent, acc_indent+indent);
                    break;
                case 'p':
                    this->print_node->print(out, indent, acc_indent+indent);
                    break;
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class DeclarationNode: public Node {
    private:
        std::vector<Node*> vars;
    public:
        DeclarationNode (std::vector<Token>& tokens, int& y) {
            while (1) {

                switch (tokens[y].type) {
                    case TokenCode::tkIdentifier:
                        this->vars.push_back(createNode("VariableDefinition", tokens, y));
                        break;
                    default:
                        throw std::invalid_argument("Not a variable definition");
                }

                if (tokens[y].type == TokenCode::tkComma) {
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkLineEnd) {
                    break;
                }
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            for(auto i: this->vars) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Declaration{\n";
            for(auto i: this->vars) {
                i->print(out, indent, acc_indent+indent);
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class VariableDefinitionNode: public Node {
    private:
        std::string identifier;
        Node* value = nullptr;
    public:
        VariableDefinitionNode (std::vector<Token>& tokens, int& y) {

            this->identifier = tokens[y].valStr;
            ++y;


            switch (tokens[y].type) {
                case TokenCode::tkComma:
                    return;
                case TokenCode::tkLineEnd:
                    return;
                case TokenCode::tkAssignment:
                    ++y;
                    this->value = createNode("Expression", tokens, y);
                    break;
                default:
                    throw std::invalid_argument("Unexpected value, expected :=");
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            if (this->value) this->value->find_nodes(out, q);
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "VariableDefinition{\n";
            out << std::string(acc_indent+indent, ' ') << "Identifier: " << this->identifier << "\n";
            out << std::string(acc_indent+indent, ' ') << "Value:";
            if (this->value) {
                out << "\n";
                this->value->print(out, indent, acc_indent+indent*2);
            } else {
                out << " Empty\n";
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class ExpressionNode: public Node {
    private:
        std::vector<Node*> relations;
        std::vector<char> ops;
    public:
        ExpressionNode (std::vector<Token>& tokens, int& y) {

            while (1) {
                this->relations.push_back(createNode("Relation", tokens, y));

                if (tokens[y].type == TokenCode::tkLogicAnd) {
                    this->ops.push_back('a');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkLogicOr) {
                    this->ops.push_back('o');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkLogicXor) {
                    this->ops.push_back('x');
                    ++y;
                    continue;
                } else {
                    return;
                }
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            for(auto i: this->relations) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Expression{\n";
            out << std::string(acc_indent+indent, ' ') << "Element amount: " << this->relations.size() << "\n";
            out << std::string(acc_indent+indent, ' ') << "Element 1: \n";
            this->relations[0]->print(out, indent, acc_indent+indent*2);
            for (int i = 1; i < relations.size(); ++i){
                out << std::string(acc_indent+indent, ' ') << "Operation " << i << ": ";
                switch (this->ops[i-1]) {
                    case 'a':
                        out << "and\n";
                        break;
                    case 'o':
                        out << "or\n";
                        break;
                    case 'x':
                        out << "xor\n";
                        break;
                }
                out << std::string(acc_indent+indent, ' ') << "Element " << i+1 << ": \n";
                this->relations[i]->print(out, indent, acc_indent+indent*2);
            }

            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class RelationNode: public Node {
    private:
        std::vector<Node*> factors;
        std::vector<char> ops;
    public:
        RelationNode (std::vector<Token>& tokens, int& y) {
            while (1) {
                this->factors.push_back(createNode("Factor", tokens, y));

                if (tokens[y].type == TokenCode::tkRelationEqual) {
                    this->ops.push_back('=');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkRelationLess) {
                    this->ops.push_back('<');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkRelationLessEq) {
                    this->ops.push_back('l');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkRelationMore) {
                    this->ops.push_back('>');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkRelationMoreEq) {
                    this->ops.push_back('m');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkRelationNotEqual) {
                    this->ops.push_back('n');
                    ++y;
                    continue;
                } else {
                    return;
                }
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            for(auto i: this->factors) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Relation{\n";
            out << std::string(acc_indent+indent, ' ') << "Element amount: " << this->factors.size() << "\n";
            out << std::string(acc_indent+indent, ' ') << "Element 1: \n";
            this->factors[0]->print(out, indent, acc_indent+indent*2);
            for (int i = 1; i < factors.size(); ++i){
                out << std::string(acc_indent+indent, ' ') << "Operation " << i << ": ";
                switch (this->ops[i-1]) {
                    case '=':
                        out << "=\n";
                        break;
                    case '<':
                        out << "<\n";
                        break;
                    case 'l':
                        out << "<=\n";
                        break;
                    case '>':
                        out << ">\n";
                        break;
                    case 'm':
                        out << ">=\n";
                        break;
                    case 'n':
                        out << "/=\n";
                        break;
                }
                out << std::string(acc_indent+indent, ' ') << "Element " << i+1 << ": \n";
                this->factors[i]->print(out, indent, acc_indent+indent*2);
            }

            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class FactorNode: public Node {
    private:
        std::vector<Node*> terms;
        std::vector<char> ops;
    public:
        FactorNode (std::vector<Token>& tokens, int& y) {
            while (1) {
                this->terms.push_back(createNode("Term", tokens, y));

                if (tokens[y].type == TokenCode::tkOperatorPlus) {
                    this->ops.push_back('+');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkOperatorMinus) {
                    this->ops.push_back('-');
                    ++y;
                    continue;
                } else {
                    return;
                }
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            for(auto i: this->terms) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Factor{\n";
            out << std::string(acc_indent+indent, ' ') << "Element amount: " << this->terms.size() << "\n";
            out << std::string(acc_indent+indent, ' ') << "Element 1: \n";
            this->terms[0]->print(out, indent, acc_indent+indent*2);
            for (int i = 1; i < terms.size(); ++i){
                out << std::string(acc_indent+indent, ' ') << "Operation " << i << ": ";
                switch (this->ops[i-1]) {
                    case '+':
                        out << "+\n";
                        break;
                    case '-':
                        out << "-\n";
                        break;
                }
                out << std::string(acc_indent+indent, ' ') << "Element " << i+1 << ": \n";
                this->terms[i]->print(out, indent, acc_indent+indent*2);
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class TermNode: public Node {
    private:
        std::vector<Node*> terms;
        std::vector<char> ops;
    public:
        TermNode (std::vector<Token>& tokens, int& y) {
            while (1) {
                this->terms.push_back(createNode("Unary", tokens, y));

                if (tokens[y].type == TokenCode::tkOperatorMultiply) {
                    this->ops.push_back('*');
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkOperatorDivide) {
                    this->ops.push_back('/');
                    ++y;
                    continue;
                } else {
                    return;
                }
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            for(auto i: this->terms) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Term{\n";
            out << std::string(acc_indent+indent, ' ') << "Element amount: " << this->terms.size() << "\n";
            out << std::string(acc_indent+indent, ' ') << "Element 1: \n";
            this->terms[0]->print(out, indent, acc_indent+indent*2);
            for (int i = 1; i < terms.size(); ++i){
                out << std::string(acc_indent+indent, ' ') << "Operation " << i << ": ";
                switch (this->ops[i-1]) {
                    case '*':
                        out << "*\n";
                        break;
                    case '/':
                        out << "/\n";
                        break;
                }
                out << std::string(acc_indent+indent, ' ') << "Element " << i+1 << ": \n";
                this->terms[i]->print(out, indent, acc_indent+indent*2);
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class UnaryNode: public Node {
    private:
        char type;

        char unaryop = '#';
        Node* primary;
        Node* type_ind = nullptr;

        Node* literal;

        Node* expression;

    public:
        UnaryNode (std::vector<Token>& tokens, int& y) {

            switch (tokens[y].type) {
                case TokenCode::tkUnaryPlus:
                    this->type = 'p';
                    this->unaryop = '+';

                    ++y;
                    this->primary = createNode("Primary", tokens, y);


                    if (tokens[y].type == TokenCode::tkIs) {
                        ++y;
                        this->type_ind = createNode("TypeIndicator", tokens, y);
                    }

                    break;
                case TokenCode::tkUnaryMinus:
                    this->type = 'p';
                    this->unaryop = '-';

                    ++y;
                    this->primary = createNode("Primary", tokens, y);


                    if (tokens[y].type == TokenCode::tkIs) {
                        ++y;
                        this->type_ind = createNode("TypeIndicator", tokens, y);
                    }

                    break;
                case TokenCode::tkUnaryNot:
                    this->type = 'p';
                    this->unaryop = 'n';

                    ++y;
                    this->primary = createNode("Primary", tokens, y);


                    if (tokens[y].type == TokenCode::tkIs) {
                        ++y;
                        this->type_ind = createNode("TypeIndicator", tokens, y);
                    }

                    break;
                case TokenCode::tkIdentifier:
                case TokenCode::tkReadInt:
                case TokenCode::tkReadReal:
                case TokenCode::tkReadString:
                case TokenCode::tkInt:
                case TokenCode::tkBooleanFalse:
                case TokenCode::tkBooleanTrue:
                case TokenCode::tkString:
                case TokenCode::tkBracketSquareLeft:
                case TokenCode::tkBracketCurvyLeft:
                case TokenCode::tkReal:
                case TokenCode::tkFunc:
                case TokenCode::tkEmpty:
                    this->type = 'p';

                    this->primary = createNode("Primary", tokens, y);


                    if (tokens[y].type == TokenCode::tkIs) {
                        ++y;
                        this->type_ind = createNode("TypeIndicator", tokens, y);
                    }

                    break;
                case TokenCode::tkBracketNormalLeft:
                    this->type = 'e';
                    ++y;

                    this->expression = createNode("Expression", tokens, y);


                    if (tokens[y].type != TokenCode::tkBracketNormalRight){
                        throw std::invalid_argument("Expected closing bracket after expression");
                    }
                    ++y;

                    break;
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            switch (this->type) {
                case 'p':
                    this->primary->find_nodes(out, q);
                    if (this->type_ind) this->type_ind->find_nodes(out, q);
                    break;
                case 'l':
                    this->literal->find_nodes(out, q);
                    break;
                case 'e':
                    this->expression->find_nodes(out, q);
                    break;
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Unary{\n";
            out << std::string(acc_indent+indent, ' ') << "Type: ";
            switch (this->type) {
                case 'p':
                    out << "Primary\n";
                    out << std::string(acc_indent+indent, ' ') << "UnaryOperation: ";
                    switch (this->unaryop) {
                        case '#':
                            out << "None\n";
                            break;
                        case '+':
                            out << "+\n";
                            break;
                        case '-':
                            out << "-\n";
                            break;
                        case 'n':
                            out << "not\n";
                            break;
                    }
                    out << std::string(acc_indent+indent, ' ') << "Value:\n";
                    this->primary->print(out, indent, acc_indent+indent*2);
                    out << std::string(acc_indent+indent, ' ') << "Type indicator:";
                    if (this->type_ind) {
                        out << "\n";
                        this->type_ind->print(out, indent, acc_indent+indent*2);
                    } else {
                        out << " None\n";
                    }

                    break;
                case 'l':
                    out << "Literal\n";
                    out << std::string(acc_indent+indent, ' ') << "Value:\n";
                    this->literal->print(out, indent, acc_indent+indent*2);
                    break;
                case 'e':
                    out << "Expression\n";
                    out << std::string(acc_indent+indent, ' ') << "Value:\n";
                    this->expression->print(out, indent, acc_indent+indent*2);
                    break;
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};


class TypeIndicatorNode: public Node {
    private:
        char type;
    public:
        TypeIndicatorNode (std::vector<Token>& tokens, int& y) {

            switch (tokens[y].type) {
                case TokenCode::tkTypeInt:
                    this->type = 'i';
                    ++y;
                    break;
                case TokenCode::tkTypeReal:
                    this->type = 'r';
                    ++y;
                    break;
                case TokenCode::tkTypeBool:
                    this->type = 'b';
                    ++y;
                    break;
                case TokenCode::tkTypeString:
                    this->type = 's';
                    ++y;
                    break;
                case TokenCode::tkTypeEmpty:
                    this->type = 'e';
                    ++y;
                    break;
                case TokenCode::tkBracketSquareLeft:
                    this->type = 'a';
                    ++y;


                    if (tokens[y].type != TokenCode::tkBracketSquareRight){
                        throw std::invalid_argument("Expected closing bracket in type indicator");
                    }
                    ++y;

                    break;
                case TokenCode::tkBracketCurvyLeft:
                    this->type = 't';
                    ++y;


                    if (tokens[y].type != TokenCode::tkBracketCurvyRight){
                        throw std::invalid_argument("Expected closing bracket in type indicator");
                    }
                    ++y;

                    break;
                case TokenCode::tkTypeFunc:
                    this->type = 'f';
                    ++y;
                    break;
                default:
                    throw std::invalid_argument("Expected type indicator");
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "TypeIndicator{";
            switch (this->type) {
                case 'i':
                    out << "Int}\n";
                    break;
                case 'r':
                    out << "Real}\n";
                    break;
                case 'b':
                    out << "Boolean}\n";
                    break;
                case 's':
                    out << "String}\n";
                    break;
                case 'e':
                    out << "Empty}\n";
                    break;
                case 'a':
                    out << "Array}\n";
                    break;
                case 't':
                    out << "Tuple}\n";
                    break;
                case 'f':
                    out << "Function}\n";
                    break;
            }
        }
};

class IfNode: public Node {
    private:
        Node* expression;
        Node* if_body;
        Node* else_body = nullptr;

    public:
        IfNode (std::vector<Token>& tokens, int& y) {
            this->expression = createNode("Expression", tokens, y);


            if (tokens[y].type != TokenCode::tkDelimeterThen) {
                throw std::invalid_argument("Expected if body beginning");
            }
            ++y;

            this->if_body = createNode("Body", tokens, y);

            if (tokens[y].type == TokenCode::tkElse) {
                ++y;
                this->else_body = createNode("Body", tokens, y);
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            this->expression->find_nodes(out, q);
            this->if_body->find_nodes(out, q);
            if (this->else_body) this->else_body->find_nodes(out, q);
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "If{\n";
            out << std::string(acc_indent+indent, ' ') << "Expression:\n";
            this->expression->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent+indent, ' ') << "Body:\n";
            this->if_body->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent+indent, ' ') << "ElseBody:";
            if (this->else_body) {
                out << "\n";
                this->else_body->print(out, indent, acc_indent+indent*2);
            } else {
                out << "None\n";
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class ForNode: public Node {
    private:
        std::string identifier;
        Node* range;
        Node* body;

    public:
        ForNode (std::vector<Token>& tokens, int& y) {

            if (tokens[y].type != TokenCode::tkIdentifier) {
                throw std::invalid_argument("Expected iteration variable name");
            }
            this->identifier = tokens[y].valStr;
            ++y;


            if (tokens[y].type != TokenCode::tkIn) {
                throw std::invalid_argument("Expected range after for loop declaration");
            }
            ++y;

            this->range = createNode("Range", tokens, y);


            if (tokens[y].type != TokenCode::tkDelimeterLoop) {
                throw std::invalid_argument("Expected loop body after range");
            }
            ++y;

            this->body = createNode("Body", tokens, y);


            if (tokens[y].type != TokenCode::tkLineEnd) {
                throw std::invalid_argument("Expected line end or ; at the end of loop");
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            this->range->find_nodes(out, q);
            this->body->find_nodes(out, q);
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "For{\n";
            out << std::string(acc_indent+indent, ' ') << "Identifier: \"" << this->identifier << "\"\n";
            out << std::string(acc_indent+indent, ' ') << "Range:\n";
            this->range->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent+indent, ' ') << "Body:\n";
            this->body->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class WhileNode: public Node {
    private:
        Node* expression;
        Node* body;

    public:
        WhileNode (std::vector<Token>& tokens, int& y) {

            this->expression = createNode("Expression", tokens, y);


            if (tokens[y].type != TokenCode::tkDelimeterLoop) {
                throw std::invalid_argument("Expected loop body after range");
            }
            ++y;

            this->body = createNode("Body", tokens, y);


            if (tokens[y].type != TokenCode::tkLineEnd) {
                throw std::invalid_argument("Expected line end or ; at the end of loop");
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            this->expression->find_nodes(out, q);
            this->body->find_nodes(out, q);
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "While{\n";
            out << std::string(acc_indent+indent, ' ') << "Expression:\n";
            this->expression->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent+indent, ' ') << "Body:\n";
            this->body->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class RangeNode: public Node {
    private:
        Node* expression_1;
        Node* expression_2;

    public:
        RangeNode (std::vector<Token>& tokens, int& y) {
            this->expression_1 = createNode("Expression", tokens, y);


            if (tokens[y].type != TokenCode::tkDoubleDot) {
                throw std::invalid_argument("Expected .. in range definition");
            }
            ++y;

            this->expression_2 = createNode("Expression", tokens, y);
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            this->expression_1->find_nodes(out, q);
            this->expression_2->find_nodes(out, q);
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Range{\n";
            out << std::string(acc_indent+indent, ' ') << "ExpressionBegin:\n";
            this->expression_1->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent+indent, ' ') << "ExpressionEnd:\n";
            this->expression_2->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class PrimaryNode: public Node {
    private:
        char type;

        Node* literal;

        std::string identifier;
        std::vector<Node*> tails;
    public:
        PrimaryNode (std::vector<Token>& tokens, int& y) {
            switch (tokens[y].type) {
                case TokenCode::tkReadInt:
                    this->type = 'i';
                    ++y;
                    break;
                case TokenCode::tkReadReal:
                    this->type = 'r';
                    ++y;
                    break;
                case TokenCode::tkReadString:
                    this->type = 's';
                    ++y;
                    break;
                case TokenCode::tkInt:
                case TokenCode::tkBooleanFalse:
                case TokenCode::tkBooleanTrue:
                case TokenCode::tkString:
                case TokenCode::tkBracketSquareLeft:
                case TokenCode::tkBracketCurvyLeft:
                case TokenCode::tkReal:
                case TokenCode::tkFunc:
                case TokenCode::tkEmpty:
                    this->type = 'l';
                    this->literal = createNode("Literal", tokens, y);
                    break;
                case TokenCode::tkIdentifier:
                    this->type = 'v';
                    this->identifier = tokens[y].valStr;
                    ++y;

                    while (1) {

                        if(tokens[y].type == TokenCode::tkDot ||
                           tokens[y].type == TokenCode::tkBracketSquareLeft ||
                           tokens[y].type == TokenCode::tkBracketNormalLeft) {
                            this->tails.push_back(createNode("Tail", tokens, y));
                        } else {
                            break;
                        }
                    }
                    break;
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            if (this->type == 'v') {
                for(auto i : this->tails) {
                    i->find_nodes(out, q);
                }
            } else if (this->type == 'l') {
                this->literal->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Primary{\n";
            out << std::string(acc_indent+indent, ' ') << "Type: ";
            switch  (this->type) {
                case 'i':
                    out << "ReadInt\n";
                    break;
                case 'r':
                    out << "ReadReal\n";
                    break;
                case 's':
                    out << "ReadString\n";
                    break;
                case 'v':
                    out << "Value\n";
                    out << std::string(acc_indent+indent, ' ') << "Identifier: \"" << this->identifier << "\"\n";
                    out << std::string(acc_indent+indent, ' ') << "TailAmount: " << this->tails.size() << "\n";
                    for(int i = 0; i < this->tails.size(); ++i){
                        out << std::string(acc_indent+indent, ' ') << "Tail " << i+1 << ":\n";
                        this->tails[i]->print(out, indent, acc_indent+indent*2);
                    }
                    break;
                case 'l':
                    out << "Literal\n";
                    out << std::string(acc_indent+indent, ' ') << "Literal:\n";
                    this->literal->print(out, indent, acc_indent+indent*2);
                    break;
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class TailNode: public Node {
    private:
        char type;

        long long tuple_idx;

        std::string identifier;

        Node* subscript;

        std::vector<Node*> params;

    public:
        TailNode (std::vector<Token>& tokens, int& y) {

            switch (tokens[y].type) {
                case TokenCode::tkDot:
                    ++y;


                    if (tokens[y].type == TokenCode::tkInt) {
                        this->type = 't';
                        this->tuple_idx = tokens[y].valInt;
                        ++y;
                    } else if (tokens[y].type == TokenCode::tkIdentifier) {
                        this->type = 'i';
                        this->identifier = tokens[y].valStr;
                        ++y;
                    } else {
                         throw std::invalid_argument("Expected either identifier or int after . in tail");
                    }

                    break;
                case TokenCode::tkBracketNormalLeft:
                    this->type = 'p';
                    ++y;

                    while (1) {
                        this->params.push_back(createNode("Expression", tokens, y));


                        if (tokens[y].type == TokenCode::tkComma) {
                            ++y;
                            continue;
                        } else if (tokens[y].type == TokenCode::tkBracketNormalRight) {
                            ++y;
                            break;
                        } else {
                            throw std::invalid_argument("Expected closing bracket or comma after function argument");
                        }
                    }

                    break;
                case TokenCode::tkBracketSquareLeft:
                    this->type = 's';
                    ++y;

                    this->subscript = createNode("Expression", tokens, y);


                    if (tokens[y].type != TokenCode::tkBracketSquareRight) {
                        throw std::invalid_argument("Expected closing bracket");
                    }
                    ++y;

                    break;
                default:
                    throw std::invalid_argument("SOMEHOW TRIED TO CREATE TAIL FROM INVALID STATE");
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            switch (this->type) {
                case 'p':
                    this->subscript->find_nodes(out, q);
                    break;
                case 's':
                    for(auto i : this->params) {
                        i->find_nodes(out, q);
                    }
                    break;
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Tail{\n";
            out << std::string(acc_indent+indent, ' ') << "Type: ";
            switch  (this->type) {
                case 't':
                    out << "TupleIndex\n";
                    out << std::string(acc_indent+indent, ' ') << "Index: " << this->tuple_idx << "\n";
                    break;
                case 'i':
                    out << "Identifier\n";
                    out << std::string(acc_indent+indent, ' ') << "Identifier: \"" << this->identifier << "\"\n";
                    break;
                case 'p':
                    out << "Parameters\n";
                    out << std::string(acc_indent+indent, ' ') << "ParametersAmount: " << this->params.size() << "\n";
                    for(int i = 0; i < this->params.size(); ++i){
                        out << std::string(acc_indent+indent, ' ') << "Parameter " << i+1 << ":\n";
                        this->params[i]->print(out, indent, acc_indent+indent*2);
                    }
                    break;
                case 's':
                    out << "Subscript\n";
                    out << std::string(acc_indent+indent, ' ') << "Expression:\n";
                    this->subscript->print(out, indent, acc_indent+indent*2);
                    break;
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class PrintNode: public Node {
    private:
        std::vector<Node*> values;

    public:
        PrintNode (std::vector<Token>& tokens, int& y) {

            while (1) {
                this->values.push_back(createNode("Expression", tokens, y));


                if (tokens[y].type == TokenCode::tkComma) {
                    ++y;
                    continue;
                } else {
                   break;
                }
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            for(auto i : this->values) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Print{\n";
            out << std::string(acc_indent+indent, ' ') << "ExpressionsAmount: " << this->values.size() << "\n";
            for(int i = 0; i < this->values.size(); ++i){
                out << std::string(acc_indent+indent, ' ') << "Value " << i+1 << ":\n";
                this->values[i]->print(out, indent, acc_indent+indent*2);
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class ReturnNode: public Node {
    private:
        Node* value;

    public:
        ReturnNode (std::vector<Token>& tokens, int& y) {
            this->value = createNode("Expression", tokens, y);
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            this->value->find_nodes(out, q);
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Resturn{\n";
            out << std::string(acc_indent+indent, ' ') << "Expression:\n";
            this->value->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class LiteralNode: public Node {
    private:
        char type;

        long long int_val;
        long double real_val;
        bool bool_val;
        std::string string_val;
        Node* array_val;
        Node* tuple_val;
        Node* func_val;

    public:
        LiteralNode (std::vector<Token>& tokens, int& y) {
            switch (tokens[y].type) {
                case TokenCode::tkInt:
                    this->type = 'i';

                    this->int_val = tokens[y].valInt;
                    ++y;
                    break;
                case TokenCode::tkReal:
                    this->type = 'r';

                    this->real_val = tokens[y].valReal;
                    ++y;
                    break;
                case TokenCode::tkBooleanFalse:
                    this->type = 'b';

                    this->bool_val = false;
                    ++y;
                    break;
                case TokenCode::tkBooleanTrue:
                    this->type = 'b';

                    this->bool_val = true;
                    ++y;
                    break;
                case TokenCode::tkString:
                    this->type = 's';

                    this->string_val = tokens[y].valStr;
                    ++y;
                    break;
                case TokenCode::tkEmpty:
                    this->type = 'e';
                    ++y;
                    break;
                case TokenCode::tkBracketSquareLeft:
                    this->type = 'a';

                    ++y;
                    this->array_val = createNode("ArrayLiteral", tokens, y);
                    break;
                case TokenCode::tkBracketCurvyLeft:
                    this->type = 't';

                    ++y;
                    this->tuple_val = createNode("TupleLiteral", tokens, y);
                    break;
                case TokenCode::tkFunc:
                    this->type = 'f';

                    ++y;
                    this->func_val = createNode("Function", tokens, y);
                    break;
                default:
                    throw std::invalid_argument("SOMEHOW TRIED TO CREATE LITERAL FROM INVALID STATE");
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            switch (this->type) {
                case 'a':
                    this->array_val->find_nodes(out, q);
                    break;
                case 't':
                    this->tuple_val->find_nodes(out, q);
                    break;
                case 'f':
                    this->func_val->find_nodes(out, q);
                    break;
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Literal{\n";
            out << std::string(acc_indent+indent, ' ') << "Type: ";
            switch  (this->type) {
                case 'i':
                    out << "Integer\n";
                    out << std::string(acc_indent+indent, ' ') << "Value: " << this->int_val << "\n";
                    break;
                case 'r':
                    out << "Real\n";
                    out << std::string(acc_indent+indent, ' ') << "Value: " << this->real_val << "\n";
                    break;
                case 's':
                    out << "String\n";
                    out << std::string(acc_indent+indent, ' ') << "Value: \"" << this->string_val << "\"\n";
                    break;
                case 'b':
                    out << "Boolean\n";
                    out << std::string(acc_indent+indent, ' ') << "Value: " << this->bool_val << "\n";
                    break;
                case 'e':
                    out << "Empty\n";
                    break;
                case 'a':
                    out << "Array\n";
                    out << std::string(acc_indent+indent, ' ') << "Value:\n";
                    this->array_val->print(out, indent, acc_indent+indent*2);
                    break;
                case 't':
                    out << "Tuple\n";
                    out << std::string(acc_indent+indent, ' ') << "Value:\n";
                    this->tuple_val->print(out, indent, acc_indent+indent*2);
                    break;
                case 'f':
                    out << "Function\n";
                    out << std::string(acc_indent+indent, ' ') << "Value:\n";
                    this->func_val->print(out, indent, acc_indent+indent*2);
                    break;
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class ArrayLiteralNode: public Node {
    private:
        std::vector<Node*> values;

    public:
        ArrayLiteralNode (std::vector<Token>& tokens, int& y) {

            while (1) {
                this->values.push_back(createNode("Expression", tokens, y));

                if (tokens[y].type == TokenCode::tkComma) {
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkBracketSquareRight) {
                    ++y;
                    break;
                } else {
                    throw std::invalid_argument("Expected closing bracket or comma");
                }
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

           for(auto i : this->values) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "ArrayLiteral{\n";
            out << std::string(acc_indent+indent, ' ') << "EntriesAmount: " << this->values.size() << "\n";
            for(int i = 0; i < this->values.size(); ++i){
                out << std::string(acc_indent+indent, ' ') << "Value " << i+1 << ":\n";
                this->values[i]->print(out, indent, acc_indent+indent*2);
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class TupleLiteralNode: public Node {
    private:
        std::vector<std::string> identifiers;
        std::vector<Node*> values;

    public:
        TupleLiteralNode (std::vector<Token>& tokens, int& y) {

            if (tokens[y].type == TokenCode::tkBracketCurvyRight) {
                return;
            }
            while (1) {
                if (tokens[y].type == TokenCode::tkIdentifier) {
                    if (tokens[y + 1].type == TokenCode::tkAssignment) {
                        this->identifiers.push_back(tokens[y].valStr);
                        y += 2;
                    } else {
                        this->identifiers.push_back("");
                    }
                } else {
                    this->identifiers.push_back("");
                }

                this->values.push_back(createNode("Expression", tokens, y));


                if (tokens[y].type == TokenCode::tkComma) {
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkBracketCurvyRight) {
                    ++y;
                    break;
                } else {
                    throw std::invalid_argument("Expected closing bracket or comma");
                }
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

           for(auto i : this->values) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "TupleLiteral{\n";
            out << std::string(acc_indent+indent, ' ') << "EntriesAmount: " << this->values.size() << "\n";
            for(int i = 0; i < this->values.size(); ++i){
                out << std::string(acc_indent+indent, ' ') << "Identifier " << i+1 << ": ";
                if (this->identifiers[i] != "") {
                    out << "\"" << this->identifiers[i] << "\"\n";
                } else {
                    out << "None\n";
                }
                out << std::string(acc_indent+indent, ' ') << "Value " << i+1 << ":\n";
                this->values[i]->print(out, indent, acc_indent+indent*2);
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class FunctionNode: public Node {
    private:
        char type;
        std::vector<std::string> params;

        Node* body;

        Node* expression;

    public:
        FunctionNode (std::vector<Token>& tokens, int& y) {
            if (tokens[y].type != TokenCode::tkBracketNormalLeft) {
                throw std::invalid_argument("Expected iteration variable name");
            }
            ++y;
            while (1) {
                if (tokens[y].type != TokenCode::tkIdentifier) {
                    throw std::invalid_argument("Expected identifier");
                }
                this->params.push_back(tokens[y].valStr);
                ++y;

                if (tokens[y].type == TokenCode::tkComma) {
                    ++y;
                    continue;
                } else if (tokens[y].type == TokenCode::tkBracketNormalRight) {
                    ++y;
                    break;
                } else {
                    throw std::invalid_argument("Expected closing bracket or comma");
                }
            }

            if (tokens[y].type == TokenCode::tkDelimeterIs) {
                this->type = 'b';
                ++y;
                this->body = createNode("Body", tokens, y);
            } else if (tokens[y].type == TokenCode::tkLambda) {
                this->type = 'l';
                ++y;
                this->expression = createNode("Expression", tokens, y);
            }

        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            switch (this->type) {
                case 'b':
                    this->body->find_nodes(out, q);
                    break;
                case 'l':
                    this->expression->find_nodes(out, q);
                    break;
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Function{\n";
            out << std::string(acc_indent+indent, ' ') << "Type: ";
            switch  (this->type) {
                case 'b':
                    out << "Body\n";
                    out << std::string(acc_indent+indent, ' ') << "ParamAmount: " << this->params.size() << "\n";
                    for(int i = 0; i < this->params.size(); ++i){
                        out << std::string(acc_indent+indent, ' ') << "Parameter " << i+1 << ": \"" << params[i] << "\"\n";
                    }
                    out << std::string(acc_indent+indent, ' ') << "Body:\n";
                    this->body->print(out, indent, acc_indent+indent*2);
                    break;
                case 'l':
                    out << "Lambda\n";
                    out << std::string(acc_indent+indent, ' ') << "ParamAmount: " << this->params.size() << "\n";
                    for(int i = 0; i < this->params.size(); ++i){
                        out << std::string(acc_indent+indent, ' ') << "Parameter " << i+1 << ": \"" << params[i] << "\"\n";
                    }
                    out << std::string(acc_indent+indent, ' ') << "Expression:\n";
                    this->expression->print(out, indent, acc_indent+indent*2);
                    break;
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class AssignmentNode: public Node {
    private:
        Node* primary;
        Node* expression;

    public:
        AssignmentNode (std::vector<Token>& tokens, int& y) {
            this->primary = createNode("Primary", tokens, y);


            if (tokens[y].type != TokenCode::tkAssignment) {
                throw std::invalid_argument("Expected := in supposed assignment");
            }
            ++y;

            this->expression = createNode("Expression", tokens, y);
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            this->primary->find_nodes(out, q);
            this->expression->find_nodes(out, q);
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Assignment{\n";
            out << std::string(acc_indent+indent, ' ') << "Primary:\n";
            this->primary->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent+indent, ' ') << "Expression:\n";
            this->expression->print(out, indent, acc_indent+indent*2);
            out << std::string(acc_indent, ' ') << "}\n";
        }
};

class BodyNode: public Node {
    private:
        std::vector<Node*> statements;
    public:
        BodyNode (std::vector<Token>& tokens, int& y) {
            while (1) {

                while (tokens[y].type == TokenCode::tkLineEnd){
                    ++y;

                }
                if (tokens[y].type == TokenCode::tkDelimeterEnd) {
                    ++y;
                    break;
                } else if (tokens[y].type == TokenCode::tkElse) {
                    break;
                }
                this->statements.push_back(createNode("Statement", tokens, y));
            }
        }

        void find_nodes(std::vector<void*>& out, std::type_info& q) {
            if (typeid(*this) == q) {
                out.push_back(this);
            }

            for(auto i: statements) {
                i->find_nodes(out, q);
            }
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Body{\n";
            for(auto i: statements) {
                i->print(out, indent, acc_indent+indent);
            }
            out << std::string(acc_indent, ' ') << "}\n";
        }
};


Node* createNode(const std::string& t, std::vector<Token>& tokens, int& y) {
    if (t.compare("Program") == 0) {
        return new ProgramNode(tokens, y);
    } else if (t.compare("Statement") == 0) {
        return new StatementNode(tokens, y);
    } else if (t.compare("Declaration") == 0) {
        return new DeclarationNode(tokens, y);
    } else if (t.compare("VariableDefinition") == 0) {
        return new VariableDefinitionNode(tokens, y);
    } else if (t.compare("Expression") == 0) {
        return new ExpressionNode(tokens, y);
    } else if (t.compare("Relation") == 0) {
        return new RelationNode(tokens, y);
    } else if (t.compare("Factor") == 0) {
        return new FactorNode(tokens, y);
    } else if (t.compare("Term") == 0) {
        return new TermNode(tokens, y);
    } else if (t.compare("Unary") == 0) {
        return new UnaryNode(tokens, y);
    } else if (t.compare("Primary") == 0) {
        return new PrimaryNode(tokens, y);
    } else if (t.compare("Tail") == 0) {
        return new TailNode(tokens, y);
    } else if (t.compare("Assignment") == 0) {
        return new AssignmentNode(tokens, y);
    } else if (t.compare("Print") == 0) {
        return new PrintNode(tokens, y);
    } else if (t.compare("Return") == 0) {
        return new ReturnNode(tokens, y);
    } else if (t.compare("If") == 0) {
        return new IfNode(tokens, y);
    } else if (t.compare("For") == 0) {
        return new ForNode(tokens, y);
    } else if (t.compare("While") == 0) {
        return new WhileNode(tokens, y);
    } else if (t.compare("Body") == 0) {
        return new BodyNode(tokens, y);
    } else if (t.compare("TypeIndicator") == 0) {
        return new TypeIndicatorNode(tokens, y);
    } else if (t.compare("Literal") == 0) {
        return new LiteralNode(tokens, y);
    } else if (t.compare("ArrayLiteral") == 0) {
        return new ArrayLiteralNode(tokens, y);
    } else if (t.compare("TupleLiteral") == 0) {
        return new TupleLiteralNode(tokens, y);
    } else if (t.compare("Function") == 0) {
        return new FunctionNode(tokens, y);
    } else if (t.compare("Range") == 0) {
        return new RangeNode(tokens, y);
    } else {
        throw std::invalid_argument("Incorrect type");
    }
}
