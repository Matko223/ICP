#ifndef STATEITEM_H
#define STATEITEM_H

#include <QGraphicsEllipseItem>

class StateItem : public QGraphicsEllipseItem
{
public:
    explicit StateItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // STATEITEM_H
