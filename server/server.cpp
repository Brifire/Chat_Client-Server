#include "server.h"
#include <QDataStream>

Server::Server()
{
    if(this->listen(QHostAddress::Any,2323))
    {
        qDebug()<<" start ";
    }
    else
    {
       qDebug()<<" error ";
    }
    nextBlockSize=0;

}

void Server::SendToClient(QString str)
{
 Data.clear();
 QDataStream out(&Data,QIODevice::WriteOnly);
 out.setVersion(QDataStream::Version::Qt_5_15);
 out<<quint16(0)<<str;
 out.device()->seek(0);
 out<<quint16(Data.size()-sizeof(quint16));
 //socket->write(Data);
 for(int i=0;i< Sockets.size();++i)
 {
     Sockets[i]->write(Data);
 }
}

void Server::incomingConnection(qintptr SocketDiscriptor)
{
    socket=new QTcpSocket;
    socket->setSocketDescriptor(SocketDiscriptor);
    connect(socket,&QTcpSocket::readyRead,this,&Server::slotReadyRead);
    connect(socket,&QTcpSocket::disconnected,socket,&QTcpSocket::deleteLater);
    Sockets.push_back(socket);
    qDebug()<<" client connected "<< SocketDiscriptor;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Version::Qt_5_15);
    if(in.status()==QDataStream::Ok)
    {
        qDebug()<<"read...";
//        QString str;
//        in>>str;
//        qDebug()<<str;
//        SendToClient(str);
        for(;;)
        {
            if(nextBlockSize==0)
            {
                qDebug()<<"nextBlockSize = 0";
             if(socket->bytesAvailable()<2)
             {
                 qDebug()<<"Data<2 ";
                 break;
             }
             in>>nextBlockSize;
             qDebug()<<"nextBlocksize: "<< nextBlockSize;
            }
            if(socket->bytesAvailable()<nextBlockSize)
            {
                qDebug()<<"Data not full: "<< nextBlockSize;
                break;
            }
            QString str;
            in>>str;
            nextBlockSize=0;
            qDebug()<<str;
            SendToClient(str);
            break;


        }
    }
     else
    {
        qDebug()<<"datastream error";
    }
}
