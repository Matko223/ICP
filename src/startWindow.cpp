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

// parse the loaded json document
// TODO create an automaton based on the json structure
void StartupWindow::jsonParser(const QJsonObject &root)
{
    Automaton automaton;

    automaton.name = root.value("Name").toString();
    automaton.description = root.value("Description").toString();
    QString name = automaton.name;
    QString description = automaton.description;

    // debug
    qDebug() << name;
    qDebug() << description;

    // open main window with parsed name and description of automaton
    MainWindow *mainWindow = new MainWindow(name, description);
    mainWindow->show();
    this->close(); // close this window
}

// destructor
StartupWindow::~StartupWindow()
{
    delete ui;
}
