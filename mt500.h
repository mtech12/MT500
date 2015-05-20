#ifndef MT500_H
#define MT500_H

#include <QMainWindow>
#include <QFile>
#include <QTextBrowser>
#include <QTextStream>
#include <QTimer>
#include <Q3Socket>
#include <Q3SocketDevice>
#include <QStringList>
#include <QDebug>
#include <QDateTime>
#include <QHash>
#include <QtAddOnSerialPort/serialport.h>
#include <QtAddOnSerialPort/serialportinfo.h>
#include <QThread>
#include <QTcpSocket>

QT_USE_NAMESPACE_SERIALPORT

namespace Ui {
    class MT500;
}

struct ipStruct {
    QString ip;
    int port;
    QString dataType;
};

class MT500 : public QMainWindow {
    Q_OBJECT
public:
    MT500(QWidget *parent = 0);
    ~MT500();

protected:
    void changeEvent(QEvent *e);
    int countFile(QString);
    void getIPs();
    QByteArray encode(QString message);
    void sendRaw(QString);
    void sendBase(QString, bool);
    void sendIFLOWS(QByteArray);
    void getConfig();
    void getFipsCounts();
    void setupRS232(QString, QString);
    QString decode(QByteArray);
    void testConnection();
    void checkResetFIPS();
    void sendRawStrtoIflows(QString);
    void sendFips();
    QStringList sortFips();
    bool inFilter(int node, int gid);

public slots:
    void getData();
    void sendHB();
    void getFips();
    void readData();
    void ipError(int);
    void resetPorts();
    void clrBuf();

private slots:
    void on_addButton_clicked();
    void on_delButton_clicked();
    void on_testButton_clicked();
    void on_clrButton_clicked();

    void on_reconfigureButton_clicked();

    void on_runModeComboBox_currentIndexChanged(int index);

private:
    Ui::MT500 *ui;
    int initialCount, heartbeatInterval, msgCount, fipsInterval, byteCount,boxGID,recordCnt, byteCnt, ipRetries, socketCtr, cloudCnt;
    qint32 outBaud, inBaud;
    QTimer *pollTimer, *hbTimer, *fipsTimer, *resetTimer, *validMsgTimer;
    QTcpSocket *sockets[20], *test;
    ipStruct ipArray[6];
    QStringList getFiles;
    QString fipsNo, ipConfig, fipsDir, COMin, COMout, currentIP, progVer, HWaddr;
    QHash<QString, QMap<QDateTime, QString> > fipsCount;
    SerialPort *inPort, *outPort;
    QByteArray msg, ba;
    bool initial, log, m_processingFips;
    QHash<int, QDateTime> fipsFilter; //GID, Last Record
    QHash<int, QString> sendList; //Index, Time:Raw Data
    QHash<QString, QString> filterList; //Node, GID
};

class SleeperThread : public QThread
{
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};

class CountFipsThread : public QThread
{
    Q_OBJECT

    public:
        CountFipsThread(QObject *parent = 0) {
            m_getFiles = QStringList ();
            m_fipsDir = "";
            m_processingFips = false;
        }
        ~CountFipsThread();

        void countFips ( QStringList getFiles, QString fipsDir, bool &processingFips ) {
            m_getFiles = getFiles;
            m_fipsDir = fipsDir;
            m_processingFips = processingFips;
        }

    protected:

        void run() Q_DECL_OVERRIDE {
            m_processingFips = true;
            for(int i = 0; i < getFiles.size(); i++) {
                QString line;
                QMap<QDateTime, QString> tempSorter;
                QString filename = fipsDir+getFiles.at(i).trimmed()+".dat";
                emit threadLog(QString("Getting newest record in %1").arg(filename));
                QFile file(filename);
                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    while (!in.atEnd()) {
                        line = in.readLine();
                        QStringList fields = line.split(",");
                        QString dateTime = fields.at(0);
                        QDateTime newDate = QDateTime::fromString(dateTime.trimmed(), "MM/dd/yyyy HH:mm:ss");
                        tempSorter.insertMulti(newDate, line);
                    }
                    QMap<QDateTime, QString>::iterator it;
                    QDateTime newestRecord;
                    int ctr = 0;
                    for(it = tempSorter.begin(); it != tempSorter.end(); ++it) {
                        ctr++;
                        if(ctr == tempSorter.size()) {
                            newestRecord = it.key();
                            QMap<QDateTime, QString> tempMap;
                            tempMap.insert(newestRecord, it.value());
                            emit insertIntoFipsCount(getFiles.at(i).trimmed(), tempMap);
                        }
                    }
                    emit threadLog(QString("Newest Record for %1: %2").arg(filename).arg(newestRecord.toString()));
                }
                else {
                    emit threadLog(QString("Error opening file %1").arg(filename));
                    QMap<QDateTime, QString> tempMap;
                    tempMap.insert(QDateTime::fromString("01/01/1970 00:00:00", "MM/dd/yyyy HH:mm:ss"), "Empty Placeholder");
                    emit insertIntoFipsCount(getFiles.at(i).trimmed(), tempMap);
                }
            }
            m_processingFips = false;
        }

    signals:

        void threadLog(QString);
        void insertIntoFipsCount(QString, QMap<QDateTime, QString>;

    private:
        QStringList m_getFiles;
        QString m_fipsDir;
        bool m_processingFips;
};

#endif // MT500_H
