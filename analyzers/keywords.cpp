#include <filesystem>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <format>
#include <cstring>

#include "../syntax_tree_nodes.hpp"

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

int nodes = 0;

bool prev_foo = false;

vector<int> foo_wraps;

void at_enter (Node* node) {
    ++nodes;

    bool is_body = dynamic_cast<BodyNode*>(node) != nullptr;
    bool is_ret = dynamic_cast<ReturnNode*>(node) != nullptr;

    if (is_body) {
        if (prev_foo) {
            foo_wraps.push_back(node->id);
        }
    } else if (is_ret) {
        if (foo_wraps.size() == 0) {
            throw invalid_argument("Return statement must be inside of function!");
        }
    }

    prev_foo = dynamic_cast<FunctionNode*>(node) != nullptr;

    return;
}

void at_repeat (Node* node) {
    return;
}

void at_exit (Node* node) {
    bool is_body = dynamic_cast<BodyNode*>(node) != nullptr;

    if (is_body) {
        if (foo_wraps.back() == node->id) {
            foo_wraps.pop_back();
        }
    }

    return;
}


int main(int argc, char *argv[]) {
    input_params param;
    parse_args(argc, argv, param);

    Node* tree = readTree(*param.in_stream);

    human_output_nodes = false;
    tree->visit(at_enter, at_repeat, at_exit);

    cout << "Success!" << endl;
}
