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
    updateRanking();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnStartRound()
{
    manager->setR(ui->spinR->value());
    int iterations = ui->spinIterations->value();
    for(int i=0; i<iterations; i++)
        manager->runMarket();
    manager->penalize();

    updateUI();
}

void MainWindow::btnRestart()
{
    manager->restart();
    updateRanking();
}

void MainWindow::updateUI()
{
    updateRanking();
}

void MainWindow::updateRanking()
{
    std::map<unsigned int,TradeGame::Agent*> agents = manager->getAgents();
    std::vector<std::pair<unsigned int,TradeGame::Agent*> > vec;
    for(std::map<unsigned int,TradeGame::Agent*>::const_iterator it=agents.begin(); it!=agents.end(); it++)
    {
        vec.push_back(*it);
    }
    std::sort(vec.begin(), vec.end(), &MainWindow::agentSortPredicate);
    QStringList list;
    for(std::vector<std::pair<unsigned int,TradeGame::Agent*> >::const_iterator it=vec.begin(); it!=vec.end(); it++)
    {
        TradeGame::Assets assets = it->second->getAssets();
        QString item = QString::number(it->first);
        item.append(": ");
        item.append(QString::fromStdString(it->second->getName()));
        item.append(": S=");
        item.append(QString::number(assets.silver));
        item.append(", G=");
        item.append(QString::number(assets.gold));
        item.append(", P=");
        item.append(QString::number(assets.platinum));
        item.append("  \t- Total score: ");
        item.append(QString::number(BlackBoard::calculateAssetValue(assets)));
        list.push_back(item);
    }
    QStringListModel* model = new QStringListModel(list, this);
    ui->listRanking->setModel(model);
}

bool MainWindow::agentSortPredicate(std::pair<unsigned int,TradeGame::Agent*> a, std::pair<unsigned int,TradeGame::Agent*> b)
{
    int aScore = BlackBoard::calculateAssetValue(a.second->getAssets());

    int bScore = BlackBoard::calculateAssetValue(b.second->getAssets());

    return aScore > bScore;
}
