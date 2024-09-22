#include <filesystem>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <format>
#include <cstring>

#include "token_data.hpp"

using namespace std;

string readfile (ifstream& in) {
    stringstream buf;

    buf << in.rdbuf();
    in.close();

    return buf.str();
}

struct input_params{
    ifstream* in_stream;
    bool out_is_file;
    ofstream* out_stream;
    bool human;
};

void parse_args(int& argc, char* argv[], input_params& par){

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " infile\n";
        cerr << "  infile      path to input file\n";
        throw invalid_argument("No input file specified");
    }

    if (!filesystem::is_regular_file(argv[1]) && !filesystem::is_symlink(argv[1])){
        cerr << "Usage: " << argv[0] << " infile\n";
        cerr << "  infile      path to input file\n";
        throw invalid_argument("Input file does not refer to a file");
    }

    ifstream* in = new ifstream(argv[1]);

    if (in->fail()){
        cerr << "Usage: " << argv[0] << " infile\n";
        cerr << "  infile      path to input file\n";
        throw invalid_argument("Could not open input file");
    }

    par.in_stream = in;
    par.out_stream = nullptr;
    par.out_is_file = false;
    par.human = true;
}


int main(int argc, char *argv[]) {
    input_params param;
    parse_args(argc, argv, param);

    human_output = param.human;

    vector<Token> tokenized;
    (*param.in_stream) >> tokenized;

    cout << tokenized;
}
