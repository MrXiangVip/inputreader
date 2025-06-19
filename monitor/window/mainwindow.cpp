#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../mangmi/MangmiPolicy.h"
#include "../mangmi/MangmiFlinger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    ////  先创建策略线程 收来自客户端的配置投递
    pthread_t policyThread;
    try {
        policyThread= MangmiPolicy::getInstance()->startSocketServerThread();
    }catch(const std::exception& e){
        std::cerr<<e.what()<<std::endl;
    }
//
////  再启动接收投递 flinger 线程
    pthread_t flingerThread;
    try{
        flingerThread= MangmiFlinger::getInstance()->startFlingerThread();
    }catch (const std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

