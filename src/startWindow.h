#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QIODevice>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDebug>

// structure for json states
struct JsonState {
    QString name;
    QString action;
    bool isInitial = false;
    bool isFinal = false;
};

// structure for json variables
struct JsonVariable {
    QString name;
    QString type;
    QVariant value;
};

// structure for json transitions
struct JsonTransition {
    QString name;
    QString fromName;
    QString toName;
    JsonState *fromState = nullptr;
    JsonState *toState = nullptr;
    QString event;
    QString condition;
    QString delay;
};

// structure representing automaton from json document
struct JsonAutomaton {
    QString name;
    QString description;
    QStringList inputs;
    QStringList outputs;
    QList<JsonVariable> variableList;
    QList<JsonState> stateList;
    QList<JsonTransition> transitionList;
};

namespace Ui {
class startWindow;
}

class StartupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartupWindow(QWidget *parent = nullptr);
    void initButtons();
    ~StartupWindow();

private:
    void createButton();
    void loadButton();
    QString readFile();
    void jsonParser(const QJsonObject &root);
    void parseInputs(const QJsonObject &root, JsonAutomaton &automaton);
    void parseOutputs(const QJsonObject &root, JsonAutomaton &automaton);
    void parseVariables(const QJsonObject &root, JsonAutomaton &automaton);
    void parseStates(const QJsonObject &root, JsonAutomaton &automaton);
    void parseTransitions(const QJsonObject &root, JsonAutomaton &automaton);
    Ui::startWindow *ui;
};

#endif // STARTWINDOW_H
