/**
 * @file generateCode.h
 * @author header file for code generator
 * @author Róbert Páleš (xpalesr00)
*/

#ifndef GENERATECODE_H
#define GENERATECODE_H

using namespace std;

#include "json.hpp"
#include <string>
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

        /**
         * @brief process escape quotes while processing output
         * @param str string to be checked
         * @return escaped string
         */
        static string escapeQuotes(const string &str);
};

#endif // GENERATECODE_H
