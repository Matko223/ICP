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

    // Goes through the transitions of each state and checks if they are reachable
    // If the state is reachable it will save the state into visited
    void dfs(int state, std::unordered_set<int>& visited);

    // Check for the validity of type of added variable
    bool isValidType(const std::string& type);

    // Parses string into TransitionExpression for easier work
    TransitionExpression parseExpr(const std::string& expr);

    // Helper function to remove spaces right after [ and right before ]
    std::string trimBracketSpaces(std::string str);

    // Changes Variable struct to json
    nlohmann::ordered_json varToJSON(const Variable& var);

    // Changes TransitionExpression struct to json
    nlohmann::ordered_json transitionExprToJSON(const TransitionExpression& expr);

    // Changes State struct to json
    nlohmann::ordered_json stateToJSON(const State& state, bool onlyStates);

    // Changes json to Variable struct
    Variable jsonToVariable(const nlohmann::ordered_json& j);

    // Checks if delay is valid
    bool delayValid(const std::string& delayValue);

    // Gets the value of the delay
    int getDelayValue(const std::string& delayValue);

public:
    // Constructor
    MooreMachine();

    // Get index of current state
    int getCurrentState();

    // Adds new start state to the machine
    int addStartState(std::string name, std::string outputExpr, const std::unordered_map<TransitionExpression, int>& transitions = {});
    
    // Adds new state to the machine
    int addState(std::string name, std::string outputExpr, const std::unordered_map<TransitionExpression, int>& transitions = {});
    
    // Adds new transitions for the state
    void addTransition(int fromState, const std::string& expr, int toState);
    
    // Adds new variable globally for the whole machine
    // Variable is in format {type} {name} = {value}
    // Checks if the type is valid
    void addVariable(const std::string& type, const std::string& name, const std::string& value = "0");
    
    // Adds inputs to the machine
    void addInputs();
    
    // Adds outputs to the machine
    void addOutputs();
    
    // Adds name to the machine
    void addMachineName(const std::string& machineName);
    
    // Adds description to the machine
    void addMachineDescription(const std::string& machineDescription);
    
    // Set value for output
    // Example: out: 1
    void setCurrentOutput(const std::string& outputName, const std::string& outputValue);
    
    // Set initial output values to empty string for all outputs when the machine starts
    void setInitialOutput();
    
    // Handle start state when we start the machine
    // Even when starting it should do state action
    void processStartState();

    // Processes given input with its value
    // Checks current state, tries to find transition and if it can make transition it will do it and then do state action
    void processInput(const std::string& inputName, const std::string& inputValue);
    
    // Function to check if input we use is valid meaning is in list of inputs
    bool isInputValid(const std::string& inputName);
    
    // Retrieves all the transitions of the state
    std::unordered_map<TransitionExpression, int> getTransitions(State& currentState);

    // Get all the variables defined in the machine
    std::vector<Variable> getVars();

    // Get all the inputs that machine can accept
    std::vector<std::string> getInputs();

    // Resolves delay value and creates thread for the state
    void handleDelay(const std::string& delayValue, int nextState);

    // Sends interruption signal to the thread
    void interruptDelay();

    // Going through all the states, if the state is not in the visited it means it is not reachable
    void checkReachability();
    
    // Going through all the states, if you can't go anywhere from it then it is dead
    void checkDeadStates();

    // Checks every combination of states, if they have same output and transition then they might be redundant
    void checkRedundancy();

    // Method for doing all the checks at once
    void doAllChecks();

    // Helper function to remove all spaces from string
    std::string removeSpaces(std::string str);

    // Prints all the internal machine attributes
    void printMachine();

    // Creates json file from internal attributes
    void createJSONFile(const std::string& name);

    // Loads the file, parses json and add values to internal attributes
    void loadFromJSONFile(const std::string& filename);

    std::string getMachineName() {
        return machineName;
    }

    // Gets machine description
    std::string getMachineDescription() {
        return machineDescription;
    }

    // Gets all states
    const std::vector<State>& getStates() {
        return states;
    }

    // Gets start state
    int getStartState() {
        return startState;
    }

    // Get current output for gui logging
    std::string getCurrentOutput()
    {
        std::string result;
        for (const auto& [out, value] : currentOutput) {
            result += out + ":" + value;
        }
        return result;
    }

    // Callback function for auto transition to the next state (after delay)
    std::function<void(int)> autoTransition;
};

#endif
