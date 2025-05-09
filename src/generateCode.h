/**
 * @file generateCode.h
 * @brief Header file for the CodeGenerator class
 * @author Róbert Páleš (xpalesr00)
*/

#ifndef GENERATECODE_H
#define GENERATECODE_H

using namespace std;

#include "json.hpp"
#include <string>
#include "MooreMachine.h"
#include <iostream>
#include <fstream>

/**
 * @class CodeGenerator
 * @brief Generates C++ code from Moore machine JSON definitions
 */
class CodeGenerator
{
    public:
        /**
         * @brief Generates C++ code from Moore machine JSON
         * @param json Moore machine definition in JSON format
         * @param fileName Output file path for generated code
         * @return true if generation succeeded, false otherwise
         */
        static bool generateCode(const nlohmann::ordered_json json, const string &fileName);

    private:
        /**
         * @brief Moore machine instance used during code generation
         */
        static MooreMachine machine;
};

#endif // GENERATECODE_H
