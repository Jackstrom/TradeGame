#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new BlackBoard(this);
    updateUI();
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

    updateUI();
}

void MainWindow::updateUI()
{/*
    std::map<unsigned int,TradeGame::Agent*> agents = manager->getAgents();
    std::vector<std::pair<unsigned int,TradeGame::Agent*> > vec;
    std::transform(agents.begin(), agents.end(), std::back_inserter, vec.begin());
    QStringList list;
    for(std::map<unsigned int,TradeGame::Agent*>::const_iterator it=agents.begin(); it!=agents.end(); it++)
    {
        list.push_back(QString::number(it->first).append(": ").append(QString::fromStdString(it->second->getName())));
    }
    QStringListModel* model = new QStringListModel(list, this);
    ui->listRanking->setModel(model);*/
}
