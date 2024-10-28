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
//map<int, int> // id -> True\False\None as 1\-1\0

void at_enter (Node* node) {
    ++nodes;

    VariableDefinitionNode* vardef_node =   dynamic_cast<VariableDefinitionNode*>(node); // .identifier
    ForNode*                for_node =      dynamic_cast<ForNode*>(node); // .identifier
    PrimaryNode*            primary_node =  dynamic_cast<PrimaryNode*>(node); // .identifier
    FunctionNode*           func_node =     dynamic_cast<FunctionNode*>(node); // .params
    BodyNode*               body_node =     dynamic_cast<BodyNode*>(node);
    IfNode*                  if_node =      dynamic_cast<IfNode*>(node);

    if (if_node != nullptr) {
        ExpressionNode* expression_node = dynamic_cast<ExpressionNode*>(if_node->expression);
        if (expression_node->relations.size() != 1) {
            return;
        }

        RelationNode* relation_node = dynamic_cast<RelationNode*>(expression_node->relations[0]);
        if (relation_node->factors.size() != 1) {
            return;
        }

        FactorNode* factor_node = dynamic_cast<FactorNode*>(relation_node->factors[0]);
        if (factor_node->terms.size() != 1) {
            return;
        }

        TermNode* term_node = dynamic_cast<TermNode*>(factor_node->terms[0]);
        if (term_node->terms.size() != 1) {
            return;
        }

        UnaryNode* unary_node = dynamic_cast<UnaryNode*>(term_node->terms[0]);
        if (unary_node->type != 'p') {
            return;
        }

        PrimaryNode* primary_node = dynamic_cast<PrimaryNode*>(unary_node->primary);
        if (primary_node->type != 'l') {
            return;
        }

        LiteralNode* literal_node = dynamic_cast<LiteralNode*>(primary_node->literal);
        if (literal_node->type != 'b') {
            return;
        }

        ProgramNode*    programm_node =             dynamic_cast<ProgramNode*>  (node->parent->parent);
        BodyNode*       body_node =                 dynamic_cast<BodyNode*>     (node->parent->parent);

        vector<Node*> statements_to_change;
        vector<Node*> resulting_statements;

        if (programm_node != nullptr) {
            statements_to_change = programm_node->statements;
        } else {
            statements_to_change = body_node->statements;
        }

//        cout << "If node with True or False detected at " << nodes << endl;

        if (literal_node->bool_val) {
            BodyNode* if_body_node = dynamic_cast<BodyNode*> (if_node->if_body);

            for (auto & i : statements_to_change) {
                if (i == node->parent) {
                    for (auto & j : if_body_node->statements) {
                        resulting_statements.push_back(j);
                    }
                } else {
                    resulting_statements.push_back(i);
                }
            }
            if (programm_node != nullptr) {
                programm_node->statements = resulting_statements;
            } else {
                body_node->statements = resulting_statements;
            }
            return;
        } else {
            if (if_node->else_body == nullptr) {
                for (auto & i : statements_to_change) {
                    if (i == node->parent) {
                        continue;
                    } else {
                        resulting_statements.push_back(i);
                    }
                }
                if (programm_node != nullptr) {
                    programm_node->statements = resulting_statements;
                } else {
                    body_node->statements = resulting_statements;
                }
                return;
            }

            BodyNode* else_body_node = dynamic_cast<BodyNode*> (if_node->else_body);

            for (auto & i : statements_to_change) {
                if (i == node->parent) {
                    for (auto & j : else_body_node->statements) {
                        resulting_statements.push_back(j);
                    }
                } else {
                    resulting_statements.push_back(i);
                }
            }
            if (programm_node != nullptr) {
                programm_node->statements = resulting_statements;
            } else {
                body_node->statements = resulting_statements;
            }
            return;
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

    reassign_ids(tree);

    if(param.out_is_file){
        (*param.out_stream) << tree;
        (*param.out_stream).close();
    } else {
        cout << tree;
    }
    //cout << "Total nodes: " << nodes << endl;
}
