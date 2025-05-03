#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <QString>
#include <QVariant>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

struct JsonVariable
{
    QString name;
    QString type;
    QVariant value;
};

struct JsonState
{
    QString name;
    QString action;
    bool isInitial;
    bool isFinal;
};

struct JsonTransition
{
    QString name;
    QString fromName;
    QString toName;
    QString event;
    QString condition;
    QString delay;
};

struct JsonAutomaton
{
    QString name;
    QString description;
    QList<QString> inputs;
    QList<QString> outputs;
    QList<JsonVariable> variableList;
    QList<JsonState> stateList;
    QList<JsonTransition> transitionList;
};

class FileParser
{
    public:
        static JsonAutomaton parse(const QJsonObject &root);

    private:
        static void parseInputs(const QJsonObject &root, JsonAutomaton &automaton);
        static void parseOutputs(const QJsonObject &root, JsonAutomaton &automaton);
        static void parseVariables(const QJsonObject &root, JsonAutomaton &automaton);
        static void parseStates(const QJsonObject &root, JsonAutomaton &automaton);
        static void parseTransitions(const QJsonObject &root, JsonAutomaton &automaton);
};

#endif // FILEPARSER_H
