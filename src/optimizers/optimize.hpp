#ifndef __OPTIMIZERS_INCLUDED__
#define __OPTIMIZERS_INCLUDED__

#include <vector>
#include <string>
#include <iostream>

#include "../ast_lib.hpp"
#include "./modules/ifSimplifier.hpp"
#include "./modules/unreachableSimplifier.hpp"
#include "./modules/constExprSimplifier.hpp"

namespace optimizers {

    bool verbose = false;

    struct optimizer_data {
        std::string name;

        void (* optimize)(ast_nodes::Node*, std::ostream*);
    };

    void optimize(ast_nodes::Node* tree, std::ostream* log = &std::cerr) {
        std::vector <optimizer_data> optimizers = {
                {if_simplifier::name,          if_simplifier::optimize},
                {unreachable_simplifier::name, unreachable_simplifier::optimize},
                {const_simplifier::name, const_simplifier::optimize}
        };

        for (optimizer_data& i: optimizers) {
            if (verbose) (*log) << "Optimizing with " << i.name << "...\n";
            i.optimize(tree, log);
            if (verbose) (*log) << "Success!\n";
        }
    }
}

#endif // __OPTIMIZERS_INCLUDED__
