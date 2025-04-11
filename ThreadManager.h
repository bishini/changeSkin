#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <functional>
#include <QWaitCondition>
#include <QMutexLocker>
#include <QString>
#include <QDebug>


//*******************************************************************
//
//   考虑改成线程池的方式来管理零散的线程(目前是可以在任何地方创建线程进行任务)
//
//*******************************************************************
class ThreadManager : public QObject
{
    Q_OBJECT
public:
    explicit
    //构造函数,接受一个任务函数（线程执行的函数）和可选的线程名称
    ThreadManager(std::function<void()> task , const QString& name = "WorkerThread")
        :task_(task), name_(name), stopFlag_(false), isRunning_(false){}

    ThreadManager(std::function<void()> task ,const int &delay, const QString& name = "WorkerThread")
        :task_(task), name_(name), stopFlag_(false), isRunning_(false), delay_(delay){}

    bool start()
    {
        //std::lock_guard<std::mutex>lock(mutex_);
        QMutexLocker locker(&mutex_);               //此锁是局部变量，当进入此函数时就会锁定互斥量，当函数返回时他就会被销毁
        if (isRunning_)
        {
            //线程已经启动则退出
            return false;
        }
        stopFlag_ = false;
        workerThread_ = new QThread(this);//std::thread(&ThreadManager::run, this);

        connect(workerThread_,&QThread::started,this,&ThreadManager::run);
        connect(workerThread_,&QThread::finished,this,&ThreadManager::onFinished);

        moveToThread(workerThread_);
        workerThread_->start();

        isRunning_ = true;

        //至此线程已启动
        return true;
    }

    bool stop()
    {
        //std::lock_guard<std::mutex> lock(mutex_);
        QMutexLocker locker(&mutex_);
        if (!isRunning_)
        {
            //没有运行线程
            return false;
        }
        stopFlag_ = true;

        if (workerThread_ && workerThread_->isRunning())
        {
            workerThread_->quit();          //退出线程循环
            workerThread_->wait();          //等待退出线程循环
        }

        {
            QMutexLocker locker(&mutex_);
            isRunning_ = false;
            //waitcondition.wait(&mutex_);
        }
        return true;
    }

    bool isRunning() const
    {
        //std::lock_guard<std::mutex> lock(mutex_);
        QMutexLocker locker(&mutex_);

        return isRunning_;
    }

    ~ThreadManager()
    {
        stop();
    }

    QString ThreadName(){return name_;}
private slots:
    void run()
    {
        while (!stopFlag_)
        {
            //void(QIvfTable::*thisFunction_)();//执行线程任务
            task_();
            emit complete();
            QThread::msleep(delay_);//防止跑的太快，可以设置一个延时，默认1秒
        }
    }

    void onFinished()
    {
        stop();
        qDebug() << "线程名称：" << this->ThreadName() << " 执行完毕";
    }
signals:
    void complete();                //线程完成一轮循环可以发出一个完成信号
private:
    std::function<void()> task_;    //线程任务函数
    QString name_;                  //线程名称
    bool stopFlag_;                 //线程停止标签
    bool isRunning_;				//线程启动标签
    QThread* workerThread_;         //工作的线程
    mutable QMutex mutex_;          //互斥锁
//	std::condition_variable cv_;
    QWaitCondition waitcondition;
    int delay_ = 1000;
};


#endif // THREADMANAGER_H
