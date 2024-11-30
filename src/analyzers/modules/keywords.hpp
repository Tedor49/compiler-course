#ifndef __ANALYZERS_KEYWORDS_INCLUDED__
#define __ANALYZERS_KEYWORDS_INCLUDED__

#include <vector>
#include <format>
#include <string>
#include <iostream>

#include "../../ast_lib.hpp"

namespace analyzers {
    namespace keywords {

        const std::string name = "keywords";

        std::vector<int> foo_wraps;

        void at_enter (ast_nodes::Node* node) {
            bool is_foo = dynamic_cast<ast_nodes::FunctionNode*>(node) != nullptr;
            bool is_loop = dynamic_cast<ast_nodes::ForNode*>(node) != nullptr || dynamic_cast<ast_nodes::WhileNode*>(node) != nullptr;
            ast_nodes::ControlNode* control = dynamic_cast<ast_nodes::ControlNode*>(node);

            if (is_foo) {
                foo_wraps.push_back(0);
            } else if (is_loop) {
                foo_wraps.back() += 1;
            } else if (control != nullptr) {
				switch (control->type) {
					case 'b':
					case 'c':
						if (foo_wraps.back() == 0) {
							throw std::invalid_argument("Break or continue statement outside of loop");
						}
						break;
					case 'r':
						if (foo_wraps.size() == 1) {
							throw std::invalid_argument("Return statement outside of function");
						}
						break;
					default:
						throw std::invalid_argument("CONTROL NODE HAS INVALID TYPE");
				}
			}

            return;
        }

        void at_exit (ast_nodes::Node* node) {
            bool is_foo = dynamic_cast<ast_nodes::FunctionNode*>(node) != nullptr;
            bool is_loop = dynamic_cast<ast_nodes::ForNode*>(node) != nullptr || dynamic_cast<ast_nodes::WhileNode*>(node) != nullptr;
            
			if (is_foo) {
				foo_wraps.pop_back();
			} else if (is_loop) {
				foo_wraps.back() -= 1;
			}
			
            return;
        }

        void analyze (ast_nodes::Node* tree, std::ostream* log=&std::cerr) {
            foo_wraps.clear();
			foo_wraps.push_back(0);

            tree->visit(at_enter, ast_nodes::dummy, at_exit);
        }
    }
}

#endif // __ANALYZERS_KEYWORDS_INCLUDED__
