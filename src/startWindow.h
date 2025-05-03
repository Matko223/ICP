#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QIODevice>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDebug>

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
    Ui::startWindow *ui;
};

#endif // STARTWINDOW_H
