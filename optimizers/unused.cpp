
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


int nodes = 0;

bool prev_foo = false;

vector<int> scope_wraps;
vector<vector<string>> scopes;

void at_enter (Node* node) {
    ++nodes;

    VariableDefinitionNode* vardef_node = dynamic_cast<VariableDefinitionNode*>(node); // .identifier
    ForNode* for_node = dynamic_cast<ForNode*>(node); // .identifier
    PrimaryNode* primary_node = dynamic_cast<PrimaryNode*>(node); // .identifier
    FunctionNode* func_node = dynamic_cast<FunctionNode*>(node); // .params
    BodyNode* body_node = dynamic_cast<BodyNode*>(node);

    if (vardef_node != nullptr) {
        scopes.back().push_back(vardef_node->identifier);
    } else if (for_node != nullptr) {
        scope_wraps.push_back(node->id);
        scopes.push_back(vector<string>());
        scopes.back().push_back(for_node->identifier);
    } else if (func_node != nullptr) {
        scope_wraps.push_back(node->id);
        scopes.push_back(vector<string>(func_node->params.begin(), func_node->params.end()));
    } else if (body_node != nullptr) {
        scope_wraps.push_back(node->id);
        scopes.push_back(vector<string>());
    } else if (primary_node != nullptr) {
        if (primary_node->type != 'v') return;

        bool exists = false;
        for (auto& i : scopes) {
            for (auto j : i) {
                if (j == primary_node->identifier) {
                    exists = true;
                    break;
                }
            }
        }

        if (!exists) {
            throw invalid_argument(format("Undefined variable {}", primary_node->identifier));
        }
    }

    return;
}

void at_repeat (Node* node) {
    return;
}

void at_exit (Node* node) {
    if (scope_wraps.back() == node->id) {
        scope_wraps.pop_back();
        scopes.pop_back();
    }
    return;
}


int main(int argc, char *argv[]) {
    input_params param;
    parse_args(argc, argv, param);

    Node* tree = readTree(*param.in_stream);

    //cout << "TREE ID: " << reinterpret_cast<std::uintptr_t>(tree) << endl;

    human_output_nodes = false;

    scope_wraps.push_back(-1);
    scopes.resize(1);

    tree->visit(at_enter, at_repeat, at_exit);

    if(param.out_is_file){
        (*param.out_stream) << tree;
        (*param.out_stream).close();
    } else {
        cout << tree;
    }
    //cout << "Total nodes: " << nodes << endl;
}
