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

    string name = json["name"];
    string description = json["description"];

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
    code << "#include <variant>\n";
    code << "#include <algorithm>\n";
    code << "#include <cctype>\n\n";

    code << "using ExprValue = std::variant<bool, int, std::string>;";
    code << "using namespace std;\n";

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

    code << "string removeSpaces(const string& str) {\n";
    code << "   string tmp = str;";
    code << "   tmp.erase(remove(tmp.begin(), tmp.end(), ' '), tmp.end());\n";
    code << "   return tmp;\n";
    code << "}\n\n";

    code << "bool isNumber(const string& expr) {\n";
    code << "    for (char ch : expr) {\n";
    code << "    if (!isdigit(ch)) {\n";
    code << "       return false; }\n";
    code << "    }\n";
    code << "    return true;\n";
    code << "}\n\n";

    code << "string valueof(const string& name, const string& inputName, const string& inputValue) {\n";
    code << "    if (name == inputName) {\n";
    code << "       return inputValue;\n";
    code << "    }\n";
    code << "    for (const auto& var : variables) {\n";
    code << "        if (var.name == name) {\n";
    code << "           return var.value;\n";
    code << "        }\n";
    code << "    }\n";
    code << "    return \"\";\n";
    code << "}\n\n";

    code << "ExprValue evaluateExpr(const string& expr, const string& inputName, const string& inputValue) {\n";
    code << "    if (expr.empty()) {\n";
    code << "        return -1;\n";
    code << "    }\n\n";

    code << "    size_t definedPos = expr.find(\"defined(\");\n";
    code << "    if (definedPos != string::npos) {\n";
    code << "        size_t varStart = expr.find('\"', definedPos) + 1;\n";
    code << "        size_t varEnd = expr.find('\"', varStart);\n";
    code << "        string varName = expr.substr(varStart, varEnd - varStart);\n";
    code << "        return inputName == varName;\n";
    code << "    }\n\n";

    code << "    size_t atoiPos = expr.find(\"atoi(\");\n";
    code << "    if (atoiPos != string::npos) {\n";
    code << "        size_t insideExprStart = expr.find('(', atoiPos) + 1;\n";
    code << "        size_t insideExprEnd = expr.find(')', insideExprStart);\n";
    code << "        string insideExpr = expr.substr(insideExprStart, insideExprEnd - insideExprStart);\n";
    code << "        ExprValue inner = evaluateExpr(insideExpr, inputName, inputValue);\n";
    code << "        if (holds_alternative<string>(inner)) {\n";
    code << "            string val = get<string>(inner);\n";
    code << "            if (isNumber(val)) {\n";
    code << "               return stoi(get<string>(inner));\n";
    code << "            }\n";
    code << "        }\n";
    code << "    }\n\n";

    code << "    size_t valueofPos = expr.find(\"valueof(\");\n";
    code << "    if (valueofPos != string::npos) {\n";
    code << "        size_t varStart = expr.find('\"', valueofPos) + 1;\n";
    code << "        size_t varEnd = expr.find('\"', varStart);\n";
    code << "        string varName = expr.substr(varStart, varEnd - varStart);\n";
    code << "        return valueof(varName, inputName, inputValue);\n";
    code << "    }\n\n";

    code << "    if (isNumber(expr)) {\n";
    code << "        return stoi(expr);\n";
    code << "    }\n";
    code << "    return expr;\n";
    code << "}\n\n";

    code << "bool compareExprValues(const ExprValue& left, const ExprValue& right, const string& op) {\n";
    code << "    if (left.index() != right.index()) {\n";
    code << "        return false;\n";
    code << "    }\n\n";

    code << "    if (holds_alternative<int>(left)) {\n";
    code << "        int l = get<int>(left);\n";
    code << "        int r = get<int>(right);\n";
    code << "        if (op == \"==\") {\n";
    code << "            return l == r;\n";
    code << "        }\n";
    code << "        if (op == \"!=\") {\n";
    code << "            return l != r;\n";
    code << "        }\n";
    code << "        if (op == \"<\") {\n";
    code << "            return l < r;\n";
    code << "        }\n";
    code << "        if (op == \"<=\") {\n";
    code << "            return l <= r;\n";
    code << "        }\n";
    code << "        if (op == \">\") {\n";
    code << "            return l > r;\n";
    code << "        }\n";
    code << "        if (op == \">=\") {\n";
    code << "            return l >= r;\n";
    code << "        }\n";
    code << "    }\n\n";

    code << "    if (holds_alternative<string>(left)) {\n";
    code << "        string l = get<string>(left);\n";
    code << "        string r = get<string>(right);\n";
    code << "        if (op == \"==\") {\n";
    code << "            return l == r;\n";
    code << "        }\n";
    code << "        if (op == \"!=\") {\n";
    code << "            return l != r;\n";
    code << "        }\n";
    code << "    }\n";
    code << "    return false;\n";
    code << "}\n\n";

    code << "bool evaluateBoolExpr(string cond, const string& inputName, const string& inputValue) {\n";
    code << "    cond = removeSpaces(cond);\n";
    code << "    vector<string> operators = { \"==\", \"!=\", \"<=\", \">=\", \"<\", \">\" };\n";
    code << "    for (const string& op : operators) {\n";
    code << "        size_t opPos = cond.find(op);\n";
    code << "        if (opPos != string::npos) {\n";
    code << "            string left = cond.substr(0, opPos);\n";
    code << "            string right = cond.substr(opPos + op.length());\n";
    code << "            ExprValue leftVal = evaluateExpr(left, inputName, inputValue);\n";
    code << "            ExprValue rightVal = evaluateExpr(right, inputName, inputValue);\n";
    code << "            return compareExprValues(leftVal, rightVal, op);\n";
    code << "        }\n";
    code << "    }\n";
    code << "    cerr << \"Unsupported operator in condition: \" << cond << endl;\n";
    code << "    return false;\n";
    code << "}\n";



    code << "States currentState = " << stateNames[0] << ";\n\n";

    code << "void processOutput() {\n";
    code << "    switch(currentState) {\n";
                for (const auto &state : stateNames)
                {
    code << "        case " << state << ":\n";
    code << "        {\n";
    code << "            cout << \"Output expression for state " << state << " is: " << escapeQuotes(stateOutputs[state]) << "\" << endl;\n";
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
                    code << "                string transitionExpr = \"" << escapeQuotes(boolExpr) << "\";\n";
                    code << "                if (evaluateBoolExpr(transitionExpr, input, value)) {\n";
                    code << "                    processOutput();\n";
                    code << "                    currentState = " << nextState << ";\n";
                    code << "                }\n";
                }
                code << "            }\n";
            }
            else if (!delay.empty())
            {
                code << "            int delay = 0;\n";
                code << "            for (const auto &var : variables) {\n";
                code << "                if (var.name == \"" << delay << "\") {\n";
                code << "                    delay = stoi(var.value);\n";
                code << "                    break;\n";
                code << "                }\n";
                code << "            }\n\n";
                code << "            cout << \"DELAY started...\\n\";\n";
                code << "            this_thread::sleep_for(chrono::milliseconds(delay));\n";
                code << "            cout << \"TIMEOUT! Moving to state: " << nextState << "\\n\";\n";
                code << "            currentState = " << nextState << ";\n";
                code << "            processOutput();\n\n";
            }
        }
        code << "            break;\n";
        code << "        }\n";
    }
    code << "        default:\n";
    code << "            break;\n";
    code << "    }\n";
    code << "}\n\n";

    code << "int main() {\n";
    code << "    string inputEvent, value;\n";
    code << "    while(true) {\n";
    code << "        cout << \"Enter input event and value: \";\n";
    code << "        cin >> inputEvent >> value;\n";
    code << "        processInput(inputEvent, value);\n";
    code << "    }\n";
    code << "    return 0;\n";
    code << "}\n";

    code.close();
    return true;
}
