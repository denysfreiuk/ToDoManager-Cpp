#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void animateIconRotation(QToolBox *toolBox, int pageIndex, bool opening);

private:
    Ui::MainWindow *ui;
};

//class MyObjects : public QObject {
//    Q_OBJECT
//public:
//    explicit MyObjects(QObject *parent = nullptr);
//
//signals:
//    void mySignal(); // власний сигнал
//
//public slots:
//    void mySlot();   // власний слот
//};

#endif // MAINWINDOW_H
