#ifndef __ANALYZERS_INCLUDED__
#define __ANALYZERS_INCLUDED__

#include <vector>
#include <string>
#include <iostream>

#include "../ast_lib.hpp"
#include "./modules/keywords.hpp"
#include "./modules/definitions.hpp"
#include "./modules/immutable.hpp"

namespace analyzers {

    bool verbose = false;

    struct analyzer_data {
        std::string name;

        void (* analyze)(ast_nodes::Node*, std::ostream*);
    };

    void analyze(ast_nodes::Node* tree, std::ostream* log = &std::cerr) {
        std::vector <analyzer_data> analyzers = {
                {keywords::name,    keywords::analyze},
                {definitions::name, definitions::analyze},
                {immutable::name,   immutable::analyze}
        };

        for (analyzer_data& i: analyzers) {
            if (verbose) (*log) << "Scanning with " << i.name << "...\n";
            i.analyze(tree, log);
            if (verbose) (*log) << "Success!\n";
        }
    }
}

#endif // __ANALYZERS_INCLUDED__
