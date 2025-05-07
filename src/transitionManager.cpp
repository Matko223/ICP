/**
 * @file transitionManager.cpp
 * @brief Implementation of the TransitionManager class, where transition is created
 * @author Róbert Páleš (xpalesr00), Martin Valapka (xvalapm00)
 */

#include "transitionManager.h"

QPainterPath TransitionManager::createTransitionPath(StateItem *fromState, StateItem *toState, QPointF &arrowPos, double &angle)
{
    QPointF point1 = fromState->sceneBoundingRect().center();
    QPointF point2 = toState->sceneBoundingRect().center();

    QLineF line(point1, point2);
    qreal radius = fromState->rect().width() / 2;

    if (line.length() > 0) {
        line.setLength(line.length() - radius);
        point2 = line.p2();
        line.setLength(radius);
        point1 = line.p2();
    }

    QPainterPath path(point1);

    if (fromState == toState) {
        qreal offset = 40.0;
        qreal distance = 20.0;
        QPointF top = point1 + QPointF(0, -radius);
        QPointF startPoint = top + QPointF(-distance / 2, 0);
        QPointF endPoint = top + QPointF(distance / 2, 0);
        QPointF control1 = startPoint + QPointF(-offset, -offset * 1.5);
        QPointF control2 = endPoint + QPointF(offset, -offset * 1.5);

        path.moveTo(startPoint);
        path.cubicTo(control1, control2, endPoint);
        arrowPos = endPoint;
    } else {
        QPointF control1(point1.x(), (point1.y() + point2.y()) / 2);
        path.quadTo(control1, point2);
        arrowPos = point2;
    }

    angle = qDegreesToRadians(-path.angleAtPercent(1.0));
    return path;
}

QGraphicsPolygonItem *TransitionManager::createArrow(QGraphicsScene *scene, const QPointF &arrowPos, double angle)
{
    qreal size = 10.0;
    QPointF point1 = arrowPos - QPointF(qCos(angle + PI / 6) * size, qSin(angle + PI / 6) * size);
    QPointF point2 = arrowPos - QPointF(qCos(angle - PI / 6) * size, qSin(angle - PI / 6) * size);
    QPolygonF arrow;
    arrow << arrowPos << point1 << point2;
    return scene->addPolygon(arrow, QPen(Qt::black), QBrush(Qt::black));
}

void TransitionManager::setTransitionLabel(QGraphicsScene *scene, const QString &transitionName, StateItem *from, StateItem *to, QGraphicsPathItem *transition, const QPainterPath &path,QMap<QString, Transition> &transitionItems)
{
    Transition t;
    t.from_state = from;
    t.to_state = to;
    t.path = transition;
    t.label = scene->addText(transitionName);
    t.label->setDefaultTextColor(Qt::black);

    if (from == to) {
        QPointF point1 = from->sceneBoundingRect().center();
        qreal radius = from->rect().width() / 2;
        QPointF top = point1 + QPointF(0, -radius);
        qreal offset = 55;

        t.label->setPos(top.x() - t.label->boundingRect().width() / 2, top.y() - offset - t.label->boundingRect().height() / 2);
    } else {
        QPointF center = path.pointAtPercent(0.5);
        qreal angle = qDegreesToRadians(path.angleAtPercent(0.5));
        QPointF offset(-qSin(angle) * 10.0, -qCos(angle) * 10.0);
        QPointF pos = center + offset;

        t.label->setPos(pos.x() - t.label->boundingRect().width() / 2, pos.y() - t.label->boundingRect().height() / 2);
    }

    QPointF arrowPos;
    double angle;
    QPainterPath arrowPath = createTransitionPath(from, to, arrowPos, angle);
    t.arrow = createArrow(scene, arrowPos, angle);

    // Create unique identifier for this transition
    QString uniqueId = QString("%1_%2_%3").arg(transitionName).arg((quintptr)from).arg((quintptr)to);

    t.name = transitionName;

    // Use the unique ID as the map key
    transitionItems.insert(uniqueId, t);
}

void TransitionManager::updateTransitions(QGraphicsScene *scene, QMap<QString, Transition> &transitionItems)
{
    for (auto it = transitionItems.begin(); it != transitionItems.end(); ++it) {
        Transition &t = it.value();

        QString transitionName = t.name;

        scene->removeItem(t.path);
        scene->removeItem(t.arrow);
        scene->removeItem(t.label);
        delete t.path;
        delete t.arrow;
        delete t.label;

        QPointF arrowPos;
        double angle = 0.0;
        QPainterPath path = createTransitionPath(t.from_state, t.to_state, arrowPos, angle);
        t.path = scene->addPath(path, QPen(Qt::black));
        t.arrow = createArrow(scene, arrowPos, angle);
        t.label = scene->addText(transitionName);
        t.label->setDefaultTextColor(Qt::black);

        if (t.from_state == t.to_state) {
            QPointF point1 = t.from_state->sceneBoundingRect().center();
            qreal radius = t.from_state->rect().width() / 2;
            QPointF top = point1 + QPointF(0, -radius);
            qreal offset = 55;
            t.label->setPos(top.x() - t.label->boundingRect().width() / 2, top.y() - offset - t.label->boundingRect().height() / 2);
        } else {
            QPointF center = path.pointAtPercent(0.5);
            qreal angle = qDegreesToRadians(path.angleAtPercent(0.5));
            QPointF offset(-qSin(angle) * 10.0, -qCos(angle) * 10.0);
            QPointF pos = center + offset;
            t.label->setPos(pos.x() - t.label->boundingRect().width() / 2, pos.y() - t.label->boundingRect().height() / 2);
        }
    }
}

void TransitionManager::buildTransitionsFromLoaded(QGraphicsScene *scene, QMap<QString, StateItem*> &stateItems, QMap<QString, Transition> &transitionItems, const QList<JsonTransition> &transitions, std::function<void(const QString&)> logFunction)
{
    for (const JsonTransition &transition : transitions)
    {
        if (!stateItems.contains(transition.fromName) || !stateItems.contains(transition.toName))
        {
            logFunction("Error: Unknown state in transition");
            continue;
        }

        StateItem *fromState = stateItems[transition.fromName];
        StateItem *toState = stateItems[transition.toName];

        QPointF arrowPos;
        double angle;
        QPainterPath path = createTransitionPath(fromState, toState, arrowPos, angle);

        QGraphicsPathItem *pathItem = scene->addPath(path, QPen(Qt::black));

        setTransitionLabel(scene, transition.name, fromState, toState, pathItem, path, transitionItems);

        logFunction("Added transition: " + transition.name + " from state: " + transition.fromName + " to state: " + transition.toName);
    }
}
