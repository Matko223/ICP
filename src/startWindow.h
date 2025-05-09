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

/**
 * @class StartupWindow
 * @brief Initial window allowing users to create or load a Moore machine
 */
class StartupWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for StartupWindow
     * @param parent Parent widget
     */
    explicit StartupWindow(QWidget *parent = nullptr);

    /**
     * @brief Initializes button connections
     */
    void initButtons();

    /**
     * @brief Destructor for StartupWindow
     */
    ~StartupWindow();

private:
    /**
     * @brief Handles create button click event
     */
    void createButton();

    /**
     * @brief Handles load button click event
     */
    void loadButton();

    /**
     * @brief UI components for the startup window
     */
    Ui::startWindow *ui;
};

#endif // STARTWINDOW_H
