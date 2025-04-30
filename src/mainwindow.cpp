#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO: prechod aaa a->a a potom aaa a->b chyba

MainWindow::MainWindow(const QString &name, const QString &description, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAcceptDrops(true);
    ui->listWidget->setDragEnabled(true);

    ui->graphicsView->viewport()->setAcceptDrops(true); // graphicsView receives drops and reacts on events using eventFilter
    ui->graphicsView->viewport()->installEventFilter(this); // events are handled in eventFilter

    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget->setDragDropMode(QAbstractItemView::DragOnly);

    // start initDrag, create mime and drag object
    connect(ui->listWidget, &QListWidget::itemPressed, this, [this](QListWidgetItem *)
    {
        initDrag();
    });

    // init scene
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setSceneRect(0,0,500,500);

    ui->nameDesc->setText(name + " - " + description); // add automaton name and description from startWindow

    initializeControlWidget(); // connect control buttons
    logText("Automaton " + name + " initialized");
}

// each action is logged with number, date and description
void MainWindow::logText(QString str)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->plainTextEdit->appendPlainText(
    QString::number(logCounter) + ". [" + timestamp + "] " + str);
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

}

// pause simulation button handling
void MainWindow::pauseSimulation()
{

}

// reset simulation
// clear scene
void MainWindow::resetSimulation()
{
    scene->clear();
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

    if (dialog.exec() == QDialog::Accepted)
    {
        StartupWindow *startWindow = new StartupWindow(); // start window
        startWindow->show();
        this->close(); // close this window
    }
    else
    {
        return;
    }
}

// create new state and add it to the scene
// distinguish between normal and final state
QGraphicsEllipseItem* MainWindow::createState(QString type, QPointF position)
{
    QGraphicsEllipseItem *state = new QGraphicsEllipseItem(0, 0, 50, 50);
    state->setPos(position);
    state->setFlag(QGraphicsItem::ItemIsMovable, false); // sticky for now
    state->setFlag(QGraphicsItem::ItemIsSelectable, true);

    if (type == "Final state")
    {
        state->setPen(QPen(Qt::black));
        state->setBrush(Qt::transparent);

        // final state has 2 circles
        QGraphicsEllipseItem *innerCircle = new QGraphicsEllipseItem(5, 5, 40, 40, state);
        innerCircle->setPen(QPen(Qt::black));
        innerCircle->setBrush(Qt::transparent);
    }
    // normal state
    else
    {
        state->setPen(QPen(Qt::black));
        state->setBrush(Qt::transparent);
    }
    scene->addItem(state); // add state to the scene
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

    if (dialog.exec() == QDialog::Accepted)
    {
        transitionName = nameOfTransition->text();
        fromState = fromStateTransition->text();
        toState = toStateTransition->text();

        if (transitionName.isEmpty() || fromState.isEmpty() || toState.isEmpty())
        {
            QMessageBox::warning(this, "Error", "Empty transition(s)");
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

// create a path between 2 states
// set an arrow position and rotation angle based on the path
QPainterPath MainWindow::createTransitionPath(QGraphicsEllipseItem *fromState, QGraphicsEllipseItem *toState, QPointF &arrowPos, double &angle)
{
    // get a center of each state
    QPointF point1 = fromState->sceneBoundingRect().center();
    QPointF point2 = toState->sceneBoundingRect().center();

    // draw a line between states
    QLineF line(point1, point2);

    // state is a circle -> radius is width / 2
    qreal radius = fromState->rect().width() / 2;

    // transition starts and ends on the edges of state
    if (line.length() > 0)
    {
        // move a point from center to the edge using radius
        // update points
        line.setLength(line.length() - radius);
        point2 = line.p2();
        line.setLength(radius);
        point1 = line.p2();
    }

    QPainterPath path(point1);

    // selfloop state transition
    // draw a bezier curve above state
    if (fromState == toState)
    {
        qreal offset = 40.0;  // offset from the state edge
        qreal distance = 20.0; // distance between start point and end point

        QPointF top = point1 + QPointF(0, -radius); // point at the top of the state above center

        // start and end of the curve
        QPointF startPoint = top + QPointF(-distance / 2, 0);
        QPointF endPoint = top + QPointF(distance / 2, 0);

        // helper points for bezier curve construction (set its height and shape)
        QPointF control1 = startPoint + QPointF(-offset, -offset * 1.5);
        QPointF control2 = endPoint + QPointF(offset, -offset * 1.5);

        path.moveTo(startPoint); // start drawing from startPoint
        path.cubicTo(control1, control2, endPoint); // draw a cubic curve from startPoint to endPoint through 2 control points

        arrowPos = endPoint; // arrow is at the end of the curve
    }
    // normal transition
    else
    {
        QPointF control1(point1.x(), (point1.y() + point2.y()) / 2); // helper point for curve construction

        path.quadTo(control1, point2); // draw a quadratic curve
        arrowPos = point2; // arrow is at the end of the curve (point 2 which is "to state")
    }
    // get the angle of the tangent at the end of the path
    // sin and cos function works with radians, so the conversion from deegres to radians is neccessary
    // 1.0 represent the end of the path
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

void MainWindow::setTransitionLabel(const QString &transitionName, QGraphicsEllipseItem *from, QGraphicsEllipseItem *to, QGraphicsPathItem *transition, const QPainterPath &path)
{
    Transition t = { from, to, transition };
    transitionItems.insert(transitionName, t); // save transition name to map

    QGraphicsTextItem *label = scene->addText(transitionName);
    label->setDefaultTextColor(Qt::black);

    // selfloop state transition
    if (from == to)
    {
        QPointF point1 = from->sceneBoundingRect().center(); // get the center
        qreal radius = from->rect().width() / 2;
        QPointF top = point1 + QPointF(0, -radius); // get the top center point
        qreal offset = 55; // distance from the top center for the label

        label->setPos(top.x() - label->boundingRect().width() / 2,
                      top.y() - offset - label->boundingRect().height() / 2); // set the label position
    }
    // normal transition
    else
    {
        QPointF center = path.pointAtPercent(0.5); // get the center of the path
        qreal angle = qDegreesToRadians(path.angleAtPercent(0.5)); // angle of tanget in the center
        QPointF offset(-qSin(angle) * 10.0, -qCos(angle) * 10.0);
        QPointF pos = center + offset;

        label->setPos(pos.x() - label->boundingRect().width() / 2,
                      pos.y() - label->boundingRect().height() / 2);  // set the label position
    }
}

// handle drop to scene
// create states and transitions between states
void MainWindow::handleDropEvent(QDropEvent *event)
{
    // get the type and position of an item
    QString type = event->mimeData()->text();
    QPointF position = ui->graphicsView->mapToScene(event->pos());

    if (type == "State" || type == "Final state")
    {
        // create a state
        QGraphicsEllipseItem *state = createState(type, position);

        bool ok; // for tracking ok or cancel button
        QString stateName;
        while (true)
        {
            // show and input dialog and ask for name of the state
            stateName = QInputDialog::getText(this, "New state", "Enter state name", QLineEdit::Normal, "", &ok);

            // if the name already exists -> error and ask again
            if (stateItems.contains(stateName))
            {
                QMessageBox::warning(this, "Error", "Duplicate! Enter new name", QMessageBox::Ok);
                continue;
            }

            // cancel button deletes an item
            if (!ok)
            {
                delete state;
                return;
            }

            if (stateName.isEmpty())
            {
                QMessageBox::warning(this, "Error", "Name cannot be empty");
                continue;
            }
            break;
        }

        // text label of the state
        QGraphicsTextItem *label = new QGraphicsTextItem(stateName, state);
        label->setDefaultTextColor(Qt::black);

        QRectF rect = state->rect(); // get the state dimensions inside
        QRectF labelRect = label->boundingRect();
        label->setPos(rect.width() / 2 - labelRect.width() / 2, rect.height() / 2 - labelRect.height() / 2); // align to center of the state

        stateItems[stateName] = state; // save the state to the map
        logText("Added state " + stateName);
    }
    else if (type == "Transition")
    {
        QString transitionName, fromState, toState;

        if (createTransitionDialog(transitionName, fromState, toState))
        {
            if (!stateItems.contains(fromState) || !stateItems.contains(toState))
            {
                QMessageBox::warning(this, "Error", "State not found.");
                return;
            }

            QGraphicsEllipseItem *from = stateItems[fromState];
            QGraphicsEllipseItem *to = stateItems[toState];

            QPointF arrowPos;
            double angle = 0.0;

            QPainterPath path = createTransitionPath(from, to, arrowPos, angle);

            QGraphicsPathItem *transition = scene->addPath(path, QPen(Qt::black));

            drawArrow(arrowPos, angle); // draw arrow
            setTransitionLabel(transitionName, from, to, transition, path); // set label

            logText("Added transition from " + fromState + " to " + toState);
        }
    }
    event->acceptProposedAction(); // drag&drop successful
}

// track events on graphicsView
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->graphicsView->viewport())
    {
        if (event->type() == QEvent::DragEnter)
        {
            QDragEnterEvent *dragEnterEvent = static_cast<QDragEnterEvent*>(event);
            dragEnterEvent->acceptProposedAction();
            return true;
        }
        else if (event->type() == QEvent::DragMove)
        {
            QDragMoveEvent *dragMoveEvent = static_cast<QDragMoveEvent*>(event);
            dragMoveEvent->acceptProposedAction();
            return true;
        }
        else if (event->type() == QEvent::Drop)
        {
            QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
            handleDropEvent(dropEvent);
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

// initalizes drag from list widget to scene
void MainWindow::initDrag()
{
    QListWidgetItem *item = ui->listWidget->currentItem(); // find item in list widget
    if (!item)
    {
        return;
    }

    QMimeData *data = new QMimeData; // create mime object
    data->setText(item->text()); // save the item into data object

    QDrag *drag = new QDrag(ui->listWidget); // create drag object
    drag->setMimeData(data); // add drag data into drag object

    drag->exec(Qt::CopyAction); // start drag as copy action
}

// desctructor
MainWindow::~MainWindow()
{
    delete ui;
}
