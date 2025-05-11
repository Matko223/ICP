/**
 * @file generateCode.cpp
 * @author generates c++ code representing created automaton
 * @author Róbert Páleš (xpalesr00)
*/

#include "generateCode.h"

string CodeGenerator::escapeQuotes(const string &str)
{
    string escaped;
    for (char c : str)
    {
        if (c == '"') escaped += "\\\"";
        else escaped += c;
    }
    return escaped;
}

bool CodeGenerator::generateCode(const nlohmann::ordered_json json, const string &fileName)
{
    ofstream code(fileName);
    if (!code.is_open())
    {
        return false;
    }

    string machineName = json["name"];
    string machineDescription = json["description"];

    vector<string> stateNames;
    for (const auto& state : json["states"]) {
        stateNames.push_back(state["name"]);
    }

    map<string, string> stateOutputs;
    for (const auto& state : json["states"]) {
        stateOutputs[state["name"]] = state["outputExpr"];
    }

    vector<string> inputs;
    for (const auto &input : json["inputs"]) {
        inputs.push_back(input);
    }

    vector<string> variables;
    for (const auto &var : json["variables"]) {
        variables.push_back(var["type"]);
        variables.push_back(var["name"]);
        variables.push_back(var["value"]);
    }

    code << "#include <iostream>\n";
    code << "#include <string>\n";
    code << "#include <vector>\n";
    code << "#include <chrono>\n";
    code << "#include <thread>\n";

    code << "using namespace std;\n";
    code << "string machineName = \"" << machineName << "\";\n";
    code << "string machineDescription = \"" << machineDescription << "\";\n\n";

    code << "enum States {\n";
    for (size_t i = 0; i < stateNames.size(); i++)
    {
        code << "   " << stateNames[i];
        if (i < stateNames.size() - 1)
        {
            code << ",";
        }
        code << "\n";
    }
    code << "};\n\n";

    code << "enum Inputs {\n";
    for (size_t i = 0; i < inputs.size(); i++)
    {
        code << "   " << inputs[i];
        if (i < inputs.size() - 1)
        {
            code << ",";
        }
        code << "\n";
    }
    code << "};\n\n";

    code << "struct Variables {\n";
    code << "   string type;\n";
    code << "   string name;\n";
    code << "   string value;\n";
    code << "};\n\n";

    code << "vector<Variables> variables = {\n";
    for (size_t i = 0; i < variables.size(); i += 3)
    {
        string type = variables[i];
        string name = variables[i + 1];
        string value = variables[i + 2];

        code << "   {\"" << type << "\", \"" << name << "\", \"" << value << "\"}";
        if (i + 3 < variables.size())
        {
            code << ",";
        }
        code << "\n";
    }
    code << "};\n\n";

    code << "// first state in json states\n";
    code << "States currentState = " << stateNames[0] << ";\n\n";

    code << "void processTimeoutState() {\n";
    code << "    switch(currentState) {\n";

    for (const auto &transitionBlock : json["transitions"]) {
        string state = transitionBlock["name"];
        for (const auto &transition : transitionBlock["transitions"]) {
            string inputEvent = transition["expression"]["inputEvent"];
            string delayName = transition["expression"]["delay"];
            string nextState = transition["nextState"];

            if (inputEvent.empty() && !delayName.empty()) {
                code << "        case " << state << ": {\n";
                code << "            int delay = 0;\n";
                code << "            for (const auto &var : variables) {\n";
                code << "                if (var.name == \"" << delayName << "\") {\n";
                code << "                    delay = stoi(var.value);\n";
                code << "                    break;\n";
                code << "                }\n";
                code << "            }\n";
                code << "            cout << \"Entering state with DELAY... DELAY started with time \" << delay << \"[ms]\\n\";\n";
                code << "            this_thread::sleep_for(chrono::milliseconds(delay));\n";
                code << "            cout << \"TIMEOUT! Moving to state: " << nextState << "\\n\";\n";
                code << "            currentState = " << nextState << ";\n";
                code << "            break;\n";
                code << "        }\n";
                break;
            }
        }
    }
    code << "        default:\n";
    code << "            break;\n";
    code << "    }\n";
    code << "}\n\n";

    code << "void processOutput() {\n";
    code << "    switch(currentState) {\n";
                for (const auto &state : stateNames)
                {
    code << "        case " << state << ":\n";
    code << "        {\n";
    code << "            cout << \"State " << state << " processed, output is: " << escapeQuotes(stateOutputs[state]) << "\" << endl;\n";
    code << "            break;\n";
    code << "        }\n";
                }
    code << "    }\n";
    code << "}\n\n";

    code << "void processInput(const string &input, const string &value) {\n";
    code << "    switch(currentState) {\n";

    for (const auto &transitionBlock : json["transitions"])
    {
        string state = transitionBlock["name"];
        code << "        case " << state << ":\n";
        code << "        {\n";

        for (const auto &transition : transitionBlock["transitions"])
        {
            string inputEvent = transition["expression"]["inputEvent"];
            string boolExpr = transition["expression"]["boolExpr"];
            string delay = transition["expression"]["delay"];
            string nextState = transition["nextState"];

            if (!inputEvent.empty())
            {
                code << "            if (input == \"" << inputEvent << "\") {\n";
                if (!boolExpr.empty())
                {
                    code << "                // transition expression = \"" << escapeQuotes(boolExpr) << "\"\n";
                    code << "                processOutput();\n";
                    code << "                currentState = " << nextState << ";\n";
                }
                code << "            }\n";
            }
        }
        code << "            break;\n";
        code << "        }\n";
    }
    code << "        default:\n";
    code << "            break;\n";
    code << "    }\n";
    code << "   processTimeoutState();\n";
    code << "}\n\n";

    code << "int main() {\n";
    code << "    cout << \"Running automaton: \" << machineName << \" - \" << machineDescription << endl;\n";
    code << "    return 0;\n";
    code << "}\n";

    code.close();
    return true;
}
