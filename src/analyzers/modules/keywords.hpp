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
            bool is_ret = dynamic_cast<ast_nodes::ReturnNode*>(node) != nullptr;

            if (is_foo) {
                foo_wraps.push_back(node->id);
            } else if (is_ret) {
                if (foo_wraps.size() == 0) {
                    throw std::invalid_argument("Return statement must be inside of function!");
                }
            }

            return;
        }

        void at_exit (ast_nodes::Node* node) {
            if (foo_wraps.size() != 0 && foo_wraps.back() == node->id) {
                foo_wraps.pop_back();
            }
            return;
        }

        void analyze (ast_nodes::Node* tree, std::ostream* log=&std::cerr) {
            foo_wraps.clear();

            tree->visit(at_enter, ast_nodes::dummy, at_exit);
        }
    }
}

#endif // __ANALYZERS_KEYWORDS_INCLUDED__
