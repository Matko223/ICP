#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

// TODO: prechod aaa a->a a potom aaa a->b chyba; simulaciu

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

    // Initialize scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 1000, 1000);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setSceneRect(0, 0, 1000, 1000);
    ui->graphicsView->resetTransform();
    ui->graphicsView->setMinimumSize(500, 500);

    // Enable scrollbars
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    ui->graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    connect(scene, &QGraphicsScene::changed, this, &MainWindow::updateTransitions);
    ui->nameDesc->setText(name + " - " + description);

    initializeControlWidget();
    logText("Automaton " + name + " initialized");

    // input handling
    connect(ui->inEdit, &QLineEdit::returnPressed, this, &MainWindow::handleInput);
}

// New slot to handle input
void MainWindow::handleInput()
{
    QString input = ui->inEdit->text().trimmed();
    if (input.isEmpty()) {
        QMessageBox::warning(this, "Error", "Input cannot be empty.");
        return;
    }

    lastInput = input;
    logText("Input received: " + input);
    ui->inEdit->clear();
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

    // TODO: DOKONOCIT SIMULACIU
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
// clear scene
void MainWindow::resetSimulation()
{
    if (currentState) {
        clearHighlight(currentState);
        currentState = nullptr;
    }
    scene->clear();
    stateItems.clear();
    transitionItems.clear();
    lastInput.clear(); // Clear lastInput on reset
    logText("Scene cleared");
}

// go back to start window and start again
// open dialog with ok and cancel buttons
// ok pressed -> cancel simulation and open new
// cancel pressed -> stay
void MainWindow::cancelSimulation()
{
    QDialog dialog(this);

    dialog.setWindowTitle("Cancel simulation");
    QLabel *label = new QLabel("Are you sure you want to cancel simulation?", &dialog);

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

    if (dialog.exec() == QDialog::Accepted) {
        StartupWindow *startWindow = new StartupWindow(); // start window
        startWindow->show();
        this->close(); // close this window
    } else {
        return;
    }
}

// create new state and add it to the scene
// distinguish between normal and final state
// Create state using StateItem
StateItem *MainWindow::createState(QString type, QPointF position)
{
    StateItem *state = new StateItem(0, 0, 50, 50);
    state->setPos(position);

    if (type == "Final state") {
        state->setPen(QPen(Qt::black));
        state->setBrush(Qt::transparent);

        // Final state has 2 circles
        StateItem *innerCircle = new StateItem(5, 5, 40, 40, state);
        innerCircle->setPen(QPen(Qt::black));
        innerCircle->setBrush(Qt::transparent);
        // Disable movability, selectability, and event handling for the inner circle
        innerCircle->setFlag(QGraphicsItem::ItemIsMovable, false);
        innerCircle->setFlag(QGraphicsItem::ItemIsSelectable, false);
        innerCircle->setAcceptHoverEvents(false);
        innerCircle->setAcceptedMouseButtons(Qt::NoButton);
    } else {
        state->setPen(QPen(Qt::black));
        state->setBrush(Qt::transparent);
    }

    scene->addItem(state);
    return state;
}

// transition dialog
// user enters transition name, from state and to state
// confirm with ok button, cancel with cancel button
bool MainWindow::createTransitionDialog(QString &transitionName, QString &fromState, QString &toState)
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
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        transitionName = nameOfTransition->text();
        fromState = fromStateTransition->text();
        toState = toStateTransition->text();

        if (transitionName.isEmpty() || fromState.isEmpty() || toState.isEmpty()) {
            QMessageBox::warning(this, "Error", "Empty transition(s)");
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

void MainWindow::drawArrow(const QPointF &arrowPos, double angle)
{
    qreal size = 10.0;
    // control points for arrow drawing
    QPointF point1 = arrowPos - QPointF(qCos(angle + PI / 6) * size, qSin(angle + PI / 6) * size);
    QPointF point2 = arrowPos - QPointF(qCos(angle - PI / 6) * size, qSin(angle - PI / 6) * size);

    QPolygonF arrow;
    arrow << arrowPos << point1 << point2;
    scene->addPolygon(arrow, QPen(Qt::black), QBrush(Qt::black)); // draw an arrow in the direction of angle
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

    transitionItems.insert(transitionName, t);
}

// Update all transitions when a state is moved
void MainWindow::updateTransitions()
{
    for (auto it = transitionItems.begin(); it != transitionItems.end(); ++it) {
        Transition &t = it.value();
        QString transitionName = it.key();

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

    if (type == "State" || type == "Final state") {
        StateItem *state = createState(type, position);

        bool ok;
        QString stateName;
        while (true) {
            stateName = QInputDialog::getText(this, "New state", "Enter state name", QLineEdit::Normal, "", &ok);
            if (stateItems.contains(stateName)) {
                QMessageBox::warning(this, "Error", "Duplicate! Enter new name", QMessageBox::Ok);
                continue;
            }
            if (!ok) {
                delete state;
                return;
            }
            if (stateName.isEmpty()) {
                QMessageBox::warning(this, "Error", "Name cannot be empty");
                continue;
            }
            break;
        }

        QGraphicsTextItem *label = new QGraphicsTextItem(stateName, state);
        label->setDefaultTextColor(Qt::black);
        QRectF rect = state->rect();
        QRectF labelRect = label->boundingRect();
        label->setPos(rect.width() / 2 - labelRect.width() / 2, rect.height() / 2 - labelRect.height() / 2);

        stateItems[stateName] = state;
        logText("Added state " + stateName);

        if (!currentState && stateItems.size() == 1) {
            currentState = state;
            highlightState(currentState);
            logText("Set initial state: " + stateName);
        }
    } else if (type == "Transition") {
        QString transitionName, fromState, toState;
        if (createTransitionDialog(transitionName, fromState, toState)) {
            if (!stateItems.contains(fromState) || !stateItems.contains(toState)) {
                QMessageBox::warning(this, "Error", "State not found.");
                return;
            }

            StateItem *from = stateItems[fromState];
            StateItem *to = stateItems[toState];

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

            transitionItems.insert(transitionName, t);
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
void MainWindow::highlightState(StateItem *state)
{
    if (state) {
        state->setBrush(QBrush(Qt::yellow));
        state->update();
    }
}

void MainWindow::clearHighlight(StateItem *state)
{
    if (state) {
        state->setBrush(Qt::transparent);
        state->update();
    }
}

// destructor
MainWindow::~MainWindow()
{
    delete ui;
}
