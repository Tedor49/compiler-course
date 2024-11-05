#include <vector>
#include <string>

#include "cmd_utils.hpp"
#include "token_data.hpp"
#include "syntax_tree_nodes.hpp"
#include "analyzers/analyze.hpp"
#include "optimizers/optimize.hpp"

int main(int argc, char *argv[]) {
    cmd_utils::input_params param;
    cmd_utils::parse_args(argc, argv, param);

    tokens::human_output = param.human;

    std::string prog = cmd_utils::readfile(*param.in_stream);

    std::vector<tokens::Token> tokenized;
    tokens::tokenize(prog, tokenized);

    ast_nodes::Node* tree = ast_nodes::construct(tokenized);

    analyzers::verbose = param.verbose;
    optimizers::verbose = param.verbose;

    analyzers::analyze(tree, &std::cout);
    for (int iter = 0; iter < 3; ++iter) optimizers::optimize(tree, &std::cout);

    if(param.out_is_file){
        (*param.out_stream) << tree;
        (*param.out_stream).close();
    } else {
        std::cout << tree;
    }
}
