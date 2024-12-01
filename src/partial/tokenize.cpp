#include <vector>
#include <string>

#include "../token_data.hpp"
#include "../cmd_utils.hpp"

int main(int argc, char* argv[]) {
    cmd_utils::input_params param;
    cmd_utils::parse_args(argc, argv, param);

    tokens::human_output = param.human;

    std::string prog = cmd_utils::readfile(*param.in_stream);

    std::vector <tokens::Token> tokenized;
    tokens::tokenize(prog, tokenized);

    if (param.out_is_file) {
        (*param.out_stream) << tokenized;
        (*param.out_stream).close();
    } else {
        std::cout << tokenized;
    }
}
