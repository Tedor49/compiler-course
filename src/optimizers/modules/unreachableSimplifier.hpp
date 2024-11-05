#ifndef __OPTIMIZERS_UNREACHABLE_SIMPLIFIER_INCLUDED__
#define __OPTIMIZERS_UNREACHABLE_SIMPLIFIER_INCLUDED__

#include <iostream>
#include <vector>

#include "../../syntax_tree_nodes.hpp"

namespace optimizers {
    namespace unreachable_simplifier {

        const std::string name = "unreachable_simplifier";

        void at_enter (ast_nodes::Node* node) {
            ast_nodes::ReturnNode* return_node = dynamic_cast<ast_nodes::ReturnNode*>(node);

            if (return_node != nullptr) {
                ast_nodes::StatementNode* statement_node = dynamic_cast<ast_nodes::StatementNode*>(return_node->parent);
                if (statement_node == nullptr) {
                    return;
                }

                ast_nodes::ProgramNode*    programm_node =             dynamic_cast<ast_nodes::ProgramNode*>  (statement_node->parent);
                ast_nodes::BodyNode*       body_node =                 dynamic_cast<ast_nodes::BodyNode*>     (statement_node->parent);

                std::vector<ast_nodes::Node*> statements_to_change;
                std::vector<ast_nodes::Node*> resulting_statements;

                if (programm_node != nullptr) {
                    statements_to_change = programm_node->statements;
                } else {
                    statements_to_change = body_node->statements;
                }

                for (auto & i : statements_to_change) {
                    resulting_statements.push_back(i);
                    if (i == statement_node) {
                        break;
                    }
                }
                if (programm_node != nullptr) {
                    programm_node->statements = resulting_statements;
                } else {
                    body_node->statements = resulting_statements;
                }
            }
        }

        void optimize(ast_nodes::Node* tree, std::ostream* log=&std::cerr) {
            tree->visit(at_enter, ast_nodes::dummy, ast_nodes::dummy);
            reassign_ids(tree);
        }
    }
}

#endif // __OPTIMIZERS_UNREACHABLE_SIMPLIFIER_INCLUDED__
