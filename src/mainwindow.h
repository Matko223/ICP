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
#include <QProcess>
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

/**
 * @class MainWindow
 * @brief Main application window for the Moore machine editor and simulator
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for creating a new Moore machine
     * @param name Name of the Moore machine
     * @param description Description of the Moore machine
     * @param parent Parent widget
     */
    explicit MainWindow(const QString &name, const QString &description, QWidget *parent = nullptr);

    /**
     * @brief Initializes the graphics scene for state diagram
     */
    void initScene();

    /**
     * @brief Logs a message to the application log
     * @param str Message to log
     */
    void logText(QString str);

    /**
     * @brief Adds a new variable to the Moore machine
     */
    void addNewVariable();

    /**
     * @brief Builds states from loaded JSON data
     * @param states List of state definitions
     */
    void buildStatesFromLoaded(const QList<JsonState> &states);

    /**
     * @brief Builds transitions from loaded JSON data
     * @param transitions List of transition definitions
     */
    void buildTransitionsFromLoaded(const QList<JsonTransition> &transitions);

    /**
     * @brief Initializes the simulation control panel
     */
    void initializeControlWidget();

    /**
     * @brief Starts the Moore machine simulation
     */
    void startSimulation();

    /**
     * @brief Pauses the active simulation
     */
    void pauseSimulation();

    /**
     * @brief Resets the simulation to initial state
     */
    void resetSimulation();

    /**
     * @brief Closes the application window
     */
    void cancelWindow();

    /**
     * @brief Clears the graphics scene
     */
    void deleteScene();

    /**
     * @brief Initializes the code generation panel
     */
    void initializeGenerateWidget();

    /**
     * @brief Initializes menu bar
     */
    void initializeMenu();

    /**
     * @brief quit application
     */
    void quitApp();

    /**
     * @brief print help dialog
     */
    void menuHelp();

    /**
     * @brief Creates a new state at the specified position
     * @param position Position for the new state
     * @return Pointer to the created StateItem
     */
    StateItem *createState(QPointF position);

    /**
     * @brief Sets the label for a state
     * @param stateName Name of the state
     * @param state StateItem to label
     */
    void setStateLabel(QString stateName, StateItem *state);

    /**
     * @brief Sets the label for a transition
     * @param name Transition name
     * @param from Source state
     * @param to Destination state
     * @param pathItem Path representing the transition
     * @param path Painter path for the transition
     */
    void setTransitionLabel(const QString &name, StateItem *from, StateItem *to, QGraphicsPathItem *pathItem, const QPainterPath &path);

    /**
     * @brief Handles drop events for drag-and-drop operations
     * @param event Drop event to handle
     */
    void handleDropEvent(QDropEvent *event);

    /**
     * @brief Filters events for the application
     * @param obj Object that received the event
     * @param event Event that occurred
     * @return true if event was handled, false otherwise
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

    /**
     * @brief Initializes drag operation
     */
    void initDrag();

    /**
     * @brief Highlights a state to indicate it's active
     * @param state State to highlight
     */
    void highlightState(StateItem *state);

    /**
     * @brief Removes highlighting from a state
     * @param state State to clear highlight from
     */
    void clearHighlight(StateItem *state);

    /**
     * @brief Loads Moore machine from a file
     * @param filename Path to the file
     */
    void loadAutomatonFromMooreMachine(const QString &filename);

    /**
     * @brief Opens file dialog for loading automaton
     */
    void openFileHandler();

    /**
     * @brief Updates the current state in the simulation
     * @param currentStateIndex Index of the new current state
     */
    void updateState(int currentStateIndex);

    /**
     * @brief Generates JSON file from the current machine
     */
    void generateJson();

    /**
     * @brief Generates C++ code from the current machine
     */
    void generateCode();

    // void compileAndRun(const QString &fileName);

    /**
     * @brief Destructor for MainWindow
     */
    ~MainWindow();

private slots:
    /**
     * @brief Updates transitions when states are moved
     */
    void updateTransitions();

    /**
     * @brief Handles user input during simulation
     */
    void handleInput();

private:
    Ui::MainWindow *ui;                         // UI components
    QGraphicsScene *scene;                      // Scene for state diagram
    QMap<QString, Transition> transitionItems;  // Map of transitions
    QMap<QString, StateItem *> stateItems;      // Map of states
    int logCounter = 1;                         // Counter for log messages
    StateItem *currentState = nullptr;          // Currently active state
    QString lastInput;                          // Last input string
    MooreMachine machine;                       // Moore machine model
    bool simulationStart = false;               // Flag for simulation state
    QMap<QString, int> stateIndexMap;           // Maps state names to indixes
};

#endif // MAINWINDOW_H
