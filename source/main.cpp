#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //This was used to force XML Attributes to maintain order
    //Found here http://geidav.wordpress.com/2013/02/27/deterministic-attribute-order-in-xml-documents-in-qt-5/
    //qputenv("QT_HASH_SEED", "12345");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
