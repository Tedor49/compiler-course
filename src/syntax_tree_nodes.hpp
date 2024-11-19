#ifndef __TREE_NODES_INCLUDED__
#define __TREE_NODES_INCLUDED__

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#include "token_data.hpp"

namespace ast_nodes {

    int id_counter = 1;
    bool human_output = false;

    class Node;

    typedef void (*callback_function)(Node* me);

    void dummy (Node* _) {
        return;
    }

    class Node {
        public:
            unsigned int id;
            Node* parent = nullptr;

            virtual Node* from_tokens(std::vector<tokens::Token>& tokens, int& y) = 0;
            virtual void from_config(std::vector<Node*>& nodes, std::string& confstr) = 0;
            virtual void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) = 0;
            virtual void print(std::ostream& out, int indent=4, int acc_indent=0) = 0;
            virtual void machine_print(std::ostream& out) = 0;
    };

    class DeclarationNode;
    class ExpressionNode;
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
    class LiteralNode;
    class ArrayLiteralNode;
    class TupleLiteralNode;
    class FunctionNode;

    Node* createNodeFromTokens(const std::string& t, std::vector<tokens::Token>& tokens, int& y);
    Node* createNodeByName(const std::string& t);
    void assign_parents(Node* tree);
    void reassign_ids(Node* tree);

    std::ostream& operator<<(std::ostream& out, Node* node) {
        reassign_ids(node);
        if (ast_nodes::human_output) node->print(out, 2);
        else node->machine_print(out);
        return out;
    }

    void read_until_delim(std::istream& in, std::pair<std::string, char>& dest) {
        dest.first.clear();
        char cur;
        in.get(cur);
        while (cur != ')' && cur != '|' && cur != '\n') {
            dest.first += cur;
            in.get(cur);
        }
        dest.second = cur;

    }

    class DeclarationNode: public Node {
        public:
        std::string identifier;
        Node* value = nullptr;

        Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
            this->identifier = tokens[y].valStr;
            ++y;

            switch (tokens[y].type) {
                case tokens::TokenCode::tkComma:
                case tokens::TokenCode::tkLineEnd:
                    return this;
                case tokens::TokenCode::tkAssignment:
                    ++y;
                    this->value = createNodeFromTokens("Expression", tokens, y);
                    break;
                default:
                    throw std::invalid_argument("Unexpected value, expected :=");
            }
            return this;
        }

        void from_config(std::vector<Node*>& nodes, std::string& confstr) {
            std::stringstream s(confstr);

            char trash;

            std::pair<std::string, char> read;

            read_until_delim(s, read);
            id = std::stoll(read.first);

            read_until_delim(s, read);
            identifier = read.first;

            read_until_delim(s, read);
            value = nodes[std::stoll(read.first)];
        }

        void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=false) {
            at_enter(this);
            if (this->value) this->value->visit(at_enter, at_repeat, at_exit, visit_body);
            at_exit(this);
        }

        void print(std::ostream& out, int indent=4, int acc_indent=0){
            out << std::string(acc_indent, ' ') << "Declaration{\n";
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

        void machine_print(std::ostream& out){
            out << "Declaration|" << id << "|" << identifier << "|";
            if (value) {
                out << value->id << "\n";
                value->machine_print(out);
            } else {
                out << "0\n";
            }
        }
    };

    class ExpressionNode: public Node {
        public:
            std::vector<Node*> terms;
            std::vector<char> ops;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                bool complete = false;

                while (1) {
                    this->terms.push_back(createNodeFromTokens("Unary", tokens, y));

                    switch (tokens[y].type) {
                        case tokens::TokenCode::tkOperatorMultiply:
                            this->ops.push_back('*');
                            break;
                        case tokens::TokenCode::tkOperatorDivide:
                            this->ops.push_back('/');
                            break;
                        case tokens::TokenCode::tkOperatorPlus:
                            this->ops.push_back('+');
                            break;
                        case tokens::TokenCode::tkOperatorMinus:
                            this->ops.push_back('-');
                            break;
                        case tokens::TokenCode::tkRelationEqual:
                            this->ops.push_back('=');
                            break;
                        case tokens::TokenCode::tkRelationLess:
                            this->ops.push_back('<');
                            break;
                        case tokens::TokenCode::tkRelationLessEq:
                            this->ops.push_back('l');
                            break;
                        case tokens::TokenCode::tkRelationMore:
                            this->ops.push_back('>');
                            break;
                        case tokens::TokenCode::tkRelationMoreEq:
                            this->ops.push_back('m');
                            break;
                        case tokens::TokenCode::tkRelationNotEqual:
                            this->ops.push_back('n');
                            break;
                        case tokens::TokenCode::tkLogicAnd:
                            this->ops.push_back('a');
                            break;
                        case tokens::TokenCode::tkLogicOr:
                            this->ops.push_back('o');
                            break;
                        case tokens::TokenCode::tkLogicXor:
                            this->ops.push_back('x');
                            break;
                        default:
                            complete = true;
                            break;
                    }
                    if (complete) break;
                    ++y;
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                s >> trash;
                read_until_delim(s, read);
                while (read.second != ')') {
                    terms.push_back(nodes[std::stoll(read.first)]);
                    read_until_delim(s, read);
                }
                if (read.first != "") {
                    terms.push_back(nodes[std::stoll(read.first)]);
                }
                s >> trash;

                s >> trash;
                read_until_delim(s, read);
                while (read.second != ')') {
                    ops.push_back(read.first[0]);
                    read_until_delim(s, read);
                }
                if (read.first != "") {
                    ops.push_back(read.first[0]);
                }

            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                bool first = true;
                for(auto i: this->terms) {
                    if (!first) {
                        at_repeat(this);
                    } else {
                        first = false;
                    }
                    i->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
            }

            void print(std::ostream& out, int indent=4, int acc_indent=0){
                out << std::string(acc_indent, ' ') << "Expression{\n";
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

            void machine_print(std::ostream& out){
                out << "Expression|" << id << "|(";
                for(int i = 0; i < terms.size(); ++i) {
                    if (i) out << "|";
                    out << terms[i]->id;
                }
                out << ")|(";
                for(int i = 0; i < ops.size(); ++i) {
                    if (i) out << "|";
                    out << ops[i];
                }
                out << ")\n";

                for(auto i: terms) {
                    i->machine_print(out);
                }
            }
    };

    class UnaryNode: public Node {
        private:
            char parse_type_ind(std::vector<tokens::Token>& tokens, int& y) {
                switch (tokens[y].type) {
                    case tokens::TokenCode::tkTypeInt:
                        ++y;
                        return 'i';
                    case tokens::TokenCode::tkTypeReal:
                        ++y;
                        return 'r';
                    case tokens::TokenCode::tkTypeBool:
                        ++y;
                        return 'b';
                    case tokens::TokenCode::tkTypeString:
                        ++y;
                        return 's';
                    case tokens::TokenCode::tkTypeEmpty:
                        ++y;
                        return 'e';
                    case tokens::TokenCode::tkBracketSquareLeft:
                        if (tokens[y+1].type != tokens::TokenCode::tkBracketSquareRight){
                            throw std::invalid_argument("Expected closing bracket in type indicator");
                        }
                        y += 2;
                        return 'a';
                    case tokens::TokenCode::tkBracketCurvyLeft:
                        if (tokens[y+1].type != tokens::TokenCode::tkBracketCurvyRight){
                            throw std::invalid_argument("Expected closing bracket in type indicator");
                        }
                        y += 2;
                        return 't';
                    case tokens::TokenCode::tkTypeFunc:
                        ++y;
                        return 'f';
                    default:
                        throw std::invalid_argument("Expected type indicator");
                }
            }
        public:
            char unaryop = '#';
            Node* primary;
            char type_ind = '#';

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                switch (tokens[y].type) {
                    case tokens::TokenCode::tkUnaryPlus:
                        this->unaryop = '+';

                        ++y;
                        this->primary = createNodeFromTokens("Primary", tokens, y);


                        if (tokens[y].type == tokens::TokenCode::tkIs) {
                            ++y;
                            this->type_ind = parse_type_ind(tokens, y);
                        }

                        break;
                    case tokens::TokenCode::tkUnaryMinus:
                        this->unaryop = '-';

                        ++y;
                        this->primary = createNodeFromTokens("Primary", tokens, y);


                        if (tokens[y].type == tokens::TokenCode::tkIs) {
                            ++y;
                            this->type_ind = parse_type_ind(tokens, y);
                        }

                        break;
                    case tokens::TokenCode::tkUnaryNot:
                        this->unaryop = 'n';

                        ++y;
                        this->primary = createNodeFromTokens("Primary", tokens, y);


                        if (tokens[y].type == tokens::TokenCode::tkIs) {
                            ++y;
                            this->type_ind = parse_type_ind(tokens, y);
                        }

                        break;
                    case tokens::TokenCode::tkIdentifier:
                    case tokens::TokenCode::tkReadInt:
                    case tokens::TokenCode::tkReadReal:
                    case tokens::TokenCode::tkReadString:
                    case tokens::TokenCode::tkInt:
                    case tokens::TokenCode::tkBooleanFalse:
                    case tokens::TokenCode::tkBooleanTrue:
                    case tokens::TokenCode::tkString:
                    case tokens::TokenCode::tkBracketSquareLeft:
                    case tokens::TokenCode::tkBracketCurvyLeft:
                    case tokens::TokenCode::tkReal:
                    case tokens::TokenCode::tkFunc:
                    case tokens::TokenCode::tkEmpty:
                    case tokens::TokenCode::tkBracketNormalLeft:
                        this->primary = createNodeFromTokens("Primary", tokens, y);


                        if (tokens[y].type == tokens::TokenCode::tkIs) {
                            ++y;
                            this->type_ind = parse_type_ind(tokens, y);
                        }

                        break;
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                unaryop = read.first[0];

                read_until_delim(s, read);
                primary = nodes[std::stoll(read.first)];

                read_until_delim(s, read);
                type_ind = read.first[0];
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);
                this->primary->visit(at_enter, at_repeat, at_exit, visit_body);
                at_exit(this);
            }

            void print(std::ostream& out, int indent=4, int acc_indent=0){
                out << std::string(acc_indent, ' ') << "Unary{\n";
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
                out << std::string(acc_indent+indent, ' ') << "Type indicator: " << type_ind << "\n";
                out << std::string(acc_indent, ' ') << "}\n";
            }

            void machine_print(std::ostream& out){
                out << "Unary|" << id << "|";
                out << this->unaryop << "|" << primary->id << "|" << type_ind << "\n";
                primary->machine_print(out);
            }
    };

    class IfNode: public Node {
        public:
            Node* expression;
            Node* if_body;
            Node* else_body = nullptr;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                this->expression = createNodeFromTokens("Expression", tokens, y);


                if (tokens[y].type != tokens::TokenCode::tkDelimeterThen) {
                    throw std::invalid_argument("Expected if body beginning");
                }
                ++y;

                this->if_body = createNodeFromTokens("Body", tokens, y);

                if (tokens[y].type == tokens::TokenCode::tkElse) {
                    ++y;
                    this->else_body = createNodeFromTokens("Body", tokens, y);
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                expression = nodes[std::stoll(read.first)];

                read_until_delim(s, read);
                if_body = nodes[std::stoll(read.first)];

                read_until_delim(s, read);
                else_body = nodes[std::stoll(read.first)];
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                this->expression->visit(at_enter, at_repeat, at_exit, visit_body);
                if (visit_body) {
                    at_repeat(this);
                    this->if_body->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                if (visit_body && this->else_body) {
                    at_repeat(this);
                    this->else_body->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
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

            void machine_print(std::ostream& out){
                out << "If|" << id << "|" << expression->id << "|" << if_body->id << "|";
                if (else_body) {
                    out << else_body->id << "\n";
                    expression->machine_print(out);
                    if_body->machine_print(out);
                    else_body->machine_print(out);
                } else {
                    out << "0\n";
                    expression->machine_print(out);
                    if_body->machine_print(out);
                }
            }
    };

    class ForNode: public Node {
        public:
            std::string identifier;
            Node* range_expr_l;
            Node* range_expr_r;
            Node* body;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                if (tokens[y].type != tokens::TokenCode::tkIdentifier) {
                    throw std::invalid_argument("Expected iteration variable name");
                }
                this->identifier = tokens[y].valStr;
                ++y;


                if (tokens[y].type != tokens::TokenCode::tkIn) {
                    throw std::invalid_argument("Expected range after for loop declaration");
                }
                ++y;

                this->range_expr_l = createNodeFromTokens("Expression", tokens, y);


                if (tokens[y].type != tokens::TokenCode::tkDoubleDot) {
                    throw std::invalid_argument("Expected .. in range definition");
                }
                ++y;

                this->range_expr_r = createNodeFromTokens("Expression", tokens, y);


                if (tokens[y].type != tokens::TokenCode::tkDelimeterLoop) {
                    throw std::invalid_argument("Expected loop body after range");
                }
                ++y;

                this->body = createNodeFromTokens("Body", tokens, y);


                if (tokens[y].type != tokens::TokenCode::tkLineEnd) {
                    throw std::invalid_argument("Expected line end or ; at the end of loop");
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                identifier = read.first;

                read_until_delim(s, read);
                range_expr_l = nodes[std::stoll(read.first)];

                read_until_delim(s, read);
                range_expr_r = nodes[std::stoll(read.first)];

                read_until_delim(s, read);
                body = nodes[std::stoll(read.first)];
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                this->range_expr_l->visit(at_enter, at_repeat, at_exit, visit_body);
                at_repeat(this);
                this->range_expr_r->visit(at_enter, at_repeat, at_exit, visit_body);

                if (visit_body) {
                    at_repeat(this);
                    this->body->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
            }

            void print(std::ostream& out, int indent=4, int acc_indent=0){
                out << std::string(acc_indent, ' ') << "For{\n";
                out << std::string(acc_indent+indent, ' ') << "Identifier: \"" << this->identifier << "\"\n";
                out << std::string(acc_indent+indent, ' ') << "Range_L:\n";
                this->range_expr_l->print(out, indent, acc_indent+indent*2);
                out << std::string(acc_indent+indent, ' ') << "Range_R:\n";
                this->range_expr_r->print(out, indent, acc_indent+indent*2);
                out << std::string(acc_indent+indent, ' ') << "Body:\n";
                this->body->print(out, indent, acc_indent+indent*2);
                out << std::string(acc_indent, ' ') << "}\n";
            }

            void machine_print(std::ostream& out){
                out << "For|" << id << "|" << identifier << "|" << range_expr_l->id << "|" << range_expr_r->id << "|" << body->id << "\n";
                range_expr_l->machine_print(out);
                range_expr_r->machine_print(out);
                body->machine_print(out);
            }
    };

    class WhileNode: public Node {
        public:
            Node* expression;
            Node* body;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                this->expression = createNodeFromTokens("Expression", tokens, y);


                if (tokens[y].type != tokens::TokenCode::tkDelimeterLoop) {
                    throw std::invalid_argument("Expected loop body after range");
                }
                ++y;

                this->body = createNodeFromTokens("Body", tokens, y);


                if (tokens[y].type != tokens::TokenCode::tkLineEnd) {
                    throw std::invalid_argument("Expected line end or ; at the end of loop");
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                expression = nodes[std::stoll(read.first)];

                read_until_delim(s, read);
                body = nodes[std::stoll(read.first)];
            }


            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                this->expression->visit(at_enter, at_repeat, at_exit, visit_body);
                if (visit_body) {
                    at_repeat(this);
                    this->body->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
            }

            void print(std::ostream& out, int indent=4, int acc_indent=0){
                out << std::string(acc_indent, ' ') << "While{\n";
                out << std::string(acc_indent+indent, ' ') << "Expression:\n";
                this->expression->print(out, indent, acc_indent+indent*2);
                out << std::string(acc_indent+indent, ' ') << "Body:\n";
                this->body->print(out, indent, acc_indent+indent*2);
                out << std::string(acc_indent, ' ') << "}\n";
            }

            void machine_print(std::ostream& out){
                out << "While|" << id << "|" << expression->id << "|" << body->id << "\n";
                expression->machine_print(out);
                body->machine_print(out);
            }
    };

    class PrimaryNode: public Node {
        public:
            char type;

            Node* literal;

            Node* expression;

            std::string identifier;
            std::vector<Node*> tails;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                switch (tokens[y].type) {
                    case tokens::TokenCode::tkReadInt:
                        this->type = 'i';
                        ++y;
                        break;
                    case tokens::TokenCode::tkReadReal:
                        this->type = 'r';
                        ++y;
                        break;
                    case tokens::TokenCode::tkReadString:
                        this->type = 's';
                        ++y;
                        break;
                    case tokens::TokenCode::tkInt:
                    case tokens::TokenCode::tkBooleanFalse:
                    case tokens::TokenCode::tkBooleanTrue:
                    case tokens::TokenCode::tkString:
                    case tokens::TokenCode::tkBracketSquareLeft:
                    case tokens::TokenCode::tkBracketCurvyLeft:
                    case tokens::TokenCode::tkReal:
                    case tokens::TokenCode::tkFunc:
                    case tokens::TokenCode::tkEmpty:
                        this->type = 'l';
                        this->literal = createNodeFromTokens("Literal", tokens, y);
                        break;
                    case tokens::TokenCode::tkIdentifier:
                        this->type = 'v';
                        this->identifier = tokens[y].valStr;
                        ++y;

                        while (1) {

                            if(tokens[y].type == tokens::TokenCode::tkDot ||
                               tokens[y].type == tokens::TokenCode::tkBracketSquareLeft ||
                               tokens[y].type == tokens::TokenCode::tkBracketNormalLeft) {
                                this->tails.push_back(createNodeFromTokens("Tail", tokens, y));
                            } else {
                                break;
                            }
                        }
                        break;
                    case tokens::TokenCode::tkBracketNormalLeft:
                        this->type = 'e';
                        ++y;

                        this->expression = createNodeFromTokens("Expression", tokens, y);

                        if (tokens[y].type != tokens::TokenCode::tkBracketNormalRight){
                            throw std::invalid_argument("Expected closing bracket after expression");
                        }
                        ++y;

                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                type = read.first[0];

                switch (type) {
                    case 'i':
                    case 'r':
                    case 's':
                        break;
                    case 'v':
                        read_until_delim(s, read);
                        identifier = read.first;

                        s >> trash;
                        read_until_delim(s, read);
                        while (read.second != ')') {
                            tails.push_back(nodes[std::stoll(read.first)]);
                            read_until_delim(s, read);
                        }
                        if (read.first != "") {
                            tails.push_back(nodes[std::stoll(read.first)]);
                        }

                        break;
                    case 'l':
                        read_until_delim(s, read);
                        literal = nodes[std::stoll(read.first)];

                        break;
                    case 'e':
                        read_until_delim(s, read);
                        expression = nodes[std::stoll(read.first)];

                        break;
                    default:
                        throw std::invalid_argument("Expected valid type");
                }
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                if (this->type == 'v') {
                    bool first = true;
                    for(auto i : this->tails) {
                        if (!first) {
                            at_repeat(this);
                        } else {
                            first = false;
                        }
                        i->visit(at_enter, at_repeat, at_exit, visit_body);
                    }
                } else if (this->type == 'l') {
                    this->literal->visit(at_enter, at_repeat, at_exit, visit_body);
                } else if (this->type == 'e') {
                    this->expression->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
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
                    case 'e':
                        out << "Expression\n";
                        out << std::string(acc_indent+indent, ' ') << "Expression:\n";
                        this->expression->print(out, indent, acc_indent+indent*2);
                        break;
                }
                out << std::string(acc_indent, ' ') << "}\n";
            }

            void machine_print(std::ostream& out){
                out << "Primary|" << id << "|" << type;
                switch  (this->type) {
                    case 'i':
                    case 'r':
                    case 's':
                        out << "\n";
                        break;
                    case 'v':
                        out << "|" << identifier << "|(";

                        for(int i = 0; i < tails.size(); ++i) {
                            if (i) out << "|";
                            out << tails[i]->id;
                        }
                        out << ")\n";

                        for(auto i: tails) {
                            i->machine_print(out);
                        }
                        break;
                    case 'l':
                        out << "|" << literal->id << "\n";
                        literal->machine_print(out);
                        break;
                    case 'e':
                        out << "|" << expression->id << "\n";
                        expression->machine_print(out);
                        break;
                }
            }
    };

    class TailNode: public Node {
        public:
            char type;

            long long tuple_idx;

            std::string identifier;

            Node* subscript;

            std::vector<Node*> params;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                switch (tokens[y].type) {
                    case tokens::TokenCode::tkDot:
                        ++y;


                        if (tokens[y].type == tokens::TokenCode::tkInt) {
                            this->type = 't';
                            this->tuple_idx = tokens[y].valInt;
                            ++y;
                        } else if (tokens[y].type == tokens::TokenCode::tkIdentifier) {
                            this->type = 'i';
                            this->identifier = tokens[y].valStr;
                            ++y;
                        } else {
                             throw std::invalid_argument("Expected either identifier or int after . in tail");
                        }

                        break;
                    case tokens::TokenCode::tkBracketNormalLeft:
                        this->type = 'p';
                        ++y;

                        while (1) {
                            this->params.push_back(createNodeFromTokens("Expression", tokens, y));

                            if (tokens[y].type == tokens::TokenCode::tkComma) {
                                ++y;
                                continue;
                            } else if (tokens[y].type == tokens::TokenCode::tkBracketNormalRight) {
                                ++y;
                                break;
                            } else {
                                throw std::invalid_argument("Expected closing bracket or comma after function argument");
                            }
                        }

                        break;
                    case tokens::TokenCode::tkBracketSquareLeft:
                        this->type = 's';
                        ++y;

                        this->subscript = createNodeFromTokens("Expression", tokens, y);


                        if (tokens[y].type != tokens::TokenCode::tkBracketSquareRight) {
                            throw std::invalid_argument("Expected closing bracket");
                        }
                        ++y;

                        break;
                    default:
                        throw std::invalid_argument("SOMEHOW TRIED TO CREATE TAIL FROM INVALID STATE");
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                type = read.first[0];

                switch (type) {
                    case 't':
                        read_until_delim(s, read);
                        tuple_idx = std::stoll(read.first);
                        break;
                    case 'i':
                        read_until_delim(s, read);
                        identifier = read.first;
                        break;
                    case 'p':
                        s >> trash;
                        read_until_delim(s, read);
                        while (read.second != ')') {
                            params.push_back(nodes[std::stoll(read.first)]);
                            read_until_delim(s, read);
                        }
                        if (read.first != "") {
                            params.push_back(nodes[std::stoll(read.first)]);
                        }
                        break;
                    case 's':
                        read_until_delim(s, read);
                        //std::cout << read.first << std::endl;
                        subscript = nodes[std::stoll(read.first)];
                        break;
                    default:
                        throw std::invalid_argument("Expected valid type");
                }
            }


            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                bool first = true;
                switch (this->type) {
                    case 's':
                        //std::cout << "MY ID: " << this->id << std::endl;
                        //std::cout << "SUBSCRIPT: " << reinterpret_cast<std::uintptr_t>(subscript) << std::endl;
                        //std::cout << "SUB IS NULL: " << (subscript == nullptr) << std::endl;
                        //std::cout << typeid(*subscript).name() << std::endl;
                        this->subscript->visit(at_enter, at_repeat, at_exit, visit_body);

                        break;
                    case 'p':
                        for(auto i : this->params) {
                            if (!first) {
                                at_repeat(this);
                            } else {
                                first = false;
                            }
                            i->visit(at_enter, at_repeat, at_exit, visit_body);
                        }
                        break;
                }
                at_exit(this);
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

            void machine_print(std::ostream& out){
                out << "Tail|" << id << "|" << type << "|";
                switch  (this->type) {
                    case 't':
                        out << tuple_idx << "\n";
                        break;
                    case 'i':
                        out << identifier << "\n";
                        break;
                    case 'p':
                        out << "(";
                        for(int i = 0; i < params.size(); ++i) {
                            if (i) out << "|";
                            out << params[i]->id;
                        }
                        out << ")\n";

                        for(auto i: params) {
                            i->machine_print(out);
                        }
                        break;
                    case 's':
                        out << subscript->id << "\n";
                        subscript->machine_print(out);
                        break;
                }
            }
    };

    class PrintNode: public Node {
        public:
            std::vector<Node*> values;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                while (1) {
                    this->values.push_back(createNodeFromTokens("Expression", tokens, y));


                    if (tokens[y].type == tokens::TokenCode::tkComma) {
                        ++y;
                        continue;
                    } else {
                       break;
                    }
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                s >> trash;
                read_until_delim(s, read);
                while (read.second != ')') {
                    values.push_back(nodes[std::stoll(read.first)]);
                    read_until_delim(s, read);
                }
                if (read.first != "") {
                    values.push_back(nodes[std::stoll(read.first)]);
                }
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                bool first = true;
                for(auto i : this->values) {
                    if (!first) {
                        at_repeat(this);
                    } else {
                        first = false;
                    }
                    i->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
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

            void machine_print(std::ostream& out){
                out << "Print|" << id << "|(";
                for(int i = 0; i < values.size(); ++i) {
                    if (i) out << "|";
                    out << values[i]->id;
                }
                out << ")\n";

                for(auto i: values) {
                    i->machine_print(out);
                }
            }
    };

    class ReturnNode: public Node {
        public:
            Node* value;

            ReturnNode() {
                this->id = id_counter;
                ++id_counter;
            }

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                this->value = createNodeFromTokens("Expression", tokens, y);
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                value = nodes[std::stoll(read.first)];
            }



            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                this->value->visit(at_enter, at_repeat, at_exit, visit_body);
                at_exit(this);
            }

            void print(std::ostream& out, int indent=4, int acc_indent=0){
                out << std::string(acc_indent, ' ') << "Resturn{\n";
                out << std::string(acc_indent+indent, ' ') << "Expression:\n";
                this->value->print(out, indent, acc_indent+indent*2);
                out << std::string(acc_indent, ' ') << "}\n";
            }

            void machine_print(std::ostream& out){
                out << "Return|" << id << "|" << value->id << "\n";
                value->machine_print(out);
            }
    };

    class LiteralNode: public Node {
        public:
            char type;

            long long int_val;
            long double real_val;
            bool bool_val;
            std::string string_val;
            Node* array_val;
            Node* tuple_val;
            Node* func_val;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                switch (tokens[y].type) {
                    case tokens::TokenCode::tkInt:
                        this->type = 'i';

                        this->int_val = tokens[y].valInt;
                        ++y;
                        break;
                    case tokens::TokenCode::tkReal:
                        this->type = 'r';

                        this->real_val = tokens[y].valReal;
                        ++y;
                        break;
                    case tokens::TokenCode::tkBooleanFalse:
                        this->type = 'b';

                        this->bool_val = false;
                        ++y;
                        break;
                    case tokens::TokenCode::tkBooleanTrue:
                        this->type = 'b';

                        this->bool_val = true;
                        ++y;
                        break;
                    case tokens::TokenCode::tkString:
                        this->type = 's';

                        this->string_val = tokens[y].valStr;
                        ++y;
                        break;
                    case tokens::TokenCode::tkEmpty:
                        this->type = 'e';
                        ++y;
                        break;
                    case tokens::TokenCode::tkBracketSquareLeft:
                        this->type = 'a';

                        ++y;
                        this->array_val = createNodeFromTokens("ArrayLiteral", tokens, y);
                        break;
                    case tokens::TokenCode::tkBracketCurvyLeft:
                        this->type = 't';

                        ++y;
                        this->tuple_val = createNodeFromTokens("TupleLiteral", tokens, y);
                        break;
                    case tokens::TokenCode::tkFunc:
                        this->type = 'f';

                        ++y;
                        this->func_val = createNodeFromTokens("Function", tokens, y);
                        break;
                    default:
                        throw std::invalid_argument("SOMEHOW TRIED TO CREATE LITERAL FROM INVALID STATE");
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                type = read.first[0];

                switch (type) {
                    case 'i':
                        read_until_delim(s, read);
                        int_val = std::stoll(read.first);
                        break;
                    case 'r':
                        read_until_delim(s, read);
                        real_val = std::stold(read.first);
                        break;
                    case 'b':
                        read_until_delim(s, read);
                        bool_val = std::stoi(read.first);
                        break;
                    case 's':
                        std::getline(s, string_val);
                        break;
                    case 'e':
                        break;
                    case 'a':
                        read_until_delim(s, read);
                        array_val = nodes[std::stoll(read.first)];
                        break;
                    case 't':
                        read_until_delim(s, read);
                        tuple_val = nodes[std::stoll(read.first)];
                        break;
                    case 'f':
                        read_until_delim(s, read);
                        func_val = nodes[std::stoll(read.first)];
                        break;
                    default:
                        throw std::invalid_argument("Expected valid type");
                }
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                switch (this->type) {
                    case 'a':
                        this->array_val->visit(at_enter, at_repeat, at_exit, visit_body);
                        break;
                    case 't':
                        this->tuple_val->visit(at_enter, at_repeat, at_exit, visit_body);
                        break;
                    case 'f':
                        this->func_val->visit(at_enter, at_repeat, at_exit, visit_body);
                        break;
                }
                at_exit(this);
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

            void machine_print(std::ostream& out){
                out << "Literal|" << id << "|" << type;
                switch  (this->type) {
                    case 'i':
                        out << "|" << int_val << "\n";
                        break;
                    case 'r':
                        out << "|" << real_val << "\n";
                        break;
                    case 's':
                        out << "|" << string_val << "\n";
                        break;
                    case 'b':
                        out << "|" << bool_val << "\n";
                        break;
                    case 'e':
                        out << "\n";
                        break;
                    case 'a':
                        out << "|" << array_val->id << "\n";
                        array_val->machine_print(out);
                        break;
                    case 't':
                        out << "|" << tuple_val->id << "\n";
                        tuple_val->machine_print(out);
                        break;
                    case 'f':
                        out << "|" << func_val->id << "\n";
                        func_val->machine_print(out);
                        break;
                }
            }
    };

    class ArrayLiteralNode: public Node {
        public:
            std::vector<Node*> values;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                if (tokens[y].type == tokens::TokenCode::tkBracketSquareRight) {
                    ++y;
                    return this;
                }
                while (1) {
                    this->values.push_back(createNodeFromTokens("Expression", tokens, y));

                    if (tokens[y].type == tokens::TokenCode::tkComma) {
                        ++y;
                        continue;
                    } else if (tokens[y].type == tokens::TokenCode::tkBracketSquareRight) {
                        ++y;
                        break;
                    } else {
                        throw std::invalid_argument("Expected closing bracket or comma");
                    }
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                s >> trash;
                read_until_delim(s, read);
                while (read.second != ')') {
                    values.push_back(nodes[std::stoll(read.first)]);
                    read_until_delim(s, read);
                }
                if (read.first != "") {
                    values.push_back(nodes[std::stoll(read.first)]);
                }
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                bool first = true;
                for(auto i : this->values) {
                    if (!first) {
                        at_repeat(this);
                    } else {
                        first = false;
                    }
                    i->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
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

            void machine_print(std::ostream& out){
                out << "ArrayLiteral|" << id << "|(";
                for(int i = 0; i < values.size(); ++i) {
                    if (i) out << "|";
                    out << values[i]->id;
                }
                out << ")\n";

                for(auto i: values) {
                    i->machine_print(out);
                }
            }
    };

    class TupleLiteralNode: public Node {
        public:
            std::vector<std::string> identifiers;
            std::vector<Node*> values;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                if (tokens[y].type == tokens::TokenCode::tkBracketCurvyRight) {
                    return this;
                }
                while (1) {
                    if (tokens[y].type == tokens::TokenCode::tkIdentifier) {
                        if (tokens[y + 1].type == tokens::TokenCode::tkAssignment) {
                            this->identifiers.push_back(tokens[y].valStr);
                            y += 2;
                        } else {
                            this->identifiers.push_back("");
                        }
                    } else {
                        this->identifiers.push_back("");
                    }

                    this->values.push_back(createNodeFromTokens("Expression", tokens, y));


                    if (tokens[y].type == tokens::TokenCode::tkComma) {
                        ++y;
                        continue;
                    } else if (tokens[y].type == tokens::TokenCode::tkBracketCurvyRight) {
                        ++y;
                        break;
                    } else {
                        throw std::invalid_argument("Expected closing bracket or comma");
                    }
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                s >> trash;
                read_until_delim(s, read);
                while (read.second != ')') {
                    identifiers.push_back(read.first);
                    read_until_delim(s, read);
                }
                if (read.first != "") {
                    identifiers.push_back(read.first);
                }
                s >> trash;

                s >> trash;
                read_until_delim(s, read);
                while (read.second != ')') {
                    values.push_back(nodes[std::stoll(read.first)]);
                    read_until_delim(s, read);
                }
                if (read.first != "") {
                    values.push_back(nodes[std::stoll(read.first)]);
                }
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                bool first = true;
                for(auto i : this->values) {
                    if (!first) {
                        at_repeat(this);
                    } else {
                        first = false;
                    }
                    i->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
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

            void machine_print(std::ostream& out){
                out << "TupleLiteral|" << id << "|(";
                for(int i = 0; i < identifiers.size(); ++i) {
                    if (i) out << "|";
                    out << identifiers[i];
                }
                out << ")|(";
                for(int i = 0; i < values.size(); ++i) {
                    if (i) out << "|";
                    out << values[i]->id;
                }
                out << ")\n";

                for(auto i: values) {
                    i->machine_print(out);
                }
            }
    };

    class FunctionNode: public Node {
        public:
            char type;
            std::vector<std::string> params;

            Node* body;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                if (tokens[y].type != tokens::TokenCode::tkBracketNormalLeft) {
                    throw std::invalid_argument("Expected iteration variable name");
                }
                ++y;
                while (1) {
                    if (tokens[y].type != tokens::TokenCode::tkIdentifier) {
                        throw std::invalid_argument("Expected identifier");
                    }
                    this->params.push_back(tokens[y].valStr);
                    ++y;

                    if (tokens[y].type == tokens::TokenCode::tkComma) {
                        ++y;
                        continue;
                    } else if (tokens[y].type == tokens::TokenCode::tkBracketNormalRight) {
                        ++y;
                        break;
                    } else {
                        throw std::invalid_argument("Expected closing bracket or comma");
                    }
                }

                if (tokens[y].type == tokens::TokenCode::tkDelimeterIs) {
                    this->type = 'b';
                    ++y;
                    this->body = createNodeFromTokens("Body", tokens, y);
                } else if (tokens[y].type == tokens::TokenCode::tkLambda) {
                    this->type = 'l';
                    ++y;
                    this->body = createNodeFromTokens("Expression", tokens, y);
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                type = read.first[0];

                s >> trash;
                read_until_delim(s, read);
                while (read.second != ')') {
                    params.push_back(read.first);
                    read_until_delim(s, read);
                }
                if (read.first != "") {
                    params.push_back(read.first);
                }
                s >> trash;
				
				if (type != 'b' && type != 'l') {
					throw std::invalid_argument("Expected valid type");
				}
				
				read_until_delim(s, read);
                body = nodes[std::stoll(read.first)];
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);
                if (visit_body) {
                    this->body->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
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
                        this->body->print(out, indent, acc_indent+indent*2);
                        break;
                }
                out << std::string(acc_indent, ' ') << "}\n";
            }

            void machine_print(std::ostream& out){
                out << "Function|" << id << "|" << type << "|(";
                for(int i = 0; i < params.size(); ++i) {
                    if (i) out << "|";
                    out << params[i];
                }
                out << ")|";
                out << body->id << "\n";
                body->machine_print(out);
            }
    };

    class AssignmentNode: public Node {
        public:
            Node* primary;
            Node* expression;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                this->primary = createNodeFromTokens("Primary", tokens, y);


                if (tokens[y].type != tokens::TokenCode::tkAssignment) {
                    throw std::invalid_argument("Expected := in supposed assignment");
                }
                ++y;

                this->expression = createNodeFromTokens("Expression", tokens, y);
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                read_until_delim(s, read);
                primary = nodes[std::stoll(read.first)];

                read_until_delim(s, read);
                expression = nodes[std::stoll(read.first)];
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                this->primary->visit(at_enter, at_repeat, at_exit, visit_body);
                at_repeat(this);
                this->expression->visit(at_enter, at_repeat, at_exit, visit_body);
                at_exit(this);
            }

            void print(std::ostream& out, int indent=4, int acc_indent=0){
                out << std::string(acc_indent, ' ') << "Assignment{\n";
                out << std::string(acc_indent+indent, ' ') << "Primary:\n";
                this->primary->print(out, indent, acc_indent+indent*2);
                out << std::string(acc_indent+indent, ' ') << "Expression:\n";
                this->expression->print(out, indent, acc_indent+indent*2);
                out << std::string(acc_indent, ' ') << "}\n";
            }

            void machine_print(std::ostream& out){
                out << "Assignment|" << id << "|" << primary->id << "|" << expression->id << "\n";
                primary->machine_print(out);
                expression->machine_print(out);
            }
    };

    class BodyNode: public Node {
        public:
            std::vector<Node*> statements;

            Node* from_tokens(std::vector<tokens::Token>& tokens, int& y){
                while (tokens.size() != y) {
                    while (tokens[y].type == tokens::TokenCode::tkLineEnd){
                        ++y;
                        if (tokens.size() == y) break;
                    }
                    if (tokens.size() == y) break;

                    if (tokens[y].type == tokens::TokenCode::tkDelimeterEnd) {
                        ++y;
                        break;
                    } else if (tokens[y].type == tokens::TokenCode::tkElse) {
                        break;
                    }

                    switch (tokens[y].type) {
                        case tokens::TokenCode::tkVar:
							do {
                                ++y;
                                statements.push_back(createNodeFromTokens("Declaration", tokens, y));
                            } while (tokens[y].type == tokens::TokenCode::tkComma);
                            break;
                        case tokens::TokenCode::tkIdentifier:
                            statements.push_back(createNodeFromTokens("Assignment", tokens, y));
                            break;
                        case tokens::TokenCode::tkIf:
                            ++y;
                            statements.push_back(createNodeFromTokens("If", tokens, y));
                            break;
                        case tokens::TokenCode::tkFor:
                            ++y;
                            statements.push_back(createNodeFromTokens("For", tokens, y));
                            break;
                        case tokens::TokenCode::tkWhile:
                            ++y;
                            statements.push_back(createNodeFromTokens("While", tokens, y));
                            break;
                        case tokens::TokenCode::tkReturn:
                            ++y;
                            statements.push_back(createNodeFromTokens("Return", tokens, y));
                            break;
                        case tokens::TokenCode::tkPrint:
                            ++y;
                            statements.push_back(createNodeFromTokens("Print", tokens, y));
                            break;
                        default:
                            throw std::invalid_argument("Not a statement");
                    }
                }
                return this;
            }

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

                s >> trash;
                read_until_delim(s, read);
                while (read.second != ')') {
                    statements.push_back(nodes[std::stoll(read.first)]);
                    read_until_delim(s, read);
                }
                if (read.first != "") {
                    statements.push_back(nodes[std::stoll(read.first)]);
                }
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                bool first = true;
                for(auto i: statements) {
                    if (!first) {
                        at_repeat(this);
                    } else {
                        first = false;
                    }
                    i->visit(at_enter, at_repeat, at_exit, visit_body);
                }
                at_exit(this);
            }

            void print(std::ostream& out, int indent=4, int acc_indent=0){
                out << std::string(acc_indent, ' ') << "Body{\n";
                for(auto i: statements) {
                    i->print(out, indent, acc_indent+indent);
                }
                out << std::string(acc_indent, ' ') << "}\n";
            }

            void machine_print(std::ostream& out){
                out << "Body|" << id << "|(";
                for(int i = 0; i < statements.size(); ++i) {
                    if (i) out << "|";
                    out << statements[i]->id;
                }
                out << ")\n";

                for(auto i: statements) {
                    i->machine_print(out);
                }

                if (this->parent == nullptr) {
                    out << "END|-1\n";
                }
            }
    };

    Node* createNodeByName(const std::string& t) {
        if (t.compare("Declaration") == 0) {
            return (new DeclarationNode());
        } else if (t.compare("Expression") == 0) {
            return (new ExpressionNode());
        } else if (t.compare("Unary") == 0) {
            return (new UnaryNode());
        } else if (t.compare("Primary") == 0) {
            return (new PrimaryNode());
        } else if (t.compare("Tail") == 0) {
            return (new TailNode());
        } else if (t.compare("Assignment") == 0) {
            return (new AssignmentNode());
        } else if (t.compare("Print") == 0) {
            return (new PrintNode());
        } else if (t.compare("Return") == 0) {
            return (new ReturnNode());
        } else if (t.compare("If") == 0) {
            return (new IfNode());
        } else if (t.compare("For") == 0) {
            return (new ForNode());
        } else if (t.compare("While") == 0) {
            return (new WhileNode());
        } else if (t.compare("Body") == 0) {
            return (new BodyNode());
        } else if (t.compare("Literal") == 0) {
            return (new LiteralNode());
        } else if (t.compare("ArrayLiteral") == 0) {
            return (new ArrayLiteralNode());
        } else if (t.compare("TupleLiteral") == 0) {
            return (new TupleLiteralNode());
        } else if (t.compare("Function") == 0) {
            return (new FunctionNode());
        } else {
            std::cout << t << std::endl;
            throw std::invalid_argument("Incorrect type");
        }
    }

    Node* createNodeFromTokens(const std::string& t, std::vector<tokens::Token>& tokens, int& y) {
        return createNodeByName(t)->from_tokens(tokens, y);
    }

    Node* readTree(std::istream& in) {
        id_counter = 1;
        std::vector<std::pair<std::string, std::string>> configs;
        std::vector<Node*> nodes;

        nodes.push_back(nullptr);

        configs.push_back({"", ""});

        std::string s;
        std::getline(in, s, '|');
        while (s != "END") {
            configs.push_back({s, ""});
            std::getline(in, configs.back().second);
            configs.back().second += '\n';
            nodes.push_back(createNodeByName(configs.back().first));
            std::getline(in, s, '|');
        }

        //for (int i = 1; i < nodes.size(); ++i) {
        //    std::cout << i << ": " << typeid(*nodes[i]).name() << std::endl;
        //}

        std::getline(in, s, '\n');

        for (int i = 1; i < nodes.size(); ++i) {
            //std::cout << "MAKING NODE:\t" << i << std::endl;
            //std::cout << "OF TYPE:\t" << configs[i].first << std::endl;
            //std::cout << "FROM CONFSTR:\t" << configs[i].second << std::endl;
            nodes[i]->from_config(nodes, configs[i].second);
        }

        assign_parents(nodes[1]);
        reassign_ids(nodes[1]);

        return nodes[1];
    }

    void assign_id(Node* self) {
        self->id = id_counter++;
    }

    void reassign_ids(Node* tree) {
        id_counter = 1;
        tree->visit(assign_id, dummy, dummy);
    }


    Node* last_node_ptr = nullptr;

    void assign_parent(Node* self) {
        self->parent = last_node_ptr;
        last_node_ptr = self;
    }

    void update_last(Node* self) {
        last_node_ptr = self;
    }

    void assign_parents(Node* tree) {
        last_node_ptr = nullptr;
        tree->visit(assign_parent, update_last, dummy);
    }

    Node* construct(std::vector<tokens::Token>& tokenized){
        int walker = 0;

        try {
            ast_nodes::BodyNode* tree = new BodyNode();
            tree->from_tokens(tokenized, walker);
            assign_parents(tree);
            reassign_ids(tree);
            return tree;
        } catch (std::invalid_argument& ex) {
            throw std::invalid_argument(std::format("{} at line {}, pos {}", ex.what(), tokenized[walker].line, tokenized[walker].pos));
        } catch (std::out_of_range& ex) {
            throw std::invalid_argument("Unexpected program end");
        }
    }
}

#endif // __TREE_NODES_INCLUDED__
