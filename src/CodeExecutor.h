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

/**
 * @typedef ExprValue
 * @brief Type definition for values that can be returned from expression evaluation
 */
using ExprValue = std::variant<bool, int, std::string>;

/**
 * @class CodeExecutor
 * @brief Class for executing code in the Moore machine
 */
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

    /**
     * @brief Checks if the input name is defined
     * 
     * @param inputName The name of the input to check
     * @return true if the input is defined, false otherwise
     */
    bool defined(const std::string& inputName);

    /**
     * @brief Gets the value of the input
     * 
     * @param inputName The name of the input
     * @return The value of the input
     */
    std::string valueof(const std::string& inputName);

    /**
     * @brief Outputs the value to the specified output name
     * 
     * @param outputName The name of the output
     * @param value The value to output
     * @return void
     */
    void output(const std::string& outputName, const std::string& value);

    /**
     * @brief Converts a string to an integer
     * 
     * @param value The string to convert
     * @return The converted integer
     */
    int atoi(const std::string& value);

    /**
     * @brief Compares two expression values based on the operator
     * 
     * @param left The left expression value
     * @param right The right expression value
     * @param op The operator to use for comparison
     * @return true if the comparison is true, false otherwise
     */
    bool compareExprValues(const ExprValue& left, const ExprValue& right, const std::string& op);

public:
    /**
     * @brief Constructor for the CodeExecutor class
     * 
     * @param mooreMachine Reference to the MooreMachine object
     * @param stateExpr The state action expression
     * @param transitionExpr The transition expression
     * @param inputName The name of the input
     * @param inputValue The value of the input
     */
    CodeExecutor(MooreMachine& mooreMachine, const std::string& stateExpr, const std::string& transitionExpr, const std::string& inputName, const std::string& inputValue);
    
    /**
     * @brief Executes the transition expression to determine if a transition should occur
     * 
     * @return true if the transition should be taken, false otherwise
     */
    bool executeTransitionBoolExpr();

    /**
     * @brief Executes actions associated with entering a state
     * 
     * @param expr The expression to execute
     */
    void executeStateExpr(const std::string& expr);

    /**
     * @brief Evaluates a boolean condition expression
     * 
     * @param cond The condition to evaluate
     * @return The boolean result of the condition
     */
    bool evaluateCond(const std::string& cond);

    /**
     * @brief Sets a variable in the Moore machine
     * 
     * @param type The data type of the variable ("int", "bool", "string")
     * @param name The name of the variable
     * @param value The value to assign to the variable
     */
    void setVariable(const std::string &type, const std::string &name, const std::string &value);

    /**
     * @brief Retrieves the value of a variable from the Moore machine
     * 
     * @param name The name of the variable to retrieve
     * @return The string representation of the variable's value
     */
    std::string getVariable(const std::string &name);

    /**
     * @brief Evaluates an expression and returns its value
     * 
     * @param expr The expression to evaluate
     * @return The value of the expression as an ExprValue (bool, int, or string)
     */
    ExprValue evaluateExpr(const std::string& expr);

    /**
     * @brief Checks if a string represents a numeric value
     * 
     * @param expr The string to check
     * @return true if the string is a valid number, false otherwise
     */
    static bool isNumber(const std::string& expr);
};

#endif // CODEEXECUTOR_H
