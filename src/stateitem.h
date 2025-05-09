/**
 * @file stateItem.h
 * @brief Header file for the StateItem class
 * @author Martin Valapka (xvalapm00)
 */

#ifndef STATEITEM_H
#define STATEITEM_H

#include <QGraphicsEllipseItem>

/**
 * @class StateItem
 * @brief A graphical representation of a state in the Moore machine
 */
class StateItem : public QGraphicsEllipseItem
{
public:
    /**
     * @brief Constructor for StateItem
     * @param x X-coordinate of the item
     * @param y Y-coordinate of the item
     * @param width Width of the item
     * @param height Height of the item
     * @param parent Parent item
     */
    explicit StateItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);
protected:
    /**
     * @brief Handles item changes
     * @param change Type of change
     * @param value New value
     * @return Updated value
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // STATEITEM_H
