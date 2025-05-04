#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDateTime>
#include <QDockWidget>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QtMath>
#include "startWindow.h"
#include "stateitem.h"
#include "fileParser.h"

#define PI 3.14159

struct Transition
{
    StateItem *from_state; // Changed to StateItem
    StateItem *to_state;   // Changed to StateItem
    QGraphicsPathItem *path;
    QGraphicsPolygonItem *arrow;
    QGraphicsTextItem *label;
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &name, const QString &description, QWidget *parent = nullptr);
    explicit MainWindow(const JsonAutomaton &automaton, QWidget *parent = nullptr);
    void initScene();
    void logText(QString str);

    void buildStatesFromLoaded(const QList<JsonState> &states);
    void buildTransitionsFromLoaded(const QList<JsonTransition> &transitions);

    void initializeControlWidget();
    void startSimulation();
    void pauseSimulation();
    void cancelSimulation();
    void resetSimulation();

    StateItem *createState(QPointF position); // Changed to StateItem

    bool createTransitionDialog(QString &transitionName, QString &fromState, QString &toState);
    QPainterPath createTransitionPath(StateItem *from, StateItem *to, QPointF &arrowPos, double &angle); // Changed to StateItem
    void drawArrow(const QPointF &arrowPos, double angle);
    QGraphicsPolygonItem *createArrow(const QPointF &arrowPos, double angle);
    void setTransitionLabel(const QString &name, StateItem *from, StateItem *to, QGraphicsPathItem *pathItem, const QPainterPath &path); // Changed to StateItem

    void handleDropEvent(QDropEvent *event);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void initDrag();
    void setStateLabel(QString stateName, StateItem *state);
    void highlightState(StateItem *state); // Changed to StateItem
    void clearHighlight(StateItem *state); // Changed to StateItem

    ~MainWindow();

private slots:
    void updateTransitions();
    void handleInput();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QMap<QString, Transition> transitionItems;
    QMap<QString, StateItem *> stateItems; // Changed to StateItem
    int logCounter = 1;
    StateItem *currentState = nullptr; // Changed to StateItem
    QString lastInput;
};

#endif // MAINWINDOW_H
