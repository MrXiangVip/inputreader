#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../mangmi/MangmiPolicy.h"
#include "../mangmi/MangmiFlinger.h"
#include "../mangmi/InputFilter.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( ui->pushButton, &QPushButton::clicked, this, &MainWindow::handleButtonClick);
    ////  先创建策略线程 收来自客户端的配置投递
    try {
        policyThread= MangmiPolicy::getInstance()->startSocketServerThread();
    }catch(const std::exception& e){
        std::cerr<<e.what()<<std::endl;
    }
////  再启动接收投递 flinger 线程
    try{
        flingerThread= MangmiFlinger::getInstance()->startFlingerThread();
    }catch (const std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
}

void MainWindow::handleButtonClick(){
    std::cout<<"Button Clicked";
    QPushButton *sender = qobject_cast<QPushButton*>(QObject::sender());
    if( sender ==ui->pushButton ){
        RawEvent  event;
        event.deviceId =1;
        event.type=EV_KEY;
        event.code=BUTTON_A;
        event.value=1; //down
        InputFilter::getInstance()->inputRawEvent( event);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        RawEvent  eventUp;
        eventUp.deviceId =1;
        eventUp.type=EV_KEY;
        eventUp.code=BUTTON_A;
        eventUp.value=0; //up
        InputFilter::getInstance()->inputRawEvent( eventUp);

    }else if( sender ==ui->pushButton_2){

    }

}


void MainWindow::closeEvent(QCloseEvent *event) {
    QMainWindow::closeEvent(event);

    MangmiFlinger::getInstance()->stop();
    MangmiPolicy::getInstance()->stop();
//    pthread_join( policyThread, NULL);
//    pthread_join( flingerThread, NULL);
    ALOGD("Close All Thread");
}
MainWindow::~MainWindow()
{
    delete ui;
}

