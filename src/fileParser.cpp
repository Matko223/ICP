/**
 * @file fileParser.cpp
 * @brief Implementation of the FileParser class
 * @author Martin Valapka (xvalapm00)
 */

#include "fileParser.h"
#include <QString>
#include <QDebug>

JsonAutomaton FileParser::loadAutomatonFromMooreMachine(const QString& filename, MooreMachine& machine) {
    machine.loadFromJSONFile(filename.toStdString());

    // Convert to JsonAutomaton structure
    JsonAutomaton automaton;
    automaton.name = QString::fromStdString(machine.getMachineName());
    automaton.description = QString::fromStdString(machine.getMachineDescription());

    // Convert states
    QList<JsonState> states;
    const auto& machineStates = machine.getStates();
    for (size_t i = 0; i < machineStates.size(); i++) {
        const auto& state = machineStates[i];
        JsonState jsonState;
        jsonState.name = QString::fromStdString(state.name);
        jsonState.isInitial = (i == static_cast<size_t>(machine.getStartState()));
        states.append(jsonState);
    }
    automaton.stateList = states;

    // Convert transitions
    QList<JsonTransition> transitions;
    for (size_t i = 0; i < machineStates.size(); i++) {
        const auto& state = machineStates[i];
        for (const auto& [transExpr, nextStateIdx] : state.transitions) {
            JsonTransition jsonTransition;
            jsonTransition.fromName = QString::fromStdString(state.name);
            jsonTransition.toName = QString::fromStdString(machineStates[nextStateIdx].name);

            // Create name for the transition
            QString transName = QString::fromStdString(transExpr.inputEvent);
            if (!transExpr.boolExpr.empty()) {
                transName += "[" + QString::fromStdString(transExpr.boolExpr) + "]";
            }
            jsonTransition.name = transName;

            transitions.append(jsonTransition);
        }
    }
    automaton.transitionList = transitions;

    return automaton;
}
