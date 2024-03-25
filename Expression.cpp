#include "ExpressionParser.h"
#include <iostream>

using namespace expression;

/*
	TEST A input:
	<number of variables: [n]>
		<name 1> <value 1>
		<name 2> <value 2>
		..
		<name n> <value n>
	<number of expressions: [m]>
		<expression 1>
		<expression 2>
		..
		<expression m>
*/

static void TestA() {
	std::map<std::string, double> vars;
	int n;
	std::cin >> n;
	for (int i = 0; i < n; i++) {
		std::string name;
		double value;
		std::cin >> name >> value;
		vars[name] = value;
	}
	int m;
	std::cin >> m;
	ExpressionParser parser;
	std::istringstream* input = nullptr;
	for (int i = 0; i < m; i++) {
		std::string inputString;
		getline(std::cin, inputString);
		getline(std::cin, inputString);
		input = new std::istringstream(inputString);
		parser = ExpressionParser(input);
		Expression* expression = parser.Read();
		std::cout << expression->ToString() << std::endl;
		std::cout << expression->ToMinString() << std::endl;
		std::cout << expression->Evaluate(vars) << std::endl;
	}
}

/*
	TEST B input:
	-> 'V' <name> <value>
	<- vars[name] = value

	-> 'E' <expression>
	<- expressions.push_back(expression)

	-> 'S' <i>
	<- expressions[i].ToString()

	-> 'M' <i>
	<- expressions[i].ToMinString()

	-> 'C' <i>
	<- expressions[i].Evaluate()

	-> 'F' <i>
	<- expressions[i].ToString()
		expressions[i].ToMinString()
		expressions[i].Evaluate()

	-> ANY
	<- end
*/
/*
static void TestB() {
	std::map<std::string, double> vars;
	std::string name;
	int value;
	std::istringstream* input = nullptr;
	input = &std::cin;
	ExpressionParser parser(input);
	std::vector<Expression*> expressions;
	Expression* expression;
	int i;
	while (true) {
		int c;
		std::cin >> c;
		switch (c) {
		case 'V':
			std::cin >> name >> value;
			vars[name] = value;
			break;
		case 'E':
			expression = parser.Read();
			break;
		case 'S':
			std::cin >> i;
			std::cout << expressions[i]->ToString() << std::endl;
			break;
		case 'M':
			std::cin >> i;
			std::cout << expressions[i]->ToMinString() << std::endl;
			break;
		case 'C':
			std::cin >> i;
			std::cout << expressions[i]->Evaluate(vars) << std::endl;
			break;
		case 'F':
			std::cin >> i;
			std::cout << expressions[i]->ToString() << std::endl;
			std::cout << expressions[i]->ToMinString() << std::endl;
			std::cout << expressions[i]->Evaluate(vars) << std::endl;
			break;
		default:
			return;
		}
	}
}
*/

int main()
{
	TestA();
}

