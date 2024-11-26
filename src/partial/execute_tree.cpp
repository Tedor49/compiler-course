#include "../ast_lib.hpp"
#include "../cmd_utils.hpp"

int main(int argc, char *argv[]) {
    cmd_utils::input_params param;
    cmd_utils::parse_args(argc, argv, param);

    ast_nodes::Node* tree = ast_nodes::readTree(*param.in_stream);

    ast_nodes::execute(tree, std::cin, std::cout);
}
