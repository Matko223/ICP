/**
 * @file main.cpp
 * @brief Main function
 * @author Róbert Páleš (xpalesr00)
 */

#include <QApplication>
#include "startWindow.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    StartupWindow w;
    w.show();
    return a.exec();
}
