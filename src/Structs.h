/**
 * @file structs.h
 * @brief Helper structs
 * @author Tomáš Šedo (xsedot00)
*/

#ifndef STRUCTS_H
#define STRUCTS_H
#pragma once

#include <string>

/**
 * @struct Variable
 * @brief Represents a variable in the Moore machine
 */
struct Variable {
    std::string type; // C type(int, bool, string etc.)
    std::string name; // Name of the variable
    std::string value; // Value of the variable
};

/**
 * @struct TransitionExpression
 * @brief Defines conditions for state transitions
 */
struct TransitionExpression {
    std::string inputEvent;
    std::string boolExpr;
    std::string delay;

    /**
     * @brief Equality comparison operator
     * @param other TransitionExpression to compare with
     * @return true if expressions are identical, false otherwise
     */
    bool operator==(const TransitionExpression& other) const {
        return inputEvent == other.inputEvent &&
               boolExpr == other.boolExpr &&
               delay == other.delay;
    }
};

namespace std {
    /**
     * @brief Hash function specialization for TransitionExpression
     */
    template <>
    struct hash<TransitionExpression> {
        /**
         * @brief Hash function for TransitionExpression
         * @param expr TransitionExpression to hash
         * @return Hash value
         */
        size_t operator()(const TransitionExpression& expr) const {
            return hash<string>()(expr.inputEvent) ^
                   (hash<string>()(expr.boolExpr) << 1) ^
                   (hash<string>()(expr.delay) << 2);
        }
    };
}

/**
 * @struct State
 * @brief Represents a state in the Moore machine
 */
struct State {
    std::string name; // Name of the state
    std::string outputExpr; // OutputExpr

    /**
     * @brief Mapping from transition expressions to next state indices
     */
    std::unordered_map<TransitionExpression, int> transitions; // transition: {expression, next state}
};

#endif // STRUCTS_H
