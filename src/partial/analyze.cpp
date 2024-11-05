#include "../syntax_tree_nodes.hpp"
#include "../analyzers/analyze.hpp"
#include "../cmd_utils.hpp"

int main(int argc, char *argv[]) {
    cmd_utils::input_params param;
    cmd_utils::parse_args(argc, argv, param);

    ast_nodes::Node* tree = ast_nodes::readTree(*param.in_stream);

    analyzers::verbose = param.verbose;

    analyzers::analyze(tree, &std::cout);
}