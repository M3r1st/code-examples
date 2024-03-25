#include <iostream>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Expression.h"

namespace expression {

	class ExpressionParser {

		enum TokenValue : char {
			NAME, NUMBER, END = '$',
			ADD = '+', SUB = '-', MUL = '*', DIV = '/',
			PRINT = ';', ASSIGN = '=', LP = '(', RP = ')'
		};

		enum NumberValue : char {
			NUM0 = '0',
			NUM1 = '1', NUM2 = '2', NUM3 = '3',
			NUM4 = '4', NUM5 = '5', NUM6 = '6',
			NUM7 = '7', NUM8 = '8', NUM9 = '9',
			NUMP = '.'
		};

		bool IsOperator(std::string s) {
			const std::vector<std::string> operators = {
				"+", "-", "*", "/", "(", ")"
			};
			return std::find(operators.begin(), operators.end(), s) != std::end(operators);
		}

		struct StringToken {
			std::string stringValue;
			ExpressionValue expressionValue;
			Expression* expression;
			StringToken* parent;

			StringToken* GetHead() {
				if (parent == nullptr) {
					return this;
				} else {
					return parent->GetHead();
				}
			}
		};

		struct listToken {
			StringToken* head;
		};

		Expression* Parse(std::vector<StringToken>& tokens, int left, ExpressionValue delimiter) {
			int right = left;
			while (right < tokens.size()) {
				if (tokens[right].expressionValue == delimiter) {
					break;
				}
				right++;
			}
			return Parse(tokens, left, right);
		}

		Expression* Parse(std::vector<StringToken>& tokens, int left, int right) {
			std::vector<std::pair<int, StringToken*>> ids(right - left);
			for (int i = left; i < right; i++) {
				ids[i - left].first = i;
				ids[i - left].second = &tokens[i];
			}
			sort(ids.begin(), ids.end(), [](const std::pair<int, StringToken*>& left, const std::pair<int, StringToken*>& right) {
				int lp = GetPriority(left.second->expressionValue);
				int rp = GetPriority(right.second->expressionValue);
				if (lp < rp) {
					return true;
				} else if (lp == rp) {
					return left.first < right.first;
				}
				return false;
			});
			for  (std::pair<int, StringToken*> id : ids) {
				int i = id.first;
				if (tokens[i].expression != nullptr) {
					continue;
				}
				switch (tokens[i].expressionValue) {
				case ExpressionValue::CONST:
					tokens[i].expression = new Const(std::stod(tokens[i].stringValue));
					break;
				case ExpressionValue::VAR:
					tokens[i].expression = new Variable(tokens[i].stringValue);
					break;
				case ExpressionValue::NEG:
					tokens[i].expression = new Negate(tokens[i + 1].GetHead()->expression);
					tokens[i + 1].GetHead()->parent = &tokens[i];
					break;
				case ExpressionValue::ADD:
					tokens[i].expression = new Add(tokens[i - 1].GetHead()->expression, tokens[i + 1].GetHead()->expression);
					tokens[i - 1].GetHead()->parent = &tokens[i];
					tokens[i + 1].GetHead()->parent = &tokens[i];
					break;
				case ExpressionValue::SUB:
					tokens[i].expression = new Subtract(tokens[i - 1].GetHead()->expression, tokens[i + 1].GetHead()->expression);
					tokens[i - 1].GetHead()->parent = &tokens[i];
					tokens[i + 1].GetHead()->parent = &tokens[i];
					break;
				case ExpressionValue::MUL:
					tokens[i].expression = new Multiply(tokens[i - 1].GetHead()->expression, tokens[i + 1].GetHead()->expression);
					tokens[i - 1].GetHead()->parent = &tokens[i];
					tokens[i + 1].GetHead()->parent = &tokens[i];
					break;
				case ExpressionValue::DIV:
					tokens[i].expression = new Divide(tokens[i - 1].GetHead()->expression, tokens[i + 1].GetHead()->expression);
					tokens[i - 1].GetHead()->parent = &tokens[i];
					tokens[i + 1].GetHead()->parent = &tokens[i];
					break;
				case ExpressionValue::LP:
					tokens[i].expression = Parse(tokens, i + 1, ExpressionValue::RP);
					tokens[i].parent = tokens[i + 1].GetHead();
					break;
				case ExpressionValue::RP:
					tokens[i].expression = tokens[i - 1].GetHead()->expression;
					tokens[i].parent = tokens[i - 1].GetHead();
					break;
				default:
					break;
				}
			}
			Expression* expression = tokens[left].GetHead()->expression;
			return expression;
		}

	protected:
		std::istream* input;

	public:
		ExpressionParser() {
			input = nullptr;
		}

		ExpressionParser(std::istream* input) {
			this->input = input;
		}

		~ExpressionParser() {}

		Expression* Read() {
			char c;
			std::vector<StringToken> tokens;
			std::string stringValue;
			int numberValue;
			ExpressionValue expr;
			bool flag = false;

			while (input->get(c)) {
				if (c != '\n' && isspace(c)) {
					continue;
				}
				switch (c) {
				case 0:
					return nullptr;
				case PRINT:
					flag = true;
					break;
				case '\n':
					flag = true;
					break;
				case ADD:
					expr = ExpressionValue::ADD;
					tokens.push_back({ std::string(1, c), expr });
					break;
				case SUB:
					if (IsOperator(tokens.back().stringValue)) {
						expr = ExpressionValue::NEG;
						tokens.push_back({ std::string(1, c), expr });
					}
					else {
						expr = ExpressionValue::SUB;
						tokens.push_back({ std::string(1, c), expr });
					}
					break;
				case MUL:
					expr = ExpressionValue::MUL;
					tokens.push_back({ std::string(1, c), expr });
					break;
				case DIV:
					expr = ExpressionValue::DIV;
					tokens.push_back({ std::string(1, c), expr });
					break;
				case LP:
					expr = ExpressionValue::LP;
					tokens.push_back({ std::string(1, c), expr });
					break;
				case RP:
					expr = ExpressionValue::RP;
					tokens.push_back({ std::string(1, c), expr });
					break;
				case ASSIGN:
					break;
				case NUM0: case NUM1: case NUM2: case NUM3: case NUM4: case NUM5: case NUM6: case NUM7: case NUM8: case NUM9: case NUMP:
					input->putback(c);
					*input >> numberValue;
					expr = ExpressionValue::CONST;
					tokens.push_back({ std::to_string(numberValue), expr });
					break;
				default:
					if (isalpha(c)) {
						stringValue = c;
						while (input->get(c) && isalnum(c)) {
							stringValue.push_back(c);
						}
						input->putback(c);
						expr = ExpressionValue::VAR;
						tokens.push_back({ stringValue, expr });
					}
					break;
				}
				if (flag) {
					break;
				}
			}

			return Parse(tokens, 0, tokens.size());
		}
	};
}
