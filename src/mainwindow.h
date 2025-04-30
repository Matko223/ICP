#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "startWindow.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QMap>
#include <QDockWidget>
#include <QMimeData>
#include <QInputDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QtMath>
#include <QFileDialog>
#include <QTextEdit>
#include <QRadioButton>
#include <QDrag>
#include <QDateTime>

#define PI 3.14159

struct Transition
{
    QGraphicsEllipseItem *from_state;
    QGraphicsEllipseItem *to_state;
    QGraphicsPathItem *path;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(const QString &name, const QString &description, QWidget *parent = nullptr);
        void logText(QString str);

        void initializeControlWidget();
        void startSimulation();
        void pauseSimulation();
        void cancelSimulation();
        void resetSimulation();

        QGraphicsEllipseItem *createState(QString type, QPointF position);

        bool createTransitionDialog(QString &transitionName, QString &fromState, QString &toState);
        QPainterPath createTransitionPath(QGraphicsEllipseItem *from, QGraphicsEllipseItem *to, QPointF &arrowPos, double &angle);
        void drawArrow(const QPointF &arrowPos, double angle);
        void setTransitionLabel(const QString &name, QGraphicsEllipseItem *from, QGraphicsEllipseItem *to, QGraphicsPathItem *pathItem, const QPainterPath &path);

        void handleDropEvent(QDropEvent *event);
        bool eventFilter(QObject *obj, QEvent *event) override;
        void initDrag();

        ~MainWindow();

    private:
        Ui::MainWindow *ui;
        QGraphicsScene *scene;
        QMap<QString, Transition> transitionItems;
        QMap<QString, QGraphicsEllipseItem*> stateItems;
        int logCounter = 1;
};

#endif // MAINWINDOW_H
