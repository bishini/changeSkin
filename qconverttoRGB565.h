#ifndef QCONVERTTORGB565_H
#define QCONVERTTORGB565_H

#include "QString"
#include "QDebug"
#include "QImage"
#include "QFile"

class QConvertToRGB565
{
public:
    QConvertToRGB565(const QString imagePath,const QString binPath);
public:
    QString binPath_;
private:
    void ConvertToRGB565OutPut();
private:
    QString imagePath_;
};

#endif // QCONVERTTORGB565_H
