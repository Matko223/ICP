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

// TODO load external text file and process
void StartupWindow::loadButton() {}

// destructor
StartupWindow::~StartupWindow()
{
    delete ui;
}
