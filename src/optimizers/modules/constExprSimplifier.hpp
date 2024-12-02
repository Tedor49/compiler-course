#ifndef __OPTIMIZERS_CONSTANT_SIMPLIFIER_INCLUDED__
#define __OPTIMIZERS_CONSTANT_SIMPLIFIER_INCLUDED__

#include <iostream>
#include <vector>

#include "../../ast_lib.hpp"
#include "../../arithmetic.hpp"

namespace optimizers {
    namespace const_simplifier {

        const std::string name = "const_simplifier";

        void at_exit(ast_nodes::Node* node) {
            ast_nodes::ExpressionNode* expr_node = dynamic_cast<ast_nodes::ExpressionNode*>(node);

            if (expr_node != nullptr) {
                std::vector <arithmetic::AmbiguousVariable*> calced_terms;
                calced_terms.reserve(expr_node->terms.size());

                for (auto i: expr_node->terms) {
                    ast_nodes::UnaryNode* unary_node = dynamic_cast<ast_nodes::UnaryNode*>(i);

                    if (unary_node == nullptr) {
                        return;
                    }

                    ast_nodes::PrimaryNode* primary_node = dynamic_cast<ast_nodes::PrimaryNode*>(unary_node->primary);

                    if (primary_node == nullptr) {
                        return;
                    }

                    if (primary_node->type != 'l') {
                        return;
                    }

                    ast_nodes::LiteralNode* literal_node = dynamic_cast<ast_nodes::LiteralNode*>(primary_node->literal);

                    if (literal_node == nullptr) {
                        return;
                    }

                    arithmetic::AmbiguousVariable* value = new arithmetic::AmbiguousVariable();
                    value->type = literal_node->type;

                    switch (literal_node->type) {
                        case 'i':
                            value->int_val = literal_node->int_val;
                            break;
                        case 'r':
                            value->real_val = literal_node->real_val;
                            break;
                        case 'b':
                            value->bool_val = literal_node->bool_val;
                            break;
                        case 's':
                            value->string_val = literal_node->string_val;
                            break;
                        case 'e':
                            break;
                        default:
                            return;
                    }


                    try {
                        value = perform_unary_op(unary_node->unaryop, value);
                    } catch (std::invalid_argument& ex) {
                        return;
                    } catch (std::runtime_error& ex) {
                        return;
                    }

                    if (unary_node->type_ind != '#') {
                        arithmetic::AmbiguousVariable* is_type = new arithmetic::AmbiguousVariable();
                        is_type->type = 'b';
                        is_type->bool_val = (unary_node->type_ind == value->type);
                        value = is_type;
                    }
                    calced_terms.push_back(value);
                }

                arithmetic::AmbiguousVariable* simplified;
                try {
                    simplified = evaluate_expression(calced_terms, expr_node->ops);
                } catch (std::invalid_argument& ex) {
                    return;
                } catch (std::runtime_error& ex) {
                    return;
                }

                expr_node->ops.clear();
                expr_node->terms.resize(1);

                ast_nodes::UnaryNode* unary_node = dynamic_cast<ast_nodes::UnaryNode*>(expr_node->terms[0]);
                unary_node->unaryop = '#';
                unary_node->type_ind = '#';


                ast_nodes::PrimaryNode* primary_node = dynamic_cast<ast_nodes::PrimaryNode*>(unary_node->primary);

                primary_node->type = 'l';

                ast_nodes::LiteralNode* literal_node = dynamic_cast<ast_nodes::LiteralNode*>(primary_node->literal);

                literal_node->type = simplified->type;
                switch (simplified->type) {
                    case 'i':
                        literal_node->int_val = simplified->int_val;
                        break;
                    case 'r':
                        literal_node->real_val = simplified->real_val;
                        break;
                    case 'b':
                        literal_node->bool_val = simplified->bool_val;
                        break;
                    case 's':
                        literal_node->string_val = simplified->string_val;
                        break;
                    case 'e':
                        break;
                    default:
                        return;
                }
            }
        }

        void optimize(ast_nodes::Node* tree, std::ostream* log = &std::cerr) {
            tree->visit(ast_nodes::dummy, ast_nodes::dummy, at_exit);
            reassign_ids(tree);
            assign_parents(tree);
        }
    }
}

#endif // __OPTIMIZERS_CONSTANT_SIMPLIFIER_INCLUDED__
