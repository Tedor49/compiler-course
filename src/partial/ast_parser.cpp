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

    int walker = 0;
    try {
        ast_nodes::ProgramNode tree;
        tree.from_tokens(tokenized, walker);

        if(param.out_is_file){
            (*param.out_stream) << &tree;
            (*param.out_stream).close();
        } else {
            std::cout << &tree;
        }

    } catch (std::invalid_argument& ex) {
        throw std::invalid_argument(std::format("{} at line {}, pos {}", ex.what(), tokenized[walker].line, tokenized[walker].pos));
    } catch (std::out_of_range& ex) {
        throw std::invalid_argument("Unexpected program end");
    }
}
