/**
 * @file stateManager.h
 * @brief Header file for the StateManager class
 * @author Martin Valapka (xvalapm00), Róbert Páleš (xpalesr00)
 */

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <QGraphicsScene>
#include <QMap>
#include <QString>
#include "stateitem.h"
#include "fileParser.h"

class StateManager {
public:
    /**
     * @brief Create a new state and add it to the scene.
     * @param scene The graphics scene to add the state to.
     * @param position The position of the state in the scene.
     * @return Pointer to the created StateItem.
     */
    static StateItem* createState(QGraphicsScene* scene, const QPointF& position);

    /**
     * @brief Highlight a state.
     * @param state The state to highlight.
     */
    static void highlightState(StateItem* state);

    /**
     * @brief Clear highlight from a state.
     * @param state The state to clear the highlight from.
     */
    static void clearHighlight(StateItem* state);

    /**
     * @brief Load states from a JsonAutomaton and add them to the scene.
     * @param scene The graphics scene to add the states to.
     * @param stateItems Map to store the created states.
     * @param currentState Pointer to the current state (updated if an initial state is found).
     * @param states List of states from the JsonAutomaton.
     */
    static void loadStates(QGraphicsScene* scene, QMap<QString, StateItem*>& stateItems, StateItem*& currentState, const QList<JsonState>& states);

    /**
     * @brief Set a label for a state.
     * @param stateName The name of the state.
     * @param state The state to set the label for.
     */
    static void setStateLabel(const QString& stateName, StateItem* state);

    /**
     * @brief Update the current state during simulation.
     * @param stateItems Map of state names to StateItem objects.
     * @param currentState Pointer to the current state (updated by this function).
     * @param stateName Name of the new state to transition to.
     * @param logFunction Function to log messages (e.g., from MainWindow).
     */
    static void updateState(QMap<QString, StateItem*>& stateItems, StateItem*& currentState, const QString& stateName, std::function<void(const QString&)> logFunction);
};

#endif // STATEMANAGER_H
