#include "generateCode.h"

bool CodeGenerator::generateCode(const nlohmann::ordered_json json, const string &fileName)
{
    ofstream code(fileName);
    if (!code.is_open())
    {
        return false;
    }

    string name = json["name"];
    string description = json["description"];

    // Load state names from JSON
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

    vector<string> outputs;
    for (const auto &output : json["outputs"]) {
        outputs.push_back(output);
    }

    vector<string> variables;
    for (const auto &var : json["variables"]) {
        variables.push_back(var["type"]);
        variables.push_back(var["name"]);
        variables.push_back(var["value"]);
    }

    code << "#include <iostream>\n#include <string>\n\n";
    code << "using namespace std;\n\n";

    code << "enum States {\n";
    for (size_t i = 0; i < stateNames.size(); ++i)
    {
        code << "   " << stateNames[i];
        if (i < stateNames.size() - 1)
        {
            code << ",";
        }
        code << "\n";
    }
    code << "};\n\n";

    code << "States currentState = " << stateNames[0] << ";\n\n";

    code << "void processInput(const string &input, const string &value) {\n";
        for (const auto &transitionBlock : json["transitions"]) {
            string state = transitionBlock["name"];
            code << "    if (currentState == " << state << ") {\n";

            for (const auto &transition : transitionBlock["transitions"]) {
                string inputEvent = transition["expression"]["inputEvent"];
                string boolExpr = transition["expression"]["boolExpr"];
                string delay = transition["expression"]["delay"];
                string nextState = transition["nextState"];

                if (!inputEvent.empty()) {
                    code << "        if (input == \"" << inputEvent << "\") {\n";
                    if (!boolExpr.empty()) {
                        code << "            if (" << boolExpr << ") {\n";
                        code << "                currentState = " << nextState << ";\n";
                        code << "                processOutput();\n";
                        code << "            }\n";
                    } else {
                        code << "            currentState = " << nextState << ";\n";
                        code << "            processOutput();\n";
                    }
                    code << "        }\n";
                } else if (!delay.empty()) {
                    code << "        // TODO: Handle delay" << "\n";
                }
            }

            code << "    }\n";
        }
        code << "}\n\n";

    code << "void processOutput() {\n";
    code << "   switch(currentState) {\n";
                for (const auto &state : stateNames) {
    code << "      case " << state << ":\n";
    code << "         cout << \"in state: " << state << "\" << endl;\n";
    code << "         break;\n";
                }
    code << "   }\n";
    code << "}\n";

    code << "int main() {\n";
    code << "   //TODO\n";
    code << "}\n";

    code.close();
    return true;
}
