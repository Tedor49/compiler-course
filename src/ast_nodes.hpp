#ifndef __TREE_NODES_INCLUDED__
#define __TREE_NODES_INCLUDED__

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

# include "token_data.hpp"

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
			
			int line = 0;
			int pos = 0;
			
			virtual Node* from_tokens(std::vector<tokens::Token>& tokens, int& y) = 0;
			virtual void execute(std::istream& in, std::ostream& out) = 0;
			
            virtual void from_config(std::vector<Node*>& nodes, std::string& confstr) = 0;
            virtual void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) = 0;
            virtual void machine_print(std::ostream& out) = 0;
    };

    class DeclarationNode;
    class ExpressionNode;
    class UnaryNode;
    class PrimaryNode;
    class TailNode;
    class AssignmentNode;
    class PrintNode;
    class ControlNode;
    class IfNode;
    class ForNode;
    class WhileNode;
    class BodyNode;
    class LiteralNode;
    class ArrayLiteralNode;
    class TupleLiteralNode;
    class FunctionNode;

    Node* createNodeByName(const std::string& t);
    void assign_parents(Node* tree);
    void reassign_ids(Node* tree);

    std::ostream& operator<<(std::ostream& out, Node* node) {
        reassign_ids(node);
        node->machine_print(out);
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
			void from_config(std::vector<Node*>& nodes, std::string& confstr) {
				std::stringstream s(confstr);

				char trash;

				std::pair<std::string, char> read;

				read_until_delim(s, read);
				id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

			void machine_print(std::ostream& out){
				out << "Declaration|" << id << "|" << line << "|"  << pos << "|" << identifier << "|";
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "Expression|" << id << "|" << line << "|"  << pos << "|(";
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
			char parse_type_ind(std::vector<tokens::Token>& tokens, int& y);
        public:
            char unaryop = '#';
            Node* primary;
            char type_ind = '#';
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "Unary|" << id << "|" << line << "|"  << pos << "|";
                out << this->unaryop << "|" << primary->id << "|" << type_ind << "\n";
                primary->machine_print(out);
            }
    };

    class IfNode: public Node {
        public:
            Node* expression;
            Node* if_body;
            Node* else_body = nullptr;
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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
			
            void machine_print(std::ostream& out){
                out << "If|" << id << "|" << line << "|"  << pos << "|" << expression->id << "|" << if_body->id << "|";
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "For|" << id << "|" << line << "|"  << pos << "|" << identifier << "|" << range_expr_l->id << "|" << range_expr_r->id << "|" << body->id << "\n";
                range_expr_l->machine_print(out);
                range_expr_r->machine_print(out);
                body->machine_print(out);
            }
    };

    class WhileNode: public Node {
        public:
            Node* expression;
            Node* body;
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "While|" << id << "|" << line << "|"  << pos << "|" << expression->id << "|" << body->id << "\n";
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);

            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "Primary|" << id << "|" << line << "|"  << pos << "|" << type;
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "Tail|" << id << "|" << line << "|"  << pos << "|" << type << "|";
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "Print|" << id << "|" << line << "|"  << pos << "|(";
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

    class ControlNode: public Node {
        public:
			char type;
		
            Node* value = nullptr;
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

                read_until_delim(s, read);
                type = read.first[0];
				
				if (type == 'r') {
					read_until_delim(s, read);
					value = nodes[std::stoll(read.first)];
				}
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);
				
				if (this->type == 'r') {
					this->value->visit(at_enter, at_repeat, at_exit, visit_body);
				}
				
                at_exit(this);
            }

            void machine_print(std::ostream& out){
                out << "Control|" << id << "|" << line << "|"  << pos << "|" << type;
				if (type == 'r') {
					out << "|" << value->id << "\n";
					value->machine_print(out);
				} else out << "\n";
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "Literal|" << id << "|" << line << "|"  << pos << "|" << type;
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "ArrayLiteral|" << id << "|" << line << "|"  << pos << "|(";
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "TupleLiteral|" << id << "|" << line << "|"  << pos << "|(";
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
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "Function|" << id << "|" << line << "|"  << pos << "|" << type << "|(";
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
            char type;
            Node* primary;
            Node* expression = nullptr;
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

                read_until_delim(s, read);
                type = read.first[0];

                read_until_delim(s, read);
                primary = nodes[std::stoll(read.first)];
				
				if (type != '#') {
					read_until_delim(s, read);
					expression = nodes[std::stoll(read.first)];
				}
            }

            void visit(callback_function at_enter, callback_function at_repeat, callback_function at_exit, bool visit_body=true) {
                at_enter(this);

                this->primary->visit(at_enter, at_repeat, at_exit, visit_body);
				
				if (this->type != '#') {
					at_repeat(this);
					this->expression->visit(at_enter, at_repeat, at_exit, visit_body);
				}
                at_exit(this);
            }

            void machine_print(std::ostream& out){
                out << "Assignment|" << id << "|" << line << "|"  << pos << "|" << type << "|" << primary->id;
				if (type != '#') {
					out << "|" << expression->id << "\n";
					primary->machine_print(out);
					expression->machine_print(out);
				} else {
					out << "\n";
					primary->machine_print(out);
				}
            }
    };

    class BodyNode: public Node {
        public:
            std::vector<Node*> statements;
			
			Node* from_tokens(std::vector<tokens::Token>& tokens, int& y);
			void execute(std::istream& in, std::ostream& out);
			
            void from_config(std::vector<Node*>& nodes, std::string& confstr) {
                std::stringstream s(confstr);

                char trash;

                std::pair<std::string, char> read;
                read_until_delim(s, read);

                id = std::stoll(read.first);

				read_until_delim(s, read);
				line = std::stoi(read.first);

				read_until_delim(s, read);
				pos = std::stoi(read.first);

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

            void machine_print(std::ostream& out){
                out << "Body|" << id << "|" << line << "|"  << pos << "|(";
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
        } else if (t.compare("Control") == 0) {
            return (new ControlNode());
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

        std::getline(in, s, '\n');

        for (int i = 1; i < nodes.size(); ++i) {
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
}

#endif // __TREE_NODES_INCLUDED__
