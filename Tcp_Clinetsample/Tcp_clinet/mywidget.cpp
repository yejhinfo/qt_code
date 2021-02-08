#include "mywidget.h"
#include "ui_mywidget.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
myWidget::myWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::myWidget)
{
    ui->setupUi(this);
    tcpSocket = NULL;
    tcpSocket= new QTcpSocket(this);
    setWindowTitle("Clinet");

    connect(tcpSocket,&QTcpSocket::connected,[=]()
    {
        ui->textEditRead->setText("connect success");
    }
    );

    connect(tcpSocket,&QTcpSocket::readyRead,[=]()
    {
      // QByteArray array = tcpSocket->readAll();
      // ui->textEditRead->append(array);
        QByteArray buf = tcpSocket->readAll();

        if(true==isHead){
            isHead = false;
            fileNamerec = QString(buf).section("##",0,0);
            fileSizerec = QString(buf).section("##",1,1).toInt();
            currentSize = 0;

            filerec.setFileName(fileNamerec);

            bool isOk= filerec.open(QIODevice::WriteOnly);
            if(false==isOk){
                qDebug()<<"open error";
            }

        }else{

            qint64 len = filerec.write(buf);
            currentSize += len;

            if(currentSize==fileSizerec){
                filerec.close();
                QMessageBox::information(this,"success","success");
                tcpSocket->disconnectFromHost();
                tcpSocket->close();
            }

        }

    }
    );
}

myWidget::~myWidget()
{
    delete ui;
}


void myWidget::on_ButtonConnect_clicked()
{
    //获取服务器ip端口
    QString ip = ui->lineEditip->text();
    qint16 port = ui->lineEditPort->text().toInt();

    tcpSocket->connectToHost(QHostAddress(ip),port);
}

void myWidget::on_ButtonSend_clicked()
{
    QString str = ui->textEditWrite->toPlainText();

    tcpSocket->write(str.toUtf8().data());

}

void myWidget::on_ButtonClose_clicked()
{
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
}

