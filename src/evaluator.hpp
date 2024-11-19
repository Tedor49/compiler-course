#ifndef __EVALUATOR_INCLUDED__
#define __EVALUATOR_INCLUDED__

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <stdexcept>
#include <map>
#include <unordered_map>

namespace evaluator {
    struct AmbiguousVariable {
        char type{};
        long long int_val{};
        long double real_val{};
        bool bool_val{};
        std::string string_val;
        std::unordered_map<long long, long long> array_identifiers;
        std::vector<AmbiguousVariable> array_values;
        std::unordered_map<std::string, long long> tuple_identifiers;
        std::vector<AmbiguousVariable> tuple_values;

    };

    bool tuple_conflicts(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        for (const auto& pair : a.tuple_identifiers) {
            if (b.tuple_identifiers.find(pair.first) != b.tuple_identifiers.end()) {
                return true;
            }
        }
        return false;
    }

    AmbiguousVariable tuple_addition(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        long long vec_size = a.tuple_values.size();
        if (tuple_conflicts(a, b)) {
            throw std::runtime_error("Impossible to concatenate two tuples because they have the same key");
        }
        AmbiguousVariable c;
        c.type = 't';
        c.tuple_values.insert(c.tuple_values.end(), a.tuple_values.begin(), a.tuple_values.end());
        c.tuple_values.insert(c.tuple_values.end(), b.tuple_values.begin(), b.tuple_values.end());
        std::unordered_map<std::string, long long> concatenated_map;
        for (const auto& [key, value] : a.tuple_identifiers) {
            concatenated_map[key] = value;
        }

        for (const auto& [key, value] : b.tuple_identifiers) {
            concatenated_map[key] = vec_size + value;
        }
        c.tuple_identifiers = concatenated_map;
        return c;
    }

    long long array_length(const std::unordered_map<long long, long long >& myMap) {
        long long len = 0;
        for (const auto& pair : myMap) {
            if (pair.first > len) {
                len = pair.first;
            }
        }
        return len;
    }

    AmbiguousVariable array_addition(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        long long length_of_array = array_length(a.array_identifiers);
        long long vec_size = a.array_values.size();
        AmbiguousVariable c;
        c.type = 'a';
        c.array_values.insert(c.array_values.end(), a.array_values.begin(), a.array_values.end());
        c.array_values.insert(c.array_values.end(), b.array_values.begin(), b.array_values.end());
        std::unordered_map<long long, long long > concatenated_map;

        for (const auto& [key, value] : a.array_identifiers) {
            concatenated_map[key] = value;
        }

        for (const auto& [key, value] : b.array_identifiers) {
            concatenated_map[length_of_array + key] = vec_size + value;
        }
        c.array_identifiers = concatenated_map;
        return c;
    }




    AmbiguousVariable op_addition(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.type = 'i';
                        c.int_val = a.int_val + b.int_val;
                        return c;
                    case 'r':
                        c.type = 'r';
                        c.real_val = a.int_val + b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.type = 'r';
                        c.real_val = a.real_val + b.int_val;
                        return c;
                    case 'r':
                        c.type = 'r';
                        c.real_val = a.real_val + b.real_val;
                        return c;
                }
            case 's':
                switch (b.type) {
                    case 's':
                        c.type = 's';
                        c.string_val = a.string_val + b.string_val;
                        return c;
                }
            case 't':
                switch (b.type) {
                    case 't':
                        c = tuple_addition(a, b);
                        return c;
                }
            case 'a':
                switch (b.type) {
                    case 'a':
                        c = array_addition(a, b);
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for addition");
    }


    AmbiguousVariable op_subtraction(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.type = 'i';
                        c.int_val = a.int_val - b.int_val;
                        return c;
                    case 'r':
                        c.type = 'r';
                        c.real_val = a.int_val - b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.type = 'r';
                        c.real_val = a.real_val - b.int_val;
                        return c;
                    case 'r':
                        c.type = 'r';
                        c.real_val = a.real_val - b.real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for subtraction");
    }

    AmbiguousVariable op_multiplication(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.type = 'i';
                        c.int_val = a.int_val * b.int_val;
                        return c;
                    case 'r':
                        c.type = 'r';
                        c.real_val = a.int_val * b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.type = 'r';
                        c.real_val = a.real_val * b.int_val;
                        return c;
                    case 'r':
                        c.type = 'r';
                        c.real_val = a.real_val * b.real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for multiplication");
    }


    AmbiguousVariable op_division(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.type = 'i';
                        c.int_val = a.int_val / b.int_val;
                        return c;
                    case 'r':
                        c.type = 'r';
                        c.real_val = a.int_val / b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.type = 'r';
                        c.real_val = a.real_val / b.int_val;
                        return c;
                    case 'r':
                        c.type = 'r';
                        c.real_val = a.real_val / b.real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for division");
    }

    AmbiguousVariable op_lt(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        c.type = 'b';
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.int_val < b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.int_val < b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.real_val < b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.real_val < b.real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for <");
    }

    AmbiguousVariable op_gt(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        c.type = 'b';
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.int_val > b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.int_val > b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.real_val > b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.real_val > b.real_val;
                        return c;
                }
        }

        throw std::runtime_error("Unsupported operand type for >");
    }

    AmbiguousVariable op_lte(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        c.type = 'b';
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.int_val <= b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.int_val <= b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.real_val <= b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.real_val <= b.real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for <=");
    }

    AmbiguousVariable op_gte(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        c.type = 'b';
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.int_val >= b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.int_val >= b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.real_val >= b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.real_val >= b.real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for >=");
    }


    AmbiguousVariable op_eq(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        c.type = 'b';
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.int_val == b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.int_val == b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.real_val == b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.real_val == b.real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for ==");
    }




    AmbiguousVariable op_ne(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        c.type = 'b';
        switch (a.type) {
            case 'i':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.int_val != b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.int_val != b.real_val;
                        return c;
                }
            case 'r':
                switch (b.type) {
                    case 'i':
                        c.bool_val = a.real_val != b.int_val;
                        return c;
                    case 'r':
                        c.bool_val = a.real_val != b.real_val;
                        return c;
                }
        }
        throw std::runtime_error("Unsupported operand type for \\=");
    }



    AmbiguousVariable op_xor(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        if (!(a.type == 'b' and b.type == 'b')) {
            throw std::runtime_error("xor is possible only with two boolean arguments");
        }
        c.type = 'b';
        c.bool_val = a.bool_val xor b.bool_val;
        return c;
    }


    AmbiguousVariable op_and(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        if (!(a.type == 'b' and b.type == 'b')) {
            throw std::runtime_error("and is possible only with two boolean arguments");
        }
        c.type = 'b';
        c.bool_val = a.bool_val and b.bool_val;
        return c;
    }

    AmbiguousVariable op_or(const AmbiguousVariable& a, const AmbiguousVariable& b) {
        AmbiguousVariable c;
        if (!(a.type == 'b' and b.type == 'b')) {
            throw std::runtime_error("or is possible only with two boolean arguments");
        }
        c.type = 'b';
        c.bool_val = a.bool_val or b.bool_val;
        return c;
    }


    AmbiguousVariable op_unary_plus(AmbiguousVariable& a) {
        AmbiguousVariable c;
        switch (a.type) {
            case 'r':
                c.type = 'r';
                c.real_val = a.real_val;
                return c;
            case 'i':
                c.type = 'i';
                c.int_val = a.int_val;
                return c;
            default:
                throw std::runtime_error("unary + operation can be performed only with integer or real");
        }
    }

    AmbiguousVariable op_unary_minus(AmbiguousVariable& a) {
        AmbiguousVariable c;
        switch (a.type) {
            case 'r':
                c.type = 'r';
                c.real_val = - c.real_val;
                return c;
            case 'i':
                c.type = 'i';
                c.int_val = -a.int_val;
                return c;
            default:
                throw std::runtime_error("unary + operation can be performed only with integer or real");
        }
    }

    AmbiguousVariable op_unary_not(AmbiguousVariable& a) {
        AmbiguousVariable c;
        if (a.type != 'b') {
            throw std::runtime_error("not operation can be performed only with boolean");
        }
        c.type = 'b';
        c.bool_val = not a.bool_val;
        return c;
    }

    AmbiguousVariable perform_unary_op(char op, AmbiguousVariable& variable) {
        switch (op) {
            case '+':
                return op_unary_plus(variable);
            case '-':
                return op_unary_minus(variable);
            case 'n':
                return op_unary_not(variable);
            default:
                throw std::runtime_error("unsupported unary operation");
        }
    }

    void printAmbiguousVariable(const AmbiguousVariable& a) {
        switch (a.type) {
            case 'i':
                std::cout << a.int_val << std::endl;
                break;
            case 'r':
                std::cout << a.real_val << std::endl;
                break;
            case 'b':
                std::cout << a.bool_val << std::endl;
                break;
        }
    }

    int precedence(const char op) {
        if (op == '+' || op == '-') return 3;
        if (op == '*' || op == '/') return 4;
        if (op == '<' || op == 'l' || op == '>' || op == 'r' || op == 'e' || op == 'n') return 2;
        if (op == 'x' || op == 'a' || op == 'o') return 1;
        return 0; // Invalid operator
    }

    bool has_higher_precedence(const char op1, const char op2) {
        return precedence(op1) < precedence(op2);
    }

    AmbiguousVariable apply_operator(const AmbiguousVariable& a, const AmbiguousVariable& b, const char& op) {
        if (op == '+') return op_addition(a, b);
        if (op == '-') return op_subtraction(a, b);
        if (op == '*') return op_multiplication(a, b);
        if (op == '/') return op_division(a, b);
        if (op == '<') return op_lt(a, b);
        if (op == 'l') return op_lte(a, b);
        if (op == '>') return op_gt(a, b);
        if (op == 'm') return op_gte(a, b);
        if (op == 'e') return op_eq(a, b);
        if (op == 'n') return op_ne(a, b);
        if (op == 'x') return op_xor(a, b);
        if (op == 'a') return op_and(a, b);
        if (op == 'o') return op_or(a, b);
        throw std::runtime_error("Unknown operator");
    }

    AmbiguousVariable evaluate_expression(const std::vector<AmbiguousVariable>& variables,
                                          const std::vector<char>& operators) {
        std::stack<AmbiguousVariable> values;
        std::stack<char> ops;
        if (!variables.empty()) {
            values.push(variables[0]);
        }

        for (size_t i = 0; i < operators.size(); ++i) {
            const char& op = operators[i];
            while (!ops.empty() && has_higher_precedence(op, ops.top())) {
                AmbiguousVariable b = values.top(); values.pop();
                AmbiguousVariable a = values.top(); values.pop();
                char operator_to_apply = ops.top(); ops.pop();
                AmbiguousVariable result = apply_operator(a, b, operator_to_apply);
                values.push(result);
            }
            ops.push(op);
            if (i + 1 < variables.size()) {
                values.push(variables[i + 1]);
            }
        }
        while (!ops.empty()) {
            AmbiguousVariable b = values.top(); values.pop();
            AmbiguousVariable a = values.top(); values.pop();
            char operator_to_apply = ops.top(); ops.pop();
            AmbiguousVariable result = apply_operator(a, b, operator_to_apply);
            values.push(result);
        }
        return values.top();
    }

    AmbiguousVariable get_by_index(const AmbiguousVariable& a, long long index) {
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

    AmbiguousVariable get_by_key(const AmbiguousVariable& m, const std::string& key) {
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
        return m.tuple_values[real_index];
    }
}

#endif // __EVALUATOR_INCLUDED__