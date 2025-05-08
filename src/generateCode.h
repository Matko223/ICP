#ifndef GENERATECODE_H
#define GENERATECODE_H

using namespace std;

#include "json.hpp"
#include <string>
#include "MooreMachine.h"
#include <iostream>
#include <fstream>


class CodeGenerator
{
    public:
        static bool generateCode(const nlohmann::ordered_json json, const string &fileName);

    private:
        static MooreMachine machine;
};

#endif // GENERATECODE_H
