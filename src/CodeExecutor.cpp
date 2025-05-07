/**
 * @file CodeExecutor.cpp
 * @brief Implementation of the CodeExecutor class
 * @author Tomáš Šedo (xsedot00)
*/

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "CodeExecutor.h"

using namespace std;

CodeExecutor::CodeExecutor(MooreMachine& mooreMachine, const string& stateExpr, const string& transitionExpr, const string& inputName, const string& inputValue)
    : mooreMachine(mooreMachine), stateExpr(stateExpr), transitionExpr(transitionExpr), inputName(inputName), inputValue(inputValue) {};

bool CodeExecutor::defined(const string& inputName) {
    if (this->inputName == inputName) {
        return true;
    }

    return false;
}

// TODO variable handling
string CodeExecutor::valueof(const string& inputName) {
    for (const auto& input : mooreMachine.getInputs()) {
        if (input == inputName) {
            return inputValue;
        }
    }
    return "";
}

void CodeExecutor::output(const string& outputName, const string& value) {
    mooreMachine.setCurrentOutput(outputName, value);
}

int CodeExecutor::atoi(const string& value) {
    return stoi(value);
}

bool CodeExecutor::executeTransitionBoolExpr() {
    bool condResult = evaluateCond(mooreMachine.removeSpaces(transitionExpr));
    return condResult;
}

// TODO: make it better, limitations
void CodeExecutor::executeStateExpr(const string& expr) {
    // Remove all the whitespaces from the expression, easier to parse
    string trimmedExpr = mooreMachine.removeSpaces(expr);
    // Look for if statement
    size_t ifPos = trimmedExpr.find("if{");
    // Continue till there is no more if statements left
    while (ifPos != string::npos) {
        // if format: if {condition} {body}
        // First we search condition and evaluate given condition
        size_t condStart = trimmedExpr.find('{', ifPos) + 1;
        size_t condEnd = trimmedExpr.find('}', condStart);
        string condition = trimmedExpr.substr(condStart, condEnd - condStart);
        bool condResult = evaluateCond(condition);

        // Then we look for body and evaluate it
        // TODO
        size_t bodyStart = trimmedExpr.find('{', condEnd) + 1;
        size_t bodyEnd = trimmedExpr.find('}', bodyStart);
        string body = trimmedExpr.substr(bodyStart, bodyEnd - bodyStart);
        cout << body << endl;

        // We remove whole if statement and continue with rest of the expression
        trimmedExpr = trimmedExpr.substr(bodyEnd + 1);
        ifPos = trimmedExpr.find("if{");
    }

    // Look for built-in functions in the expr
    // TODO: remove found func from the expr
    string builtinFuncExpr;
    size_t builtinFuncPos;
    for (const auto& builtinFunc : builtinFuncs) {
        size_t pos = trimmedExpr.find(builtinFunc);
        if (pos != string::npos) {
            builtinFuncExpr = trimmedExpr.substr(pos, trimmedExpr.find(')', pos) + 1 - pos);;
            builtinFuncPos = pos;
        }
    }

    ExprValue builtinFuncEval = evaluateExpr(builtinFuncExpr);

    // TODO: variables
}

bool CodeExecutor::evaluateCond(const string& cond) {
    string operatorFound;
    size_t operatorPos;

    // Look for the operator
    for (const auto& op : operators) {
        size_t pos = cond.find(op);
        if (pos != string::npos) {
            operatorFound = op;
            operatorPos = pos;
            break;
        }
    }

    // If operator was not found then it means it can be built-in func that return bool value like defined
    if (operatorFound.empty()) {
        ExprValue exprEval = evaluateExpr(cond);
        if (holds_alternative<bool>(exprEval)) {
            return get<bool>(exprEval);
        }

        else {
            cout << "Transition expression condition doesn't return bool value" << endl;
        }
    }

    // Extract expression from both sides
    string left = cond.substr(0, operatorPos);
    string right = cond.substr(operatorPos + operatorFound.length());

    // Evaluate both expressions
    ExprValue leftEval = evaluateExpr(left);
    ExprValue rightEval = evaluateExpr(right);

    // Compare those expressions
    bool condResult = compareExprValues(leftEval, rightEval, operatorFound);
    return condResult;
}

ExprValue CodeExecutor::evaluateExpr(const string& expr) {
    if ( expr == "") {
        return -1;
    }

    // defined func
    size_t definedPos = expr.find("defined(");
    if (definedPos != string::npos) {
        size_t varStart = expr.find('"', definedPos) + 1;
        size_t varEnd = expr.find('"', varStart);
        string varName = expr.substr(varStart, varEnd - varStart);

        return defined(varName);
    }

    // atoi func
    // We check if the string inside is a number and only then make it an integer
    // Recursively checks if variable or input was given
    size_t atoiPos = expr.find("atoi(");
    if (atoiPos != string::npos) {
        size_t insideExprStart = expr.find('(', atoiPos) + 1;
        size_t insideExprEnd = expr.find(')', insideExprStart);
        string insideExpr = expr.substr(insideExprStart, insideExprEnd - insideExprStart);

        if (holds_alternative<string>(evaluateExpr(insideExpr)) && isNumber(get<string>(evaluateExpr(insideExpr)))) {
            return atoi(get<string>(evaluateExpr(insideExpr)));
        };
    }

    // valueof func
    // TODO: Works only with inputs, would be great for variables too
    size_t valueofPos = expr.find("valueof(");
    if (valueofPos != string::npos) {
        size_t varStart = expr.find('"', valueofPos) + 1;
        size_t varEnd = expr.find('"', varStart);
        string varName = expr.substr(varStart, varEnd - varStart);

        return valueof(varName);
    }

    // output func
    // Extracts output name and value to be assigned in it
    size_t outputPos = expr.find("output(");
    if (outputPos != string::npos) {
        size_t outNameStart = expr.find('"', outputPos) + 1;
        size_t outNameEnd = expr.find('"', outNameStart);
        string outName = expr.substr(outNameStart, outNameEnd - outNameStart);
        size_t outValueStart = expr.find(',', outputPos) + 1;
        size_t outValueEnd = expr.find(')', outValueStart);
        string outValue = expr.substr(outValueStart, outValueEnd - outValueStart);
        output(outName, outValue);
        return -1;
    }

    // Expressions like string, integer
    if (isNumber(expr)) {
        return stoi(expr);
    }

    // TODO: would be great to add variable value changing
    // Example timeout = valueof('in')

    return -1;
}

bool CodeExecutor::isNumber(const string& expr) {
    for (char ch : expr) {
        if (!isdigit(ch)) {
            return false;
        }
    }

    return true;
}

bool CodeExecutor::compareExprValues(const ExprValue& left, const ExprValue& right, const string& op) {
    // Only compare if the types are the same
    if (left.index() != right.index()) {
        return false;
    }

    // Both are bool
    if (holds_alternative<bool>(left) && holds_alternative<bool>(right)) {
        bool leftValue = get<bool>(left);
        bool rightValue = get<bool>(right);
        if (op == "==") {
            return leftValue == rightValue;
        }
        if (op == "!=") {
            return leftValue != rightValue;
        }
    }

    // Both are int
    else if (holds_alternative<int>(left) && holds_alternative<int>(right)) {
        int leftValue = get<int>(left);
        int rightValue = get<int>(right);
        if (op == "==") {
            return leftValue == rightValue;
        }
        if (op == "!=") {
            return leftValue != rightValue;
        }
        if (op == "<") {
            return leftValue < rightValue;
        }
        if (op == "<=") {
            return leftValue <= rightValue;
        }
        if (op == ">") {
            return leftValue > rightValue;
        }
        if (op == ">=") {
            return leftValue >= rightValue;
        }
    }

    // Both are string
    else if (holds_alternative<string>(left) && holds_alternative<string>(right)) {
        string leftValue = get<string>(left);
        string rightValue = get<string>(right);
        if (op == "==") {
            return leftValue == rightValue;
        }
        if (op == "!=") {
            return leftValue != rightValue;
        }
    }

    return false;
}
