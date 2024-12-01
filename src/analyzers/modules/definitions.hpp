#ifndef __ANALYZERS_DEFINITIONS_INCLUDED__
#define __ANALYZERS_DEFINITIONS_INCLUDED__

#include <iostream>
#include <vector>
#include <string>
#include <format>

#include "../../ast_lib.hpp"

namespace analyzers {
    namespace definitions {

        const std::string name = "definitions";

        std::vector<int> scope_wraps;
        std::vector <std::vector<std::string>> scopes;

        void at_enter(ast_nodes::Node* node) {
            ast_nodes::DeclarationNode* decl_node =    dynamic_cast<ast_nodes::DeclarationNode*>(node); // .identifier
            ast_nodes::ForNode*         for_node =     dynamic_cast<ast_nodes::ForNode*>        (node); // .identifier
            ast_nodes::PrimaryNode*     primary_node = dynamic_cast<ast_nodes::PrimaryNode*>    (node); // .identifier
            ast_nodes::FunctionNode*    func_node =    dynamic_cast<ast_nodes::FunctionNode*>   (node); // .params
            ast_nodes::BodyNode*        body_node =    dynamic_cast<ast_nodes::BodyNode*>       (node);

            if (decl_node != nullptr) {
                scopes.back().push_back(decl_node->identifier);
            } else if (for_node != nullptr) {
                scope_wraps.push_back(node->id);
                scopes.push_back(std::vector<std::string>());
                scopes.back().push_back(for_node->identifier);
            } else if (func_node != nullptr) {
                scope_wraps.push_back(node->id);
                scopes.push_back(std::vector<std::string>(func_node->params.begin(), func_node->params.end()));
            } else if (body_node != nullptr) {
                scope_wraps.push_back(node->id);
                scopes.push_back(std::vector<std::string>());
            } else if (primary_node != nullptr) {
                if (primary_node->type != 'v') return;

                bool exists = false;
                for (auto& i: scopes) {
                    for (auto j: i) {
                        if (j == primary_node->identifier) {
                            exists = true;
                            break;
                        }
                    }
                }

                if (!exists) {
                    throw std::invalid_argument(
                            std::format("Error at line {}, pos {}:\n\tUndefined variable {}", node->line, node->pos,
                                        primary_node->identifier));
                }
            }

            return;
        }

        void at_exit(ast_nodes::Node* node) {
            if (scope_wraps.back() == node->id) {
                scope_wraps.pop_back();
                scopes.pop_back();
            }
            return;
        }

        void analyze(ast_nodes::Node* tree, std::ostream* log = &std::cerr) {

            scope_wraps.clear();
            scopes.clear();

            scope_wraps.push_back(-1);
            scopes.resize(1);

            tree->visit(at_enter, ast_nodes::dummy, at_exit);
        }
    }
}

#endif // __ANALYZERS_DEFINITIONS_INCLUDED__
