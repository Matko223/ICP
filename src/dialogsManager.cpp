/**
 * @file dialogsManager.cpp
 * @brief Implementation of the DialogManager class
 * @author Róbert Páleš (xpalesr00)
*/

#include "dialogsManager.h"

bool DialogManager::createTransitionDialog(QWidget *parent, QString &transitionName, QString &fromState, QString &toState, QString &inputEvent, QString &boolExpr, QString &delay)
{
    QDialog dialog(parent);
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
    QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // Ok is pressed -> return all values
    if (dialog.exec() == QDialog::Accepted) {
        transitionName = nameOfTransition->text();
        fromState = fromStateTransition->text();
        toState = toStateTransition->text();
        inputEvent = inputEventEdit->text();
        boolExpr = boolExprEdit->text();
        delay = delayEdit->text();

        if (transitionName.isEmpty() || fromState.isEmpty() || toState.isEmpty()) {
            QMessageBox::warning(parent, "Error", "Empty transition(s)");
            return false;
        }

        return true;
    }
    // Cancel
    else {
        return false;
    }
}

bool DialogManager::createStateDialog(QWidget *parent, QString &stateName, QString &outputExpr)
{
    QDialog dialog(parent);
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

    QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // OK is pressed -> return all values
    if (dialog.exec() == QDialog::Accepted) {
        stateName = nameEdit->text();
        outputExpr = outputEdit->text();

        if (stateName.isEmpty()) {
            QMessageBox::warning(parent, "Error", "State name cannot be empty");
            return false;
        }

        return true;
    }
    // Cancel
    else {
        return false;
    }
}

bool DialogManager::createVariableDialog(QWidget *parent, QString &type, QString &name, QString &value)
{
    QDialog dialog(parent);
    dialog.setWindowTitle("Create variable");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLineEdit *varType = new QLineEdit(&dialog);
    varType->setPlaceholderText("Enter variable type");

    QLineEdit *varName = new QLineEdit(&dialog);
    varName->setPlaceholderText("Enter variable name");

    QLineEdit *varValue = new QLineEdit(&dialog);
    varValue->setPlaceholderText("Enter variable value");

    QPushButton *okButton = new QPushButton("OK", &dialog);
    QPushButton *cancelButton = new QPushButton("Cancel", &dialog);

    layout->addWidget(new QLabel("Variable type:"));
    layout->addWidget(varType);
    layout->addWidget(new QLabel("Variable name:"));
    layout->addWidget(varName);
    layout->addWidget(new QLabel("Variable value:"));
    layout->addWidget(varValue);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addLayout(buttonLayout);
    dialog.setLayout(layout);

    QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // Ok is pressed -> return all values
    if (dialog.exec() == QDialog::Accepted) {
        type = varType->text();
        name = varName->text();
        value = varValue->text();

        if (type.isEmpty() || name.isEmpty() || value.isEmpty()) {
            QMessageBox::warning(parent, "Error", "Empty values");
            return false;
        }

        return true;
    }
    // Cancel
    else {
        return false;
    }
}

bool DialogManager::confirmDialog(QWidget *parent, const QString &windowTitle, const QString &dialogLabel)
{
    QDialog dialog(parent);

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

    QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    return dialog.exec() == QDialog::Accepted;
}

bool DialogManager::showHelpDialog(QWidget *parent)
{
    QDialog dialog(parent);

    dialog.setWindowTitle("Help");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QPlainTextEdit *helpText = new QPlainTextEdit(&dialog);

    helpText->setPlainText(R"(
    APPLICATION USAGE HELP

    This application is used for visual design, simulation, and code generation of Moore finite automata.

    1. User can choose between creating a new automaton or loading existing one from JSON file.
    2. Every automaton must have a name (required) and can include description (optional).
    3. In the Main Window, user can design the automaton.
    4. States and transitions are added to the graphics scene via drag&drop.

    States:
    - Each state must have:
        • a name (required)
        • an optional output expression (e.g., output("out", 1))
    - The output expression is executed when the automaton enters the state.
    - User can move states after adding them into the scene.

    Transitions:
    - Each transition includes the following:
        • name (required): label identifying the transition
        • from: starting state of the transition
        • to: destination state of the transition (indicated by an arrow)
        • input event: name of the input event
        • condition (optional): a logical expression that must be satisfied to complete the transition (e.g., atoi(valueof("in")) == 1)
        • delay (optional): transition occurs after a time delay (if no input is received)
    )");

    helpText->setReadOnly(true);
    layout->addWidget(helpText);

    QPushButton *cancelButton = new QPushButton("Cancel", &dialog);
    layout->addWidget(cancelButton);

    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    dialog.setMinimumSize(300, 350);
    dialog.setLayout(layout);
    return dialog.exec() == QDialog::Accepted;
}
