#ifndef __ANALYZERS_IMMUTABLE_INCLUDED__
#define __ANALYZERS_IMMUTABLE_INCLUDED__

#include <iostream>
#include <vector>
#include <string>
#include <format>

#include "../../ast_lib.hpp"

namespace analyzers {
    namespace immutable {

        const std::string name = "immutable";
		
        void at_enter (ast_nodes::Node* node) {
            ast_nodes::AssignmentNode* asgn_node = dynamic_cast<ast_nodes::AssignmentNode*>(node);
			
			if (asgn_node == nullptr) return;
			
			if (asgn_node->type == '#') return;
			
			ast_nodes::PrimaryNode* potential_tuple = dynamic_cast<ast_nodes::PrimaryNode*>(asgn_node->primary);
			
			for (auto i : potential_tuple->tails) {
				ast_nodes::TailNode* tail = dynamic_cast<ast_nodes::TailNode*>(i);
				if (tail->type == 't' || tail->type == 'i') {
					throw std::invalid_argument(std::format("Error at line {}, pos {}:\n\t{}", node->line, node->pos, "Elements in tuple are immutable"));
				}
			}
			
            return;
        }

        void analyze (ast_nodes::Node* tree, std::ostream* log=&std::cerr) {
            tree->visit(at_enter, ast_nodes::dummy, ast_nodes::dummy);
        }
    }
}

#endif // __ANALYZERS_IMMUTABLE_INCLUDED__
