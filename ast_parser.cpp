#include <filesystem>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <format>
#include <cstring>

#include "syntax_tree_nodes.hpp"

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

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " infile [-o outfile] [-h]\n";
        cerr << "  infile      path to input file\n";
        cerr << "  -o outfile  path to output file\n";
        cerr << "  -h          output in a human readable way\n";
        throw invalid_argument("No input file specified");
    }

    if (!filesystem::is_regular_file(argv[1]) && !filesystem::is_symlink(argv[1])){
        cerr << "Usage: " << argv[0] << " infile [-o outfile] [-h]\n";
        cerr << "  infile      path to input file\n";
        cerr << "  -o outfile  path to output file\n";
        cerr << "  -h          output in a human readable way\n";
        throw invalid_argument("Input file does not refer to a file");
    }

    ifstream* in = new ifstream(argv[1]);

    if (in->fail()){
        cerr << "Usage: " << argv[0] << " infile [-o outfile] [-h]\n";
        cerr << "  infile      path to input file\n";
        cerr << "  -o outfile  path to output file\n";
        cerr << "  -h          output in a human readable way\n";
        throw invalid_argument("Could not open input file");
    }

    par.in_stream = in;
    par.out_stream = nullptr;
    par.out_is_file = false;
    par.human = false;

    int y = 2;

    while (y != argc) {
        if (strcmp(argv[y], "-o") == 0) {
            if (y + 1 == argc){
                cerr << "Usage: " << argv[0] << " infile [-o outfile] [-h]\n";
                cerr << "  infile      path to input file\n";
                cerr << "  -o outfile  path to output file\n";
                cerr << "  -h          output in a human readable way\n";
                throw invalid_argument("No output file after flag");
            }

            ofstream* out = new ofstream(argv[y+1]);

            if (out->fail()) {
                cerr << "Usage: " << argv[0] << " infile [-o outfile] [-h]\n";
                cerr << "  infile      path to input file\n";
                cerr << "  -o outfile  path to output file\n";
                cerr << "  -h          output in a human readable way\n";
                throw invalid_argument("Could not open output file");
            }

            par.out_stream = out;
            par.out_is_file = true;
            ++y;

        } else if (strcmp(argv[y], "-h") == 0) {
            par.human = true;
        }
        ++y;
    }
}

int main(int argc, char *argv[]){
    input_params param;
    parse_args(argc, argv, param);

    vector<Token> tokenized;
    (*param.in_stream) >> tokenized;

    int walker = 0;
    try{
        ProgramNode prog(tokenized, walker);

        if(param.out_is_file){
            (*param.out_stream) << &prog;
            (*param.out_stream).close();
        } else {
            cout << &prog;
        }
    } catch (const invalid_argument& ex) {
        throw invalid_argument(format("{} at line {}, pos {}", ex.what(), tokenized[walker].line, tokenized[walker].pos));
    } catch (const out_of_range& ex) {
        throw invalid_argument("Unexpected program end");
    }
}
