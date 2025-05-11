/**
 * @file mainwindow.cpp
 * @brief Implementation of MainWindow class, which is the hearth of GUI
 * @author Róbert Páleš (xpalesr00), Martin Valapka (xvalapm00)
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

// initialize scene
void MainWindow::initScene()
{
    scene = new QGraphicsScene(this);

    scene->setSceneRect(0, 0, 1000, 1000);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setSceneRect(0, 0, 1000, 1000);
    ui->graphicsView->resetTransform();
    ui->graphicsView->setMinimumSize(500, 500);
    ui->graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Enable scrollbars
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(scene, &QGraphicsScene::changed, this, &MainWindow::updateTransitions);
}

// constructor for creating automaton
MainWindow::MainWindow(const QString &name, const QString &description, QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAcceptDrops(true);
    ui->addItemWidget->setDragEnabled(true);

    ui->graphicsView->viewport()->setAcceptDrops(true);
    ui->graphicsView->viewport()->installEventFilter(this);

    ui->addItemWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->addItemWidget->setDragDropMode(QAbstractItemView::DragOnly);

    // Initialize drag
    connect(ui->addItemWidget, &QListWidget::itemPressed, this, [this](QListWidgetItem *) {
        initDrag();
    });

    initScene();

    connect(scene, &QGraphicsScene::changed, this, &MainWindow::updateTransitions);
    ui->nameDesc->setText(name + " - " + description);

    initializeControlWidget();
    initializeGenerateWidget();
    initializeMenu();

    // Connect add new variable button
    connect(ui->addVar, &QPushButton::clicked, this , &MainWindow::addNewVariable);

    // Input handling
    connect(ui->inValue, &QLineEdit::returnPressed, this, &MainWindow::handleInput);
}

// Build states from loaded file
void MainWindow::buildStatesFromLoaded(const QList<JsonState> &states) {
    StateManager::loadStates(scene, stateItems, currentState, states);
}

// Build transition from loaded file
void MainWindow::buildTransitionsFromLoaded(const QList<JsonTransition> &transitions)
{
    TransitionManager::buildTransitionsFromLoaded(scene, stateItems, transitionItems, transitions,[this](const QString& message) { 
        logText(message); 
    });
}

// New slot to handle input
void MainWindow::handleInput() {
    if (!simulationStart)
    {
        QMessageBox::warning(this, "Warning", "Simulation has not started yet!\nStart the simulation first");
        return;
    }

    QString input = ui->inDropdown->currentText().trimmed();
    QString value = ui->inValue->text().trimmed();

    if (input.isEmpty() || value.isEmpty()) {
        QMessageBox::warning(this, "Error", "Input or value is empty");
        return;
    }

    machine.processInput(input.toStdString(), value.toStdString());

    int index = machine.getCurrentState();
    updateState(index);

    // Update state highlight when delay is triggered
    machine.autoTransition = [this](int index)
    {
        QMetaObject::invokeMethod(this, "handleStateUpdate", Qt::QueuedConnection, Q_ARG(int, index));
    };

    ui->inValue->clear();
    ui->inLast->appendPlainText(input + " = " + value);
    ui->outValue->appendPlainText(QString::fromStdString(machine.getCurrentOutput()));
}

// Update highlight to the next state
void MainWindow::updateState(int currentStateIndex)
{
    const auto& states = machine.getStates();
    if (currentStateIndex >= 0 && currentStateIndex < static_cast<int>(states.size())) {
        QString stateName = QString::fromStdString(states[currentStateIndex].name);

        StateManager::updateState(stateItems, currentState, stateName, [this](const QString& message) {
            logText(message);
        });
    }
}

// Each action is logged with number, date and description
void MainWindow::logText(QString str)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->logText->appendPlainText(QString::number(logCounter) + ". [" + timestamp + "] " + str);
    logCounter++;
}

// Connect control widget buttons
void MainWindow::initializeControlWidget()
{
    connect(ui->startSimulation, &QPushButton::clicked, this, &MainWindow::startSimulation);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::deleteScene);
    connect(ui->resetSimulation, &QPushButton::clicked, this, &MainWindow::resetSimulation);
}

void MainWindow::initializeGenerateWidget()
{
    connect(ui->generateJson, &QPushButton::clicked, this, &MainWindow::generateJson);
    connect(ui->generateCode, &QPushButton::clicked, this, &MainWindow::generateCode);
}

void MainWindow::initializeMenu()
{
    connect(ui->menuHelp_2, &QAction::triggered, this, &MainWindow::menuHelp);
    connect(ui->menuNew, &QAction::triggered, this, &MainWindow::cancelWindow);
    connect(ui->menuSave, &QAction::triggered, this, &MainWindow::generateJson);
    connect(ui->menuGenerate, &QAction::triggered, this, &MainWindow::generateCode);
    connect(ui->menuQuit, &QAction::triggered, this, &MainWindow::quitApp);
}

void MainWindow::quitApp()
{
    if (DialogManager::confirmDialog(this, "Quit application", "Are you sure you want to quit application?"))
    {
        QApplication::quit();
    }
}

void MainWindow::menuHelp()
{
    DialogManager::showHelpDialog(this);
}

// Start simulation button handling
void MainWindow::startSimulation()
{
    if (stateItems.isEmpty()) {
        QMessageBox::warning(this, "Error", "No states available to start simulation");
        return;
    }

    if (!currentState) {
        currentState = stateItems.first();
        highlightState(currentState);
        logText("Simulation started with initial state: " + stateItems.key(currentState));
    } else {
        highlightState(currentState);
        logText("Simulation resumed with current state: " + stateItems.key(currentState));
    }

    simulationStart = true;
    machine.processStartState();
    highlightState(currentState);
    ui->outValue->appendPlainText(QString::fromStdString(machine.getCurrentOutput()));
}

void MainWindow::resetSimulation()
{
    if (!simulationStart)
    {
        QMessageBox::warning(this, "Error", "Simulation hasn´t started yet");
        return;
    }

    if (currentState)
    {
        clearHighlight(currentState);
        currentState = nullptr;
    }

    machine.interruptDelay();
    ui->outValue->clear();
    ui->inLast->clear();
    simulationStart = false;
    machine.setInitialOutput();
    machine.processStartState();

    const auto &states = machine.getStates();
    int startIndex = machine.getStartState();
    if (startIndex >= 0 && startIndex < static_cast<int>(states.size())) {
        QString stateName = QString::fromStdString(states[startIndex].name);
        if (stateItems.contains(stateName)) {
            currentState = stateItems[stateName];
            highlightState(currentState);
        }
    }

    logText("Simulation reset");
}

// reset simulation
// clear scene and all text fields
// clear machine
void MainWindow::deleteScene()
{
    if (!DialogManager::confirmDialog(this, "Reset scene", "Are you sure you want to clear the scene?"))
    {
        return;
    }

    if (currentState) {
        clearHighlight(currentState);
        currentState = nullptr;
    }

    scene->clear();
    ui->inLast->clear();
    ui->outValue->clear();
    stateItems.clear();
    transitionItems.clear();
    machine.clear();
    stateIndexMap.clear();
    lastInput.clear();
    logText("Scene cleared");
}

// Button for adding new variable
void MainWindow::addNewVariable()
{
    QString type, name, value;
    if (DialogManager::createVariableDialog(this, type, name, value))
    {
        machine.addVariable(type.toStdString(), name.toStdString(), value.toStdString());
        ui->varValue->appendPlainText(type + " " + name + " = " + value);
        logText("Added new variable: " + name);
    }
}

// go back to start window and start again
// open dialog with ok and cancel buttons
// ok pressed -> cancel simulation and open new
// cancel pressed -> stay
void MainWindow::cancelWindow()
{
    if (!DialogManager::confirmDialog(this, "Cancel Simulation", "Are you sure you want to cancel simulation?"))
    {
        return;
    }

    StartupWindow *startWindow = new StartupWindow();
    startWindow->show();
    this->close();
}

// create new state and add it to the scene
// distinguish between normal and final state
// Create state using StateItem
StateItem *MainWindow::createState(QPointF position)
{
    return StateManager::createState(scene, position);
}

// Set a label for a state
void MainWindow::setStateLabel(QString stateName, StateItem* state) {
    StateManager::setStateLabel(stateName, state);
}

// Set a label for a transition
void MainWindow::setTransitionLabel(const QString &transitionName, StateItem *from, StateItem *to, QGraphicsPathItem *transition, const QPainterPath &path)
{
    TransitionManager::setTransitionLabel(scene, transitionName, from, to, transition, path, transitionItems);
}

// Update all transitions when a state is moved
void MainWindow::updateTransitions()
{
    TransitionManager::updateTransitions(scene, transitionItems);
}

// handle drop to scene
// create states and transitions between states
void MainWindow::handleDropEvent(QDropEvent *event)
{
    QString type = event->mimeData()->text();
    QPointF position = ui->graphicsView->mapToScene(event->pos());

    if (type == "State") {
        StateItem *state = createState(position);

        QString stateName;
        QString outputExpr;
        if (!DialogManager::createStateDialog(this, stateName, outputExpr))
        {
            delete state;
            return;
        }

        if (stateItems.contains(stateName))
        {
            QMessageBox::warning(this, "Error", "Duplicate state!");
            delete state;
            return;
        }

        int stateIndex;
        if (machine.getStates().empty())
        {
             stateIndex = machine.addStartState(stateName.toStdString(), outputExpr.toStdString(), {});
        }
        else
        {
             stateIndex = machine.addState(stateName.toStdString(), outputExpr.toStdString(), {});
        }

        stateIndexMap[stateName] = stateIndex;
        setStateLabel(stateName, state);
        stateItems[stateName] = state;
        logText("Added state " + stateName);

        if (!currentState && stateItems.size() == 1) {
            currentState = state;
            highlightState(currentState);
            logText("Set initial state: " + stateName);
        }
    } else if (type == "Transition") {
        QString transitionName, fromState, toState, inputEvent, boolExpr, delay;
        if (DialogManager::createTransitionDialog(this, transitionName, fromState, toState, inputEvent, boolExpr, delay)) {
            if (!stateItems.contains(fromState) || !stateItems.contains(toState)) {
                QMessageBox::warning(this, "Error", "State not found.");
                return;
            }

            StateItem *from = stateItems[fromState];
            StateItem *to = stateItems[toState];

            int fromIndex = stateIndexMap[fromState];
            int toIndex = stateIndexMap[toState];

            QString expression = inputEvent;
            if (!boolExpr.isEmpty())
                  expression += " [ " + boolExpr + " ]";
            if (!delay.isEmpty())
                  expression += " @ " + delay;

            machine.addTransition(fromIndex, expression.toStdString(), toIndex);

            machine.addInputs();
            ui->inDropdown->clear();

            // Add input event into dropDown menu
            for (const auto& s : machine.getInputs())
            {
                QString input = QString::fromStdString(s);
                ui->inDropdown->addItem(input);
            }

            QPointF arrowPos;
            double angle = 0.0;
            QPainterPath path = TransitionManager::createTransitionPath(from, to, arrowPos, angle);
            QGraphicsPathItem *transition = scene->addPath(path, QPen(Qt::black));

            TransitionManager::setTransitionLabel(scene, transitionName, from, to, transition, path, transitionItems);

            logText("Added transition from " + fromState + " to " + toState);
        }
    }
    event->acceptProposedAction();
}

// track events on graphicsView
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->graphicsView->viewport()) {
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent *dragEnterEvent = static_cast<QDragEnterEvent *>(event);
            dragEnterEvent->acceptProposedAction();
            return true;
        } else if (event->type() == QEvent::DragMove) {
            QDragMoveEvent *dragMoveEvent = static_cast<QDragMoveEvent *>(event);
            dragMoveEvent->acceptProposedAction();
            return true;
        } else if (event->type() == QEvent::Drop) {
            QDropEvent *dropEvent = static_cast<QDropEvent *>(event);
            handleDropEvent(dropEvent);
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

// initalizes drag from list widget to scene
void MainWindow::initDrag()
{
    QListWidgetItem *item = ui->addItemWidget->currentItem();
    if (!item) {
        return;
    }

    QMimeData *data = new QMimeData;
    data->setText(item->text());

    QDrag *drag = new QDrag(ui->addItemWidget);
    drag->setMimeData(data);

    drag->exec(Qt::CopyAction);
}

// Highlight a state
void MainWindow::highlightState(StateItem* state) {
    StateManager::highlightState(state);
}

// Clear highlight from a state
void MainWindow::clearHighlight(StateItem* state) {
    StateManager::clearHighlight(state);
}

// Load automaton from MooreMachine JSON file
void MainWindow::loadAutomatonFromMooreMachine(const QString &filename)
{
    JsonAutomaton automaton = FileParser::loadAutomatonFromMooreMachine(filename, machine);

    ui->nameDesc->setText(automaton.name + " - " + automaton.description);
    logText("Loaded automaton: " + automaton.name);

    // Get all defined inputs
    for (const string &s : machine.getInputs()) {
        ui->inDropdown->addItem(QString::fromStdString(s));
    }

    // Get all defined variables
    for (const auto& var : machine.getVars())
    {
        QString variable = QString::fromStdString(var.type + " " + var.name + " = " + var.value);
         ui->varValue->appendPlainText(variable);
    }

    // For adding new states and transitions into loaded file
    stateIndexMap.clear();
    const auto &states = machine.getStates();
    for (size_t i = 0; i < states.size(); ++i)
    {
        stateIndexMap[QString::fromStdString(states[i].name)] = static_cast<int>(i);
    }

    buildStatesFromLoaded(automaton.stateList);
    buildTransitionsFromLoaded(automaton.transitionList);
}

// Open file dialog to load automaton from file
void MainWindow::openFileHandler()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Automaton"), "", tr("JSON Files (*.json)"));

    if (!filename.isEmpty()) {
        resetSimulation();
        loadAutomatonFromMooreMachine(filename);
    }
}

// Generate json file from actual automaton
void MainWindow::generateJson()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save automaton"), "", tr("JSON Files (*.json)"));

    if (fileName.isEmpty())
    {
        return;
    }

    machine.createJSONFile(fileName.toStdString());
}

void MainWindow::generateCode()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save code"), "", tr("C++ Files (*.cpp)"));

    if (fileName.isEmpty())
    {
        return;
    }

    auto json = machine.getJson();

    CodeGenerator generator;
    if (generator.generateCode(json, fileName.toStdString()))
    {
        logText("C++ code generated");
    }
    else
    {
        QMessageBox::warning(this, "Error", "Code generating failed");
    }
    compileCode(fileName);
}

void MainWindow::compileCode(const QString &fileName)
{
    QString bin = fileName;
    bin.chop(4);
    #ifdef _WIN32
        bin += ".exe";
    #else
        bin += "_bin";
    #endif

    QProcess compile;
    compile.setProgram("g++");
    compile.setArguments({fileName, "-o", bin, "-static-libgcc", "-static-libstdc++", "-std=c++17"});
    compile.start();
    compile.waitForFinished();

    if (compile.exitStatus() != QProcess::NormalExit || compile.exitCode() != 0)
    {
        QMessageBox::warning(this, "Compilation Error", compile.readAllStandardError());
    }

    logText("Compilation success");
}

void MainWindow::handleStateUpdate(int index)
{
    updateState(index);
    const auto &states = machine.getStates();
    if (index >= 0 && index < static_cast<int>(states.size()))
    {
        QString stateName = QString::fromStdString(states[index].name);
        logText("TIMEOUT, moving to state: " + stateName);
    }
}

// destructor
MainWindow::~MainWindow()
{
    delete ui;
}
