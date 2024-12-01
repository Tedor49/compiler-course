#ifndef __TREE_EXECUTE_INCLUDED__
#define __TREE_EXECUTE_INCLUDED__

#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

#include "ast_nodes.hpp"
#include "arithmetic.hpp"

namespace ast_nodes {

    arithmetic::AmbiguousVariable constempty;

    enum ControlState {
        Normal,
        Break,
        Continue,
        Return,
    };

    int control_flag = ControlState::Normal;
    arithmetic::AmbiguousVariable* return_register;

    struct scopeinfo {
        int node_id;
        std::unordered_map<std::string, arithmetic::AmbiguousVariable*> variables;
        std::unordered_map<int, arithmetic::AmbiguousVariable*> intermediates;
    };

    std::ostream& operator<<(std::ostream& out, scopeinfo& var) {
        out << "Scope " << var.node_id << std::endl;
        out << "Variables:" << std::endl;
        for (auto i: var.variables) {
            out << '\t' << i.first << ' ' << *i.second << std::endl;
        }
        out << "Intermediates:" << std::endl;
        for (auto i: var.intermediates) {
            if (i.second != nullptr) {
                out << '\t' << i.first << ' ' << *i.second << std::endl;

            } else {
                out << '\t' << i.first << ' ' << 0 << std::endl;
            }
        }
        out << "END OF SCOPE" << std::endl;
        return out;
    }

    std::vector <scopeinfo> scopes;

    void open_scope(ast_nodes::Node* parent) {
        scopes.resize(scopes.size() + 1);
        scopes.back().node_id = parent->id;
    }

    void close_scope(ast_nodes::Node* parent) {
        if (scopes.size() == 0) {
            throw std::invalid_argument("SCOPE DOES NOT EXIST WHILE CLOSING");
        }
        while (scopes.back().node_id != parent->id) {
            scopes.resize(scopes.size() - 1);
            if (scopes.size() == 0) {
                throw std::invalid_argument("SCOPE DOES NOT EXIST WHILE CLOSING");
            }
        }
        scopes.resize(scopes.size() - 1);
    }

    arithmetic::AmbiguousVariable* get_variable(std::string identifier) {
        for (int i = (signed) (scopes.size()) - 1; i >= 0; --i) {
            if (scopes[i].variables.count(identifier)) {
                return scopes[i].variables[identifier];
            }
        }
        throw std::invalid_argument(std::format("Variable {} referenced before declaration", identifier));
    }

    //Create new scope
    void BodyNode::execute(std::istream& in, std::ostream& out) {
        open_scope(this);
        for (auto i: statements) {
            i->execute(in, out);
            if (control_flag != ControlState::Normal) break;
        }
        close_scope(this);
    }

    //Adds variables to current scope
    void DeclarationNode::execute(std::istream& in, std::ostream& out) {
        if (value != nullptr) {
            value->execute(in, out);
            scopes.back().variables[identifier] = scopes.back().intermediates[value->id];
        } else {
            scopes.back().variables[identifier] = new arithmetic::AmbiguousVariable();
        }
    }

    // Evaluate expression and put its value into current scope
    void ExpressionNode::execute(std::istream& in, std::ostream& out) {
        std::vector<arithmetic::AmbiguousVariable*> calced_terms;
        calced_terms.reserve(terms.size());

        for (auto i: terms) {
            i->execute(in, out);
            calced_terms.push_back(scopes.back().intermediates[i->id]);
        }

        try {
            scopes.back().intermediates[id] = evaluate_expression(calced_terms, ops);
        } catch (std::invalid_argument& ex) {
            throw std::invalid_argument(
                    std::format("Evaluation error at line {}, pos {}:\n\t{}", line, pos, ex.what()));
        } catch (std::runtime_error& ex) {
            throw std::invalid_argument(
                    std::format("Evaluation error at line {}, pos {}:\n\t{}", line, pos, ex.what()));
        }

    }

    // Evaluate unary operator and put its value into current scope
    void UnaryNode::execute(std::istream& in, std::ostream& out) {
        primary->execute(in, out);
        arithmetic::AmbiguousVariable* new_term;

        try {
            new_term = perform_unary_op(unaryop, scopes.back().intermediates[primary->id]);
        } catch (std::invalid_argument& ex) {
            throw std::invalid_argument(
                    std::format("Evaluation error at line {}, pos {}:\n\t{}", line, pos, ex.what()));
        } catch (std::runtime_error& ex) {
            throw std::invalid_argument(
                    std::format("Evaluation error at line {}, pos {}:\n\t{}", line, pos, ex.what()));
        }

        if (type_ind == '#') {
            scopes.back().intermediates[id] = new_term;
        } else {
            arithmetic::AmbiguousVariable* is_type = new arithmetic::AmbiguousVariable();
            is_type->type = 'b';
            is_type->bool_val = (type_ind == new_term->type);
            scopes.back().intermediates[id] = is_type;
        }
    }

    // Evaluate primary and put its value into current scope

    void PrimaryNode::execute(std::istream& in, std::ostream& out) {
        if (type == 'i') {
            arithmetic::AmbiguousVariable* inputted_value = new arithmetic::AmbiguousVariable();
            inputted_value->type = 'i';
            in >> inputted_value->int_val;
            scopes.back().intermediates[id] = inputted_value;
        } else if (type == 'r') {
            arithmetic::AmbiguousVariable* inputted_value = new arithmetic::AmbiguousVariable();
            inputted_value->type = 'r';
            in >> inputted_value->real_val;
            scopes.back().intermediates[id] = inputted_value;
        } else if (type == 's') {
            arithmetic::AmbiguousVariable* inputted_value = new arithmetic::AmbiguousVariable();
            inputted_value->type = 's';
            in >> inputted_value->string_val;
            scopes.back().intermediates[id] = inputted_value;
        } else if (type == 'v') {
            arithmetic::AmbiguousVariable* var = get_variable(identifier);
            //var.type = 'f';

            for (ast_nodes::Node* i: tails) {
                i->execute(in, out);
                ast_nodes::TailNode* tail = dynamic_cast<ast_nodes::TailNode*> (i);

                if (tail->type == 't') {
                    if (var->type != 't') {
                        throw std::invalid_argument(
                                std::format("Error at line {}, pos {}:\n\t{}", line, pos, "Expected tuple"));
                    }

                    if (var->array_values.size() < tail->tuple_idx - 1) {
                        throw std::invalid_argument(
                                std::format("Error at line {}, pos {}:\n\t{}", line, pos, "Tuple index out of range"));
                    }

                    var = var->array_values[tail->tuple_idx - 1];
                } else if (tail->type == 'i') {
                    if (var->type != 't') {
                        throw std::invalid_argument(
                                std::format("Error at line {}, pos {}:\n\t{}", line, pos, "Expected tuple"));
                    }

                    if (var->tuple_identifiers.count(tail->identifier) == 0) {
                        throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                                "Tuple identifier not present"));
                    }

                    var = var->array_values[var->tuple_identifiers[tail->identifier]];
                } else if (tail->type == 'p') {
                    if (var->type != 'f') {
                        throw std::invalid_argument(
                                std::format("Error at line {}, pos {}:\n\t{}", line, pos, "Expected function"));
                    }

                    ast_nodes::FunctionNode* foo = var->function_pointer;

                    if (foo->params.size() != tail->params.size()) {
                        throw std::invalid_argument(std::format(
                                "Error at line {}, pos {}:\n\tArgument amount mismatch: Expected: {} got: {}", line,
                                pos, foo->params.size(), tail->params.size()));
                    }

                    scopeinfo capture;
                    capture.variables = var->function_scope;
                    scopes.push_back(capture);

                    open_scope(i);

                    for (int j = 0; j < foo->params.size(); ++j) {
                        scopes.back().variables[foo->params[j]] = scopes[scopes.size() -
                                                                         3].intermediates[tail->params[j]->id];
                    }

                    return_register = &constempty;

                    foo->body->execute(in, out);

                    if (control_flag == ControlState::Return) {
                        control_flag = ControlState::Normal;
                    }

                    if (control_flag != ControlState::Normal) {
                        throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                                "Unexpected break or continue in function call"));
                    }

                    if (foo->type == 'l') {
                        return_register = scopes.back().intermediates[foo->body->id];
                    }

                    var = arithmetic::copy(return_register);

                    close_scope(i);
                    scopes.resize(scopes.size() - 1);
                } else if (tail->type == 's') {
                    if (var->type != 'a') {
                        throw std::invalid_argument(
                                std::format("Error at line {}, pos {}:\n\t{}", line, pos, "Expected array"));
                    }

                    arithmetic::AmbiguousVariable* sub = scopes.back().intermediates[tail->subscript->id];
                    if (sub->type != 'i') {
                        throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                                "Expected integer as array index"));
                    }

                    if (var->array_identifiers.count(sub->int_val) == 0) {
                        var->array_identifiers[sub->int_val] = var->array_values.size();
                        var->array_values.push_back(new arithmetic::AmbiguousVariable());
                    }

                    var = var->array_values[var->array_identifiers[sub->int_val]];
                } else {
                    throw std::invalid_argument(
                            std::format("Error at line {}, pos {}:\n\t{}", line, pos, "Expected valid type"));
                }
            }
            scopes.back().intermediates[id] = var;
        } else if (type == 'l') {

            literal->execute(in, out);
            scopes.back().intermediates[id] = scopes.back().intermediates[literal->id];
        } else if (type == 'e') {
            expression->execute(in, out);
            scopes.back().intermediates[id] = scopes.back().intermediates[expression->id];
        } else {
            throw std::invalid_argument(
                    std::format("Error at line {}, pos {}:\n\t{}", line, pos, "Expected valid type"));
        }
    }

    // Assign value to variable in current scope
    void AssignmentNode::execute(std::istream& in, std::ostream& out) {
        primary->execute(in, out);

        if (type == '#') return;

        expression->execute(in, out);
        switch (type) {
            case '=':
                *scopes.back().intermediates[primary->id] = *scopes.back().intermediates[expression->id];
                break;
            case '+':
                arithmetic::op_plus_equality(scopes.back().intermediates[primary->id],
                                             scopes.back().intermediates[expression->id]);
                break;
            case '-':
                arithmetic::op_minus_equality(scopes.back().intermediates[primary->id],
                                              scopes.back().intermediates[expression->id]);
                break;
            default:
                throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                        "Expected :=, += or -= in supposed assignment"));
        }
    }

    // Print some expressions
    void PrintNode::execute(std::istream& in, std::ostream& out) {
        for (auto i: values) {
            i->execute(in, out);
            out << *scopes.back().intermediates[i->id];
        }
    }

    // Return some expression from function
    void ControlNode::execute(std::istream& in, std::ostream& out) {
        switch (type) {
            case 'b':
                control_flag = ControlState::Break;
                break;
            case 'r':
                value->execute(in, out);
                control_flag = ControlState::Return;
                return_register = scopes.back().intermediates[value->id];
                break;
            case 'c':
                control_flag = ControlState::Continue;
                break;
        };
    }

    // Handle if statement and execute one of the bodies
    void IfNode::execute(std::istream& in, std::ostream& out) {
        expression->execute(in, out);
        const arithmetic::AmbiguousVariable* val = scopes.back().intermediates[expression->id];
        if (val->type == 'b') {
            if (val->bool_val) {
                if_body->execute(in, out);
            } else if (else_body != nullptr) {
                else_body->execute(in, out);
            }
        } else {
            throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                    "If statement does not contain boolean as argument"));
        }
    }

    void TailNode::execute(std::istream& in, std::ostream& out) {
        switch (type) {
            case 'p':
                for (auto i: params) {
                    i->execute(in, out);
                }
                break;
            case 's':
                subscript->execute(in, out);
                break;
        }
    }


    void ForNode::execute(std::istream& in, std::ostream& out) {
        open_scope(this);

        range_expr_l->execute(in, out);
        range_expr_r->execute(in, out);

        arithmetic::AmbiguousVariable* rng_l = scopes.back().intermediates[range_expr_l->id];
        arithmetic::AmbiguousVariable* rng_r = scopes.back().intermediates[range_expr_r->id];

        if (rng_l->type != 'i') {
            throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                    "Left bound of for loop is not and integer"));
        }

        if (rng_r->type != 'i') {
            throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                    "Right bound of for loop is not and integer"));
        }

        scopes.back().variables[identifier] = new arithmetic::AmbiguousVariable();
        scopes.back().variables[identifier]->type = 'i';
        for (long long i = rng_l->int_val; i <= rng_r->int_val; ++i) {
            scopes.back().variables[identifier]->int_val = i;
            body->execute(in, out);
            if (control_flag == ControlState::Return) {
                break;
            } else if (control_flag == ControlState::Break) {
                control_flag = ControlState::Normal;
                break;
            } else if (control_flag == ControlState::Continue) {
                control_flag = ControlState::Normal;
                continue;
            }
        }
        close_scope(this);
    }

    bool while_check_cond(WhileNode* node, std::istream& in, std::ostream& out, int pos, int line) {
        node->expression->execute(in, out);
        arithmetic::AmbiguousVariable* val = scopes.back().intermediates[node->expression->id];

        if (val->type != 'b') {
            throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                    "Expression in while loop does not evaluate to a boolean"));
        }

        return val->bool_val;
    }

    // Handle while statement and execute the body multiple times
    void WhileNode::execute(std::istream& in, std::ostream& out) {
        while (while_check_cond(this, in, out, pos, line)) {
            body->execute(in, out);
            if (control_flag == ControlState::Return) {
                break;
            } else if (control_flag == ControlState::Break) {
                control_flag = ControlState::Normal;
                break;
            } else if (control_flag == ControlState::Continue) {
                control_flag = ControlState::Normal;
                continue;
            }
        }
    }

    // Construct AmbiguousVariable from literal and put it into current scope
    void LiteralNode::execute(std::istream& in, std::ostream& out) {
        arithmetic::AmbiguousVariable* lit = new arithmetic::AmbiguousVariable();
        lit->type = type;

        switch (type) {
            case 'i':
                lit->int_val = int_val;
                break;
            case 'r':
                lit->real_val = real_val;
                break;
            case 'b':
                lit->bool_val = bool_val;
                break;
            case 's':
                lit->string_val = string_val;
                break;
            case 'e':
                break;
            case 'a':
                array_val->execute(in, out);
                lit = scopes.back().intermediates[array_val->id];
                break;
            case 't':
                tuple_val->execute(in, out);
                lit = scopes.back().intermediates[tuple_val->id];
                break;
            case 'f':
                func_val->execute(in, out);
                lit = scopes.back().intermediates[func_val->id];
                break;
            default:
                throw std::invalid_argument(
                        std::format("Error at line {}, pos {}:\n\t{}", line, pos, "Expected valid type"));
        }

        scopes.back().intermediates[id] = lit;
    }

    // Construct Array out of expressions and put it into current scope
    void ArrayLiteralNode::execute(std::istream& in, std::ostream& out) {
        arithmetic::AmbiguousVariable* arr = new arithmetic::AmbiguousVariable();
        arr->type = 'a';

        for (int i = 0; i < values.size(); ++i) {
            values[i]->execute(in, out);
            arr->array_values.push_back(scopes.back().intermediates[values[i]->id]);
            arr->array_identifiers[i + 1] = i;
        }
        scopes.back().intermediates[id] = arr;
    }

    // Construct Tuple out of expressions and put it into current scope
    void TupleLiteralNode::execute(std::istream& in, std::ostream& out) {
        arithmetic::AmbiguousVariable* tup = new arithmetic::AmbiguousVariable();
        tup->type = 't';

        for (int i = 0; i < values.size(); ++i) {
            values[i]->execute(in, out);
            tup->array_values.push_back(scopes.back().intermediates[values[i]->id]);
            if (!identifiers[i].empty()) {
                if (tup->tuple_identifiers.count(identifiers[i])) {
                    throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", line, pos,
                                                            "Cannot have multiple entries with same key in tuple"));
                }
                tup->tuple_identifiers[identifiers[i]] = i;
            }
        }
        scopes.back().intermediates[id] = tup;
    }

    // Construct Function out of variables and put it into current scope
    void FunctionNode::execute(std::istream& in, std::ostream& out) {
        arithmetic::AmbiguousVariable* foo = new arithmetic::AmbiguousVariable();
        foo->type = 'f';
        foo->function_pointer = this;
        // -> get all stuff that is currently available
        for (auto& i: scopes) {
            for (auto j: i.variables) {
                foo->function_scope[j.first] = j.second;
            }
        }
        scopes.back().intermediates[id] = foo;
    }

    void execute(ast_nodes::Node* tree, std::istream& in=std::cin, std::ostream& out=std::cout) {
        tree->execute(in, out);
    }
}

#endif // __TREE_EXECUTE_INCLUDED__
