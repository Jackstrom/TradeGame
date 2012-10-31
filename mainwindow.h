#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "blackboard.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    BlackBoard * manager;
};

#endif // MAINWINDOW_H
