/**
 * @file MooreMachine.h
 * @brief Header file for the MooreMachine class
 * @author Tomáš Šedo (xsedot00)
*/

#ifndef MOORE_MACHINE_H
#define MOORE_MACHINE_H
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include "json.hpp"
#include "Structs.h"

class MooreMachine {
private:
    // Name of the machine
    std::string machineName;

    // Description of the machine
    std::string machineDescription;

    // States for the machine
    std::vector<State> states;

    // Keeps track of current state, at the beginning set to startState value
    int currentState;

    // Stores index of start state in states, set to -1 meaning nothing assigned
    int startState = -1;

    // Variables defined for this machine
    std::vector<Variable> variables;

    // All the machines inputs
    std::vector<std::string> inputs;

    // All the machines outputs
    std::vector<std::string> outputs;

    // Allowed types for variables
    std::vector<std::string> allowedTypes = {"int", "string", "char", "bool", "double", "float"};

    // Contains output of the current state
    std::unordered_map<std::string, std::string> currentOutput;

    // Contains whole output history
    std::unordered_map<std::string, std::string> outputHistory;

    // Value to tell if delay should be cancelled or not
    bool delayCancel = false;

    // Value to tell if delay is active or not
    bool delayActive = false;

    // Mutex for protecting shared data in the thread
    std::mutex mtx;

    // For blocking threads
    std::condition_variable cv;

    /**
     * @brief Depth-first search for reachability check
     * @param state Current state index
     * @param visited Set of visited state indices
     */
    void dfs(int state, std::unordered_set<int>& visited);

    /**
     * @brief Checks if type is among allowed variable types
     * @param type Type to validate
     * @return true if type is valid, false otherwise
     */
    bool isValidType(const std::string& type);

    /**
     * @brief Parses string into TransitionExpression
     * @param expr Expression string to parse
     * @return Parsed TransitionExpression
     */
    TransitionExpression parseExpr(const std::string& expr);

    /**
     * @brief Removes spaces after [ and before ]
     * @param str String to trim
     * @return Trimmed string
     */
    std::string trimBracketSpaces(std::string str);

    /**
     * @brief Converts Variable to JSON
     * @param var Variable to convert
     * @return JSON representation of Variable
     */
    nlohmann::ordered_json varToJSON(const Variable& var);

    /**
     * @brief Converts TransitionExpression to JSON
     * @param expr TransitionExpression to convert
     * @return JSON representation of TransitionExpression
     */
    nlohmann::ordered_json transitionExprToJSON(const TransitionExpression& expr);

    /**
     * @brief Converts State to JSON
     * @param state State to convert
     * @param onlyStates If true, only state info without transitions
     * @return JSON representation of State
     */
    nlohmann::ordered_json stateToJSON(const State& state, bool onlyStates);

    /**
     * @brief Converts JSON to Variable
     * @param j JSON to convert
     * @return Variable created from JSON
     */
    Variable jsonToVariable(const nlohmann::ordered_json& j);

    /**
     * @brief Validates delay value format
     * @param delayValue Delay value to check
     * @return true if valid, false otherwise
     */
    bool delayValid(const std::string& delayValue);

    /**
     * @brief Parses delay value to milliseconds
     * @param delayValue Delay value string
     * @return Delay in milliseconds
     */
    int getDelayValue(const std::string& delayValue);

public:
    /**
     * @brief Default constructor
     */
    MooreMachine();

    /**
     * @brief Gets current state index
     * @return Index of current state
     */
    int getCurrentState();

    /**
     * @brief Adds start state to machine
     * @param name State name
     * @param outputExpr Output expression
     * @param transitions Initial transitions map
     * @return Index of added state
     */
    int addStartState(std::string name, std::string outputExpr, const std::unordered_map<TransitionExpression, int>& transitions = {});

    /**
     * @brief Adds regular state to machine
     * @param name State name
     * @param outputExpr Output expression
     * @param transitions Initial transitions map
     * @return Index of added state
     */
    int addState(std::string name, std::string outputExpr, const std::unordered_map<TransitionExpression, int>& transitions = {});

    /**
     * @brief Adds transition between states
     * @param fromState Source state index
     * @param expr Transition expression
     * @param toState Destination state index
     */
    void addTransition(int fromState, const std::string& expr, int toState);

    /**
     * @brief Adds variable to machine
     * @param type Variable type
     * @param name Variable name
     * @param value Initial value (defaults to "0")
     */
    void addVariable(const std::string& type, const std::string& name, const std::string& value = "0");

    /**
     * @brief Adds inputs to machine
     */
    void addInputs();

    /**
     * @brief Adds outputs to machine
     */
    void addOutputs();

    /**
     * @brief Sets machine name
     * @param machineName Name to set
     */
    void addMachineName(const std::string& machineName);

    /**
     * @brief Sets machine description
     * @param machineDescription Description to set
     */
    void addMachineDescription(const std::string& machineDescription);

    /**
     * @brief Sets output value
     * @param outputName Output name
     * @param outputValue Output value
     */
    void setCurrentOutput(const std::string& outputName, const std::string& outputValue);

    /**
     * @brief Initializes all outputs to empty string
     */
    void setInitialOutput();

    /**
     * @brief Processes start state actions
     */
    void processStartState();

    /**
     * @brief Processes input and performs transitions
     * @param inputName Input name
     * @param inputValue Input value
     */
    void processInput(const std::string& inputName, const std::string& inputValue);

    /**
     * @brief Validates input name
     * @param inputName Input name to check
     * @return true if input is valid, false otherwise
     */
    bool isInputValid(const std::string& inputName);

    /**
     * @brief Gets transitions for a state
     * @param currentState State to get transitions for
     * @return Map of transition expressions to destination states
     */
    std::unordered_map<TransitionExpression, int> getTransitions(State& currentState);

    /**
     * @brief Gets all variables
     * @return Vector of variables
     */
    std::vector<Variable> getVars();

        /**
     * @brief Gets all inputs
     * @return Vector of input names
     */
    std::vector<std::string> getInputs();

    /**
     * @brief Sets up delay transition
     * @param delayValue Delay value string
     * @param nextState Destination state index
     */
    void handleDelay(const std::string& delayValue, int nextState);

    /**
     * @brief Cancels current delay
     */
    void interruptDelay();

    /**
     * @brief Checks state reachability
     */
    void checkReachability();

    /**
     * @brief Identifies dead states
     */
    void checkDeadStates();

    /**
     * @brief Checks for redundant states
     */
    void checkRedundancy();

    /**
     * @brief Performs all validation checks
     */
    void doAllChecks();

    /**
     * @brief Removes spaces from string
     * @param str String to process
     * @return String without spaces
     */
    std::string removeSpaces(std::string str);

    /**
     * @brief Prints machine information
     */
    void printMachine();

    /**
     * @brief Exports machine to JSON file
     * @param name Filename
     */
    void createJSONFile(const std::string& name);

    /**
     * @brief Loads machine from JSON file
     * @param filename File to load from
     */
    void loadFromJSONFile(const std::string& filename);

    /**
     * @brief Gets machine as JSON object
     * @return JSON representation of machine
     */
    nlohmann::ordered_json getJson();

    /**
     * @brief Gets machine name
     * @return Machine name
     */
    std::string getMachineName() {
        return machineName;
    }

    /**
     * @brief Gets machine description
     * @return Machine description
     */
    std::string getMachineDescription() {
        return machineDescription;
    }

    /**
     * @brief Gets all states
     * @return Reference to states vector
     */
    const std::vector<State>& getStates() {
        return states;
    }

    /**
     * @brief Gets start state index
     * @return Start state index
     */
    int getStartState() {
        return startState;
    }

    /**
     * @brief Gets current output as string
     * @return Formatted current output
     */
    std::string getCurrentOutput()
    {
        std::string result;
        for (const auto& [out, value] : currentOutput) {
            result += out + ":" + value;
        }
        return result;
    }

    /**
     * @brief Gets variables
     * @return Reference to variables vector
     */
    std::vector<Variable>& getVariables() {
        return variables;
    }

    /**
     * @brief Gets allowed variable types
     * @return Vector of allowed types
     */
    std::vector<std::string> getAllowedTypes();

    // Callback function for auto transition to the next state (after delay)
    std::function<void(int)> autoTransition;

    /**
     * @brief clear everything
     */
    void clear();
};

#endif
