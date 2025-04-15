#include "changeski.h"

ChangeSki *dlg = nullptr;

typedef bool (*ReadSerialPort)(int);
typedef void (*WriteSerialPort)(const char *,int) ;

WriteSerialPort WriteSerialPort_;                  //写串口函数指针
ReadSerialPort ReadSerialPort_;                    //读串口函数指针

ChangeSki::ChangeSki(int i,QWidget *parent)
    :QWidget(parent),m_i(i),threadManger(nullptr),ui(new Ui::Form)
{
    this->setWindowTitle("串口皮肤编辑器");
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

void ChangeSki::showWindow()
{
    this->show();
    //QMessageBox::information(this,"测试",QString::number(m_i));

    //connect(this,&ChangeSki::startThread,this,&ChangeSki::sandSerial);
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        threadManger = new ThreadManager(std::bind(&ChangeSki::sandSerial,this));
        ui->progressBar->setValue(0);
        threadManger->start();//线程启动
    });  //当按钮点击启动线程

    connect(threadManger,&ThreadManager::complete,this,[=](){
        threadManger->stop();//线程停止
    });

    connect(this,&ChangeSki::upDateUI,this,[=](){
        ui->progressBar->setValue(percent);
    });

    connect(ui->JUMP1,&QPushButton::clicked,this,[=](){
        char buf[20];
        sprintf_s(buf, "JUMP(0);\r\n");
        WriteSerialPort_(buf,strlen(buf));//发送串口文字
    });

    connect(ui->JUMP2,&QPushButton::clicked,this,[=](){
        char buf[20];
        sprintf_s(buf, "JUMP(1);\r\n");
        WriteSerialPort_(buf,strlen(buf));//发送串口文字
    });

    connect(ui->JUMP3,&QPushButton::clicked,this,[=](){
        char buf[20];
        sprintf_s(buf, "JUMP(2);\r\n");
        WriteSerialPort_(buf,strlen(buf));//发送串口文字
    });

    connect(ui->CommandBTN,&QPushButton::clicked,this,[=](){
        QString Commandstr( ui->CommectEdit->text());
        char buf[30];
        sprintf_s(buf,QString(Commandstr+";\r\n").toUtf8().data());
        WriteSerialPort_(buf,strlen(buf));
    });
}

ChangeSki::~ChangeSki()
{
    //DestroyChangeSki();
}


void ChangeSki::sandSerial()
{
    QString str = ui->lineEdit->text();
    //QByteArray byteArray = str.toUtf8();
    //const char *buf = byteArray.constData();
    //WriteSerialPort_(buf);//发送串口文字
    QFile image(str);
    if(!image.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr,"错误","图片打开失败");
        return;
    }

    QConvertToRGB565 asd(str,"E:/Misaka/build-changeSki-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/debug/output123.bin");
    QFile binFile("E:/Misaka/build-changeSki-Desktop_Qt_5_12_12_MinGW_64_bit-Debug/debug/output.bin");
    binFile.open(QIODevice::ReadOnly);       //设置文件只读模式
    //QByteArray buffer = binFile.readAll();   //读取全部数据
    int totalSize = binFile.size();         //文件总大小
    int sentBytes = 0;//发送大小
    //首先发送指令擦除寄存器内存
    char fileInformation[50];
    sprintf_s(fileInformation,"FS_DLOAD(%d);\r\n",totalSize);
    //sprintf_s(fileInformation,"JUMP(1);\r\n",binFile.size());
     WriteSerialPort_(fileInformation,strlen(fileInformation));//发送串口文字
     //QMessageBox::information(NULL,"提示","字符总大小："+QString::number(binFile.size())+"文件大小为："+QString::number(binFile.size()));
     if(ReadSerialPort_(100))
     {
         qDebug()<<"is ready to sand image bin file data\n";
     }
    //好像分为3个部分、目前的思路就是放在一个循环里面当发送的大小等于文件大小就是发送完成，退出循环，最好放在线程里面防止ui卡顿
    //1.检测是否发送完成，当发送的大小大于等于文件大小时就是发送完成
    percent = 0;
    const int chunkSize = 16;
    char buffer[chunkSize];
    while(!binFile.atEnd())
    {
        qint64 bytesRead = binFile.read(buffer,chunkSize);
        if(bytesRead > 0)
        {
            WriteSerialPort_(buffer,static_cast<int>(bytesRead));
            sentBytes += chunkSize;

            //更新进度条
            percent = (sentBytes * 100) / binFile.size();
            emit upDateUI();
            QThread::msleep(10);
        }
    }
    if(ReadSerialPort_(100))
    {
       qDebug() << "send file data succsessful!";
    }       //这里试一下发送完成后，给客户端发送一个结束命令符

    threadManger->stop();
}

int CreateChangeSki(int i)
{
    int argc = 0;  // argc 和 argv 是 Qt 应用程序的参数
    char **argv = nullptr;

    // 创建 QApplication 对象（Qt 应用程序的主事件循环）
    QApplication app(argc, argv);

    dlg = new ChangeSki(i);

    dlg->showWindow();

    //加载DLL
    HMODULE hDll = LoadLibrary(L"./plugins/conncetedSerialPortLib.dll");
    if (!hDll)
    {
        QMessageBox::information(nullptr,"测试", "动态加载链接库失败");
        return -1;
    }
    WriteSerialPort_ = (WriteSerialPort)GetProcAddress(hDll, "WriteSerialPort");
    if (WriteSerialPort_ == NULL) {
        QMessageBox::information(nullptr,"测试", "获取 写 函数地址失败！");
        FreeLibrary(hDll);
        return -1;
    }

    ReadSerialPort_ = (ReadSerialPort)GetProcAddress(hDll, "ReadSerialPort");
    if (WriteSerialPort_ == NULL) {
        QMessageBox::information(nullptr,"测试", "获取 读 函数地址失败！");
        FreeLibrary(hDll);
        return -1;
    }

    // 进入 Qt 事件循环
    return app.exec();  // 事件循环会阻塞直到窗口关闭
}

void DestroyChangeSki()
{
    delete dlg;
}
