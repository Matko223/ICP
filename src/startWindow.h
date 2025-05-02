#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QIODevice>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

// structure representing automaton
// TODO add input, output, states, transitions, variables
struct Automaton {
    QString name;
    QString description;
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
    Ui::startWindow *ui;
};

#endif // STARTWINDOW_H
