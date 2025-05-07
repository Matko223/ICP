/**
 * @file CodeExecutor.h
 * @brief Header file for the CodeExecutor class
 * @author Tomáš Šedo (xsedot00)
*/

#ifndef CODEEXECUTOR_H
#define CODEEXECUTOR_H
#pragma once

#include <string>
#include <vector>
#include <variant>
#include "Structs.h"
#include "MooreMachine.h"

using ExprValue = std::variant<bool, int, std::string>;

class CodeExecutor {
private:
    // Reference to out machine
    MooreMachine& mooreMachine;

    // State action expression that will evaluate when we land on the state
    std::string stateExpr;

    // Transition expression of the state based on the input that will be evaluated
    std::string transitionExpr;

    // Input name to know which input was selected
    std::string inputName;

    // Value of the input
    std::string inputValue;

    // Allowed operators for bool conditions
    std::vector<std::string> operators = {"==", "!=", "<=", ">=", "<", ">"};

    // Built-in functions in our small interpreter
    std::vector<std::string> builtinFuncs = {"atoi", "output", "valueof", "defined", "elapsed"};

    // defined function
    // Checks if the input is the one passed to executor meaning user entered this input with value
    bool defined(const std::string& inputName);

    // valueof function
    // Returns string value of input user typed
    std::string valueof(const std::string& inputName);

    // output function
    // Changes current output based on the output name and value passed
    void output(const std::string& outputName, const std::string& value);

    // atoi function
    // Returns integer representation of string value
    int atoi(const std::string& value);

    // Compares expression values, values can be of different type, we can only compare values of same type, no implicit conversions
    bool compareExprValues(const ExprValue& left, const ExprValue& right, const std::string& op);

public:
    // Constructor
    CodeExecutor(MooreMachine& mooreMachine, const std::string& stateExpr, const std::string& transitionExpr, const std::string& inputName, const std::string& inputValue);
    
    // Executes BoolExpr meant for transition
    bool executeTransitionBoolExpr();

    // Executes state action, occurs when we get to the state
    void executeStateExpr(const std::string& expr);

    // Evaluates given condition
    bool evaluateCond(const std::string& cond);

    // Evaluates each expression and type depends on what is returned
    // For more info see variants
    ExprValue evaluateExpr(const std::string& expr);

    // Helper function to check if the string value is number
    static bool isNumber(const std::string& expr);
};

#endif // CODEEXECUTOR_H
