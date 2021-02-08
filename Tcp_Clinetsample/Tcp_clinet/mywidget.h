#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class myWidget; }
QT_END_NAMESPACE

class myWidget : public QWidget
{
    Q_OBJECT

public:
    myWidget(QWidget *parent = nullptr);
    ~myWidget();


private slots:
    void on_ButtonConnect_clicked();

    void on_ButtonSend_clicked();

    void on_ButtonClose_clicked();

private:
    Ui::myWidget *ui;
    QTcpSocket* tcpSocket;

      QFile filerec;         //需要接收的文件对象
      QString fileNamerec;
      int fileSizerec;

      int currentSize;    //当前已接收的文件大小

      bool isHead;        //标志位，是否是文件头
};
#endif // MYWIDGET_H
