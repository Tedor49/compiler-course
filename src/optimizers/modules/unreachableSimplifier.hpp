#ifndef __OPTIMIZERS_UNREACHABLE_SIMPLIFIER_INCLUDED__
#define __OPTIMIZERS_UNREACHABLE_SIMPLIFIER_INCLUDED__

#include <iostream>
#include <vector>

#include "../../ast_lib.hpp"

namespace optimizers {
    namespace unreachable_simplifier {

        const std::string name = "unreachable_simplifier";

        void at_enter (ast_nodes::Node* node) {
            ast_nodes::ControlNode* control_node = dynamic_cast<ast_nodes::ControlNode*>(node);

            if (control_node != nullptr) {
                ast_nodes::BodyNode*       body_node =                 dynamic_cast<ast_nodes::BodyNode*>     (control_node->parent);

                std::vector<ast_nodes::Node*> statements_to_change;
                std::vector<ast_nodes::Node*> resulting_statements;

                statements_to_change = body_node->statements;

                for (auto & i : statements_to_change) {
                    resulting_statements.push_back(i);
                    if (i == node) {
                        break;
                    }
                }

                body_node->statements = resulting_statements;
            }
        }

        void optimize(ast_nodes::Node* tree, std::ostream* log=&std::cerr) {
            tree->visit(at_enter, ast_nodes::dummy, ast_nodes::dummy);
            reassign_ids(tree);
			assign_parents(tree);
        }
    }
}

#endif // __OPTIMIZERS_UNREACHABLE_SIMPLIFIER_INCLUDED__
