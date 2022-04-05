#include "rfid.h"
#include "ui_rfid.h"

// 全局变量保存寄存器名称
QString regName[64] = {"RFU00","CommandReg","ComIEnReg ","DivlEnReg ","ComIrqReg ","DivIrqReg ","ErrorReg","Status1Reg"
                       ,"Status2Reg","FIFODataReg ","FIFOLevelReg","WaterLevelReg","ControlReg","BitFramingReg","CollReg"
                       ,"RFU0F","RFU10","ModeReg","TxModeReg ","RxModeReg ","TxControlReg","TxAskReg","TxSelReg","RxSelReg"
                       ,"RxThresholdReg ","DemodReg","RFU1A","RFU1B","MifareReg ","RFU1D","RFU1E","SerialSpeedReg "
                       ,"RFU20","CRCResultRegM","CRCResultRegL","RFU23","ModWidthReg ","RFU25","RFCfgReg","GsNReg "
                       ,"CWGsCfgReg","ModGsCfgReg ","TModeReg","TPrescalerReg","TReloadRegH ","TReloadRegL","TCounterValueRegH"
                       ,"TCounterValueRegL","RFU30","TestSel1Reg ","TestSel2Reg ","TestPinEnReg","TestPinValueReg","TestBusReg"
                       ,"AutoTestReg ","VersionReg","AnalogTestReg","TestDAC1Reg ","TestDAC2Reg ","TestADCReg","RFU3C","RFU3D"
                       ,"RFU3E","RFU3F"};
//全局变量
int flag1 = 0,flag2=0,flag3=0,flag4=0,flag5=0,flag6=0,flag7=0,flag8=-1;
int ScriptRunFlag = 0;
int readmanyflag = 0;
int Money=0;
//构造函数：包含基本的初始化和设置
RFID::RFID(QWidget *parent) : QMainWindow(parent),ui(new Ui::RFID)
{
    ui->setupUi(this);
    lineEdit_setEnable(true);
    ui->check_Button->setEnabled(true);
    ui->open_Button->setEnabled(false);
    ui->close_Button->setEnabled(false);
    ui->readCard_Button->setEnabled(false);
    ui->readData_Button->setEnabled(false);
    ui->writeData_Button->setEnabled(false);
    ui->initRc522_Butten->setEnabled(false);
    ui->findCard_Butten->setEnabled(false);
    ui->openMast_Butten->setEnabled(false);
    ui->clearDate->setEnabled(false);
    ui->saveDate->setEnabled(false);
    ui->closeMast_Butten->setEnabled(false);
}
//析构函数
RFID::~RFID()
{
    delete ui;
}

//检测串口
void RFID::on_check_Button_clicked()
{
    int flag = 0;
    //查找可用的串口
    ui->regDisplay->append(tr("\n\n*********检测串口************"));
    ui->regDisplay->append(get_current_data_time());
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            //ui->portBox->count();
            ui->portBox->clear();
            ui->portBox->addItem(serial.portName());
            serial.close();
            //打开打开串口按钮的使能
            ui->open_Button->setEnabled(true);
            flag = 1;
            ui->regDisplay->append(tr("检测到可用串口！！"));
        }
    }
    if(flag == 0){
         QMessageBox::about(this,tr("提示信息"),tr("未检测到串口或者当前串口已被占用！！！"));
         ui->regDisplay->append(tr("无可用串口(可能是当前串口已被占用)"));
    }
}

//打开串口
void RFID::on_open_Button_clicked()
{
    QString portName = ui->portBox->currentText();   //获取串口名
    serial = new QSerialPort;
    //设置串口名
    serial->setPortName(ui->portBox->currentText());
    if(serial->open(QIODevice::ReadWrite)){
        QMessageBox::information(this, tr("打开成功"), tr("已成功打开串口 ") + portName, QMessageBox::Ok);
        ui->close_Button->setEnabled(true);
        ui->open_Button->setEnabled(false);
        ui->readCard_Button->setEnabled(true);
        ui->readData_Button->setEnabled(true);
        ui->writeData_Button->setEnabled(true);
        ui->check_Button->setEnabled(false);
        ui->initRc522_Butten->setEnabled(true);
        ui->findCard_Butten->setEnabled(true);
        ui->openMast_Butten->setEnabled(true);
        ui->closeMast_Butten->setEnabled(true);
        ui->clearDate->setEnabled(true);
        ui->saveDate->setEnabled(true);
    }
    else
    {
        serial->close();
        delete serial;
        QMessageBox::critical(this, tr("打开失败"),  portName + tr("不存在"), QMessageBox::Ok);
        return;
    }

    //设置波特率
    serial->setBaudRate(ui->baudRateBox->currentText().toInt());
    //设置数据位数
    switch(ui->dataBitsBox->currentIndex())
    {
        case 5: serial->setDataBits(QSerialPort::Data5); break;
        case 6: serial->setDataBits(QSerialPort::Data6); break;
        case 7: serial->setDataBits(QSerialPort::Data7); break;
        case 8: serial->setDataBits(QSerialPort::Data8); break;
        default: break;
    }
    //设置奇偶校验
    switch(ui->parityBox->currentIndex())
    {
        case 0: serial->setParity(QSerialPort::NoParity); break;
        default: break;
     }
     //设置停止位
     switch(ui->stopBitsBox->currentIndex())
     {
        case 1: serial->setStopBits(QSerialPort::OneStop); break;
        case 2: serial->setStopBits(QSerialPort::TwoStop); break;
        default: break;
     }
     //设置流控制
     serial->setFlowControl(QSerialPort::NoFlowControl);
     //连接信号槽
     connect(serial, SIGNAL(readyRead()), this, SLOT(Read_Data()));
}

//关闭串口
void RFID::on_close_Button_clicked()
{
    QString portName = ui->portBox->currentText();   //获取串口名
    serial->close();
    delete serial;
    ui->close_Button->setEnabled(false);
    ui->readCard_Button->setEnabled(false);
    ui->readData_Button->setEnabled(false);
    ui->writeData_Button->setEnabled(false);
    ui->open_Button->setEnabled(true);
    ui->check_Button->setEnabled(true);
    ui->initRc522_Butten->setEnabled(false);
    ui->findCard_Butten->setEnabled(false);
    ui->openMast_Butten->setEnabled(false);
    ui->closeMast_Butten->setEnabled(false);
    QMessageBox::information(this, tr("关闭成功"), tr("已成功关闭串口 ") + portName, QMessageBox::Ok);
}

//读取数据
void RFID::Read_Data()
{
    int i=0;
    QByteArray buf = serial->readAll();
    if(!buf.isEmpty()){
        ScriptRunFlag = 1;
        if(buf.size() == 2){  //返回的是操作提示
            if(buf[i+1].operator ==(0xfd)){
                ui->regDisplay->append(tr("\n\n***写入数据成功*********"));
                ui->regDisplay->append(tr("数据如下："));
                if(flag8==0){
                    ui->regDisplay->append("0x"+this->ui->wData1->text());
                    ui->regDisplay->append("0x"+this->ui->wData2->text());
                    ui->regDisplay->append("0x"+this->ui->wData3->text());
                    ui->regDisplay->append("0x"+this->ui->wData4->text());
                    ui->regDisplay->append("0x"+this->ui->wData5->text());
                    ui->regDisplay->append("0x"+this->ui->wData6->text());
                    ui->regDisplay->append("0x"+this->ui->wData7->text());
                    ui->regDisplay->append("0x"+this->ui->wData8->text());
                    ui->regDisplay->append("0x"+this->ui->wData9->text());
                    ui->regDisplay->append("0x"+this->ui->wData10->text());
                    ui->regDisplay->append("0x"+this->ui->wData11->text());
                    ui->regDisplay->append("0x"+this->ui->wData12->text());
                    ui->regDisplay->append("0x"+this->ui->wData13->text());
                    ui->regDisplay->append("0x"+this->ui->wData14->text());
                    ui->regDisplay->append("0x"+this->ui->wData15->text());
                    ui->regDisplay->append("0x"+this->ui->wData16->text());
                    ui->regDisplay->append(tr("写数据完成："));
                    QMessageBox::information(this, tr("写卡成功"), tr("写卡成功！！！ "), QMessageBox::Ok);
                    flag8 = 1;
                }
            }
            ui->regDisplay->append(tr("\n\n***********操作提示************\n"));
            ui->regDisplay->append(tr("时间：")+(get_current_data_time()));
            if(buf[1].operator ==(0x00)){
                ui->regDisplay->append(tr("读卡失败\n"));
                QMessageBox::information(this, tr("验证失败"), tr("密码错误！！！ "), QMessageBox::Ok);
            }else if(buf[1].operator ==(0x01)){
                ui->regDisplay->append(tr("读数据块错误 \n"));
                QMessageBox::information(this, tr("读卡失败"), tr("读数据失败！！！ "), QMessageBox::Ok);
            }else if(buf[1].operator ==(0x02)){
                ui->regDisplay->append(tr("地址错误读卡失败,请输入合理的地址快\n"));
                QMessageBox::information(this, tr("读卡失败"), tr("块地址错误！！！ "), QMessageBox::Ok);
            }else if(buf[1].operator ==(0x04)){
                ui->regDisplay->append(tr("写入失败此地址为数据保护位无法写入\n"));
                QMessageBox::information(this, tr("写卡失败"), tr("写卡失败！！！ "), QMessageBox::Ok);
            }
        }
        else{
//            ui->regDisplay->append(tr(" 长度 ：")+ QString::number(buf.size()));
            for(i=0;i<buf.size();i++){
               // ui->regDisplay->append("(0x"+QString("%1").arg(buf[i]&0xff,2,16,QLatin1Char('0'))+")");
                if(buf[i].operator ==(0x40)&& buf[i+1].operator <= (0x37)&&buf[i+1].operator >=(0x00)){
                    if(flag1 == 0){
                        ui->regDisplay->append(tr("\n\n**RC522初始化开始,寄存器操作如下**"));
                        ui->regDisplay->append(tr("时间：")+ get_current_data_time());
                    }
                    if(buf[i+2].operator ==(0x48)){
                        ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                               +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                    }
                    if(buf[i+2].operator ==(0x47)){
                        ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                               +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                    }
                    if(flag1 == 28){
                        ui->regDisplay->append(tr("*****RC522初始化完成*******\n"));
                        flag1 = 0;
                    }
                    flag1 += 4;
                }
                if(buf[i].operator ==(0x41)&& buf[i+1].operator <= (0x37)&&buf[i+1].operator >=(0x00)){
                    if(flag2 == 0){
                        ui->regDisplay->append(tr("\n\n*****开始询卡,寄存器操作如下*****"));
                        ui->regDisplay->append(tr("时间：")+ get_current_data_time());
                     }
                    if(buf[i+2].operator ==(0x48)){
                        ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                               +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                    }
                    if(buf[i+2].operator ==(0x47)){
                        ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                               +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                    }
                    flag2 += 4;
                    if( flag2 == 112){
                        ui->regDisplay->append(tr("*********询卡完成完成********\n"));
                        flag2 = 0;
                    }
                }
                if(buf[i].operator ==(0x42)&& buf[i+1].operator <= (0x37)&&buf[i+1].operator >=(0x00)){
                    if(flag3 == 0){
                        ui->regDisplay->append(tr("\n\n*******打开天线,寄存器操作如下*********"));
                        ui->regDisplay->append(tr("时间：")+ get_current_data_time());
                    }
                    if(buf[i+2].operator ==(0x48)){
                        ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                               +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                    }
                    if(buf[i+2].operator ==(0x47)){
                        ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                               +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                    }
                    flag3 += 4;
                    if( flag3 == 4){
                        ui->regDisplay->append(tr("***********天线已打开**************\n"));
                        flag3 = 0;
                    }

                }
                if(buf[i].operator ==(0x43)&& buf[i+1].operator <= (0x37)&&buf[i+1].operator >=(0x00)){
                    if(flag4 == 0){
                        ui->regDisplay->append(tr("\n\n*******关闭天线,寄存器操作如下*********"));
                        ui->regDisplay->append(tr("时间：")+ get_current_data_time());
                    }
                    flag4 += 4;
                    if(buf[i+2].operator ==(0x48)){
                        ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                               +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                    }
                    if(buf[i+2].operator ==(0x47)){
                        ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                               +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                    }
                    if(flag4 == 8){
                        ui->regDisplay->append(tr("********天线已关闭*********\n"));
                        flag4 = 0;
                    }
                }
                if(buf[i].operator ==(0xff)&& buf[i+1].operator ==(0x49)){
                    if(flag5 == 0){
                        ui->regDisplay->append(tr("\n\n***读取卡号成功*********"));
                        ui->regDisplay->append(tr("时间：")+ get_current_data_time());
                        ui->regDisplay->append(tr("卡号如下："));
                    }
                    flag5 += 7;
                    for(int s=0;s<buf[i+2];s++){
                        ui->regDisplay->append("0x"+QString("%1").arg(buf[i+3+s]&0xff,2,16,QLatin1Char('0')));
                    }
                    if(flag5 == 7){
                        ui->regDisplay->append(tr("*********读取卡号完毕*********\n"));
                        flag5 = 0;
                    }
                    ui->testl->setText(buf.mid(i+3,4).toHex());
                    ui->card1->setText(buf.mid(i+3,1).toHex());
                    ui->card2->setText(buf.mid(i+4,1).toHex());
                    ui->card3->setText(buf.mid(i+5,1).toHex());
                    ui->card4->setText(buf.mid(i+6,1).toHex());
                }
                if(buf[i].operator ==(0xfe)){
                    if(flag6 == 0){
                        ui->regDisplay->append(tr("\n\n*******读取数据,寄存器操作如下*********"));
                        ui->regDisplay->append(tr("时间：")+ get_current_data_time());
                    }
                    if( buf[i+1].operator <= (0x37)&&buf[i+1].operator >=(0x00)){
                        flag6 += 4;
                        if(buf[i+2].operator ==(0x48)){
                            ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                    +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                                   +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                        }
                        if(buf[i+2].operator ==(0x47)){
                            ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                    +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                                   +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                        }
                    }
                    if(buf[i+1].operator ==(0x49)){
                        flag6 += 16;
                        ui->regDisplay->append(tr("\n\n***读取数据成功*********"));
                        ui->regDisplay->append(tr("时间：")+ get_current_data_time());
                        ui->regDisplay->append(tr("数据如下："));
                        for(int s=0;s<buf[i+2];s++){
                            ui->regDisplay->append("0x"+QString("%1").arg(buf[i+3+s]&0xff,2,16,QLatin1Char('0')));
                        }
                        ui->rData1->setText(buf.mid(i+3,1).toHex());ui->rData2->setText(buf.mid(i+4,1).toHex());
                        ui->rData3->setText(buf.mid(i+5,1).toHex());ui->rData4->setText(buf.mid(i+6,1).toHex());
                        ui->rData5->setText(buf.mid(i+7,1).toHex());ui->rData6->setText(buf.mid(i+8,1).toHex());
                        ui->rData7->setText(buf.mid(i+9,1).toHex());ui->rData8->setText(buf.mid(i+10,1).toHex());
                        ui->rData9->setText(buf.mid(i+11,1).toHex());ui->rData10->setText(buf.mid(i+12,1).toHex());
                        ui->rData11->setText(buf.mid(i+13,1).toHex());ui->rData12->setText(buf.mid(i+14,1).toHex());
                        ui->rData13->setText(buf.mid(i+15,1).toHex());ui->rData14->setText(buf.mid(i+16,1).toHex());
                        ui->rData15->setText(buf.mid(i+17,1).toHex());ui->rData16->setText(buf.mid(i+18,1).toHex());
                    }

                    if(flag6 == 339){
                        ui->regDisplay->append(tr("********读数据完成*********\n"));
                        flag6 = 0;
                    }
                }
                if(buf[i].operator ==(0xfd)){
                    if(flag7 == 0){
                        ui->regDisplay->append(tr("\n\n****写数据,寄存器操作如下****"));
                        ui->regDisplay->append(tr("时间：")+ get_current_data_time());
                    }
                    if( buf[i+1].operator <= (0x37)&&buf[i+1].operator >=(0x00)){
                        flag7 += 4;
                        if(buf[i+2].operator ==(0x48)){
                            ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                    +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                                   +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                        }
                        if(buf[i+2].operator ==(0x47)){
                            ui->regDisplay->append(tr("寄存器:")+regName[buf[i+1]&0xff]
                                    +"(0x"+QString("%1").arg(buf[i+1]&0xff,2,16,QLatin1Char('0'))+")"
                                   +tr(" 写入值:")+"0x"+QString("%1").arg(buf[i+3]&0xff,2,16,QLatin1Char('0')));
                        }
                    }
                    if(buf[i+1].operator ==(0xfd)){
                        ui->regDisplay->append(tr("\n\n***写入数据成功*********"));
                        ui->regDisplay->append(tr("数据如下："));
                        if(flag8==0){
                            ui->regDisplay->append("0x"+this->ui->wData1->text());
                            ui->regDisplay->append("0x"+this->ui->wData2->text());
                            ui->regDisplay->append("0x"+this->ui->wData3->text());
                            ui->regDisplay->append("0x"+this->ui->wData4->text());
                            ui->regDisplay->append("0x"+this->ui->wData5->text());
                            ui->regDisplay->append("0x"+this->ui->wData6->text());
                            ui->regDisplay->append("0x"+this->ui->wData7->text());
                            ui->regDisplay->append("0x"+this->ui->wData8->text());
                            ui->regDisplay->append("0x"+this->ui->wData9->text());
                            ui->regDisplay->append("0x"+this->ui->wData10->text());
                            ui->regDisplay->append("0x"+this->ui->wData11->text());
                            ui->regDisplay->append("0x"+this->ui->wData12->text());
                            ui->regDisplay->append("0x"+this->ui->wData13->text());
                            ui->regDisplay->append("0x"+this->ui->wData14->text());
                            ui->regDisplay->append("0x"+this->ui->wData15->text());
                            ui->regDisplay->append("0x"+this->ui->wData16->text());
                            QMessageBox::information(this, tr("写卡成功"), tr("写卡成功！！！ "), QMessageBox::Ok);
                            flag8 = 1;
                        }
                    }

                    if(flag7 == 506){
                        ui->regDisplay->append(tr("********写数据完成*********\n"));
                        flag7 = 0;
                    }
                }
            }
        }
    }
    buf.clear();
}

//读卡号
void RFID::on_readCard_Button_clicked()
{
    serial->flush();
    //发送数据  头为 “FF“表示读取卡号
    QByteArray send_data=this->hexStringtoByteArray("ff");
    serial->write(send_data);
}

//读一块数据
void RFID::on_readData_Button_clicked()
{
    clear();   //在接收新数据前先清空
    serial->flush();
    //发送数据  头为 “Fe“表示读取一块数据
    QByteArray send_data=this->hexStringtoByteArray("fe");
    serial->write(send_data);  //qDebug() <<send_data;

    send_data = this->hexStringtoByteArray(ui->address1->text());  //获取数据  地址
    serial->write(send_data);  //qDebug() <<send_data;

    sendKeyA(1);  //获取数据  6个密钥

}

//设置使能，有些窗口不能编辑
void RFID::lineEdit_setEnable(bool flag)
{
    ui->card1->setReadOnly(flag);ui->card2->setReadOnly(flag);
    ui->card3->setReadOnly(flag);ui->card4->setReadOnly(flag);
    ui->rData1->setReadOnly(flag);ui->rData2->setReadOnly(flag);
    ui->rData3->setReadOnly(flag);ui->rData4->setReadOnly(flag);
    ui->rData5->setReadOnly(flag);ui->rData6->setReadOnly(flag);
    ui->rData7->setReadOnly(flag);ui->rData8->setReadOnly(flag);
    ui->rData9->setReadOnly(flag);ui->rData10->setReadOnly(flag);
    ui->rData11->setReadOnly(flag);ui->rData12->setReadOnly(flag);
    ui->rData13->setReadOnly(flag);ui->rData14->setReadOnly(flag);
    ui->rData15->setReadOnly(flag);ui->rData16->setReadOnly(flag);
}

//转换
QByteArray RFID::hexStringtoByteArray(QString hex)
{
    QByteArray ret;
    hex=hex.trimmed();
    QStringList sl=hex.split(" ");
    foreach(QString s,sl)
    {
        if(!s.isEmpty())
            ret.append((char)s.toInt(0,16)&0xFF);
    }
    return ret;
}

int RFID::chartoInt(char i){
    int a=0;
    for(int j = 7;j>=0;--j){
        int b = 1;
        if(j==0&&(i&0x80)==1) a+=b;
        else if(i&0x80){
            int s=j;
            while(s--){
                b*=2;
            }
            a+=b;
            b=1;
        }
        i <<= 1;
    }
    return a;
}

//清空框中数据
void RFID::clear()
{
    ui->rData1->clear();ui->rData2->clear();
    ui->rData3->clear();ui->rData4->clear();
    ui->rData5->clear();ui->rData6->clear();
    ui->rData7->clear();ui->rData8->clear();
    ui->rData9->clear();ui->rData10->clear();
    ui->rData11->clear();ui->rData12->clear();
    ui->rData13->clear();ui->rData14->clear();
    ui->rData15->clear();ui->rData16->clear();
}

//清空
void RFID::on_clear_Button_clicked()
{
    ui->testl->clear();
    ui->card1->clear();
    ui->card2->clear();
    ui->card3->clear();
    ui->card4->clear();
}

//发送秘钥
void RFID::sendKeyA(int choose)
{
    QByteArray send_data;
    if(choose == 1){   //1表示读数据时获取密钥
        send_data = this->hexStringtoByteArray(ui->keyA1->text());  //获取数据  6个密钥
        serial->write(send_data);  //qDebug() <<send_data;
        send_data = this->hexStringtoByteArray(ui->keyA2->text());
        serial->write(send_data);
        send_data = this->hexStringtoByteArray(ui->keyA3->text());
        serial->write(send_data);
        send_data = this->hexStringtoByteArray(ui->keyA4->text());
        serial->write(send_data);
        send_data = this->hexStringtoByteArray(ui->keyA5->text());
        serial->write(send_data);
        send_data = this->hexStringtoByteArray(ui->keyA6->text());
        serial->write(send_data);
    }
    else if(choose == 2){
        send_data = this->hexStringtoByteArray(ui->keyA1_2->text());  //获取数据  6个秘钥
        serial->write(send_data);  //qDebug() <<send_data;
        send_data = this->hexStringtoByteArray(ui->keyA2_2->text());
        serial->write(send_data);
        send_data = this->hexStringtoByteArray(ui->keyA3_2->text());
        serial->write(send_data);
        send_data = this->hexStringtoByteArray(ui->keyA4_2->text());
        serial->write(send_data);
        send_data = this->hexStringtoByteArray(ui->keyA5_2->text());
        serial->write(send_data);
        send_data = this->hexStringtoByteArray(ui->keyA6_2->text());
        serial->write(send_data);
    }
}

//发送要写的数据
void RFID::sendData()
{
    QByteArray send_data;//获取要写的数据
    send_data = this->hexStringtoByteArray(ui->wData1->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData2->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData3->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData4->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData5->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData6->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData7->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData8->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData9->text());  serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData10->text()); serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData11->text()); serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData12->text()); serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData13->text()); serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData14->text()); serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData15->text()); serial->write(send_data);
    send_data = this->hexStringtoByteArray(ui->wData16->text()); serial->write(send_data);
}

//写一块数据
void RFID::on_writeData_Button_clicked()
{
    flag8 = 0;
    serial->flush();
    //发送数据  头为 “Fd“表示写一块数据
    QByteArray send_data=this->hexStringtoByteArray("fd");
    serial->write(send_data);  //qDebug() <<send_data;

    send_data = this->hexStringtoByteArray(ui->address2->text());  //获取数据  地址
    serial->write(send_data);  //qDebug() <<send_data;
    sendKeyA(2);   //获取数据  6个密钥
    sendData();  //获取要写的数据  16字节
}

//获取系统当前时间
QString RFID::get_current_data_time()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    return current_date;
}

//寄存器操作记录数据保存
void RFID::on_saveDate_clicked()
{
    QMessageBox::about(this,tr("提示信息"),tr("窗口数据保存成功！！！"));
}

//寄存器操作记录数据清除
void RFID::on_clearDate_clicked()
{
    ui->regDisplay->clear();
    QMessageBox::about(this,tr("提示信息"),tr("窗口数据清除成功！！！"));
}
//初始化按钮
void RFID::on_initRc522_Butten_clicked()
{
    serial->flush();
    //发送数据  头为 “Fd“表示写一块数据
    QByteArray send_data=this->hexStringtoByteArray("40");
    serial->write(send_data);  //qDebug() <<send_data;
}
// 询卡按钮
void RFID::on_findCard_Butten_clicked()
{
    serial->flush();
    QByteArray send_data = this->hexStringtoByteArray("41");
    serial->write(send_data);  //qDebug() <<send_data;
}
// 打开天线
void RFID::on_openMast_Butten_clicked()
{
    serial->flush();
    QByteArray send_data = this->hexStringtoByteArray("42");
    serial->write(send_data);  //qDebug() <<send_data;
}
// 关闭天线
void RFID::on_closeMast_Butten_clicked()
{
    serial->flush();
    QByteArray send_data = this->hexStringtoByteArray("43");
    serial->write(send_data);  //qDebug() <<send_data;
}

void RFID::on_runScriptButten_clicked()
{

    QString text = ui->showScript->toPlainText();
    if(text.length()==0){
        QMessageBox::information(this, tr("脚本操作"), tr("脚本为空请先编辑！！！ "), QMessageBox::Ok);
        return;
    }
    QStringList cmd_split1=text.split( QRegExp("end"), QString::SkipEmptyParts );
    for(int j=0;j<cmd_split1.size();j++){
        QStringList cmd_split=cmd_split1.at(j).split( QRegExp("\n"), QString::SkipEmptyParts );
        QString longoutData="";
        for(int i=0;i<cmd_split.size();i++)
        {

            QString str=cmd_split.at(i);
            str=str.trimmed();
            longoutData+=str+" ";
        }
        longoutData=longoutData.trimmed();
        QByteArray outData = hexStringtoByteArray(longoutData);
        serial->flush();
        serial->write(outData);
    }
}

QString RFID::Checksum(QString data)
{
    QStringList data_split=data.split(" ");
    int sum=0;
    for(int i=0;i<data_split.size();i++)
    {
        QString tmpstr="0x"+data_split[i];
        bool ok;
        sum+=tmpstr.toInt(&ok,16);
    }
    QString rst=QString::number(sum,16).toUpper().right(2);
    return rst;
}
//保存脚本
void RFID::on_saveScriptButten_clicked()
{
    QFile file("test.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QString text = ui->showScript->toPlainText();
    if(text.length()==0){
        QMessageBox::information(this, tr("脚本操作"), tr("脚本框为空请先编辑！！！ "), QMessageBox::Ok);
        return;
    }
    text.replace(QString("\n"), QString("\r\n"));
    QTextStream infile(&file);
    infile << text;
    QMessageBox::information(this, tr("脚本操作"), tr("脚本保存成功！！！ "), QMessageBox::Ok);
    file.close();
}
//打开脚本
void RFID::on_openScriptButten_clicked()
{
    QDir dir;
    QString fileName = QFileDialog::getOpenFileName(this, tr("open file"), dir.absolutePath(),  tr("ScriptFile(*.txt)"));
    QFile file(fileName);
    if (!file.exists()){return;}
//    ui->nameScript->clear();
//    ui->nameScript->setText(fileName);
    file.open(QFile::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString line;
//    ui->C_function->clear();
//    ui->C_function->addItem("请选择函数");
    ui->showScript->clear();
    do
    {
        line=in.readLine();
//        if (line.left(8)=="function")
//        {
//            ui->C_function->addItem(line.right(line.length()-9));

//        }
        ui->showScript->appendPlainText(line);;
    }
    while(!line.isNull());
    QMessageBox::information(this, tr("脚本操作"), tr("脚本打开成功！！！ "), QMessageBox::Ok);
    file.close();
}
//向卡中充值
void RFID::on_addMoneyButten_clicked()
{
    Money+=(ui->showMoney->text()).toInt();
    QMessageBox::information(this, tr("充值操作"), tr("充值成功！！！ "), QMessageBox::Ok);
}
//从卡扣款
void RFID::on_subMoneyButten_clicked()
{
    Money-=(ui->showMoney->text()).toInt();
    QMessageBox::information(this, tr("充值操作"), tr("扣款完成！！！ "), QMessageBox::Ok);
}

void RFID::on_pushButton_clicked()
{
    ui->showMoney->setText(QString::number(Money));
}
