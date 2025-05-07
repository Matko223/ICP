/**
 * @file stateManager.cpp
 * @brief Implementation of the StateManager class, where the state is created
 * @author Róbert Páleš (xpalesr00), Martin Valapka (xvalapm00)
 */

#include "stateManager.h"
#include <QGraphicsTextItem>
#include <QBrush>
#include <QPen>

StateItem* StateManager::createState(QGraphicsScene* scene, const QPointF& position) {
    StateItem* state = new StateItem(0, 0, 50, 50);
    state->setPos(position);
    state->setPen(QPen(Qt::black));
    state->setBrush(Qt::transparent);
    scene->addItem(state);
    return state;
}

void StateManager::highlightState(StateItem* state) {
    if (state) {
        state->setBrush(QBrush(Qt::yellow));
        state->update();
    }
}

void StateManager::clearHighlight(StateItem* state) {
    if (state) {
        state->setBrush(Qt::transparent);
        state->update();
    }
}

void StateManager::loadStates(QGraphicsScene* scene, QMap<QString, StateItem*>& stateItems, StateItem*& currentState, const QList<JsonState>& states) {
    for (const JsonState& state : states) {
        QPointF position(100 + stateItems.size() * 100, 100); // Random distance between states
        StateItem* stateItem = createState(scene, position);
        setStateLabel(state.name, stateItem);
        stateItems[state.name] = stateItem;

        if (state.isInitial && !currentState) {
            currentState = stateItem;
            highlightState(currentState);
        }
    }
}

void StateManager::setStateLabel(const QString& stateName, StateItem* state) {
    QGraphicsTextItem* label = new QGraphicsTextItem(stateName, state);
    label->setDefaultTextColor(Qt::black);
    QRectF rect = state->rect();
    QRectF labelRect = label->boundingRect();
    label->setPos(rect.width() / 2 - labelRect.width() / 2, rect.height() / 2 - labelRect.height() / 2);
}

void StateManager::updateState(QMap<QString, StateItem*>& stateItems, StateItem*& currentState, const QString& stateName, std::function<void(const QString&)> logFunction) {
    if (stateItems.contains(stateName)) {
        clearHighlight(currentState);

        currentState = stateItems[stateName];

        highlightState(currentState);

        logFunction("Moved to state: " + stateName);
    } else {
        logFunction("Error: State " + stateName + " not found.");
    }
}
