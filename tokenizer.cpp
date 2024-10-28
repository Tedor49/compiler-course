#include <filesystem>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <format>
#include <cstring>

#include "token_data.hpp"

using namespace std;

int debug = 0;

Token find_token(string s, int line, int pos){
    auto code_ref = hash_lookup.find(s);

    if (code_ref == hash_lookup.end()) {
        return {TokenCode::tkIdentifier, 0, 0, s, line, pos};
    }else if ((*code_ref).second == TokenCode::tkUndetermined) {
        return {TokenCode::tkUndetermined, 0, 0, s, line, pos};
    } else {
        return {(*code_ref).second, 0, 0, "", line, pos};
    }
}

void tokenize(string program, vector<Token>& result){
    enum TokenizerState{
        collectingWord, // Identifier or Directive
        collectingString,
        collectingInteger,
        collectingReal,
        collectingOperator,
        waiting,
        ignoringComment,
    };

    TokenizerState state = TokenizerState::waiting;
    string accum = "";

    int line = 1, pos = 1;

    for (char c : program){
        if(debug >= 2) cout << c << ' ';
        if (state==TokenizerState::ignoringComment) {
            if (c == '\n') {
                ++line;
                pos = 1;
                accum = "";
                state = TokenizerState::waiting;
                result.push_back({TokenCode::tkLineEnd, 0, 0, "", line, pos});
            }
        } else if (isLetter(c)){
            if(debug >= 2) cout << "is letter ";
            switch (state) {
                case TokenizerState::collectingWord:
                    accum.append(1, c);
                    break;
                case TokenizerState::collectingString:
                    accum.append(1, c);
                    break;
                case TokenizerState::waiting:
                    state = TokenizerState::collectingWord;
                    accum = c;
                    break;
                case TokenizerState::collectingOperator: {
                    // Finalize operator
                    Token newToken = find_token(accum, line, pos - accum.size() + 1);
                    if (newToken.type == TokenCode::tkCommentSign) {
                        state = TokenizerState::ignoringComment;
                        break;
                    }
                    if(newToken.type == TokenCode::tkIdentifier){
                        throw invalid_argument(format("Non-existent operator at line {} character {}", line, pos - accum.size() + 1)); \
                    }
                    result.push_back(newToken);

                    state = TokenizerState::collectingWord;
                    accum = c; }
                    break;
                default:
                    throw invalid_argument(format("Non-numericals are not allowed in numbers at line {} character {}", line, pos));
            }
        } else if (isNumber(c)){
            if(debug >= 2) cout << "is number ";
            switch (state) {
                case TokenizerState::waiting:
                    state = TokenizerState::collectingInteger;
                    accum = c;
                    break;
                case TokenizerState::collectingOperator: {
                    // Finalize operator
                    Token newToken = find_token(accum, line, pos - accum.size() + 1);
                    if (newToken.type == TokenCode::tkCommentSign) {
                        state = TokenizerState::ignoringComment;
                        break;
                    }
                    if(newToken.type == TokenCode::tkIdentifier){
                        throw invalid_argument(format("Non-existent operator at line {} character {}", line, pos - accum.size() + 1)); \
                    }
                    result.push_back(newToken);

                    state = TokenizerState::collectingInteger;
                    accum = c; }
                    break;
                default:
                    accum.append(1, c);
            }
        } else if (isDelimeter(c)) {
            if(debug >= 2) cout << "is delimeter ";
            switch (state) {
                case TokenizerState::collectingWord:
                    result.push_back(find_token(accum, line, pos - accum.size() + 1));
                    break;
                case TokenizerState::collectingString:
                    accum.append(1, c);
                    break;
                case TokenizerState::collectingOperator: {
                    // Finalize operator
                    Token newToken = find_token(accum, line, pos - accum.size() + 1);
                    if (newToken.type == TokenCode::tkCommentSign) {
                        state = TokenizerState::ignoringComment;
                        break;
                    }
                    if(newToken.type == TokenCode::tkIdentifier){
                        throw invalid_argument(format("Non-existent operator at line {} character {}", line, pos - accum.size() + 1)); \
                    }
                    result.push_back(newToken); }

                    break;
                case TokenizerState::collectingInteger:
                    result.push_back({TokenCode::tkInt, stoll(accum), 0, "", line, pos - accum.size() + 1});
                    break;
                case TokenizerState::collectingReal:
                    result.push_back({TokenCode::tkReal, 0, stold(accum), "", line, pos - accum.size() + 1});
                    break;
            }
            result.push_back(find_token(*new string(1, c), line, pos - accum.size() + 1));
            state = TokenizerState::waiting;
        } else if (isOperator(c)){
            if(debug >= 2) cout << "is operator ";
            switch (state) {
                case TokenizerState::collectingWord:
                    result.push_back(find_token(accum, line, pos - accum.size() + 1));
                    state = TokenizerState::collectingOperator;
                    accum = c;
                    break;
                case TokenizerState::collectingString:
                    accum.append(1, c);
                    break;
                case TokenizerState::waiting:
                    state = TokenizerState::collectingOperator;
                    accum = c;
                    break;
                case TokenizerState::collectingOperator:
                    if (find_token(accum, line, pos - accum.size() + 1).type == TokenCode::tkCommentSign) {
                        state = TokenizerState::ignoringComment;
                        break;
                    }
                    accum.append(1, c);
                    break;
                case TokenizerState::collectingInteger:
                    if (c == '.'){
                        state = TokenizerState::collectingReal;
                        accum.append(1, c);
                    } else {
                        result.push_back({TokenCode::tkInt, stoll(accum), 0, "", line, pos - accum.size() + 1});
                        state = TokenizerState::collectingOperator;
                        accum = c;
                    }
                    break;
                case TokenizerState::collectingReal:
                    if (c == '.'){
                        throw invalid_argument(format("Two decimal separators in number at line {} character {}", line, pos));
                    }
                    result.push_back({TokenCode::tkReal, 0, stold(accum), "", line, pos - accum.size() + 1});
                    state = TokenizerState::collectingOperator;
                    accum = c;
                    break;
            }
        }else{
            if(debug >= 2) cout << "is not any of them ";
            if(c == '\n' || c == ';'){
                switch (state) {
                    case TokenizerState::collectingWord:
                        result.push_back(find_token(accum, line, pos - accum.size() + 1));
                        break;
                    case TokenizerState::collectingString:
                        if (c == ';') accum.append(1, c);
                        else throw invalid_argument(format("Newline in the middle of string at line {}", line));
                        break;
                    case TokenizerState::collectingOperator: {
                        // Finalize operator
                        Token newToken = find_token(accum, line, pos - accum.size() + 1);

                        if (newToken.type == TokenCode::tkCommentSign) {
                            if (c == ';') {
                                state = TokenizerState::ignoringComment;
                            } else {
                                state = TokenizerState::waiting;
                            }
                            break;
                        }
                        if(newToken.type == TokenCode::tkIdentifier){
                            throw invalid_argument(format("Non-existent operator at line {} character {}", line, pos - accum.size() + 1)); \
                        }
                        result.push_back(newToken); }

                        break;
                    case TokenizerState::collectingInteger:
                        result.push_back({TokenCode::tkInt, stoll(accum), 0, "", line, pos - accum.size() + 1});
                        break;
                    case TokenizerState::collectingReal:
                        result.push_back({TokenCode::tkReal, 0, stold(accum), "", line, pos - accum.size() + 1});
                        break;
                }

                if (state!=TokenizerState::ignoringComment) {
                    result.push_back({TokenCode::tkLineEnd, 0, 0, "", line, pos - accum.size() + 1});
                    state = TokenizerState::waiting;
                }
                if (c == '\n') {
                    ++line;
                    pos = 1;
                }
            } else if (c == '"') {
                switch (state) {
                    case TokenizerState::collectingWord:
                        throw invalid_argument(format("Unexpected \" at line {} character {}", line, pos));
                        break;
                    case TokenizerState::collectingString:
                        result.push_back({TokenCode::tkString, 0, 0, accum, line, pos - accum.size() + 1});
                        state = TokenizerState::waiting;
                        accum = "";
                        break;
                    case TokenizerState::collectingOperator: {
                        // Finalize operator
                        Token newToken = find_token(accum, line, pos - accum.size() + 1);
                        if (newToken.type == TokenCode::tkCommentSign) {
                            state = TokenizerState::ignoringComment;
                            break;
                        }
                        if(newToken.type == TokenCode::tkIdentifier){
                            throw invalid_argument(format("Non-existent operator at line {} character {}", line, pos - accum.size() + 1)); \
                        }
                        result.push_back(newToken);

                        state = TokenizerState::collectingString;
                        accum = ""; }
                        break;
                    case TokenizerState::collectingInteger:
                        throw invalid_argument(format("Unexpected \" at line {} character {}", line, pos));
                        break;
                    case TokenizerState::collectingReal:
                        throw invalid_argument(format("Unexpected \" at line {} character {}", line, pos));
                        break;
                    case TokenizerState::waiting:
                        state = TokenizerState::collectingString;
                        accum = "";
                        break;
                }
            } else {
                switch (state) {
                    case TokenizerState::collectingWord:
                        result.push_back(find_token(accum, line, pos - accum.size() + 1));
                        break;
                    case TokenizerState::collectingString:
                        accum.append(1, c);
                        break;
                    case TokenizerState::collectingOperator:  {
                        // Finalize operator
                        Token newToken = find_token(accum, line, pos - accum.size() + 1);
                        if (newToken.type == TokenCode::tkCommentSign) {
                            state = TokenizerState::ignoringComment;
                            break;
                        }
                        if(newToken.type == TokenCode::tkIdentifier){
                            throw invalid_argument(format("Non-existent operator at line {} character {}", line, pos - accum.size() + 1)); \
                        }
                        result.push_back(newToken); }

                        break;
                    case TokenizerState::collectingInteger:
                        result.push_back({TokenCode::tkInt, stoll(accum), 0, "", line, pos - accum.size() + 1});
                        break;
                    case TokenizerState::collectingReal:
                        result.push_back({TokenCode::tkReal, 0, stold(accum), "", line, pos - accum.size() + 1});
                        break;
                }
                if(state != TokenizerState::ignoringComment && state != TokenizerState::collectingString) state = TokenizerState::waiting;
            }
        }
        ++pos;
        if (debug >= 2) cout << state << endl;
    }
    switch (state) {
        case TokenizerState::collectingWord:
            result.push_back(find_token(accum, line, pos - accum.size() + 1));
            break;
        case TokenizerState::collectingString:
            throw invalid_argument(format("EOF in the middle of string at line {}", line));
            break;
        case TokenizerState::collectingOperator: {
            // Finalize operator
            Token newToken = find_token(accum, line, pos - accum.size() + 1);
            if (newToken.type == TokenCode::tkCommentSign) {
                state = TokenizerState::ignoringComment;
                break;
            }
            if(newToken.type == TokenCode::tkIdentifier){
                throw invalid_argument(format("Non-existent operator at line {} character {}", line, pos - accum.size() + 1)); \
            }
            result.push_back(newToken); }
             // Maybe there should be an error outright because there are no postfix operators
            break;
        case TokenizerState::collectingInteger:
            result.push_back({TokenCode::tkInt, stoll(accum), 0, "", line, pos - accum.size() + 1});
            break;
        case TokenizerState::collectingReal:
            result.push_back({TokenCode::tkReal, 0, stold(accum), "", line, pos - accum.size() + 1});
            break;
    }
    result.push_back({TokenCode::tkLineEnd, 0, 0, "", line, pos - accum.size() + 1});

    for(int i = 0; i < result.size(); ++i){
        if (result[i].type == TokenCode::tkUndetermined){
            if(result[i].valStr == "+"){
                if(i != 0 && (result[i - 1].type == TokenCode::tkBooleanFalse ||
                              result[i - 1].type == TokenCode::tkBooleanTrue ||
                              result[i - 1].type == TokenCode::tkBracketCurvyRight ||
                              result[i - 1].type == TokenCode::tkBracketNormalRight||
                              result[i - 1].type == TokenCode::tkBracketSquareRight ||
                              result[i - 1].type == TokenCode::tkDelimeterEnd ||
                              result[i - 1].type == TokenCode::tkIdentifier ||
                              result[i - 1].type == TokenCode::tkInt ||
                              result[i - 1].type == TokenCode::tkReadInt ||
                              result[i - 1].type == TokenCode::tkReadReal ||
                              result[i - 1].type == TokenCode::tkReadString ||
                              result[i - 1].type == TokenCode::tkReal ||
                              result[i - 1].type == TokenCode::tkString)) {
                                result[i] = {TokenCode::tkOperatorPlus, 0, 0, ""};
                } else {
                    result[i] = {TokenCode::tkUnaryPlus, 0, 0, ""};
                }
            } else if(result[i].valStr == "-"){
                if(i != 0 && (result[i - 1].type == TokenCode::tkBooleanFalse ||
                              result[i - 1].type == TokenCode::tkBooleanTrue ||
                              result[i - 1].type == TokenCode::tkBracketCurvyRight ||
                              result[i - 1].type == TokenCode::tkBracketNormalRight||
                              result[i - 1].type == TokenCode::tkBracketSquareRight ||
                              result[i - 1].type == TokenCode::tkDelimeterEnd ||
                              result[i - 1].type == TokenCode::tkIdentifier ||
                              result[i - 1].type == TokenCode::tkInt ||
                              result[i - 1].type == TokenCode::tkReadInt ||
                              result[i - 1].type == TokenCode::tkReadReal ||
                              result[i - 1].type == TokenCode::tkReadString ||
                              result[i - 1].type == TokenCode::tkReal ||
                              result[i - 1].type == TokenCode::tkString)) {
                                result[i] = {TokenCode::tkOperatorMinus, 0, 0, ""};
                } else {
                    result[i] = {TokenCode::tkUnaryMinus, 0, 0, ""};
                }
            }else if(result[i].valStr == "is"){
                if(i != 0 && (result[i - 1].type == TokenCode::tkFunc)) {
                    result[i] = {TokenCode::tkDelimeterIs, 0, 0, ""};
                } else {
                    if(i != 0 && (result[i - 1].type == TokenCode::tkBracketNormalRight)){
                        int bcount = 1, y = i - 2;
                        while(y > 0 && bcount != 0){
                            if (result[y].type == TokenCode::tkBracketNormalRight){
                                ++bcount;
                            } else if (result[y].type == TokenCode::tkBracketNormalLeft){
                                --bcount;
                            }
                            --y;
                        }
                        if (result[y].type == TokenCode::tkFunc) {
                            result[i] = {TokenCode::tkDelimeterIs, 0, 0, ""};
                        } else {
                            result[i] = {TokenCode::tkIs, 0, 0, ""};
                        }

                    } else {
                        result[i] = {TokenCode::tkIs, 0, 0, ""};
                    }

                }
            }else if(result[i].valStr == "func"){
                if(i != 0 && (result[i - 1].type == TokenCode::tkIs)) {
                    result[i] = {TokenCode::tkTypeFunc, 0, 0, ""};
                } else {
                    result[i] = {TokenCode::tkFunc, 0, 0, ""};
                }
            }else if(result[i].valStr == "empty"){
                if(i != 0 && (result[i - 1].type == TokenCode::tkIs)) {
                    result[i] = {TokenCode::tkTypeEmpty, 0, 0, ""};
                } else {
                    result[i] = {TokenCode::tkEmpty, 0, 0, ""};
                }
            }
        }
    }

}

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


int main(int argc, char *argv[]) {

    input_params param;
    parse_args(argc, argv, param);

    human_output_tokens = param.human;

    string prog = readfile(*param.in_stream);

    vector<Token> tokenized;
    tokenize(prog, tokenized);

    if(param.out_is_file){
        (*param.out_stream) << tokenized;
        (*param.out_stream).close();
    } else {
        cout << tokenized;
    }
}
