/**
 * @file transitionManager.h
 * @brief Header file for the TransitionManager class
 * @author Róbert Páleš (xpalesr00), Martin Valapka (xvalapm00)
 */

 #ifndef TRANSITIONMANAGER_H
 #define TRANSITIONMANAGER_H
 
 #include <QGraphicsScene>
 #include <QGraphicsPathItem>
 #include <QGraphicsPolygonItem>
 #include <QGraphicsTextItem>
 #include <QMap>
 #include <QString>
 #include <QPen>
 #include <QBrush>
 #include <QtMath>
 #include <functional>
 #include "stateitem.h"
 #include "fileParser.h"
 
 #define PI 3.14159
 
 struct Transition {
     StateItem *from_state;
     StateItem *to_state;
     QGraphicsPathItem *path;
     QGraphicsPolygonItem *arrow;
     QGraphicsTextItem *label;
     QString name;
 };
 
 class TransitionManager {
 public:
     /**
      * @brief Create a path between two states
      * @param fromState The source state
      * @param toState The destination state
      * @param arrowPos Output parameter for the arrow position
      * @param angle Output parameter for the arrow angle
      * @return The created painter path
      */
     static QPainterPath createTransitionPath(StateItem *fromState, StateItem *toState, QPointF &arrowPos, double &angle);
     
     /**
      * @brief Create an arrow for transitions
      * @param scene The graphics scene to add the arrow to
      * @param arrowPos The position of the arrow
      * @param angle The angle of the arrow
      * @return The created polygon item
      */
     static QGraphicsPolygonItem *createArrow(QGraphicsScene *scene, const QPointF &arrowPos, double angle);
     
     /**
      * @brief Set a label for a transition
      * @param scene The graphics scene
      * @param transitionName The name of the transition
      * @param from The source state
      * @param to The destination state
      * @param transition The path item for the transition
      * @param path The painter path
      * @param transitionItems Map to store the created transition
      */
     static void setTransitionLabel(QGraphicsScene *scene, const QString &transitionName, StateItem *from, StateItem *to, QGraphicsPathItem *transition, const QPainterPath &path,QMap<QString, Transition> &transitionItems);
     
     /**
      * @brief Update all transitions when states move
      * @param scene The graphics scene
      * @param transitionItems Map of transitions to update
      */
     static void updateTransitions(QGraphicsScene *scene, QMap<QString, Transition> &transitionItems);
     
     /**
      * @brief Build transitions from loaded JSON file
      * @param scene The graphics scene
      * @param stateItems Map of state names to StateItem objects
      * @param transitionItems Map to store the created transitions
      * @param transitions List of transitions from the JsonAutomaton
      * @param logFunction Function to log messages
      */
     static void buildTransitionsFromLoaded(QGraphicsScene *scene, QMap<QString, StateItem*> &stateItems, QMap<QString, Transition> &transitionItems, const QList<JsonTransition> &transitions, std::function<void(const QString&)> logFunction);
 };
 
 #endif // TRANSITIONMANAGER_H
