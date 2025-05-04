#include <fileParser.h>

void FileParser::parseInputs(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonArray input = root.value("Inputs").toArray();
    for (const QJsonValue &value : input)
    {
        automaton.inputs.append(value.toString());
    }
}

void FileParser::parseOutputs(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonArray output = root.value("Outputs").toArray();
    for (const QJsonValue &value : output)
    {
        automaton.outputs.append(value.toString());
    }
}

void FileParser::parseVariables(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonObject variable = root.value("Variables").toObject();
    for (const QString &name : variable.keys())
    {
        QJsonObject content = variable[name].toObject();

        JsonVariable var;
        var.name = name;
        var.type = content["type"].toString();
        var.value = content["value"].toVariant();

        automaton.variableList.append(var);
    }
}

void FileParser::parseStates(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonObject states = root.value("States").toObject();
    for (const QString &name : states.keys())
    {
        QJsonObject content = states[name].toObject();

        JsonState state;
        state.name = name;
        state.action  = content["action"].toString();
        state.isInitial = content["initial"].toBool(false);

        automaton.stateList.append(state);
    }
}

void FileParser::parseTransitions(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonObject transitions = root.value("Transitions").toObject();
    for (const QString &name : transitions.keys())
    {
        QJsonObject content = transitions[name].toObject();

        JsonTransition transition;
        transition.name = name;
        transition.fromName = content["from"].toString();
        transition.toName = content["to"].toString();
        transition.event = content["event"].toString();
        transition.condition = content["condition"].toString();
        transition.delay = content["delay"].toString();

        automaton.transitionList.append(transition);
    }
}

// parse the loaded json document
JsonAutomaton FileParser::parse(const QJsonObject &root)
{
    JsonAutomaton automaton;
    automaton.name = root.value("Name").toString();
    automaton.description = root.value("Description").toString();

    parseInputs(root, automaton);
    parseOutputs(root, automaton);
    parseVariables(root, automaton);
    parseStates(root, automaton);
    parseTransitions(root, automaton);

    return automaton;
}
