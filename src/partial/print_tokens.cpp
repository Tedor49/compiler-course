#include "../token_data.hpp"
#include "../cmd_utils.hpp"

int main(int argc, char* argv[]) {
    cmd_utils::input_params param;
    cmd_utils::parse_args(argc, argv, param);

    tokens::human_output = param.human;

    std::vector <tokens::Token> tokenized;
    (*param.in_stream) >> tokenized;

    if (param.out_is_file) {
        (*param.out_stream) << tokenized;
        (*param.out_stream).close();
    } else {
        std::cout << tokenized;
    }
}
