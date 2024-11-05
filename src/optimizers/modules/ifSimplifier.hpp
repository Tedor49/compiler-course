#ifndef __OPTIMIZERS_IF_SIMPLIFIER_INCLUDED__
#define __OPTIMIZERS_IF_SIMPLIFIER_INCLUDED__

#include <iostream>
#include <vector>

#include "../../syntax_tree_nodes.hpp"

namespace optimizers {
    namespace if_simplifier {

        const std::string name = "if_simplifier";

        void at_enter (ast_nodes::Node* node) {
            ast_nodes::IfNode*                  if_node =      dynamic_cast<ast_nodes::IfNode*>(node);

            if (if_node != nullptr) {
                ast_nodes::ExpressionNode* expression_node = dynamic_cast<ast_nodes::ExpressionNode*>(if_node->expression);
                if (expression_node->terms.size() != 1) {
                    return;
                }

                ast_nodes::UnaryNode* unary_node = dynamic_cast<ast_nodes::UnaryNode*>(expression_node->terms[0]);
                ast_nodes::PrimaryNode* primary_node = dynamic_cast<ast_nodes::PrimaryNode*>(unary_node->primary);
                if (primary_node->type != 'l') {
                    return;
                }

                ast_nodes::LiteralNode* literal_node = dynamic_cast<ast_nodes::LiteralNode*>(primary_node->literal);
                if (literal_node->type != 'b') {
                    return;
                }

                ast_nodes::ProgramNode*    programm_node =             dynamic_cast<ast_nodes::ProgramNode*>  (node->parent->parent);
                ast_nodes::BodyNode*       body_node =                 dynamic_cast<ast_nodes::BodyNode*>     (node->parent->parent);

                std::vector<ast_nodes::Node*> statements_to_change;
                std::vector<ast_nodes::Node*> resulting_statements;

                if (programm_node != nullptr) {
                    statements_to_change = programm_node->statements;
                } else {
                    statements_to_change = body_node->statements;
                }

        //        cout << "If node with True or False detected at " << nodes << endl;

                if (literal_node->bool_val) {
                    ast_nodes::BodyNode* if_body_node = dynamic_cast<ast_nodes::BodyNode*> (if_node->if_body);

                    for (auto & i : statements_to_change) {
                        if (i == node->parent) {
                            for (auto & j : if_body_node->statements) {
                                resulting_statements.push_back(j);
                            }
                        } else {
                            resulting_statements.push_back(i);
                        }
                    }
                    if (programm_node != nullptr) {
                        programm_node->statements = resulting_statements;
                    } else {
                        body_node->statements = resulting_statements;
                    }
                    return;
                } else {
                    if (if_node->else_body == nullptr) {
                        for (auto & i : statements_to_change) {
                            if (i == node->parent) {
                                continue;
                            } else {
                                resulting_statements.push_back(i);
                            }
                        }
                        if (programm_node != nullptr) {
                            programm_node->statements = resulting_statements;
                        } else {
                            body_node->statements = resulting_statements;
                        }
                        return;
                    }

                    ast_nodes::BodyNode* else_body_node = dynamic_cast<ast_nodes::BodyNode*> (if_node->else_body);

                    for (auto & i : statements_to_change) {
                        if (i == node->parent) {
                            for (auto & j : else_body_node->statements) {
                                resulting_statements.push_back(j);
                            }
                        } else {
                            resulting_statements.push_back(i);
                        }
                    }
                    if (programm_node != nullptr) {
                        programm_node->statements = resulting_statements;
                    } else {
                        body_node->statements = resulting_statements;
                    }
                    return;
                }
            }

            return;
        }

        void optimize(ast_nodes::Node* tree, std::ostream* log=&std::cerr) {
            tree->visit(at_enter, ast_nodes::dummy, ast_nodes::dummy);
            reassign_ids(tree);
        }
    }
}

#endif // __OPTIMIZERS_IF_SIMPLIFIER_INCLUDED__
