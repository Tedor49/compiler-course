#ifndef __INTERPRETER_INCLUDED__
#define __INTERPRETER_INCLUDED__

#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <typeinfo>

#include "syntax_tree_nodes.hpp"
#include "arithmetic.hpp"	

namespace interpreter {

    void at_enter (ast_nodes::Node* node);
    void at_exit (ast_nodes::Node* node);

    std::ostream& out = std::cout;
    std::istream& in = std::cin;
	
	bool returned_flag = false;
	arithmetic::AmbiguousVariable* return_register;

    struct scopeinfo {
        int node_id;
        std::unordered_map<std::string, arithmetic::AmbiguousVariable*> variables;
        std::unordered_map<int, arithmetic::AmbiguousVariable*> intermediates;
    };
	
	std::ostream& operator<<(std::ostream& out, scopeinfo& var) {
        out << "Scope " << var.node_id << std::endl;
		out << "Variables:" << std::endl;
		for (auto i : var.variables) {
			out << '\t' << i.first << ' ' << *i.second << std::endl;
		}
		out << "Intermediates:" << std::endl;
		for (auto i : var.intermediates) {
			//out << i.first << std::endl;
			//out << i.second << std::endl;
			//out << "WHAT THE SIGMA" << std::endl;
			out << '\t' << i.first << ' ' << *i.second << std::endl;
		}
		out << "END OF SCOPE" << std::endl;
		return out;
    }

    std::vector<scopeinfo> scopes;
	
	void open_scope (ast_nodes::Node* parent) {
		scopes.resize(scopes.size() + 1);
        scopes.back().node_id = parent->id;
	}
	
	void close_scope (ast_nodes::Node* parent) {
		if (scopes.size() == 0) {
            throw std::invalid_argument("Scope does not exist while closing");
        }
		while (scopes.back().node_id != parent->id) {
			scopes.resize(scopes.size() - 1);
			if (scopes.size() == 0) {
				throw std::invalid_argument("Scope does not exist while closing");
			}			
		}
		scopes.resize(scopes.size() - 1);
	}
	
    arithmetic::AmbiguousVariable* get_variable (std::string identifier) {
        for (int i = (signed)(scopes.size()) - 1; i >= 0; --i) {
            if (scopes[i].variables.count(identifier)) {
                return scopes[i].variables[identifier];
            }
        }
        throw std::invalid_argument(std::format("Variable {} referenced before declaration", identifier));
    }

    //Create new scope
    void handle_body_en (ast_nodes::BodyNode* node) {
        open_scope(node);
    }

    //Delete scope
    void handle_body_ex (ast_nodes::BodyNode* node) {
        close_scope(node);
    }

    //Adds variables to current scope
    void handle_dec_ex (ast_nodes::DeclarationNode* node) {
        if (node->value != nullptr) {
			scopes.back().variables[node->identifier] = scopes.back().intermediates[node->value->id];
		} else {
			scopes.back().variables[node->identifier] = new arithmetic::AmbiguousVariable();
		}
    }

    // Evaluate expression and put its value into current scope
    void handle_expr_ex (ast_nodes::ExpressionNode* node) {
        std::vector<arithmetic::AmbiguousVariable*> terms;
        terms.reserve(node->terms.size());

        for (auto i : node->terms) {
            terms.push_back(scopes.back().intermediates[i->id]);
        }

        scopes.back().intermediates[node->id] = evaluate_expression(terms, node->ops);
    }

    // Evaluate unary operator and put its value into current scope
    void handle_unar_ex (ast_nodes::UnaryNode* node) {
        arithmetic::AmbiguousVariable* new_term = perform_unary_op(node->unaryop, scopes.back().intermediates[node->primary->id]);
        if (node->type_ind == '#') {
            scopes.back().intermediates[node->id] = new_term;
        } else {
            arithmetic::AmbiguousVariable* is_type = new arithmetic::AmbiguousVariable();
            is_type->type = 'b';
            is_type->bool_val = (node->type_ind == new_term->type);
            scopes.back().intermediates[node->id] = is_type;
        }
    }

    // Evaluate primary and put its value into current scope
    void handle_prim_ex (ast_nodes::PrimaryNode* node) {
		if (node->type == 'i') {
			arithmetic::AmbiguousVariable* inputted_value = new arithmetic::AmbiguousVariable();
			inputted_value->type = 'i';
			in >> inputted_value->int_val;
			scopes.back().intermediates[node->id] = inputted_value;
		} else if (node->type == 'r') {
			arithmetic::AmbiguousVariable* inputted_value = new arithmetic::AmbiguousVariable();
			inputted_value->type = 'r';
			in >> inputted_value->real_val;
			scopes.back().intermediates[node->id] = inputted_value;
		} else if (node->type == 's') {
			arithmetic::AmbiguousVariable* inputted_value = new arithmetic::AmbiguousVariable();
			inputted_value->type = 's';
			in >> inputted_value->string_val;
			scopes.back().intermediates[node->id] = inputted_value;
		} else if (node->type == 'v') {
			arithmetic::AmbiguousVariable* var = get_variable(node->identifier);
			//var.type = 'f';
			
			for (ast_nodes::Node* i : node->tails) {
				ast_nodes::TailNode* tail = dynamic_cast<ast_nodes::TailNode*> (i);
				
				if (tail->type == 't') {
					if (var->type != 't') {
						throw std::invalid_argument("Expected tuple");
					}
					
					if (var->array_values.size() < tail->tuple_idx - 1) {
						throw std::invalid_argument("Tuple index out of range");
					}
					
					var = var->array_values[tail->tuple_idx - 1];
				} else if (tail->type == 'i') {
					if (var->type != 't') {
						throw std::invalid_argument("Expected tuple");
					}
					
					if (var->tuple_identifiers.count(tail->identifier) == 0) {
						throw std::invalid_argument("Tuple identifier not present");
					}
					
					var = var->array_values[var->tuple_identifiers[tail->identifier]];
				} else if (tail->type == 'p') {
					if (var->type != 'f') {
						throw std::invalid_argument("Expected function");
					}
					
					ast_nodes::FunctionNode* foo = var->function_pointer;
					
					if (foo->params.size() != tail->params.size()) {
						throw std::invalid_argument(std::format("Argument amount mismatch: Expected: {} got: {}", foo->params.size(), tail->params.size()));
					}
					
					open_scope(i);
					
					for (int j = 0; j < foo->params.size(); ++j) {
						scopes.back().variables[foo->params[j]] = scopes[scopes.size() - 2].intermediates[tail->params[j]->id];
					}
					
					foo->body->visit(at_enter, ast_nodes::dummy, at_exit, false);
					
					if (foo->type == 'l') {
						//std::cout << "IM EXISTING" << std::endl;
						return_register = scopes.back().intermediates[foo->body->id];
						//std::cout << return_register << std::endl;
					}
					
					//std::cout << "COPYING..." << std::endl;
					var = arithmetic::copy(return_register);
					//std::cout << "FINISHED COPYING..." << std::endl;
					
					close_scope(i);
				} else if (tail->type == 's') {
					if (var->type != 'a') {
						throw std::invalid_argument("Expected array");
					}
					
					arithmetic::AmbiguousVariable* sub = scopes.back().intermediates[tail->subscript->id];
					if (sub->type != 'i') {
						throw std::invalid_argument("Expected integer as array index");
					}
					
					if (var->array_identifiers.count(sub->int_val) == 0) {
						var->array_identifiers[sub->int_val] = var->array_values.size();
						var->array_values.push_back(new arithmetic::AmbiguousVariable());
					}
					
					var = var->array_values[var->array_identifiers[sub->int_val]];
				} else {
					throw std::invalid_argument("Expected valid type");
				}
			}
			scopes.back().intermediates[node->id] = var;
		} else if (node->type == 'l') {
			scopes.back().intermediates[node->id] = scopes.back().intermediates[node->literal->id];
		} else if (node->type == 'e') {
			scopes.back().intermediates[node->id] = scopes.back().intermediates[node->expression->id];
		} else {
			throw std::invalid_argument("Expected valid type");
		}
    }

    // Assign value to variable in current scope
    void handle_asgn_ex (ast_nodes::AssignmentNode* node) {
		*scopes.back().intermediates[node->primary->id] = *scopes.back().intermediates[node->expression->id];
    }

    // Print some expressions
    void handle_print_ex (ast_nodes::PrintNode* node) {
		
		//for (auto i : scopes) {
		//	out << i << std::endl;
		//}
		
		//out << "SKIBIDI" << std::endl;
        for (auto i : node->values) {
			out << *scopes.back().intermediates[i->id];
        }
    }

    // Return some expression from function
    void handle_ret_ex (ast_nodes::ReturnNode* node) {
		returned_flag = true;
		return_register = scopes.back().intermediates[node->value->id];
        return;
    }

    // Handle if statement and execute one of the bodies
    void handle_if_ex (ast_nodes::IfNode* node) {
        const arithmetic::AmbiguousVariable* val = scopes.back().intermediates[node->expression->id];
        if (val->type == 'b') {
            if (val->bool_val) {
                node->if_body->visit(at_enter, ast_nodes::dummy, at_exit, false);
            } else if (node->else_body != nullptr) {
                node->else_body->visit(at_enter, ast_nodes::dummy, at_exit, false);
            }
        } else {
            throw std::invalid_argument("If statement does not contain boolean as argument");
        }
    }

    // Create new scope for iteration variable and corresponding intermediates
	void handle_for_en (ast_nodes::ForNode* node) {
		open_scope(node);
	}
	
	// Handle for statement and execute the body multiple times with some new variable
    void handle_for_ex (ast_nodes::ForNode* node) {
		arithmetic::AmbiguousVariable* rng_l = scopes.back().intermediates[node->range_expr_l->id];
		arithmetic::AmbiguousVariable* rng_r = scopes.back().intermediates[node->range_expr_r->id];
		
		if (rng_l->type != 'i') {
			throw std::invalid_argument("Left bound of for loop is not and integer");
		}
		
		if (rng_r->type != 'i') {
			std::cout << *rng_r << std::endl;
			for (auto i : scopes) {
				out << i << std::endl;
			}
			throw std::invalid_argument("Right bound of for loop is not and integer");
		}
		
		scopes.back().variables[node->identifier] = new arithmetic::AmbiguousVariable();
		scopes.back().variables[node->identifier]->type = 'i';
		for (long long i = rng_l->int_val; i <= rng_r->int_val; ++i) {
			scopes.back().variables[node->identifier]->int_val = i;
			node->body->visit(at_enter, ast_nodes::dummy, at_exit, false);
			//for (auto i : scopes) {
			//	out << i << std::endl;
			//}
			if (returned_flag) break;
		}
		close_scope(node);
    }

    // Handle while statement and execute the body multiple times
    void handle_whl_ex (ast_nodes::WhileNode* node) {
		while (true) {			
			arithmetic::AmbiguousVariable* val = scopes.back().intermediates[node->expression->id];

			if (val->type != 'b') {
				throw std::invalid_argument("Expression in while loop does not evaluate to an integer");
			}

			if (!val->bool_val) break;

			node->body->visit(at_enter, ast_nodes::dummy, at_exit, false);
			
			node->expression->visit(at_enter, ast_nodes::dummy, at_exit, false);
			if (returned_flag) break;
		}
    }

    // Construct AmbiguousVariable from literal and put it into current scope
    void handle_lit_ex (ast_nodes::LiteralNode* node) {
		arithmetic::AmbiguousVariable* lit = new arithmetic::AmbiguousVariable();
		lit->type = node->type;
		
		switch (node->type) {
			case 'i':
				lit->int_val = node->int_val;
				break;
			case 'r':
				lit->real_val = node->real_val;
				break;
			case 'b':
				lit->bool_val = node->bool_val;
				break;
			case 's':
				lit->string_val = node->string_val;
				break;
			case 'e':
				break;
			case 'a':
				lit = scopes.back().intermediates[node->array_val->id];
				break;
			case 't':
				lit = scopes.back().intermediates[node->tuple_val->id];
				break;
			case 'f':
				lit = scopes.back().intermediates[node->func_val->id];
				break;
			default:
				throw std::invalid_argument("Expected valid type");
		}
		
		scopes.back().intermediates[node->id] = lit;
    }

    // Construct Array out of expressions and put it into current scope
    void handle_arr_ex (ast_nodes::ArrayLiteralNode* node) {
        arithmetic::AmbiguousVariable* arr = new arithmetic::AmbiguousVariable();
		arr->type = 'a';
		for (int i = 0; i < node->values.size(); ++i) {
			arr->array_values.push_back(scopes.back().intermediates[node->values[i]->id]);
			arr->array_identifiers[i + 1] = i;
		}
		scopes.back().intermediates[node->id] = arr;
    }

    // Construct Tuple out of expressions and put it into current scope
    void handle_tup_ex (ast_nodes::TupleLiteralNode* node) {
        arithmetic::AmbiguousVariable* tup = new arithmetic::AmbiguousVariable();
		tup->type = 't';
		for (int i = 0; i < node->values.size(); ++i) {
			tup->array_values.push_back(scopes.back().intermediates[node->values[i]->id]);
			if (!node->identifiers[i].empty()) {
				tup->tuple_identifiers[node->identifiers[i]] = i;
			}
		}
		scopes.back().intermediates[node->id] = tup;
    }

    // Construct Function out of variables and put it into current scope
    void handle_func_ex (ast_nodes::FunctionNode* node) {
		arithmetic::AmbiguousVariable* foo = new arithmetic::AmbiguousVariable();
		foo->type = 'f';
		foo->function_pointer = node;
		scopes.back().intermediates[node->id] = foo;
    }
	
	 void at_enter (ast_nodes::Node* node) {
        ast_nodes::BodyNode* body_node = dynamic_cast<ast_nodes::BodyNode*>(node);
        ast_nodes::ForNode*  for_node =  dynamic_cast<ast_nodes::ForNode*> (node);
		
		if (returned_flag) {
			return;
		}
				
		if (body_node != nullptr) {
            handle_body_en(body_node);
        } else if (for_node != nullptr) {
            handle_for_en(for_node);
        }

    }

    void at_exit (ast_nodes::Node* node) {
        ast_nodes::BodyNode*         body_node =  dynamic_cast<ast_nodes::BodyNode*>        (node);
        ast_nodes::DeclarationNode*  dec_node =   dynamic_cast<ast_nodes::DeclarationNode*> (node);
        ast_nodes::ExpressionNode*   expr_node =  dynamic_cast<ast_nodes::ExpressionNode*>  (node);
        ast_nodes::UnaryNode*        unar_node =  dynamic_cast<ast_nodes::UnaryNode*>       (node);
        ast_nodes::PrimaryNode*      prim_node =  dynamic_cast<ast_nodes::PrimaryNode*>     (node);
        ast_nodes::AssignmentNode*   asgn_node =  dynamic_cast<ast_nodes::AssignmentNode*>  (node);
        ast_nodes::PrintNode*        print_node = dynamic_cast<ast_nodes::PrintNode*>       (node);
        ast_nodes::ReturnNode*       ret_node =   dynamic_cast<ast_nodes::ReturnNode*>      (node);
        ast_nodes::IfNode*           if_node =    dynamic_cast<ast_nodes::IfNode*>          (node);
        ast_nodes::ForNode*          for_node =   dynamic_cast<ast_nodes::ForNode*>         (node);
        ast_nodes::WhileNode*        whl_node =   dynamic_cast<ast_nodes::WhileNode*>       (node);
        ast_nodes::LiteralNode*      lit_node =   dynamic_cast<ast_nodes::LiteralNode*>     (node);
        ast_nodes::ArrayLiteralNode* arr_node =   dynamic_cast<ast_nodes::ArrayLiteralNode*>(node);
        ast_nodes::TupleLiteralNode* tup_node =   dynamic_cast<ast_nodes::TupleLiteralNode*>(node);
        ast_nodes::FunctionNode*     func_node =  dynamic_cast<ast_nodes::FunctionNode*>    (node);

		if (returned_flag) {
			if (body_node != nullptr && dynamic_cast<ast_nodes::FunctionNode*>(body_node->parent) != nullptr) {
				returned_flag = false;
			} else {
				return;
			}
		}

        if (body_node != nullptr) {
            handle_body_ex(body_node);
        } else if (dec_node != nullptr) {
            handle_dec_ex(dec_node);
        } else if (expr_node != nullptr) {
            handle_expr_ex(expr_node);
        } else if (unar_node != nullptr) {
            handle_unar_ex(unar_node);
        } else if (prim_node != nullptr) {
            handle_prim_ex(prim_node);
        } else if (asgn_node != nullptr) {
            handle_asgn_ex(asgn_node);
        } else if (print_node != nullptr) {
            handle_print_ex(print_node);
        } else if (ret_node != nullptr) {
            handle_ret_ex(ret_node);
        } else if (if_node != nullptr) {
            handle_if_ex(if_node);
        } else if (for_node != nullptr) {
            handle_for_ex(for_node);
        } else if (whl_node != nullptr) {
            handle_whl_ex(whl_node);
        } else if (lit_node != nullptr) {
            handle_lit_ex(lit_node);
        } else if (arr_node != nullptr) {
            handle_arr_ex(arr_node);
        } else if (tup_node != nullptr) {
            handle_tup_ex(tup_node);
        } else if (func_node != nullptr) {
            handle_func_ex(func_node);
        }

    }

    void execute(ast_nodes::Node* tree, std::ostream* log=&std::cout) {
        //out = log;
        tree->visit(at_enter, ast_nodes::dummy, at_exit, false);
    }
}

#endif // __INTERPRETER_INCLUDED__
