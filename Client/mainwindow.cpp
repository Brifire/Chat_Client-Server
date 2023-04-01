#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket= new QTcpSocket(this);
    connect(socket,SIGNAL(readyRead()),this,SLOT(slotReadyRead()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(deleteLater()));
    nextBlockSize=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_5_15);
    out<<quint16(0)<<str;
    out.device()->seek(0);
    out<<quint16(Data.size()-sizeof(quint16));
    socket->write(Data);
}


void MainWindow::on_pushButton_clicked()
{
    socket->connectToHost("127.0.0.1",2323);
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);
    if(in.status()==QDataStream::Ok)
    {
//       qDebug()<<"lets go";
//       QString str;
//       in>>str;
//       ui->textEdit->append(str);
        for(;;)
        {
            if(nextBlockSize==0)
            {
             if(socket->bytesAvailable()<2)
             {
                 break;
             }
             in>>nextBlockSize;
            }
            if(socket->bytesAvailable()<nextBlockSize)
            {
                break;
            }
            QString str;
            in>>str;
            nextBlockSize=0;
            ui->textEdit->append(str);


        }

    }
   else
    {
        ui->textEdit->append("ERROR reading");
        qDebug()<<"Error";
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    sendToServer(ui->lineEdit->text());
}


void MainWindow::on_lineEdit_returnPressed()
{
    sendToServer(ui->lineEdit->text());
}

