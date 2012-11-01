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

    void updateUI();
    void updateRanking();
    void updateHistory();
    static QString getAssetName(TradeGame::AssetType asset);

    static bool agentSortPredicate(std::pair<unsigned int,TradeGame::Agent*> a, std::pair<unsigned int,TradeGame::Agent*> b);

private slots:
    void btnStartRound();
    void btnRestart();
};

#endif // MAINWINDOW_H
