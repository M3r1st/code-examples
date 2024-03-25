#include <map>
#include <string>
#include <stdexcept>
#include <iomanip>

namespace expression {

	enum ExpressionValue {
		CONST, VAR, NEG, ADD, SUB, MUL, DIV, LP, RP, ASSIGN
	};

	static int GetPriority(ExpressionValue value) {
		switch (value) {
		case CONST:
		case VAR:
			return 0;
		case NEG:
			return 2;
		case MUL:
		case DIV:
			return 3;
		case ADD:
		case SUB:
			return 4;
		case LP:
		case RP:
			return -1;
		default:
			return INT_MAX;
		}
	}

	class Expression {
	public:
		virtual ~Expression() {}
		virtual std::string ToString() = 0;
		virtual std::string ToMinString() = 0;
		virtual double Evaluate(const std::map<std::string, double>& vars) = 0;
		virtual int GetPriority() = 0;
		virtual std::string GetOperator() = 0;
	};

	class PrimaryExpression : public Expression {
	public:
		virtual ~PrimaryExpression() {}

		int GetPriority() override {
			throw std::runtime_error("Expression is not defined.");
		}

		std::string GetOperator() override {
			throw std::runtime_error("Operator is not defined.");
		}
	};

	class Const : public PrimaryExpression {
		double value;

	public:
		Const(double value) {
			this->value = value;
		}

		~Const() {}

		int GetPriority() override {
			return expression::GetPriority(ExpressionValue::CONST);
		}

		std::string ToString() override {
			std::stringstream stream;
			// stream << std::fixed << std::setprecision(2) << value;
			stream << value;
			std::string s = stream.str();
			return s;
		}

		std::string ToMinString() override {
			std::stringstream stream;
			// stream << std::fixed << std::setprecision(2) << value;
			stream << value;
			std::string s = stream.str();
			return s;
		}

		double Evaluate(const std::map<std::string, double>& vars) override {
			return value;
		}
	};

	class Variable : public PrimaryExpression {
		std::string name;

	public:
		Variable(std::string name) {
			this->name = name;
		}

		~Variable() {}

		int GetPriority() override {
			return expression::GetPriority(ExpressionValue::VAR);
		}

		std::string ToString() override {
			return name;
		}

		std::string ToMinString() override {
			return name;
		}

		double Evaluate(const std::map<std::string, double>& vars) override {
			return vars.contains(name) ? vars.at(name) : throw std::out_of_range("Cannot evaluate. Varibable " + name + " is undefined.");
		}
	};

	class UnaryExpression : public PrimaryExpression {
	protected:
		Expression* expression;

	public:
		UnaryExpression(Expression* expression) {
			this->expression = expression;
		}

		~UnaryExpression() {}

		std::string ToString() override {
			return "(" + this->GetOperator() + expression->ToString() + ")";
		}

		std::string ToMinString() override {
			bool expressionPriority = (expression->GetPriority() > this->GetPriority());
			return (expressionPriority ?
				this->GetOperator() + "(" + expression->ToMinString() + ")"
				: this->GetOperator() + expression->ToMinString());
		}
	};

	class Negate : public UnaryExpression {
	public:
		using UnaryExpression::UnaryExpression;

		~Negate() {}

		int GetPriority() override {
			return expression::GetPriority(ExpressionValue::NEG);
		}

		std::string GetOperator() override {
			return "-";
		}

		double Evaluate(const std::map<std::string, double>& vars) override {
			return -this->expression->Evaluate(vars);
		}
	};

	class BinaryExpression : public PrimaryExpression {
	protected:
		Expression* left, * right;

	public:
		BinaryExpression(Expression* left, Expression* right) {
			this->left = left;
			this->right = right;
		}

		~BinaryExpression() {}

		virtual bool IsTransitive() = 0;

		std::string ToString() override {
			return "(" + left->ToString() + " " + this->GetOperator() + " " + right->ToString() + ")";
		}

		std::string ToMinString() override {
			bool leftPriority = (left->GetPriority() > this->GetPriority());
			bool rightPriority = (this->IsTransitive() ?
				right->GetPriority() > this->GetPriority() : right->GetPriority() >= this->GetPriority());
			return (leftPriority ? "(" + left->ToMinString() + ")" : left->ToMinString())
				+ " " + this->GetOperator() + " "
				+ (rightPriority ? "(" + right->ToMinString() + ")" : right->ToMinString());
		}
	};

	class Add : public BinaryExpression {
	public:
		using BinaryExpression::BinaryExpression;

		~Add() {}

		bool IsTransitive() override {
			return true;
		}

		int GetPriority() override {
			return expression::GetPriority(ExpressionValue::ADD);
		}

		std::string GetOperator() override {
			return "+";
		}

		double Evaluate(const std::map<std::string, double>& vars) override {
			return this->left->Evaluate(vars) + this->right->Evaluate(vars);
		}
	};

	class Subtract : public BinaryExpression {
	public:
		using BinaryExpression::BinaryExpression;

		~Subtract() {}

		bool IsTransitive() override {
			return false;
		}

		int GetPriority() override {
			return expression::GetPriority(ExpressionValue::SUB);
		}

		std::string GetOperator() override {
			return "-";
		}

		double Evaluate(const std::map<std::string, double>& vars) override {
			return this->left->Evaluate(vars) - this->right->Evaluate(vars);
		}
	};

	class Multiply : public BinaryExpression {
	public:
		using BinaryExpression::BinaryExpression;

		~Multiply() {}

		bool IsTransitive() override {
			return true;
		}

		int GetPriority() override {
			return expression::GetPriority(ExpressionValue::MUL);
		}

		std::string GetOperator() override {
			return "*";
		}

		double Evaluate(const std::map<std::string, double>& vars) override {
			return this->left->Evaluate(vars) * this->right->Evaluate(vars);
		}
	};

	class Divide : public BinaryExpression {
	public:
		using BinaryExpression::BinaryExpression;

		~Divide() {}

		bool IsTransitive() override {
			return false;
		}

		int GetPriority() override {
			return expression::GetPriority(ExpressionValue::DIV);
		}

		std::string GetOperator() override {
			return "/";
		}

		double Evaluate(const std::map<std::string, double>& vars) override {
			return this->left->Evaluate(vars) / this->right->Evaluate(vars);
		}
	};
}
