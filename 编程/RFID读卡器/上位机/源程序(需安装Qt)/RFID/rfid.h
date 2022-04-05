#ifndef RFID_H
#define RFID_H

#include <QMainWindow>
#include <QDialog>
#include <QtSerialPort/QSerialPort>      //串口
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QDateTime>      //时间日期获取类
#include <QFileDialog>    //文件操作类

namespace Ui {
class RFID;
}

extern QString regName[64];

class RFID : public QMainWindow
{
    Q_OBJECT

public:
    explicit RFID(QWidget *parent = 0);
    QString get_current_data_time();        //获取系统时间
    int chartoInt(char i);                  //计算一个自己的证书值
    QString Checksum(QString data);
    ~RFID();

private slots:
    void on_check_Button_clicked();

    void on_open_Button_clicked();

    void on_close_Button_clicked();

    void on_readCard_Button_clicked();

    QByteArray hexStringtoByteArray(QString hex);
    void lineEdit_setEnable(bool flag);
    void clear();
    void sendKeyA(int choose);
    void sendData();

    void Read_Data();

    void on_clear_Button_clicked();

    void on_readData_Button_clicked();

    void on_writeData_Button_clicked();

    //保存寄存器操作显示框数据
    void on_saveDate_clicked();

    //清除寄存器操作显示框数据
    void on_clearDate_clicked();
    //初始化按钮（完成对RC522的初始化）
    void on_initRc522_Butten_clicked();
    //询卡按钮（查询当前范围内的卡片信息）
    void on_findCard_Butten_clicked();
    //打开天线
    void on_openMast_Butten_clicked();
    //关闭天线
    void on_closeMast_Butten_clicked();
    void on_runScriptButten_clicked();

    void on_saveScriptButten_clicked();

    void on_openScriptButten_clicked();

    void on_addMoneyButten_clicked();

    void on_subMoneyButten_clicked();

    void on_pushButton_clicked();

private:
    Ui::RFID *ui;
    QSerialPort *serial;
};

#endif // RFID_H
