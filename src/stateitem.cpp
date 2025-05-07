/**
 * @file stateItem.cpp
 * @brief Implementation of the StateItem class
 * @author Martin Valapka (xvalapm00)
 */

#include "stateitem.h"
#include <QGraphicsScene>
#include <QDebug>

StateItem::StateItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent)
    : QGraphicsEllipseItem(x, y, width, height, parent)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QVariant StateItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        QRectF sceneRect = scene()->sceneRect();

        const qreal margin = 5.0;

        QRectF effectiveRect = sceneRect.adjusted(margin, margin, -margin, -margin);

        QRectF itemRect = boundingRect();
        qreal penWidth = pen().widthF();
        itemRect.adjust(-penWidth / 2, -penWidth / 2, penWidth / 2, penWidth / 2);

        qreal newX = newPos.x();
        qreal newY = newPos.y();

        if (newPos.x() < effectiveRect.left()) {
            newX = effectiveRect.left();
        }
        if (newPos.x() + itemRect.width() > effectiveRect.right()) {
            newX = effectiveRect.right() - itemRect.width();
        }
        if (newPos.y() < effectiveRect.top()) {
            newY = effectiveRect.top();
        }
        if (newPos.y() + itemRect.height() > effectiveRect.bottom()) {
            newY = effectiveRect.bottom() - itemRect.height();
        }
        return QPointF(newX, newY);
    }

    return QGraphicsEllipseItem::itemChange(change, value);
}
