/**
 * @file dialogsManager.h
 * @brief Header file for the DialogManager class
 * @author Róbert Páleš (xpalesr00)
*/

#ifndef DIALOGSMANAGER_H
#define DIALOGSMANAGER_H

#include <QString>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QObject>

class QWidget;

/**
 * @class DialogManager
 * @brief Class providing dialog functions
 *
 * Opens certain dialog when adding states, transitions, variables or when some action has to be confirmed
 */
class DialogManager
{
    public:
        /**
         * @brief Opens dialog when adding a new transition
         * @param parent Parent widget
         * @param transitionName Name of the transition
         * @param fromState Source state
         * @param toState Destination state
         * @param inputEvent Input event
         * @param boolExpr Boolean expression
         * @param delay Delay expression
         * @return true if the user confirmed the dialog, otherwise false
         */
        static bool createTransitionDialog(QWidget *parent, QString &transitionName, QString &fromState, QString &toState, QString &inputEvent, QString &boolExpr, QString &delay);

        /**
         * @brief Opens dialog when adding a new state
         * @param parent Parent widget
         * @param stateName Name of the new state
         * @param outputExpr Output of the state
         * @return true if the user confirmed the dialog, otherwise false
         */
        static bool createStateDialog(QWidget *parent, QString &stateName, QString &outputExpr);

        /**
         * @brief Opens dialog when adding a new variable
         * @param parent Parent widget
         * @param type Variable type
         * @param name Variable name
         * @param value Variable value
         * @return true if the user confirmed the dialog, otherwise false
         */
        static bool createVariableDialog(QWidget *parent, QString &type, QString &name, QString &value);

        /**
         * @brief Opens dialog with message to be confirmed, OK and cancel buttons
         * @param parent Parent widget
         * @param windowTitle Title of the dialog window
         * @param dialogLabel Message for the user
         * @return true if clicked OK, false if Cancel was clicked
         */
        static bool confirmDialog(QWidget *parent, const QString &windowTitle, const QString &dialogLabel);
};

#endif // DIALOGSMANAGER_H
