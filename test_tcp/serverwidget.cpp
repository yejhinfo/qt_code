#include "serverwidget.h"
#include "ui_serverwidget.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>

ServerWidget::ServerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWidget)
{
    ui->setupUi(this);

    setWindowTitle("服务器端口:8888");
    fileSize = 0;
    isHead = true;
    fileSizerec = 0;
    currentSize = 0;

    //未连接时设置为不可用
    ui->buttonSend->setEnabled(false);
    ui->buttonSelect->setEnabled(false);

    tcpServer = new QTcpServer(this);

    //监听端口号为8888的所有IP地址
    tcpServer->listen(QHostAddress::Any, 8888);

    //如果与客户端成功连接，会触发QTcpServer::newConnection()信号
    connect(tcpServer, &QTcpServer::newConnection, [=](){
        //取出通信套接字
        tcpSocket = tcpServer->nextPendingConnection();

        QString ip = tcpSocket->peerAddress().toString();
        quint16 port = tcpSocket->peerPort();

        ui->textBrowser->setText(QString("[%1:%2]: 成功与客户端连接").arg(ip).arg(port));

        //连接时设置“选择文件”为可用
        ui->buttonSelect->setEnabled(true);

       /* connect(tcpSocket, &QTcpSocket::readyRead, [=](){
            //取客户端的信息
            QByteArray buf = tcpSocket->readAll();
            if(QString(buf) == "file done")
            {
                //文件发送完毕
                ui->textBrowser->append("文件发送完毕 ");
                ui->buttonSend->setEnabled(false);

                //关闭文件且断开连接
                file.close();
                tcpSocket->disconnectFromHost();
                tcpSocket->close();
            }
        });*/
    });

    //定时器处理
    connect(&timer, &QTimer::timeout, [=](){

        timer.stop();

        //发送文件
        sendFile();
    });
    //服务器write()会触发客户端QTcpSocket::readyRead()信号
    connect(tcpSocket, &QTcpSocket::readyRead, [=](){
        receive();

    });

}

ServerWidget::~ServerWidget()
{
    delete ui;
}

//选择文件
void ServerWidget::on_buttonSelect_clicked()
{
    //初始化文件属性
    QString filePath = QFileDialog::getOpenFileName(this, "Open", "../");
    if(!filePath.isEmpty())
    {
        QFileInfo info(filePath);
        fileName = info.fileName();
        fileSize = info.size();
        //qDebug()<<fileName<<fileSize;

        file.setFileName(filePath);

        if(file.open(QIODevice::ReadOnly))
        {
            ui->textBrowser->append(QString("FileName:%1 \n\nFileSize:%2 KB").arg(fileName).arg(fileSize/1024));
            //连接时设置“选择文件”为不可用，“发送文件”可用
            ui->buttonSend->setEnabled(true);
            ui->buttonSelect->setEnabled(false);
        }
        else {
            qDebug()<<"打开文件失败";
        }
    }
    else
    {
        qDebug()<<"文件路径出错";
    }
}

//发送文件
void ServerWidget::on_buttonSend_clicked()
{
    //1.发送文件头  格式：文件名###文件大小
    QString fileHead = QString("%1###%2").arg(fileName).arg(fileSize);
    qint16 len = tcpSocket->write(fileHead.toUtf8());
    if(len > 0)
    {
        //让文件头与文件内容之间发送间隔相隔10ms
        timer.start(10);
    }
    else {
        //发送头部文件失败
        qDebug()<<"发送头部文件失败 ";
        file.close();
    }
}

void ServerWidget::sendFile()
{
    ui->textBrowser->append("正在发送文件...");
    qint64 len = 0;
    do{
        //每次发送4KB
        char buf[4*1024] = {0};
        len = 0;

        len = file.read(buf, sizeof(buf));
        len = tcpSocket->write(buf, len);
    }while (len > 0);
}

void ServerWidget::receive()
{
    //取出接收的内容
    QByteArray content = tcpSocket->readAll();

    if(true == isHead)          //文件头处理
    {
        isHead = false;
        fileNamerec = QString(content).section("###", 0, 0);
        fileSizerec = QString(content).section("###", 1, 1).toInt();

        //重置已接收大小
        currentSize = 0;



        filerec.setFileName(fileNamerec);
        //以只写方式打开文件
        if(!filerec.open(QIODevice::WriteOnly))
        {
            qDebug()<<"以只写方式打开文件失败";
            tcpSocket->disconnectFromHost();
            tcpSocket->close();
            return ;
        }
        QString str = QString("接收的文件: [%1: %2 KB]").arg(fileNamerec).arg(fileSizerec/1024);
        ui->textBrowser->append(str);

    }
    else                        //文件内容处理
    {
        qint64 len = filerec.write(content);
        //每次写入的大小累加
        if(len > 0)
        {
            currentSize += len;
        }

        if(currentSize == fileSizerec)   //文件传输完成的条件
        {
            ui->textBrowser->append("文件接收完成");
            tcpSocket->write("file done");
            QMessageBox::information(this, "完成", "文件传输完成");

            //传输完成,关闭文件且关闭连接
            filerec.close();
            tcpSocket->disconnectFromHost();
            tcpSocket->close();
        }
    }
}
