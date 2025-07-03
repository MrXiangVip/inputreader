#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../mangmi/MangmiPolicy.h"
#include "../mangmi/MangmiEvent.h"
#include "../mangmi/InputFilter.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( ui->pushButton, &QPushButton::clicked, this, &MainWindow::handleButtonClick);
    connect( ui->pushButton_9, &QPushButton::clicked, this, &MainWindow::handleButtonClick);
    ////  先创建策略线程 收来自客户端的配置投递
    try {
        policyThread= MangmiPolicy::getInstance()->startSocketServerThread();
    }catch(const std::exception& e){
        std::cerr<<e.what()<<std::endl;
    }
////  再启动接收投递 flinger 线程
    try{
        flingerThread= MangmiEvent::getInstance()->startEventThread();
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

    }else if( sender ==ui->pushButton_9){
        RawEvent  event;
        event.deviceId =1;
        event.type=EV_ABS;
        event.code=ABS_X;
        event.value=0;
        InputFilter::getInstance()->inputRawEvent( event);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        RawEvent  eventMove;
        eventMove.deviceId =1;
        eventMove.type=EV_ABS;
        eventMove.code=ABS_X;
        eventMove.value=100; //up
        InputFilter::getInstance()->inputRawEvent( eventMove);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        RawEvent  eventUp;
        eventUp.deviceId =1;
        eventUp.type=EV_ABS;
        eventUp.code=ABS_X;
        eventUp.value=0; //up
        InputFilter::getInstance()->inputRawEvent( eventUp);
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }

}


void MainWindow::closeEvent(QCloseEvent *event) {
    QMainWindow::closeEvent(event);

    MangmiEvent::getInstance()->stop();
    MangmiPolicy::getInstance()->stop();
//    pthread_join( policyThread, NULL);
//    pthread_join( flingerThread, NULL);
    ALOGD("Close All Thread");
}
MainWindow::~MainWindow()
{
    delete ui;
}

