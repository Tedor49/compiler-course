#ifndef __TREE_PARSER_INCLUDED__
#define __TREE_PARSER_INCLUDED__

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

# include "token_data.hpp"
# include "ast_nodes.hpp"

namespace ast_nodes {
	
	Node* Node::from_tokens(std::vector<tokens::Token>& tokens, int& y) {return this;}
	
	Node* createNodeFromTokens(const std::string& t, std::vector<tokens::Token>& tokens, int& y);

    Node* DeclarationNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->identifier = tokens.at(y).valStr;
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		++y;

		switch (tokens.at(y).type) {
			case tokens::TokenCode::tkComma:
			case tokens::TokenCode::tkLineEnd:
				return this;
			case tokens::TokenCode::tkAssignment:
				++y;
				this->value = createNodeFromTokens("Expression", tokens, y);
				break;
			default:
				throw std::invalid_argument("Unexpected value, expected :=");
		}
		return this;
	}

    Node* ExpressionNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		bool complete = false;

		while (1) {
			this->terms.push_back(createNodeFromTokens("Unary", tokens, y));

			switch (tokens.at(y).type) {
				case tokens::TokenCode::tkOperatorMultiply:
					this->ops.push_back('*');
					break;
				case tokens::TokenCode::tkOperatorDivide:
					this->ops.push_back('/');
					break;
				case tokens::TokenCode::tkOperatorPlus:
					this->ops.push_back('+');
					break;
				case tokens::TokenCode::tkOperatorMinus:
					this->ops.push_back('-');
					break;
				case tokens::TokenCode::tkRelationEqual:
					this->ops.push_back('=');
					break;
				case tokens::TokenCode::tkRelationLess:
					this->ops.push_back('<');
					break;
				case tokens::TokenCode::tkRelationLessEq:
					this->ops.push_back('l');
					break;
				case tokens::TokenCode::tkRelationMore:
					this->ops.push_back('>');
					break;
				case tokens::TokenCode::tkRelationMoreEq:
					this->ops.push_back('m');
					break;
				case tokens::TokenCode::tkRelationNotEqual:
					this->ops.push_back('n');
					break;
				case tokens::TokenCode::tkLogicAnd:
					this->ops.push_back('a');
					break;
				case tokens::TokenCode::tkLogicOr:
					this->ops.push_back('o');
					break;
				case tokens::TokenCode::tkLogicXor:
					this->ops.push_back('x');
					break;
				default:
					complete = true;
					break;
			}
			if (complete) break;
			++y;
		}
		return this;
	}
	
	char UnaryNode::parse_type_ind(std::vector<tokens::Token>& tokens, int& y) {
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		switch (tokens.at(y).type) {
			case tokens::TokenCode::tkTypeInt:
				++y;
				return 'i';
			case tokens::TokenCode::tkTypeReal:
				++y;
				return 'r';
			case tokens::TokenCode::tkTypeBool:
				++y;
				return 'b';
			case tokens::TokenCode::tkTypeString:
				++y;
				return 's';
			case tokens::TokenCode::tkTypeEmpty:
				++y;
				return 'e';
			case tokens::TokenCode::tkBracketSquareLeft:
				if (tokens.at(y+1).type != tokens::TokenCode::tkBracketSquareRight){
					throw std::invalid_argument("Expected closing bracket in type indicator");
				}
				y += 2;
				return 'a';
			case tokens::TokenCode::tkBracketCurvyLeft:
				if (tokens.at(y+1).type != tokens::TokenCode::tkBracketCurvyRight){
					throw std::invalid_argument("Expected closing bracket in type indicator");
				}
				y += 2;
				return 't';
			case tokens::TokenCode::tkTypeFunc:
				++y;
				return 'f';
			default:
				throw std::invalid_argument("Expected type indicator");
		}
	}
	
	Node* UnaryNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		switch (tokens.at(y).type) {
			case tokens::TokenCode::tkUnaryPlus:
				this->unaryop = '+';

				++y;
				this->primary = createNodeFromTokens("Primary", tokens, y);


				if (tokens.at(y).type == tokens::TokenCode::tkIs) {
					++y;
					this->type_ind = parse_type_ind(tokens, y);
				}

				break;
			case tokens::TokenCode::tkUnaryMinus:
				this->unaryop = '-';

				++y;
				this->primary = createNodeFromTokens("Primary", tokens, y);


				if (tokens.at(y).type == tokens::TokenCode::tkIs) {
					++y;
					this->type_ind = parse_type_ind(tokens, y);
				}

				break;
			case tokens::TokenCode::tkUnaryNot:
				this->unaryop = 'n';

				++y;
				this->primary = createNodeFromTokens("Primary", tokens, y);


				if (tokens.at(y).type == tokens::TokenCode::tkIs) {
					++y;
					this->type_ind = parse_type_ind(tokens, y);
				}

				break;
			case tokens::TokenCode::tkIdentifier:
			case tokens::TokenCode::tkReadInt:
			case tokens::TokenCode::tkReadReal:
			case tokens::TokenCode::tkReadString:
			case tokens::TokenCode::tkInt:
			case tokens::TokenCode::tkBooleanFalse:
			case tokens::TokenCode::tkBooleanTrue:
			case tokens::TokenCode::tkString:
			case tokens::TokenCode::tkBracketSquareLeft:
			case tokens::TokenCode::tkBracketCurvyLeft:
			case tokens::TokenCode::tkReal:
			case tokens::TokenCode::tkFunc:
			case tokens::TokenCode::tkEmpty:
			case tokens::TokenCode::tkBracketNormalLeft:
				this->primary = createNodeFromTokens("Primary", tokens, y);


				if (tokens.at(y).type == tokens::TokenCode::tkIs) {
					++y;
					this->type_ind = parse_type_ind(tokens, y);
				}

				break;
		}
		return this;
	}
	
	Node* IfNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		++y;
		
		this->expression = createNodeFromTokens("Expression", tokens, y);


		if (tokens.at(y).type != tokens::TokenCode::tkDelimeterThen) {
			throw std::invalid_argument("Expected if body beginning");
		}
		++y;

		this->if_body = createNodeFromTokens("Body", tokens, y);

		if (tokens.at(y).type == tokens::TokenCode::tkElse) {
			++y;
			this->else_body = createNodeFromTokens("Body", tokens, y);
		}
		return this;
	}

    Node* ForNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		++y;
		
		if (tokens.at(y).type != tokens::TokenCode::tkIdentifier) {
			throw std::invalid_argument("Expected iteration variable name");
		}
		this->identifier = tokens.at(y).valStr;
		++y;


		if (tokens.at(y).type != tokens::TokenCode::tkIn) {
			throw std::invalid_argument("Expected range after for loop declaration");
		}
		++y;

		this->range_expr_l = createNodeFromTokens("Expression", tokens, y);


		if (tokens.at(y).type != tokens::TokenCode::tkDoubleDot) {
			throw std::invalid_argument("Expected .. in range definition");
		}
		++y;

		this->range_expr_r = createNodeFromTokens("Expression", tokens, y);


		if (tokens.at(y).type != tokens::TokenCode::tkDelimeterLoop) {
			throw std::invalid_argument("Expected loop body after range");
		}
		++y;

		this->body = createNodeFromTokens("Body", tokens, y);


		if (tokens.at(y).type != tokens::TokenCode::tkLineEnd) {
			throw std::invalid_argument("Expected line end or ; at the end of loop");
		}
		return this;
	}
	
	Node* WhileNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		++y;

		this->expression = createNodeFromTokens("Expression", tokens, y);


		if (tokens.at(y).type != tokens::TokenCode::tkDelimeterLoop) {
			throw std::invalid_argument("Expected loop body after range");
		}
		++y;

		this->body = createNodeFromTokens("Body", tokens, y);


		if (tokens.at(y).type != tokens::TokenCode::tkLineEnd) {
			throw std::invalid_argument("Expected line end or ; at the end of loop");
		}
		return this;
	}
	
    Node* PrimaryNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		switch (tokens.at(y).type) {
			case tokens::TokenCode::tkReadInt:
				this->type = 'i';
				++y;
				break;
			case tokens::TokenCode::tkReadReal:
				this->type = 'r';
				++y;
				break;
			case tokens::TokenCode::tkReadString:
				this->type = 's';
				++y;
				break;
			case tokens::TokenCode::tkInt:
			case tokens::TokenCode::tkBooleanFalse:
			case tokens::TokenCode::tkBooleanTrue:
			case tokens::TokenCode::tkString:
			case tokens::TokenCode::tkBracketSquareLeft:
			case tokens::TokenCode::tkBracketCurvyLeft:
			case tokens::TokenCode::tkReal:
			case tokens::TokenCode::tkFunc:
			case tokens::TokenCode::tkEmpty:
				this->type = 'l';
				this->literal = createNodeFromTokens("Literal", tokens, y);
				break;
			case tokens::TokenCode::tkIdentifier:
				this->type = 'v';
				this->identifier = tokens.at(y).valStr;
				++y;

				while (1) {

					if(tokens.at(y).type == tokens::TokenCode::tkDot ||
					   tokens.at(y).type == tokens::TokenCode::tkBracketSquareLeft ||
					   tokens.at(y).type == tokens::TokenCode::tkBracketNormalLeft) {
						this->tails.push_back(createNodeFromTokens("Tail", tokens, y));
					} else {
						break;
					}
				}
				break;
			case tokens::TokenCode::tkBracketNormalLeft:
				this->type = 'e';
				++y;

				this->expression = createNodeFromTokens("Expression", tokens, y);

				if (tokens.at(y).type != tokens::TokenCode::tkBracketNormalRight){
					throw std::invalid_argument("Expected closing bracket after expression");
				}
				++y;

		}
		return this;
	}

    Node* TailNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		switch (tokens.at(y).type) {
			case tokens::TokenCode::tkDot:
				++y;


				if (tokens.at(y).type == tokens::TokenCode::tkInt) {
					this->type = 't';
					this->tuple_idx = tokens.at(y).valInt;
					++y;
				} else if (tokens.at(y).type == tokens::TokenCode::tkIdentifier) {
					this->type = 'i';
					this->identifier = tokens.at(y).valStr;
					++y;
				} else {
					 throw std::invalid_argument("Expected either identifier or int after . in tail");
				}

				break;
			case tokens::TokenCode::tkBracketNormalLeft:
				this->type = 'p';
				++y;

				if (tokens.at(y).type == tokens::TokenCode::tkBracketNormalRight) {
					++y;
				} else while (1) {
					this->params.push_back(createNodeFromTokens("Expression", tokens, y));

					if (tokens.at(y).type == tokens::TokenCode::tkComma) {
						++y;
						continue;
					} else if (tokens.at(y).type == tokens::TokenCode::tkBracketNormalRight) {
						++y;
						break;
					} else {
						throw std::invalid_argument("Expected closing bracket or comma after function argument");
					}
				}

				break;
			case tokens::TokenCode::tkBracketSquareLeft:
				this->type = 's';
				++y;

				this->subscript = createNodeFromTokens("Expression", tokens, y);


				if (tokens.at(y).type != tokens::TokenCode::tkBracketSquareRight) {
					throw std::invalid_argument("Expected closing bracket");
				}
				++y;

				break;
			default:
				throw std::invalid_argument("SOMEHOW TRIED TO CREATE TAIL FROM INVALID STATE");
		}
		return this;
	}

    Node* PrintNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		++y;
		
		while (1) {
			this->values.push_back(createNodeFromTokens("Expression", tokens, y));


			if (tokens.at(y).type == tokens::TokenCode::tkComma) {
				++y;
				continue;
			} else {
			   break;
			}
		}
		return this;
	}

    Node* ControlNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		
		switch (tokens.at(y).type) {
			case tokens::TokenCode::tkBreak:
				this->type = 'b';
				++y;
				break;
			case tokens::TokenCode::tkContinue:
				this->type = 'c';
				++y;
				break;
			case tokens::TokenCode::tkReturn:
				this->type = 'r';
				++y;
				this->value = createNodeFromTokens("Expression", tokens, y);
				break;
			default:
				throw std::invalid_argument("SOMEHOW TRIED TO CREATE CONTROL STATEMENT FROM INVALID STATE");
		}
		
		return this;
	}

    Node* LiteralNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		switch (tokens.at(y).type) {
			case tokens::TokenCode::tkInt:
				this->type = 'i';

				this->int_val = tokens.at(y).valInt;
				++y;
				break;
			case tokens::TokenCode::tkReal:
				this->type = 'r';

				this->real_val = tokens.at(y).valReal;
				++y;
				break;
			case tokens::TokenCode::tkBooleanFalse:
				this->type = 'b';

				this->bool_val = false;
				++y;
				break;
			case tokens::TokenCode::tkBooleanTrue:
				this->type = 'b';

				this->bool_val = true;
				++y;
				break;
			case tokens::TokenCode::tkString:
				this->type = 's';

				this->string_val = tokens.at(y).valStr;
				++y;
				break;
			case tokens::TokenCode::tkEmpty:
				this->type = 'e';
				++y;
				break;
			case tokens::TokenCode::tkBracketSquareLeft:
				this->type = 'a';
				this->array_val = createNodeFromTokens("ArrayLiteral", tokens, y);
				break;
			case tokens::TokenCode::tkBracketCurvyLeft:
				this->type = 't';
				this->tuple_val = createNodeFromTokens("TupleLiteral", tokens, y);
				break;
			case tokens::TokenCode::tkFunc:
				this->type = 'f';
				this->func_val = createNodeFromTokens("Function", tokens, y);
				break;
			default:
				throw std::invalid_argument("SOMEHOW TRIED TO CREATE LITERAL FROM INVALID STATE");
		}
		return this;
	}

    Node* ArrayLiteralNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		++y;
		
		if (tokens.at(y).type == tokens::TokenCode::tkBracketSquareRight) {
			++y;
			return this;
		}
		while (1) {
			this->values.push_back(createNodeFromTokens("Expression", tokens, y));

			if (tokens.at(y).type == tokens::TokenCode::tkComma) {
				++y;
				continue;
			} else if (tokens.at(y).type == tokens::TokenCode::tkBracketSquareRight) {
				++y;
				break;
			} else {
				throw std::invalid_argument("Expected closing bracket or comma");
			}
		}
		return this;
	}

    Node* TupleLiteralNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		++y;
		
		if (tokens.at(y).type == tokens::TokenCode::tkBracketCurvyRight) {
			return this;
		}
		while (1) {
			if (tokens.at(y).type == tokens::TokenCode::tkIdentifier) {
				if (tokens.at(y + 1).type == tokens::TokenCode::tkAssignment) {
					this->identifiers.push_back(tokens.at(y).valStr);
					y += 2;
				} else {
					this->identifiers.push_back("");
				}
			} else {
				this->identifiers.push_back("");
			}

			this->values.push_back(createNodeFromTokens("Expression", tokens, y));


			if (tokens.at(y).type == tokens::TokenCode::tkComma) {
				++y;
				continue;
			} else if (tokens.at(y).type == tokens::TokenCode::tkBracketCurvyRight) {
				++y;
				break;
			} else {
				throw std::invalid_argument("Expected closing bracket or comma");
			}
		}
		return this;
	}

    Node* FunctionNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		++y;
		
		if (tokens.at(y).type != tokens::TokenCode::tkBracketNormalLeft) {
			throw std::invalid_argument("Expected parameters in function definition");
		}
		++y;
		
		if (tokens.at(y).type == tokens::TokenCode::tkBracketNormalRight) {
			++y;
		} else while (1) {
			if (tokens.at(y).type != tokens::TokenCode::tkIdentifier) {
				throw std::invalid_argument("Expected identifier");
			}
			this->params.push_back(tokens.at(y).valStr);
			++y;

			if (tokens.at(y).type == tokens::TokenCode::tkComma) {
				++y;
				continue;
			} else if (tokens.at(y).type == tokens::TokenCode::tkBracketNormalRight) {
				++y;
				break;
			} else {
				throw std::invalid_argument("Expected closing bracket or comma");
			}
		}

		if (tokens.at(y).type == tokens::TokenCode::tkDelimeterIs) {
			this->type = 'b';
			++y;
			this->body = createNodeFromTokens("Body", tokens, y);
		} else if (tokens.at(y).type == tokens::TokenCode::tkLambda) {
			this->type = 'l';
			++y;
			this->body = createNodeFromTokens("Expression", tokens, y);
		}
		return this;
	}

    Node* AssignmentNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
        this->primary = createNodeFromTokens("Primary", tokens, y);

        switch (tokens.at(y).type) {
            case tokens::TokenCode::tkAssignment:
                this->type = '=';
                break;
            case tokens::TokenCode::tkPlusEq:
                this->type = '+';
                break;
            case tokens::TokenCode::tkMinusEq:
                this->type = '-';
                break;
            default:
                throw std::invalid_argument("Expected :=, += or -= in supposed assignment");
        }
        ++y;

        this->expression = createNodeFromTokens("Expression", tokens, y);
        return this;
    }

    Node* BodyNode::from_tokens(std::vector<tokens::Token>& tokens, int& y){
		this->line = tokens.at(y).line;
		this->pos = tokens.at(y).pos;
		while (tokens.size() != y) {
			while (tokens.at(y).type == tokens::TokenCode::tkLineEnd){
				++y;
				if (tokens.size() == y) break;
			}
			if (tokens.size() == y) break;

			if (tokens.at(y).type == tokens::TokenCode::tkDelimeterEnd) {
				++y;
				break;
			} else if (tokens.at(y).type == tokens::TokenCode::tkElse) {
				break;
			}

			switch (tokens.at(y).type) {
				case tokens::TokenCode::tkVar:
					do {
						++y;
						statements.push_back(createNodeFromTokens("Declaration", tokens, y));
					} while (tokens.at(y).type == tokens::TokenCode::tkComma);
					break;
				case tokens::TokenCode::tkIdentifier:
					statements.push_back(createNodeFromTokens("Assignment", tokens, y));
					break;
				case tokens::TokenCode::tkIf:
					statements.push_back(createNodeFromTokens("If", tokens, y));
					break;
				case tokens::TokenCode::tkFor:
					statements.push_back(createNodeFromTokens("For", tokens, y));
					break;
				case tokens::TokenCode::tkWhile:
					statements.push_back(createNodeFromTokens("While", tokens, y));
					break;
				case tokens::TokenCode::tkBreak:
				case tokens::TokenCode::tkContinue:
				case tokens::TokenCode::tkReturn:
					statements.push_back(createNodeFromTokens("Control", tokens, y));
					break;
				case tokens::TokenCode::tkPrint:
					statements.push_back(createNodeFromTokens("Print", tokens, y));
					break;
				default:
					throw std::invalid_argument("Not a statement");
			}
		}
		return this;
	}

    Node* createNodeFromTokens(const std::string& t, std::vector<tokens::Token>& tokens, int& y) {
        return createNodeByName(t)->from_tokens(tokens, y);
    }

    Node* construct(std::vector<tokens::Token>& tokenized){
        int walker = 0;

        try {
            ast_nodes::BodyNode* tree = new BodyNode();
            tree->from_tokens(tokenized, walker);
            assign_parents(tree);
            reassign_ids(tree);
            return tree;
        } catch (std::invalid_argument& ex) {
            throw std::invalid_argument(std::format("Unexpected token at line {}, pos {}:\n\t{}", tokenized[walker].line, tokenized[walker].pos, ex.what()));
        } catch (std::out_of_range& ex) {
            throw std::invalid_argument("Unexpected program end");
        }
    }
}

#endif // __TREE_PARSER_INCLUDED__
