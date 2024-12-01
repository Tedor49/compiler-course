#ifndef __CMD_UTILS_INCLUDED__
#define __CMD_UTILS_INCLUDED__

#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <string>

namespace cmd_utils {

    std::string readfile(std::ifstream &in) {
        std::stringstream buf;

        buf << in.rdbuf();
        in.close();

        return buf.str();
    }

    struct input_params {
        std::ifstream* in_stream;
        bool out_is_file;
        std::ofstream* out_stream;
        bool human;
        bool verbose;
    };

    void parse_args(int &argc, char* argv[], input_params &par, std::ostream* log = &std::cerr) {

        if (argc < 2) {
            (*log) << "Usage: " << argv[0] << " infile [-o outfile] [-h] [-v]\n";
            (*log) << "  infile      path to input file\n";
            (*log) << "  -o outfile  path to output file\n";
            (*log) << "  -h          output in a human readable way\n";
            (*log) << "  -v          verbose output\n";
            throw std::invalid_argument("No input file specified");
        }

        if (!std::filesystem::is_regular_file(argv[1]) && !std::filesystem::is_symlink(argv[1])) {
            (*log) << "Usage: " << argv[0] << " infile [-o outfile] [-h] [-v]\n";
            (*log) << "  infile      path to input file\n";
            (*log) << "  -o outfile  path to output file\n";
            (*log) << "  -h          output in a human readable way\n";
            (*log) << "  -v          verbose output\n";
            throw std::invalid_argument("Input file does not refer to a file");
        }

        std::ifstream* in = new std::ifstream(argv[1]);

        if (in->fail()) {
            (*log) << "Usage: " << argv[0] << " infile [-o outfile] [-h] [-v]\n";
            (*log) << "  infile      path to input file\n";
            (*log) << "  -o outfile  path to output file\n";
            (*log) << "  -h          output in a human readable way\n";
            (*log) << "  -v          verbose output\n";
            throw std::invalid_argument("Could not open input file");
        }

        par.in_stream = in;
        par.out_stream = nullptr;
        par.out_is_file = false;
        par.human = false;

        int y = 2;

        while (y != argc) {
            if (strcmp(argv[y], "-o") == 0) {
                if (y + 1 == argc) {
                    (*log) << "Usage: " << argv[0] << " infile [-o outfile] [-h] [-v]\n";
                    (*log) << "  infile      path to input file\n";
                    (*log) << "  -o outfile  path to output file\n";
                    (*log) << "  -h          output in a human readable way\n";
                    (*log) << "  -v          verbose output\n";
                    throw std::invalid_argument("No output file after flag");
                }

                std::ofstream* out = new std::ofstream(argv[y + 1]);

                if (out->fail()) {
                    (*log) << "Usage: " << argv[0] << " infile [-o outfile] [-h] [-v]\n";
                    (*log) << "  infile      path to input file\n";
                    (*log) << "  -o outfile  path to output file\n";
                    (*log) << "  -h          output in a human readable way\n";
                    (*log) << "  -v          verbose output\n";
                    throw std::invalid_argument("Could not open output file");
                }

                par.out_stream = out;
                par.out_is_file = true;
                ++y;

            } else if (strcmp(argv[y], "-h") == 0) {
                par.human = true;
            } else if (strcmp(argv[y], "-v") == 0) {
                par.verbose = true;
            }
            ++y;
        }
    }
}

#endif // __CMD_UTILS_INCLUDED__
