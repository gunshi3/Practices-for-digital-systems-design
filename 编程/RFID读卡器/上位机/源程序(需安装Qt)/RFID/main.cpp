#include "rfid.h"
#include <QApplication>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RFID w;
    QTextStream cout(stdout,  QIODevice::WriteOnly);
    cout<<w.get_current_data_time();
    w.show();
    return a.exec();
}
