#include <vector>
#include <format>

#include "../token_data.hpp"
#include "../syntax_tree_nodes.hpp"
#include "../cmd_utils.hpp"

int main(int argc, char *argv[]){
    cmd_utils::input_params param;
    cmd_utils::parse_args(argc, argv, param);

    ast_nodes::human_output = param.human;

    std::vector<tokens::Token> tokenized;
    (*param.in_stream) >> tokenized;

    ast_nodes::Node* tree = ast_nodes::construct(tokenized);

    if(param.out_is_file){
        (*param.out_stream) << tree;
        (*param.out_stream).close();
    } else {
        std::cout << tree;
    }
}
