/**
 * @file startWindow.h
 * @brief Header file for the StartupWindow class
 * @author Róbert Páleš (xpalesr00)
*/

#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>

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
