#ifndef QSIZECONVERTER_H
#define QSIZECONVERTER_H
#include <QtCore>
#include <QDebug>

class QSizeConverter
{
public:
    QSizeConverter();

    static QSize ConvertFromString(QString size)
    {
        QStringList items = size.split(",");
        if(items.count() ==2)
        {
            return QSize(items[0].toInt(), items[1].toInt());
        }
        else
        {
            return QSize(25,25);
        }
    }

    static QString ConvertToString(QSize size)
    {
        return  QString("%1, %2").arg(size.width()).arg(size.height());
    }

};

class QPointConverter
{
public:
    QPointConverter();

    static QPoint ConvertFromString(QString point)
    {
        QStringList items = point.split(",");
        if(items.count() ==2)
        {
            return QPoint(items[0].toInt(), items[1].toInt());
        }
        else
        {
            return QPoint(0,0);
        }
    }

    static QString ConvertToString(QPoint point)
    {
        return  QString("%1, %2").arg(point.x()).arg(point.y());
    }

};

class QFontConverter
{
public:
    QFontConverter();

    static QFont ConvertFromString(QString font)
    {
        QStringList items = font.split(",");
        if(items.count() ==4)
        {
            QString family = items[0];
            int pointSize = items[1].toInt();
            int weight = items[2].toInt();
            bool italic = (bool)items[3].toInt();

            return QFont(family, pointSize, weight, italic);
        }
        else
        {
            return QFont();
        }
    }

    static QString ConvertToString(QFont font)
    {
        return  QString("%1, %2, %3, %4").arg(font.family()).arg(font.pointSize()).arg(font.weight()).arg(font.italic());
    }

};

#endif // QSIZECONVERTER_H
