#ifndef __ARITHMETIC_INCLUDED__
#define __ARITHMETIC_INCLUDED__

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <stdexcept>
#include <map>
#include <unordered_map>

#include "ast_lib.hpp"

namespace arithmetic {
    struct AmbiguousVariable {
        char type = 'e';
        long long int_val;
        long double real_val;
        bool bool_val;
        std::string string_val;
        std::unordered_map<long long, long long> array_identifiers;
        std::unordered_map<std::string, long long> tuple_identifiers;
        std::vector<AmbiguousVariable*> array_values;
        ast_nodes::FunctionNode* function_pointer;
        std::unordered_map<std::string, AmbiguousVariable*> function_scope;
    };

    long long array_length(const std::unordered_map<long long, long long >& myMap) {
        long long len = 0;
        for (const auto& pair : myMap) {
            if (pair.first > len) {
                len = pair.first;
            }
        }
        return len;
    }
	
    std::string replace_substr(std::string init, std::string rep, std::string nn) {
        std::string::size_type index = 0;
        while ((index = init.find(rep, index)) != std::string::npos) {
            init.replace(index, rep.size(), nn);
            ++index;
        }
        return init;
    }
	
    std::ostream& operator<<(std::ostream& out, AmbiguousVariable& var) {
		//out << var.type << std::endl;
		std::string s;
		bool first;
		std::vector<std::string> ident;
        switch (var.type) {
            case 'i':
                out << var.int_val;
                break;
            case 'r':
                out << var.real_val;
                break;
            case 'b':
                if (var.bool_val) {
                    out << "true";
                } else {
                    out << "false";
                }
                break;
            case 'e':
                out << "empty";
                break;
            case 's':
                s = var.string_val;
                s = replace_substr(s, std::string("\\n"), std::string("\n"));
                s = replace_substr(s, std::string("\\t"), std::string("\t"));
                out << s;
                break;
            case 'a':
                out << "[";
				first = true;
				for (int i = 1; i <= array_length(var.array_identifiers); ++i) {
					if (first) {
						first = false;
					} else {
						out << ", ";
					}
					if (var.array_identifiers.count(i)) {
						out << *var.array_values[var.array_identifiers[i]];
					} else {
						out << "empty";
					}
				}
				out << "]";
                break;
            case 't':
                out << "{";
				first = true;
				ident.resize(var.array_values.size(), "");
				for(auto& i : var.tuple_identifiers) {
					ident[i.second] = i.first;
				}
				for (int i = 0; i < var.array_values.size(); ++i) {
					if (first) {
						first = false;
					} else {
						out << ", ";
					}
					
					if (ident[i] != "") {
						out << ident[i] << " := " << *var.array_values[i];
					} else {
						out << *var.array_values[i];
					}
				}
                out << "}";
                break;
            case 'f':
                out << "func (";
				first = true;
				for (auto& i : var.function_pointer->params) {
					if (first) {
						first = false;
					} else {
						out << ", ";
					}
					out << i;
				}
                out << ")";
                break;
			default:
				out << "invalid " << var.type;
        }
		//out << var.type << std::endl;
		return out;
    }

	
	AmbiguousVariable* copy (AmbiguousVariable* var) {
		AmbiguousVariable* c = new AmbiguousVariable();
		
		c->type = var->type;
		switch (var->type) {
			case 'i':
                c->int_val = (long long)var->int_val;
                break;
            case 'r':
                c->real_val = (long double)var->real_val;
                break;
            case 'b':
                c->bool_val = (bool)var->bool_val;
                break;
            case 'e':
                break;
            case 's':
                c->string_val = std::string(var->string_val.begin(), var->string_val.end());
                break;
            case 'a':
                c->array_identifiers = var->array_identifiers;
				c->array_values.reserve(var->array_values.size());
				for (auto& i : var->array_values) {
					c->array_values.push_back(copy(i));
				}
                break;
            case 't':
                c->tuple_identifiers = var->tuple_identifiers;
				c->array_values.reserve(var->array_values.size());
				for (auto& i : var->array_values) {
					c->array_values.push_back(copy(i));
				}
                break;
            case 'f':
                c->function_pointer = var->function_pointer;
                break;
			default:
				throw std::invalid_argument("Invalid type");
		}
		return c;
	}

    bool tuple_conflicts(AmbiguousVariable* a, AmbiguousVariable* b) {
        for (const auto& pair : a->tuple_identifiers) {
            if (b->tuple_identifiers.find(pair.first) != b->tuple_identifiers.end()) {
                return true;
            }
        }
        return false;
    }

    AmbiguousVariable* tuple_addition(AmbiguousVariable* a, AmbiguousVariable* b) {
        long long vec_size = a->array_values.size();
        if (tuple_conflicts(a, b)) {
            throw std::runtime_error("Impossible to concatenate two tuples because they have the same key");
        }
        AmbiguousVariable* c = new AmbiguousVariable();
        c->type = 't';
        c->array_values.insert(c->array_values.end(), a->array_values.begin(), a->array_values.end());
        c->array_values.insert(c->array_values.end(), b->array_values.begin(), b->array_values.end());
        std::unordered_map<std::string, long long> concatenated_map;
        for (const auto& [key, value] : a->tuple_identifiers) {
            concatenated_map[key] = value;
        }

        for (const auto& [key, value] : b->tuple_identifiers) {
            concatenated_map[key] = vec_size + value;
        }
        c->tuple_identifiers = concatenated_map;
        return c;
    }

    AmbiguousVariable* array_addition(AmbiguousVariable* a, AmbiguousVariable* b) {
        long long length_of_array = array_length(a->array_identifiers);
        long long vec_size = a->array_values.size();
        AmbiguousVariable* c = new AmbiguousVariable();
        c->type = 'a';
        c->array_values.insert(c->array_values.end(), a->array_values.begin(), a->array_values.end());
        c->array_values.insert(c->array_values.end(), b->array_values.begin(), b->array_values.end());
        std::unordered_map<long long, long long > concatenated_map;

        for (const auto& [key, value] : a->array_identifiers) {
            concatenated_map[key] = value;
        }

        for (const auto& [key, value] : b->array_identifiers) {
            concatenated_map[length_of_array + key] = vec_size + value;
        }
        c->array_identifiers = concatenated_map;
        return c;
    }

    AmbiguousVariable* op_addition(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->type = 'i';
                        c->int_val = a->int_val + b->int_val;
                        return c;
                    case 'r':
                        c->type = 'r';
                        c->real_val = a->int_val + b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->type = 'r';
                        c->real_val = a->real_val + b->int_val;
                        return c;
                    case 'r':
                        c->type = 'r';
                        c->real_val = a->real_val + b->real_val;
                        return c;
                }
            case 's':
                switch (b->type) {
                    case 's':
                        c->type = 's';
                        c->string_val = a->string_val + b->string_val;
                        return c;
                }
            case 't':
                switch (b->type) {
                    case 't':
                        c = tuple_addition(a, b);
                        return c;
                }
            case 'a':
                switch (b->type) {
                    case 'a':
                        c = array_addition(a, b);
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for addition");
    }


    AmbiguousVariable* op_subtraction(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->type = 'i';
                        c->int_val = a->int_val - b->int_val;
                        return c;
                    case 'r':
                        c->type = 'r';
                        c->real_val = a->int_val - b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->type = 'r';
                        c->real_val = a->real_val - b->int_val;
                        return c;
                    case 'r':
                        c->type = 'r';
                        c->real_val = a->real_val - b->real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for subtraction");
    }

    AmbiguousVariable* op_plus_equality(AmbiguousVariable* a, AmbiguousVariable* b) {
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        a->int_val = a->int_val + b->int_val;
                        return a;
                    case 'r':
                        a->type = 'r';
                        a->real_val = a->int_val + b->real_val;
                        return a;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        a->real_val = a->real_val + b->int_val;
                        return a;
                    case 'r':
                        a->real_val = a->real_val + b->real_val;
                        return a;
                }
            case 's':
                switch (b->type) {
                    case 's':
                        a->string_val = a->string_val + b->string_val;
                        return a;
                }
            case 't':
                switch (b->type) {
                    case 't':
                        *a = *tuple_addition(a, b);
                        return a;
                }
            case 'a':
                switch (b->type) {
                    case 'a':
                        *a = *array_addition(a, b);
                        return a;
                }
        }
        throw std::runtime_error("Unsupported operand type for addition");
    }


    AmbiguousVariable* op_minus_equality(AmbiguousVariable* a, AmbiguousVariable* b) {
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        a->int_val = a->int_val - b->int_val;
                        return a;
                    case 'r':
                        a->type = 'r';
                        a->real_val = a->int_val - b->real_val;
                        return a;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        a->real_val = a->real_val - b->int_val;
                        return a;
                    case 'r':
                        a->real_val = a->real_val - b->real_val;
                        return a;
                }
        }
        throw std::runtime_error("Unsupported operand type for subtraction");
    }

    AmbiguousVariable* op_multiplication(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->type = 'i';
                        c->int_val = a->int_val * b->int_val;
                        return c;
                    case 'r':
                        c->type = 'r';
                        c->real_val = a->int_val * b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->type = 'r';
                        c->real_val = a->real_val * b->int_val;
                        return c;
                    case 'r':
                        c->type = 'r';
                        c->real_val = a->real_val * b->real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for multiplication");
    }


    AmbiguousVariable* op_division(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->type = 'i';
                        c->int_val = a->int_val / b->int_val;
                        return c;
                    case 'r':
                        c->type = 'r';
                        c->real_val = a->int_val / b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->type = 'r';
                        c->real_val = a->real_val / b->int_val;
                        return c;
                    case 'r':
                        c->type = 'r';
                        c->real_val = a->real_val / b->real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for division");
    }

    AmbiguousVariable* op_lt(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        c->type = 'b';
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->int_val < b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->int_val < b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->real_val < b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->real_val < b->real_val;
                        return c;
                }
        }
		std::cout << *a << ' ' << *b << std::endl;
        throw std::runtime_error("Unsupported operand type for <");
    }

    AmbiguousVariable* op_gt(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        c->type = 'b';
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->int_val > b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->int_val > b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->real_val > b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->real_val > b->real_val;
                        return c;
                }
        }

        throw std::runtime_error("Unsupported operand type for >");
    }

    AmbiguousVariable* op_lte(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        c->type = 'b';
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->int_val <= b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->int_val <= b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->real_val <= b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->real_val <= b->real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for <=");
    }

    AmbiguousVariable* op_gte(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        c->type = 'b';
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->int_val >= b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->int_val >= b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->real_val >= b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->real_val >= b->real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for >=");
    }


    AmbiguousVariable* op_eq(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        c->type = 'b';
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->int_val == b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->int_val == b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->real_val == b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->real_val == b->real_val;
                        return c;
                }
            case 's':
                if (b->type == 's') {
					c->bool_val = a->string_val == b->string_val;
                    return c;
				}
            case 'e':
                if (b->type == 'e') {
					c->bool_val = true;
                    return c;
				}
            case 'b':
                if (b->type == 'b') {
					c->bool_val = a->bool_val == b->bool_val;
                    return c;
				}
        }
		//std::cout << *a << ' ' << *b << std::endl;
        throw std::runtime_error("Unsupported operand type for =");
    }




    AmbiguousVariable* op_ne(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        c->type = 'b';
        switch (a->type) {
            case 'i':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->int_val != b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->int_val != b->real_val;
                        return c;
                }
            case 'r':
                switch (b->type) {
                    case 'i':
                        c->bool_val = a->real_val != b->int_val;
                        return c;
                    case 'r':
                        c->bool_val = a->real_val != b->real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for \\=");
    }



    AmbiguousVariable* op_xor(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        if (!(a->type == 'b' and b->type == 'b')) {
            throw std::runtime_error("xor is possible only with two boolean arguments");
        }
        c->type = 'b';
        c->bool_val = a->bool_val xor b->bool_val;
        return c;
    }


    AmbiguousVariable* op_and(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        if (!(a->type == 'b' and b->type == 'b')) {
			std::cout << *a << ' ' << *b << std::endl;
            throw std::runtime_error("and is possible only with two boolean arguments");
        }
        c->type = 'b';
        c->bool_val = a->bool_val and b->bool_val;
        return c;
    }

    AmbiguousVariable* op_or(AmbiguousVariable* a, AmbiguousVariable* b) {
        AmbiguousVariable* c = new AmbiguousVariable();
        if (!(a->type == 'b' and b->type == 'b')) {
            throw std::runtime_error("or is possible only with two boolean arguments");
        }
        c->type = 'b';
        c->bool_val = a->bool_val or b->bool_val;
        return c;
    }


    AmbiguousVariable* op_unary_plus(AmbiguousVariable* a) {
        return copy(a);
    }

    AmbiguousVariable* op_unary_minus(AmbiguousVariable* a) {
        AmbiguousVariable* c = new AmbiguousVariable();
        switch (a->type) {
            case 'r':
                c->type = 'r';
                c->real_val = - c->real_val;
                return c;
            case 'i':
                c->type = 'i';
                c->int_val = -a->int_val;
                return c;
            default:
                throw std::runtime_error("unary + operation can be performed only with integer or real");
        }
    }

    AmbiguousVariable* op_unary_not(AmbiguousVariable* a) {
        AmbiguousVariable* c = new AmbiguousVariable();
        if (a->type != 'b') {
            throw std::runtime_error("not operation can be performed only with boolean");
        }
        c->type = 'b';
        c->bool_val = not a->bool_val;
        return c;
    }

    AmbiguousVariable* perform_unary_op(char op, AmbiguousVariable* variable) {
        switch (op) {
            case '+':
                return op_unary_plus(variable);
            case '-':
                return op_unary_minus(variable);
            case 'n':
                return op_unary_not(variable);
            case '#':
                return variable;
            default:
                throw std::runtime_error("unsupported unary operation");
        }
    }

    int precedence(const char op) {
        switch (op) {
            case '+':
            case '-':
                return 3;
            case '*':
            case '/':
                return 4;
            case '<':
            case 'l':
            case '>':
            case 'm':
            case '=':
            case 'n':
                return 2;
            case 'a':
                return 1;
            case 'x':
                return 0;
            case 'o':
                return -1;
            default:
                return -2;
        }
    }

    bool has_higher_precedence(const char op1, const char op2) {
        return precedence(op1) < precedence(op2);
    }

    AmbiguousVariable* apply_operator(AmbiguousVariable* a, AmbiguousVariable* b, const char& op) {
        if (op == '+') return op_addition(a, b);
        if (op == '-') return op_subtraction(a, b);
        if (op == '*') return op_multiplication(a, b);
        if (op == '/') return op_division(a, b);
        if (op == '<') return op_lt(a, b);
        if (op == 'l') return op_lte(a, b);
        if (op == '>') return op_gt(a, b);
        if (op == 'm') return op_gte(a, b);
        if (op == '=') return op_eq(a, b);
        if (op == 'n') return op_ne(a, b);
        if (op == 'x') return op_xor(a, b);
        if (op == 'a') return op_and(a, b);
        if (op == 'o') return op_or(a, b);
		std::cout << op << std::endl;
        throw std::runtime_error("Unknown operator");
    }

    AmbiguousVariable* evaluate_expression(const std::vector<AmbiguousVariable*>& variables,
                                          const std::vector<char>& operators) {
        std::stack<AmbiguousVariable*> values;
        std::stack<char> ops;
        if (!variables.empty()) {
            values.push(variables[0]);
        }

        for (size_t i = 0; i < operators.size(); ++i) {
            const char& op = operators[i];
            while (!ops.empty() && has_higher_precedence(op, ops.top())) {
                AmbiguousVariable* b = values.top(); values.pop();
                AmbiguousVariable* a = values.top(); values.pop();
                char operator_to_apply = ops.top(); ops.pop();
                AmbiguousVariable* result = apply_operator(a, b, operator_to_apply);
                values.push(result);
            }
            ops.push(op);
            if (i + 1 < variables.size()) {
                values.push(variables[i + 1]);
            }
        }
        while (!ops.empty()) {
            AmbiguousVariable* b = values.top(); values.pop();
            AmbiguousVariable* a = values.top(); values.pop();
            char operator_to_apply = ops.top(); ops.pop();
            AmbiguousVariable* result = apply_operator(a, b, operator_to_apply);
            values.push(result);
        }
        return values.top();
    }

    AmbiguousVariable* get_by_index(AmbiguousVariable& a, long long index) {
        if (a.type != 'a') {
            throw std::runtime_error("Incorrect type of variable for getting value by index");
        }
        long long real_index;
        try {
            real_index = a.array_identifiers.at(index);
        }
        catch (...) {
            throw std::runtime_error("Incorrect index");
        }
        return a.array_values[real_index];
    }

    AmbiguousVariable* get_by_key(AmbiguousVariable& m, const std::string& key) {
        if (m.type != 't') {
            throw std::runtime_error("Incorrect type of variable for getting value by key");
        }
        long long real_index;
        try {
            real_index = m.tuple_identifiers.at(key);
        }
        catch (...) {
            throw std::runtime_error("Incorrect key");
        }
        return m.array_values[real_index];
    }
}

#endif // __ARITHMETIC_INCLUDED__
