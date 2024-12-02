#include "../ast_lib.hpp"
#include "../cmd_utils.hpp"

int main(int argc, char* argv[]) {
    cmd_utils::input_params param;
    cmd_utils::parse_args(argc, argv, param);

    ast_nodes::human_output = param.human;

    ast_nodes::Node* tree = ast_nodes::readTree(*param.in_stream);

    if (param.out_is_file) {
        (*param.out_stream) << tree;
        (*param.out_stream).close();
    } else {
        std::cout << tree;
    }
}
