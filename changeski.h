#ifndef CHANGESKI_H
#define CHANGESKI_H

#include "changeSki_global.h"
#include "ui_ChangeSkin.h"
#include "qconverttoRGB565.h"
#include "QFile"
#include "QByteArray"
#include "QtGlobal"
#include "ThreadManager.h"
#include "functional"

class CHANGESKI_EXPORT ChangeSki : public QWidget
{
    Q_OBJECT
public:
    explicit ChangeSki(int i,QWidget *parent = nullptr);

    void showWindow();

    ~ChangeSki();
private:
    void sandSerial();
private:
    int m_i;
    Ui::Form *ui;
    ThreadManager *threadManger;
    int percent;
signals:
    void startThread();
    void upDateUI();
};

extern "C" CHANGESKI_EXPORT int CreateChangeSki(int i);
extern "C" CHANGESKI_EXPORT void DestroyChangeSki();
#endif // CHANGESKI_H
