/**
 * @file startWindow.cpp
 * @brief Implementation of StartupWindow class, where user can decide to create an automaton or load existing one from JSON file
 * @author Róbert Páleš (xpalesr00), Martin Valapka (xvalapm00)
*/

#include "startWindow.h"
#include "mainwindow.h"
#include "MooreMachine.h"
#include "ui_startup.h"

StartupWindow::StartupWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::startWindow)
{
    ui->setupUi(this);

    initButtons();
}

// Connect buttons
void StartupWindow::initButtons()
{
    connect(ui->createButton, &QPushButton::clicked, this, &StartupWindow::createButton);
    connect(ui->loadButton, &QPushButton::clicked, this, &StartupWindow::loadButton);
}

// Handle create button action
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

// Handle load button action
void StartupWindow::loadButton()
{
    // Open file dialog in actual window
    QString filePath = QFileDialog::getOpenFileName(this, "Load automaton", "","JSON Files (*.json)");
    if (filePath.isEmpty())
    {
        return;
    }

    // Create a MainWindow instance with default name and description
    MainWindow *mainWindow = new MainWindow("Loaded Automaton", "Loaded from file", nullptr);
    mainWindow->loadAutomatonFromMooreMachine(filePath);
    mainWindow->show();
    this->close();
}

StartupWindow::~StartupWindow()
{
    delete ui;
}

