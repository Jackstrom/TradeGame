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
    updateHistory();
}

void MainWindow::updateRanking()
{
    const std::map<unsigned int,TradeGame::Agent*>& agents = manager->getAgents();
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

QString MainWindow::getAssetName(TradeGame::AssetType asset)
{
    switch(asset)
    {
    case TradeGame::SILVER:
        return "Silver";
    case TradeGame::GOLD:
        return "Gold";
    case TradeGame::PLATINUM:
        return "Platinum";
    }
    return "";
}

void MainWindow::updateHistory()
{
    const std::vector<TradeGame::Trade>& history = manager->getHistory();
    std::map<unsigned int,TradeGame::Agent*> agents = manager->getAgents();
    QStringList list;
    for(std::vector<TradeGame::Trade>::const_iterator it=history.begin(); it!=history.end(); it++)
    {
        QString item("(");
        item.append(QString::number(it->seller));
        item.append(") ");
        item.append(QString::fromStdString(agents[it->seller]->getName()));
        item.append(" : ");
        item.append(QString::number(it->bid.sellingVolume));
        item.append(" ");
        item.append(getAssetName(it->bid.sellingType));
        item.append("\t<==>\t");
        item.append(QString::number(it->bid.buyingVolume));
        item.append(" ");
        item.append(getAssetName(it->bid.buyingType));
        item.append(" : ");
        item.append("(");
        item.append(QString::number(it->buyer));
        item.append(") ");
        item.append(QString::fromStdString(agents[it->buyer]->getName()));

        list.push_back(item);
    }
    QStringListModel* model = new QStringListModel(list, this);
    ui->listHistory->setModel(model);
}

bool MainWindow::agentSortPredicate(std::pair<unsigned int,TradeGame::Agent*> a, std::pair<unsigned int,TradeGame::Agent*> b)
{
    int aScore = BlackBoard::calculateAssetValue(a.second->getAssets());

    int bScore = BlackBoard::calculateAssetValue(b.second->getAssets());

    return aScore > bScore;
}
