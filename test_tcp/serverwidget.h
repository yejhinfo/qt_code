#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>

namespace Ui {
class ServerWidget;
}

class ServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWidget(QWidget *parent = nullptr);
    ~ServerWidget();

    void sendFile();
    //接收文件
    void receive();

private slots:
    void on_buttonSelect_clicked();

    void on_buttonSend_clicked();

private:
    Ui::ServerWidget *ui;

    QTcpServer  *tcpServer;
    QTcpSocket  *tcpSocket;

    QFile file;     //需要发送的文件对象
    QString fileName;
    int fileSize;
    QTimer timer;   //定时器

    QFile filerec;         //需要接收的文件对象
    QString fileNamerec;
    int fileSizerec;

    int currentSize;    //当前已接收的文件大小

    bool isHead;        //标志位，是否是文件头
};

#endif // SERVERWIDGET_H
