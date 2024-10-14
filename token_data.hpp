#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <sstream>

enum TokenCode {
    // Literals
    tkInt,
    tkReal,
    tkString,
    tkBooleanTrue,
    tkBooleanFalse,
    tkEmpty,

    // Variable names
    tkIdentifier,

    // Operators
    tkUnaryPlus, // +
    tkUnaryMinus, // -
    tkUnaryNot, // not
    tkOperatorMultiply, // *
    tkOperatorDivide, // /
    tkOperatorPlus, // +
    tkOperatorMinus, // -
    tkRelationLess, // <
    tkRelationLessEq, // <=
    tkRelationMore, // >
    tkRelationMoreEq, // >=
    tkRelationEqual, // =
    tkRelationNotEqual, // /=
    tkLogicOr, // or
    tkLogicAnd, // and
    tkLogicXor, // xor

    // Directives
    tkIf, // if
    tkElse, // else
    tkWhile, // while
    tkFor, // for
    tkIn, // in
    tkIs, // is type checker
    tkReturn, // return
    tkVar, // var
    tkAssignment, // :=
    tkPlusEq, // +=
    tkMinusEq, // -=
    tkMultiplyEq, // *=
    tkDivideEq, // /=
    tkImport, // import
    tkPrint, // print
    tkReadInt, // readInt
    tkReadReal, // readReal
    tkReadString, // readString
    tkDot, // . for accessing tuple elements
    tkDoubleDot, // .. for for loop
    tkLambda, // => for defining lambda functions
    tkFunc, // func keyword for defining functions

    // Type indicators
    tkTypeInt, // int
    tkTypeReal, // real
    tkTypeBool, // bool
    tkTypeString, // string
    tkTypeFunc, // func type
    tkTypeEmpty, // empty

    // Comment token
    tkCommentSign, // end

    // Delimeters
    tkBracketNormalLeft, // (
    tkBracketNormalRight, // )
    tkBracketSquareLeft, // [
    tkBracketSquareRight, // ]
    tkBracketCurvyLeft, // {
    tkBracketCurvyRight, // }
    tkComma, // ,
    tkDelimeterThen, // then
    tkDelimeterLoop, // loop
    tkDelimeterIs, // is in function definition
    tkDelimeterEnd, // end
    tkLineEnd, // line feed or ;

    // Indeterminate for words with multiple meanings, must be swapped out by the end
    tkUndetermined, // end
};

bool human_output = false;

struct Token {
    TokenCode type;
    long long valInt;
    long double valReal;
    std::string valStr;
    int line;
    int pos;

    friend std::istream& operator>>(std::istream& in, Token& a){
        char trash;
        std::string string_inp;
        long long int_inp;
        long double real_inp;
        TokenCode code;


        in >> trash;
        in >> int_inp;

        code = static_cast<TokenCode>(int_inp);
        a.type = code;
        a.valInt = 0;
        a.valReal = 0;
        a.valStr = "";

        in >> trash;
        in >> int_inp;
        a.line = int_inp;

        in >> trash;
        in >> int_inp;
        a.pos = int_inp;

        switch (code){
            case TokenCode::tkInt:
                in >> trash >> int_inp >> trash;
                a.valInt = int_inp;
                break;
            case TokenCode::tkReal:
                in >> trash >> real_inp >> trash;
                a.valReal = real_inp;
                break;
            case TokenCode::tkString:
                in >> trash;
                std::getline(in, string_inp);
                in >> trash;
                a.valStr = string_inp;
                break;
            case TokenCode::tkIdentifier:
                in >> trash;
                std::getline(in, string_inp);
                in >> trash;
                a.valStr = string_inp;
                break;
            default:
                in >> trash;
                break;
        }

        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, Token a){
        if (human_output) {
            switch (a.type){
                case TokenCode::tkInt:
                    out << "(tkInt " << a.line << ' ' << a.pos << ' ' << a.valInt << ")";
                    break;
                case TokenCode::tkReal:
                    out << "(tkReal " << a.line << ' ' << a.pos << ' ' << a.valReal << ")";
                    break;
                case TokenCode::tkString:
                    out << "(tkString " << a.line << ' ' << a.pos << " \"" << a.valStr << "\")";
                    break;
                case TokenCode::tkIdentifier:
                    out << "(tkIdentifier " << a.line << ' ' << a.pos << ' ' << a.valStr << ")";
                    break;
                case TokenCode::tkBooleanTrue:
                    out << "(tkBooleanTrue " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkBooleanFalse:
                    out << "(tkBooleanFalse " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkEmpty:
                    out << "(tkEmpty " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkUnaryPlus:
                    out << "(tkUnaryPlus " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkUnaryMinus:
                    out << "(tkUnaryMinus " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkUnaryNot:
                    out << "(tkUnaryNot " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkOperatorMultiply:
                    out << "(tkOperatorMultiply " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkOperatorDivide:
                    out << "(tkOperatorDivide " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkOperatorPlus:
                    out << "(tkOperatorPlus " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkOperatorMinus:
                    out << "(tkOperatorMinus " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkRelationLess:
                    out << "(tkRelationLess " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkRelationLessEq:
                    out << "(tkRelationLessEq " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkRelationMore:
                    out << "(tkRelationMore " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkRelationMoreEq:
                    out << "(tkRelationMoreEq " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkRelationEqual:
                    out << "(tkRelationEqual " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkRelationNotEqual:
                    out << "(tkRelationNotEqual " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkLogicOr:
                    out << "(tkLogicOr " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkLogicAnd:
                    out << "(tkLogicAnd " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkLogicXor:
                    out << "(tkLogicXor " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkIf:
                    out << "(tkIf " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkElse:
                    out << "(tkElse " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkWhile:
                    out << "(tkWhile " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkFor:
                    out << "(tkFor " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkIn:
                    out << "(tkIn " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkIs:
                    out << "(tkIs " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkReturn:
                    out << "(tkReturn " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkVar:
                    out << "(tkVar " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkAssignment:
                    out << "(tkAssignment " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkImport:
                    out << "(tkImport " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkPrint:
                    out << "(tkPrint " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkReadInt:
                    out << "(tkReadInt " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkReadReal:
                    out << "(tkReadReal " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkReadString:
                    out << "(tkReadString " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkDot:
                    out << "(tkDot " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkDoubleDot:
                    out << "(tkDoubleDot " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkLambda:
                    out << "(tkLambda " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkFunc:
                    out << "(tkFunc " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkTypeInt:
                    out << "(tkTypeInt " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkTypeReal:
                    out << "(tkTypeReal " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkTypeBool:
                    out << "(tkTypeBool " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkTypeString:
                    out << "(tkTypeString " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkTypeFunc:
                    out << "(tkTypeFunc " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkTypeEmpty:
                    out << "(tkTypeEmpty " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkBracketNormalLeft:
                    out << "(tkBracketNormalLeft " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkBracketNormalRight:
                    out << "(tkBracketNormalRight " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkBracketSquareLeft:
                    out << "(tkBracketSquareLeft " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkBracketSquareRight:
                    out << "(tkBracketSquareRight " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkBracketCurvyLeft:
                    out << "(tkBracketCurvyLeft " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkBracketCurvyRight:
                    out << "(tkBracketCurvyRight " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkDelimeterThen:
                    out << "(tkDelimeterThen " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkDelimeterLoop:
                    out << "(tkDelimeterLoop " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkDelimeterIs:
                    out << "(tkDelimeterIs " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkDelimeterEnd:
                    out << "(tkDelimeterEnd " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkLineEnd:
                    out << "(tkLineEnd " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkComma:
                    out << "(tkComma " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkPlusEq:
                    out << "(tkPlusEq " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkMinusEq:
                    out << "(tkMinusEq " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkMultiplyEq:
                    out << "(tkMultiplyEq " << a.line << ' ' << a.pos << ")";
                    break;
                case TokenCode::tkDivideEq:
                    out << "(tkDivideEq " << a.line << ' ' << a.pos << ")";
                    break;
            }
        } else {
            switch (a.type){
                case TokenCode::tkInt:
                    out << "(" << a.type << '|' << a.line << '|' << a.pos << '|' << a.valInt << ")";
                    break;
                case TokenCode::tkReal:
                    out << "(" << a.type  << '|' << a.line << '|' << a.pos << '|'  << a.valReal << ")";
                    break;
                case TokenCode::tkString:
                    out << "(" << a.type  << '|' << a.line << '|' << a.pos << '|'  << a.valStr << "\n)";
                    break;
                case TokenCode::tkIdentifier:
                    out << "(" << a.type  << '|' << a.line << '|' << a.pos << '|' << a.valStr << "\n)";
                    break;
                default:
                    out << "(" << a.type  << '|' << a.line << '|' << a.pos << ")";
                    break;
            }
        }
        return out;
    }

    friend std::istream& operator>>(std::istream& in, std::vector<Token>& res) {
        res.clear();

        Token cur;

        char trash;
        in >> trash;
        while(in.peek() != ')'){
            in >> cur;
            res.push_back(cur);
        }
        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, std::vector<Token>& s){
        out << "(" << s[0];
        for(int i = 1; i < s.size(); ++i){
            if (human_output){
                out << ", ";
            }
            out << s[i];
        }
        out << ")\n";
        return out;
    }
};

// Map for converting token strings to enum
const std::unordered_map<std::string, TokenCode> hash_lookup = {

    // Token strings with multiple meanings
        // Either tkIs directive or tkDelimeterIs for function definition
        { "is", TokenCode::tkUndetermined },

        // Either tkOperatorUnaryPlus or tkOperatorPlus
        { "+", TokenCode::tkUndetermined },

        // Either tkOperatorUnaryMinus or tkOperatorMinus
        { "-", TokenCode::tkUndetermined },

        // Either tkTypeFunc or tkFunc
        { "func", TokenCode::tkUndetermined },

        // Either tkEmpty or tkTypeEmpty
        { "empty", TokenCode::tkUndetermined },

    // Boolean Literals
	{ "true", TokenCode::tkBooleanTrue },
	{ "false", TokenCode::tkBooleanFalse },

	// Unary operators
	{ "not", TokenCode::tkUnaryNot },

	// Binary operators
	{ "*", TokenCode::tkOperatorMultiply },
	{ "/", TokenCode::tkOperatorDivide },
	{ "<", TokenCode::tkRelationLess },
	{ "<=", TokenCode::tkRelationLessEq },
	{ ">", TokenCode::tkRelationMore },
	{ ">=", TokenCode::tkRelationMoreEq },
	{ "=", TokenCode::tkRelationEqual },
	{ "/=", TokenCode::tkRelationNotEqual },
	{ "or", TokenCode::tkLogicOr },
	{ "and", TokenCode::tkLogicAnd },
	{ "xor", TokenCode::tkLogicXor },

	// Directives
	{ "if", TokenCode::tkIf },
	{ "else", TokenCode::tkElse },
	{ "while", TokenCode::tkWhile },
	{ "for", TokenCode::tkFor },
	{ "in", TokenCode::tkIn },
	{ "return", TokenCode::tkReturn },
	{ "var", TokenCode::tkVar },
	{ ":=", TokenCode::tkAssignment },
	{ "+=", TokenCode::tkPlusEq },
	{ "-=", TokenCode::tkMinusEq },
	{ "import", TokenCode::tkImport },
	{ "print", TokenCode::tkPrint },
	{ "readInt", TokenCode::tkReadInt },
	{ "readReal", TokenCode::tkReadReal },
	{ "readString", TokenCode::tkReadString },
	{ ".", TokenCode::tkDot },
	{ "..", TokenCode::tkDoubleDot },
	{ "=>", TokenCode::tkLambda },
	{ ",", TokenCode::tkComma },

	// Type indicators
	{ "int", TokenCode::tkTypeInt },
	{ "real", TokenCode::tkTypeReal },
	{ "bool", TokenCode::tkTypeBool },
	{ "string", TokenCode::tkTypeString },

	// Comment
	{ "//", TokenCode::tkCommentSign },

	// Delimeters
	{ "(", TokenCode::tkBracketNormalLeft },
	{ ")", TokenCode::tkBracketNormalRight },
	{ "[", TokenCode::tkBracketSquareLeft },
	{ "]", TokenCode::tkBracketSquareRight },
	{ "{", TokenCode::tkBracketCurvyLeft },
	{ "}", TokenCode::tkBracketCurvyRight },
	{ "then", TokenCode::tkDelimeterThen },
	{ "loop", TokenCode::tkDelimeterLoop },
	{ "end", TokenCode::tkDelimeterEnd }
};

// For quickly looking up which characters are brackets
// Includes symbols ()[]{},
// These are symbols that are not part of any operator, and need to be processed right away
static constexpr bool delimeters[128] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0};

// For quickly looking up which characters are operators
// Includes symbols +-*/=><.:
static constexpr bool operators[128] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Helper functions for determining the type of character quickly
#define isLetter(c) (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_'))
#define isNumber(c) ('0' <= c && c <= '9')
#define isDelimeter(c) delimeters[c]
#define isOperator(c) operators[c]
