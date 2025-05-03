#include "startWindow.h"
#include "mainwindow.h"
#include "ui_startup.h"

StartupWindow::StartupWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::startWindow)
{
    ui->setupUi(this);

    initButtons();
}

// connect buttons
void StartupWindow::initButtons()
{
    connect(ui->createButton, &QPushButton::clicked, this, &StartupWindow::createButton);
    connect(ui->loadButton, &QPushButton::clicked, this, &StartupWindow::loadButton);
}

// handle create button action
void StartupWindow::createButton()
{
    QString name = ui->name->text();
    QString description = ui->description->text();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Name cannot be empty");
        return;
    }

    MainWindow *mainWindow = new MainWindow(name, description); // automaton constructor window
    mainWindow->show();
    this->close(); // close this window
}

// handle load button
void StartupWindow::loadButton()
{
    // open file dialog in actual window
    QString filePath = QFileDialog::getOpenFileName(this, "Load automaton from external file", "", "(*.json *.txt)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    // file is read only
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Failed to open file");
        return;
    }

    // file content is byte array
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseErr;
    // create json document from byte array
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseErr);
    if (parseErr.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, "Error", "JSON parsing");
        return;
    }

    // main json object
    QJsonObject root = doc.object();
    jsonParser(root);
}

void StartupWindow::parseInputs(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonArray input = root.value("Inputs").toArray();
    for (const QJsonValue &value : input)
    {
        automaton.inputs.append(value.toString());
    }
}

void StartupWindow::parseOutputs(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonArray output = root.value("Outputs").toArray();
    for (const QJsonValue &value : output)
    {
        automaton.outputs.append(value.toString());
    }
}

void StartupWindow::parseVariables(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonObject variable = root.value("Variables").toObject();
    for (const QString &name : variable.keys())
    {
        QJsonObject content = variable[name].toObject();

        JsonVariable var;
        var.name = name;
        var.type = content["type"].toString();
        var.value = content["value"].toVariant();

        automaton.variableList.append(var);
    }
}

void StartupWindow::parseStates(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonObject states = root.value("States").toObject();
    for (const QString &name : states.keys())
    {
        QJsonObject content = states[name].toObject();

        JsonState state;
        state.name = name;
        state.action  = content["action"].toString();
        state.isInitial = content["initial"].toBool(false);
        state.isFinal = content["final"].toBool(false);

        automaton.stateList.append(state);
    }
}

void StartupWindow::parseTransitions(const QJsonObject &root, JsonAutomaton &automaton)
{
    QJsonObject transitions = root.value("Transitions").toObject();
    for (const QString &name : transitions.keys())
    {
        QJsonObject content = transitions[name].toObject();

        JsonTransition transition;
        transition.name = name;
        transition.fromName = content["from"].toString();
        transition.toName = content["to"].toString();
        transition.event = content["event"].toString();
        transition.condition = content["condition"].toString();
        transition.delay = content["delay"].toString();

        automaton.transitionList.append(transition);
    }
}

// parse the loaded json document
void StartupWindow::jsonParser(const QJsonObject &root)
{
    JsonAutomaton automaton;

    automaton.name = root.value("Name").toString();
    automaton.description = root.value("Description").toString();

    parseInputs(root, automaton);
    parseOutputs(root, automaton);
    parseVariables(root, automaton);
    parseStates(root, automaton);
    parseTransitions(root, automaton);
}

// destructor
StartupWindow::~StartupWindow()
{
    delete ui;
}
