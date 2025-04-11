#include "qconverttoRGB565.h"

QConvertToRGB565::QConvertToRGB565(const QString imagePath, const QString binPath):
    imagePath_(imagePath),binPath_(binPath)
{
    ConvertToRGB565OutPut();
}

void QConvertToRGB565::ConvertToRGB565OutPut()
{
    QImage image(imagePath_);

       if (image.isNull()) {
           qDebug() << "Failed to load image.";
           return;
       }

       // 转换为 RGB888 格式
       image = image.convertToFormat(QImage::Format_RGB888);

       QFile file(binPath_);
       if (!file.open(QIODevice::WriteOnly)) {          //设置文件只写模式
           qDebug() << "Failed to open output file.";
           return;
       }

       // 获取图像尺寸
       int width = image.width();
       int height = image.height();

       for (int y = 0; y < height; ++y) {
           const uchar *line = image.scanLine(y);
           for (int x = 0; x < width; ++x) {
               uchar r = line[x * 3 + 0];
               uchar g = line[x * 3 + 1];
               uchar b = line[x * 3 + 2];

               // 转换为 RGB565
               quint16 rgb565 = ((r & 0xF8) << 8) |
                                ((g & 0xFC) << 3) |
                                ((b & 0xF8) >> 3);

               // 写入小端格式    //这里就是写入裸数据
               file.putChar(rgb565 & 0xFF);
               file.putChar((rgb565 >> 8) & 0xFF);
           }
       }

       file.close();
}
