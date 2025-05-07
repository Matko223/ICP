#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

/* TODO - prechod aaa a->a a potom aaa a->b chyba; simulaciu
        - z vytvoreneho automatu vygenerovat json
        - dekompozicia trochu lebo je v tom border uz
*/

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
    ui->listWidget->setDragEnabled(true);

    ui->graphicsView->viewport()->setAcceptDrops(true);
    ui->graphicsView->viewport()->installEventFilter(this);

    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget->setDragDropMode(QAbstractItemView::DragOnly);

    // Initialize drag
    connect(ui->listWidget, &QListWidget::itemPressed, this, [this](QListWidgetItem *) {
        initDrag();
    });

    initScene();

    connect(scene, &QGraphicsScene::changed, this, &MainWindow::updateTransitions);
    ui->nameDesc->setText(name + " - " + description);
    initializeControlWidget();
    logText("Automaton " + name + " initialized");

    // input handling
    connect(ui->inValue, &QLineEdit::returnPressed, this, &MainWindow::handleInput);
}

// Build states from loaded file
void MainWindow::buildStatesFromLoaded(const QList<JsonState>& states) {
    StateManager::loadStates(scene, stateItems, currentState, states);
}

// build transition from loaded file
void MainWindow::buildTransitionsFromLoaded(const QList<JsonTransition> &transitions)
{
    for (const JsonTransition &transition : transitions)
    {
        if (!stateItems.contains(transition.fromName) || !stateItems.contains(transition.toName))
        {
            QMessageBox::warning(this, "Error", "Unknown state");
            continue;
        }

        StateItem *fromState = stateItems[transition.fromName];
        StateItem *toState = stateItems[transition.toName];

        QPointF arrowPos;
        double angle;
        QPainterPath path = createTransitionPath(fromState, toState, arrowPos, angle);

        QGraphicsPathItem *pathItem = scene->addPath(path, QPen(Qt::black));

        setTransitionLabel(transition.name, fromState, toState, pathItem, path);

        logText("Added transition: " + transition.name + " from state: " + transition.fromName + " to state: " + transition.toName);
    }
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
        QMetaObject::invokeMethod(this, [this, index]()
        {
            updateState(index);
            const auto &states = machine.getStates();
            if (index >= 0 && index < static_cast<int>(states.size()))
            {
                QString stateName = QString::fromStdString(states[index].name);
                logText("DELAY, moving to state: " + stateName);
            }
        }, Qt::QueuedConnection);
    };

    ui->inValue->clear();

    ui->outValue->appendPlainText(QString::fromStdString(machine.getCurrentOutput()));
}

// update highlight to the next state
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

// each action is logged with number, date and description
void MainWindow::logText(QString str)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->plainTextEdit->appendPlainText(QString::number(logCounter) + ". [" + timestamp + "] " + str);
    logCounter++;
}

// connect control widget buttons
void MainWindow::initializeControlWidget()
{
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startSimulation);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::pauseSimulation);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::resetSimulation);
    connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::cancelSimulation);
}

// start simulation button handling
void MainWindow::startSimulation()
{
    if (stateItems.isEmpty()) {
        QMessageBox::warning(this, "Error", "No states available to start simulation.");
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
}

// pause simulation button handling
void MainWindow::pauseSimulation()
{
    if (currentState) {
        logText("Simulation paused at state: " + stateItems.key(currentState));
    } else {
        logText("Simulation paused (no current state)");
    }
}

// reset simulation
// clear scene and all text fields
void MainWindow::resetSimulation()
{
    if (!confirmDialog("Reset scene", "Are you sure you want to clear the scene?"))
    {
        return;
    }

    if (currentState) {
        clearHighlight(currentState);
        currentState = nullptr;
    }

    scene->clear();
    ui->outValue->clear();
    stateItems.clear();
    transitionItems.clear();
    lastInput.clear();
    logText("Scene cleared");
}

bool MainWindow::confirmDialog(const QString &windowTitle, const QString &dialogLabel)
{
    QDialog dialog(this);

    dialog.setWindowTitle(windowTitle);
    QLabel *label = new QLabel(dialogLabel, &dialog);

    QPushButton *okButton = new QPushButton("OK", &dialog);
    QPushButton *cancelButton = new QPushButton("Cancel", &dialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addLayout(buttonLayout);

    dialog.setLayout(layout);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    return dialog.exec() == QDialog::Accepted;
}

// go back to start window and start again
// open dialog with ok and cancel buttons
// ok pressed -> cancel simulation and open new
// cancel pressed -> stay
void MainWindow::cancelSimulation()
{
    if (!confirmDialog("Cancel Simulation", "Are you sure you want to cancel simulation?"))
    {
        return;
    }

    StartupWindow *startWindow = new StartupWindow();
    startWindow->show();
    this->close();
}

// Create a new state
StateItem* MainWindow::createState(QPointF position) {
    return StateManager::createState(scene, position);
}

// transition dialog
// user enters transition name, from state and to state
// confirm with ok button, cancel with cancel button
bool MainWindow::createTransitionDialog(QString &transitionName, QString &fromState, QString &toState, QString &inputEvent, QString &boolExpr, QString &delay)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create transition");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *nameOfTransition = new QLineEdit(&dialog);
    nameOfTransition->setPlaceholderText("Enter transition name");

    QLineEdit *fromStateTransition = new QLineEdit(&dialog);
    fromStateTransition->setPlaceholderText("Enter from state");

    QLineEdit *toStateTransition = new QLineEdit(&dialog);
    toStateTransition->setPlaceholderText("Enter to state");

    QLineEdit *inputEventEdit = new QLineEdit(&dialog);
    inputEventEdit->setPlaceholderText("Enter input event (in)");

    QLineEdit *boolExprEdit = new QLineEdit(&dialog);
    boolExprEdit->setPlaceholderText("Enter condition (atoi(valueof(\"in\")) == expr)");

    QLineEdit *delayEdit = new QLineEdit(&dialog);
    delayEdit->setPlaceholderText("Enter delay (timeout)");

    QPushButton *okButton = new QPushButton("OK", &dialog);
    QPushButton *cancelButton = new QPushButton("Cancel", &dialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(new QLabel("Transition name"));
    layout->addWidget(nameOfTransition);
    layout->addWidget(new QLabel("Start state"));
    layout->addWidget(fromStateTransition);
    layout->addWidget(new QLabel("End state"));
    layout->addWidget(toStateTransition);
    layout->addWidget(new QLabel("Input Event:"));
    layout->addWidget(inputEventEdit);
    layout->addWidget(new QLabel("Condition (optional):"));
    layout->addWidget(boolExprEdit);
    layout->addWidget(new QLabel("Delay (optional):"));
    layout->addWidget(delayEdit);
    layout->addLayout(buttonLayout);

    dialog.setMinimumWidth(260);
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // if the transition is accepted return all values
    if (dialog.exec() == QDialog::Accepted) {
        transitionName = nameOfTransition->text();
        fromState = fromStateTransition->text();
        toState = toStateTransition->text();
        inputEvent = inputEventEdit->text();
        boolExpr = boolExprEdit->text();
        delay = delayEdit->text();

        if (transitionName.isEmpty() || fromState.isEmpty() || toState.isEmpty()) {
            QMessageBox::warning(this, "Error", "Empty transition(s)");
            return false;
        }

        return true;
    } else {
        return false;
    }
}

bool MainWindow::createStateDialog(QString &stateName, QString &outputExpr)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create State");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *nameEdit = new QLineEdit(&dialog);
    nameEdit->setPlaceholderText("Enter state name");

    QLineEdit *outputEdit = new QLineEdit(&dialog);
    outputEdit->setPlaceholderText("Enter output expression { output(\"out\", expr }");

    QPushButton *okButton = new QPushButton("OK", &dialog);
    QPushButton *cancelButton = new QPushButton("Cancel", &dialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(new QLabel("State Name:"));
    layout->addWidget(nameEdit);
    layout->addWidget(new QLabel("Output Expression:"));
    layout->addWidget(outputEdit);
    layout->addLayout(buttonLayout);

    dialog.setLayout(layout);
    dialog.setMinimumWidth(260);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        stateName = nameEdit->text();
        outputExpr = outputEdit->text();

        if (stateName.isEmpty()) {
            QMessageBox::warning(this, "Error", "State name cannot be empty");
            return false;
        }

        return true;
    } else {
        return false;
    }
}


// create a path between 2 states
// set an arrow position and rotation angle based on the path
QPainterPath MainWindow::createTransitionPath(StateItem *fromState, StateItem *toState, QPointF &arrowPos, double &angle)
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

// Create and return arrow for storage in Transition
QGraphicsPolygonItem *MainWindow::createArrow(const QPointF &arrowPos, double angle)
{
    qreal size = 10.0;
    QPointF point1 = arrowPos - QPointF(qCos(angle + PI / 6) * size, qSin(angle + PI / 6) * size);
    QPointF point2 = arrowPos - QPointF(qCos(angle - PI / 6) * size, qSin(angle - PI / 6) * size);
    QPolygonF arrow;
    arrow << arrowPos << point1 << point2;
    return scene->addPolygon(arrow, QPen(Qt::black), QBrush(Qt::black));
}

// Set a label for a state
void MainWindow::setStateLabel(QString stateName, StateItem* state) {
    StateManager::setStateLabel(stateName, state);
}

void MainWindow::setTransitionLabel(const QString &transitionName, StateItem *from, StateItem *to, QGraphicsPathItem *transition, const QPainterPath &path)
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
    createTransitionPath(from, to, arrowPos, angle);
    t.arrow = createArrow(arrowPos, angle);

    // Create unique identifier for this transition
    QString uniqueId = QString("%1_%2_%3").arg(transitionName).arg((quintptr)from).arg((quintptr)to);

    t.name = transitionName;

    // Use the unique ID as the map key
    transitionItems.insert(uniqueId, t);
}

// Update all transitions when a state is moved
void MainWindow::updateTransitions()
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
        t.arrow = createArrow(arrowPos, angle);
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
        if (!createStateDialog(stateName, outputExpr))
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
        if (createTransitionDialog(transitionName, fromState, toState, inputEvent, boolExpr, delay)) {
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
            ui->inDropdown->clear(); // toto tu musi byt lebo to robi nepekne veci bez toho

            // Add input event into dropDown menu
            for (const auto& s : machine.getInputs())
            {
                QString input = QString::fromStdString(s);
                ui->inDropdown->addItem(input);
            }

            QPointF arrowPos;
            double angle = 0.0;
            QPainterPath path = createTransitionPath(from, to, arrowPos, angle);
            QGraphicsPathItem *transition = scene->addPath(path, QPen(Qt::black));

            Transition t;
            t.from_state = from;
            t.to_state = to;
            t.path = transition;
            t.arrow = createArrow(arrowPos, angle);
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

            t.name = transitionName;

            // Creates unique ID
            QString uniqueId = QString("%1_%2_%3").arg(transitionName).arg((quintptr)from).arg((quintptr)to);

            // ID as key
            transitionItems.insert(uniqueId, t);
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
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) {
        return;
    }

    QMimeData *data = new QMimeData;
    data->setText(item->text());

    QDrag *drag = new QDrag(ui->listWidget);
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

    vector<string> inputs = machine.getInputs();
    for (const string &s : inputs) {
        ui->inDropdown->addItem(QString::fromStdString(s));
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

// destructor
MainWindow::~MainWindow()
{
    delete ui;
}
