#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new BlackBoard(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnStartRound()
{
    unsigned int iterations = 100;
    for(unsigned int i=0; i<iterations; i++)
        manager->runMarket();
    manager->penalize();
}
