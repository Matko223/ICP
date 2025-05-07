/**
 * @file structs.h
 * @brief Helper structs
 * @author Tomáš Šedo (xsedot00)
*/

#ifndef STRUCTS_H
#define STRUCTS_H
#pragma once

#include <string>

// Define variable
// {type, name, value}
struct Variable {
    std::string type; // C type(int, bool, string etc.)
    std::string name; // Name of the variable
    std::string value; // Value of the variable
};

// Define a transition expression
// format: {inputEvent, boolExpr, delay}
struct TransitionExpression {
    std::string inputEvent;
    std::string boolExpr;
    std::string delay;

    // Define equality operator for TransitionExpression
    bool operator==(const TransitionExpression& other) const {
        return inputEvent == other.inputEvent &&
               boolExpr == other.boolExpr &&
               delay == other.delay;
    }
};

namespace std {
    // Specialize std::hash for TransitionExpression
    template <>
    struct hash<TransitionExpression> {
        size_t operator()(const TransitionExpression& expr) const {
            return hash<string>()(expr.inputEvent) ^
                   (hash<string>()(expr.boolExpr) << 1) ^
                   (hash<string>()(expr.delay) << 2);
        }
    };
}

// Define a State
// {name, state, {transitions}}
struct State {
    std::string name; // Name of the state
    std::string outputExpr; // OutputExpr
    std::unordered_map<TransitionExpression, int> transitions; // transition: {expression, next state}
};

#endif // STRUCTS_H
