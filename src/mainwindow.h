/**
 * @file mainwindow.h
 * @brief Header file for the MainWindow class
 * @author Róbert Páleš (xpalesr00)
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDateTime>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QMessageBox>
#include <QMimeData>
#include <QTextStream>
#include "startWindow.h"
#include "startWindow.h"
#include "stateitem.h"
#include "MooreMachine.h"
#include "fileParser.h"
#include "stateManager.h"
#include "transitionManager.h"
#include "dialogsManager.h"
#include "generateCode.h"

#define PI 3.14159

using namespace std;

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

    void initScene();
    void logText(QString str);
    void addNewVariable();

    void buildStatesFromLoaded(const QList<JsonState> &states);
    void buildTransitionsFromLoaded(const QList<JsonTransition> &transitions);

    void initializeControlWidget();
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    void cancelWindow();
    void deleteScene();
    void initializeGenerateWidget();

    StateItem *createState(QPointF position);

    void setStateLabel(QString stateName, StateItem *state);

    void setTransitionLabel(const QString &name, StateItem *from, StateItem *to, QGraphicsPathItem *pathItem, const QPainterPath &path);

    void handleDropEvent(QDropEvent *event);

    bool eventFilter(QObject *obj, QEvent *event) override;

    void initDrag();

    void highlightState(StateItem *state);

    void clearHighlight(StateItem *state);

    void loadAutomatonFromMooreMachine(const QString &filename);

    void openFileHandler();

    void updateState(int currentStateIndex);

    void generateJson();

    void generateCode();

    ~MainWindow();

private slots:
    void updateTransitions();
    void handleInput();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QMap<QString, Transition> transitionItems;
    QMap<QString, StateItem *> stateItems;
    int logCounter = 1;
    StateItem *currentState = nullptr;
    QString lastInput;
    MooreMachine machine;
    bool simulationStart = false;
    QMap<QString, int>stateIndexMap;
};

#endif // MAINWINDOW_H
