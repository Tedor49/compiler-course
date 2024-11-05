#include "../syntax_tree_nodes.hpp"
#include "../optimizers/optimize.hpp"
#include "../cmd_utils.hpp"

int main(int argc, char *argv[]) {
    cmd_utils::input_params param;
    cmd_utils::parse_args(argc, argv, param);

    ast_nodes::Node* tree = ast_nodes::readTree(*param.in_stream);

    ast_nodes::human_output = param.human;
    optimizers::verbose = param.verbose;

    optimizers::optimize(tree, &std::cout);

    if(param.out_is_file){
        (*param.out_stream) << tree;
        (*param.out_stream).close();
    } else {
        std::cout << tree;
    }
}
