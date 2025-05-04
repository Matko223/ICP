#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "CodeExecutor.h"
#include "MooreMachine.h"
#include <fstream>
using namespace std;

void MooreMachine::dfs(int state, unordered_set<int>& visited) {
    visited.insert(state);
    for (const auto& transition : states[state].transitions) {
        int nextState = transition.second;
        if (visited.find(nextState) == visited.end()) {
            dfs(nextState, visited);
        }
    }
}

bool MooreMachine::isValidType(const string& type) {
    for (const string& t : allowedTypes) {
        if (t == type) {
            return true;
        }
    }

    return false;
}

TransitionExpression MooreMachine::parseExpr(const string& expr) {
    TransitionExpression parsedExpr;

    // Tries to find position of @
    // If it finds it then it returns value that follows it
    size_t atPosition = expr.find("@");
    if (atPosition != string::npos) {
        parsedExpr.delay = removeSpaces(expr.substr(atPosition + 1));
    }

    else {
        parsedExpr.delay = "";
    }

    // Tries to find position of []
    // If it finds it then it returns value that is inside
    size_t leftBracket = expr.find('[');
    size_t rightBracket = expr.find(']');
    if (leftBracket != string::npos && rightBracket != string::npos && rightBracket > leftBracket) {
        // Trim spaces between expression and []
        string modifiedBoolExpr = trimBracketSpaces(expr.substr(leftBracket, rightBracket - leftBracket + 1));
        size_t modifiedBoolExprLeftBracket = modifiedBoolExpr.find('[');
        size_t modifiedBoolExprRightBracket = modifiedBoolExpr.find(']');
        parsedExpr.boolExpr = modifiedBoolExpr.substr(modifiedBoolExprLeftBracket + 1, modifiedBoolExprRightBracket - modifiedBoolExprLeftBracket - 1);
    }

    else {
        parsedExpr.boolExpr = "";
    }

    // Try to extract input event (everything before [ or @)
    if (leftBracket != string::npos) {
        parsedExpr.inputEvent = removeSpaces(expr.substr(0, leftBracket));
    }

    else if (atPosition != string::npos) {
        parsedExpr.inputEvent = removeSpaces(expr.substr(0, atPosition));
    }

    else {
        parsedExpr.inputEvent = expr;
    }

    return parsedExpr;
}

string MooreMachine::removeSpaces(string str) {
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

string MooreMachine::trimBracketSpaces(string str) {
    // Location of [
    size_t openBracket = str.find('[');

    // Handle whitespaces after [
    if (openBracket != string::npos && openBracket + 1 < str.size()) {
        while (str[openBracket + 1] == ' ') {
            str.erase(openBracket + 1, 1);
        }
    }

    // Location of ]
    size_t closeBracket = str.find(']');
    // Handle whitespace before ]
    if (closeBracket != string::npos && closeBracket > 0) {
        while (str[closeBracket - 1] == ' ') {
            str.erase(closeBracket - 1, 1);
            --closeBracket;
        }
    }
    return str;
}

MooreMachine::MooreMachine() = default;

int MooreMachine::addStartState(string name, string outputExpr, const unordered_map<TransitionExpression, int>& transitions) {
    //If no start state is assigned add start state and return index
    if (startState == -1) {
        int stateIndex = states.size();
        states.push_back(State{name, outputExpr, transitions});
        startState = currentState = stateIndex;
        return stateIndex;
    }

    //If start index is set then return -1 and don't set start state since there can be only one
    else {
        cout << "Start State already exists, you can't add another one. It is at index: " << startState << endl;
        return -1;
    }
}

int MooreMachine::addState(string name, string outputExpr, const unordered_map<TransitionExpression, int>& transitions) {
    states.push_back(State{name, outputExpr, transitions});
    // Returns index in an array
    return states.size() - 1;
}

void MooreMachine::addTransition(int fromState, const string& expr, int toState) {
    states[fromState].transitions[parseExpr(expr)] = toState;
}


// TODO? add check if value can be of the selected type
void MooreMachine::addVariable(const string& type, const string& name, const string& value) {
    if (!isValidType(type)) {
        cout << "Invalid type: " << type << endl;
        return;
    }
    for (const auto& var : variables) {
        if (var.name == name) {
            cout << "Variable \"" << name << "\" already defined." << endl;
            return;
        }
    }

    variables.push_back({type, name, value});
}

void MooreMachine::addInput(const string& inputName) {
    if (find(inputs.begin(), inputs.end(), inputName) == inputs.end()) {
        inputs.push_back(inputName);
    }

    else {
        cout << "Input \"" << inputName << "\" already exists" << endl;
    }
}

void MooreMachine::addOutput(const string& outputName) {
    if (find(outputs.begin(), outputs.end(), outputName) == outputs.end()) {
        outputs.push_back(outputName);
    }

    else {
        cout << "Output \"" << outputName << "\" already exists" << endl;
    }
}

void MooreMachine::addMachineName(const string& machineName) {
    this->machineName = machineName;
}

void MooreMachine::addMachineDescription(const string& machineDescription) {
    this->machineDescription = machineDescription;
}

void MooreMachine::setCurrentOutput(const string& outputName, const string& outputValue) {
    currentOutput[outputName] = outputValue;
}

void MooreMachine::setInitialOutput() {
    for (const auto& output : outputs) {
        setCurrentOutput(output, "");
    }
}

void MooreMachine::processStartState() {
    CodeExecutor executor(*this, states[currentState].outputExpr, "", "", "");
    executor.executeStateExpr(states[currentState].outputExpr);
}

// TODO: handle only inputEvent and timeout, multiple combinations too
void MooreMachine::processInput(const string& inputName, const string& inputValue) {
    if(isInputValid(inputName)) {

        // Get all the transitions for current state
        unordered_map<TransitionExpression, int> stateTransitions = getTransitions(states[currentState]);

        // Find if we can do transition to next state
        for (const auto& [expr, nextStateId] : stateTransitions) {

            // Transition contains input we chose
            if (expr.inputEvent == inputName) {

                // BoolExpr in [] is existing so we have to handle it
                if (expr.boolExpr != "") {
                    // Start new executor
                    CodeExecutor executor(*this, states[currentState].outputExpr, expr.boolExpr, inputName, inputValue);
                    // Returns bool to know if we can do the transition
                    bool transitionByBool = executor.executeTransitionBoolExpr();

                    // If the transition is possible we move to the next state and do the next state action
                    if(transitionByBool) {
                        currentState = nextStateId;
                        executor.executeStateExpr(states[currentState].outputExpr);
                    }
                }
                else {
                    cout << "Not implemented" << endl;
                }
            }
        }
    }

    else {
        cout << "Input " << "\"" + inputName + "\" " << "is not valid" << endl;
    }
}

bool MooreMachine::isInputValid(const string& inputName) {
    for (const auto& input : inputs) {
        if (input == inputName) {
            return true;
        }
    }

    return false;
}

unordered_map<TransitionExpression, int> MooreMachine::getTransitions(State& currentState) {
    return currentState.transitions;
}

vector<Variable> MooreMachine::getVars() {
    return variables;
}

vector<string> MooreMachine::getInputs() {
    return inputs;
}

void MooreMachine::checkReachability() {
    unordered_set<int> visited;
    dfs(0, visited); // Start from state 0

    // Check which states are not reachable
    for (size_t i = 0; i < states.size(); ++i) {
        if (visited.find(i) == visited.end()) {
            cout << "State \"" << states[i].name << "\" is not reachable!" << endl;
        }
    }
}

void MooreMachine::checkDeadStates() {
    for (size_t i = 0; i < states.size(); ++i) {
        if (states[i].transitions.empty()) {
            cout << "State \"" << states[i].name << "\" is a dead state, can't go anywhere from it" << endl;
        }
    }
}

void MooreMachine::checkRedundancy() {
    for (size_t i = 0; i < states.size(); ++i) {
        for (size_t j = i + 1; j < states.size(); ++j) {
            if (states[i].outputExpr == states[j].outputExpr &&
                states[i].transitions == states[j].transitions) {
                cout << "States \"" << states[i].name << "\" and \"" << states[j].name << "\" are equivalent (potentially redundant)." << endl;
            }
        }
    }
}

void MooreMachine::doAllChecks() {
    checkReachability();
    checkDeadStates();
    checkRedundancy();
}

void MooreMachine::printMachine() {
    cout << "----- Moore Machine Details -----\n";

    // Prints name of the machine
    cout << "Machine name:\n" 
    << "   " << machineName << "\n";

    // Prints description of the machine
    cout << "Machine description:\n" << "   " << machineDescription << "\n";

    // Print inputs
    cout << "Inputs:\n";
    for (const auto& input : inputs) {
        cout << "   " << input << "\n";
    }

    // Print outputs
    cout << "Outputs:\n";
    for (const auto& output : outputs) {
        cout << "   " << output << "\n";
    }

    // Print variables in machine
    cout << "Variables:\n";
    for (const auto& var : variables) {
        cout << "   " << var.type << " " << var.name << " = " << var.value << "\n";
    }

    // Print all the states with their index, output and all transitions
    cout << "States:\n";
    for (size_t i = 0; i < states.size(); ++i) {
        cout << "   " << states[i].name << ": " << states[i].outputExpr << "\n";
    }

    cout << "Transitions:\n";
    for (size_t i = 0; i < states.size(); ++i) {
        for (const auto& transition : states[i].transitions) {
            cout << "   " << states[i].name + " --> " + states[transition.second].name + ": "
            << transition.first.inputEvent
            << (transition.first.boolExpr.empty() ? "" : " [ " + transition.first.boolExpr + " ]")
            << (transition.first.delay.empty() ? "" : " @ " + transition.first.delay)
            << "\n";
        }
    }

    // Print current state, at the start should be same as startState
    cout << "Current State:\n" << "   " << currentState << "\n";

    // Print current state, at the start should be same as startState
    cout << "Current Outputs:" << endl;
    for (const auto& output : currentOutput) {
        cout << "   " << output.first << ": " << output.second << endl;
    }

    // Print start state
    cout << "Start State:\n" << "   " << (startState == -1 ? "Start state isn't set" : to_string(startState)) << "\n";

    // Print allowed types for variables
    cout << "Allowed Types for Variables:\n";
    for (const auto& type : allowedTypes) {
        cout << "   " << type << "\n";
    }

    cout << "--------------------------------\n";
}

void MooreMachine::createJSONFile(const string& name) {
    nlohmann::ordered_json jsonFile;
    jsonFile["name"] = machineName;
    jsonFile["description"] = machineDescription;
    jsonFile["inputs"] = inputs;
    jsonFile["outputs"] = outputs;
    
    vector<nlohmann::ordered_json> varsJson;
    for (const auto& var : variables) {
        varsJson.push_back(varToJSON(var));
    }
    jsonFile["variables"] = varsJson;

    std::vector<nlohmann::ordered_json> statesJson;
    for (const auto& state : states) {
        statesJson.push_back(stateToJSON(state, true));
    }
    jsonFile["states"] = statesJson;

    std::vector<nlohmann::ordered_json> transitionsJson;
    for (const auto& state : states) {
        transitionsJson.push_back(stateToJSON(state, false));
    }
    jsonFile["transitions"] = transitionsJson;

    ofstream file(name + ".json");
    file << jsonFile.dump(4);
    file.close();
}

nlohmann::ordered_json MooreMachine::varToJSON(const Variable& var) {
    return nlohmann::ordered_json{
        {"type", var.type},
        {"name", var.name},
        {"value", var.value}
    };
}

nlohmann::ordered_json MooreMachine::transitionExprToJSON(const TransitionExpression& expr) {
    return nlohmann::ordered_json{
        {"inputEvent", expr.inputEvent},
        {"boolExpr", expr.boolExpr},
        {"delay", expr.delay}
    };
}

nlohmann::ordered_json MooreMachine::stateToJSON(const State& state, bool onlyStates) {
    nlohmann::ordered_json j;
    j["name"] = state.name;
    if (onlyStates) {
        j["outputExpr"] = state.outputExpr;
        return j;
    }

    else {
        // Transitions array
        std::vector<nlohmann::ordered_json> transitionsJson;
        for (const auto& [expr, nextState] : state.transitions) {
            transitionsJson.push_back({
                {"expression", transitionExprToJSON(expr)},
                {"nextState", states[nextState].name}
            });
        }
        j["transitions"] = transitionsJson;
        return j;
    }
}

Variable MooreMachine::jsonToVariable(const nlohmann::ordered_json& j) {
    return {
        j.at("type").get<string>(),
        j.at("name").get<string>(),
        j.at("value").get<string>()
    };
}

void MooreMachine::loadFromJSONFile(const string& filename) {
    ifstream file(filename);
    // Check if file exists/can be opened
    if (!file.is_open()) {
        cout << "Failed to open file: " << filename << endl;
        return;
    }

    nlohmann::ordered_json jsonFile;
    file >> jsonFile;

    // Load name and description
    machineName = jsonFile.at("name").get<string>();
    machineDescription = jsonFile.at("description").get<string>();

    // Load inputs
    inputs = jsonFile.at("inputs").get<vector<string>>();

    // Load outputs
    outputs = jsonFile.at("outputs").get<vector<string>>();

    // Load variables
    variables.clear();
    for (const auto& varJson : jsonFile.at("variables")) {
        variables.push_back(jsonToVariable(varJson));
    }

    // Load states
    for (const auto& stateJson : jsonFile.at("states")) {
        State state;
        state.name = stateJson.at("name").get<string>();
        state.outputExpr = stateJson.at("outputExpr").get<string>();
        states.push_back(state);
    }

    // Load transitions for states
    for (const auto& transBlock : jsonFile.at("transitions")) {
        string stateName = transBlock.at("name").get<string>();
    
        // Find the corresponding state by name
        auto it = find_if(states.begin(), states.end(), [&](const State& s) {
            return s.name == stateName;
        });
        
        // If state exists get all its transitions
        if (it != states.end()) {
            for (const auto& transition : transBlock.at("transitions")) {
                TransitionExpression expr;
                auto exprJson = transition.at("expression");
                expr.inputEvent = exprJson.at("inputEvent").get<string>();
                expr.boolExpr = exprJson.at("boolExpr").get<string>();
                expr.delay = exprJson.at("delay").get<string>();
    
                string nextStateName = transition.at("nextState").get<string>();
    
                // Find next state's index
                auto nextIt = find_if(states.begin(), states.end(), [&](const State& s) {
                    return s.name == nextStateName;
                });
                
                // If next state exists then get its index
                if (nextIt != states.end()) {
                    // Compute the index of found state
                    int nextIndex = distance(states.begin(), nextIt);
                    it->transitions[expr] = nextIndex;
                } else {
                    cout << "Next state not found: " << nextStateName << endl;
                }
            }
        } else {
            cout << "State not found for transitions: " << stateName << endl;
        }
    }

    // Set other initial attributes
    currentState = 0;
    startState = 0;
    setInitialOutput();
}

/* int main() {
    MooreMachine machine;

    // Add name and description
    machine.addMachineName("Example machine");
    machine.addMachineDescription("This is the description of the machine");

    // Add states
    int s0 = machine.addStartState("IDLE", "{ output(\"out\", 0) }"); // name = IDLE, state 0, output = 1, start state
    int s1 = machine.addState("ACTIVE", "{ output(\"out\", 1) }"); // name = ACTIVE, state 1, output = 0
    int s2 = machine.addState("TIMING", "{ }"); // name = TIMING,  state 2, output = -1

    // Add variables
    machine.addVariable("int", "timeout", "5000");

    machine.addInput("in");
    machine.addInput("ian");
    machine.addOutput("out");
    machine.setInitialOutput();

    // Add transitions
    machine.addTransition(s0, "in [ atoi(valueof(\"in\")) == 1  ]", s1); // state 0 -> state 1 on "in == 1"
    machine.addTransition(s1, "in [ atoi(valueof(\"in\")) == 0  ]", s2); // state 0 -> state 2 on "in == 0"
    machine.addTransition(s2, "in [ atoi(valueof(\"in\")) == 1  ]", s1); // state 1 -> state 0 on "in == 1"
    machine.addTransition(s2, "@ timeout", s0); // state 1 -> state 2 on "timeout"

    machine.checkReachability();
    machine.checkDeadStates();
    machine.checkRedundancy();

    //machine.createJSONFile("automat");

    machine.processStartState();

    machine.printMachine();


    machine.processInput("in", "1");

    machine.printMachine();

    //machine.loadFromJSONFile("automat.json");
    //machine.printMachine();

    return 0;
} */