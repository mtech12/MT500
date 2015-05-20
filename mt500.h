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
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

QT_USE_NAMESPACE_SERIALPORT

namespace Ui {
class MT500;
}

struct ipStruct {
    QString ip;
    int port;
    QString dataType;
};

class CountFipsThread : public QThread
{
    Q_OBJECT

public:
    CountFipsThread(QObject *parent = 0) : QThread(parent) {
        m_getFiles = QStringList ();
        m_fipsDir = "";
        m_processingFips = false;
        m_abort = false;
    }
    ~CountFipsThread() {
        m_mutex.lock ();
        m_abort = true;
        m_condition.wakeOne ();
        m_mutex.unlock ();
    }

    void countFips (QStringList getFiles, QString fipsDir, bool &processingFips) {
        m_getFiles = getFiles;
        m_fipsDir = fipsDir;
        m_processingFips = processingFips;
        m_abort = false;

        if (!isRunning ()) {
            start(HighPriority);
        }
        else {
            m_condition.wakeOne ();
        }
    }

protected:

    void run() {
        forever {
            if(m_abort) break;
            m_mutex.lock ();
            m_processingFips = true;
            m_mutex.unlock ();
            for(int i = 0; i < m_getFiles.size(); i++) {
                QString line;
                QMap<QDateTime, QString> tempSorter;
                QString filename = m_fipsDir+m_getFiles.at(i).trimmed()+".dat";
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
                            emit insertIntoFipsCount(m_getFiles.at(i).trimmed(), newestRecord, it.value());
                        }
                    }
                    emit threadLog(QString("Newest Record for %1: %2").arg(filename).arg(newestRecord.toString()));
                }
                else {
                    emit threadLog(QString("Error opening file %1").arg(filename));
                    emit insertIntoFipsCount(m_getFiles.at(i).trimmed(), QDateTime::fromString("01/01/1970 00:00:00", "MM/dd/yyyy HH:mm:ss"), "Empty Placeholder");
                }
            }
            m_mutex.lock ();
            m_processingFips = false;
            m_condition.wait (&m_mutex);
            m_mutex.unlock ();
        }
    }

signals:

    void threadLog(QString);
    void insertIntoFipsCount (QString key, QDateTime dateTime, QString record);

private:
    QStringList m_getFiles;
    QString m_fipsDir;
    bool m_processingFips, m_abort;
    QHash<QString, QMap<QDateTime, QString> > m_fipsCount;
    QMutex m_mutex;
    QWaitCondition m_condition;
};

class GetFipsThread : public QThread
{
    Q_OBJECT

public:
    GetFipsThread(QObject *parent = 0) : QThread(parent) {
        m_abort = false;
    }
    ~GetFipsThread() {
        m_mutex.lock ();
        m_abort = true;
        m_condition.wakeOne ();
        m_mutex.unlock ();
    }

    void getFips (QStringList getFiles, QString fipsDir, QHash<QString, QMap<DateTime, QString> > fipsCount, QHash<int, QDateTime> fipsFilter, QHash<QString, QString> filterList, bool &processingFips) {
        m_abort = false;
        m_processingFips = processingFips;
        m_getFiles = getFiles;
        m_fipsDir = fipsDir;
        m_fipsCount = fipsCount;
        m_fipsFilter = fipsFilter;
        m_filterList - filterList;

        if (!isRunning ()) {
            start(HighPriority);
        }
        else {
            m_condition.wakeOne ();
        }
    }

protected:

    void run() {
        forever {
            if(m_abort) break;
            if (!m_processingFips) {
                m_mutex.lock ();
                m_processingFips = true;
                m_mutex.unlock ();
                int listCounter = 0;
                emit threadLog("Getting fips files...");
                if(m_getFiles.size() > 0) {
                    for(int i = 0; i < m_getFiles.size(); i++) {
                        QString line;
                        QString filename = m_fipsDir+m_getFiles.at(i).trimmed()+".dat";
                        QFile file(filename);
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QMap<QDateTime, QString> tempSorter;
                            emit threadLog(QString("Reading file %1").arg(filename));
                            QTextStream in(&file);
                            while (!in.atEnd()) {
                                line = in.readLine();
                                QStringList fields2 = line.split(",");
                                QString dateTime = fields2.at(0);
                                QDateTime newDate = QDateTime::fromString(dateTime.trimmed(), "MM/dd/yyyy HH:mm:ss");
                                tempSorter.insertMulti(newDate, line);
                            }
                            file.close();
                            //Loop over sorted file and check for shit
                            QMap<QDateTime, QString>::iterator it;
                            for(it = tempSorter.begin(); it != tempSorter.end(); ++it) {
                                QStringList recordFields = it.value().split(",");
                                QDateTime dateTime = it.key();
                                QString record = it.value();
                                QMap<QDateTime, QString> recordInfo = m_fipsCount.value(m_getFiles.at(i));
                                QMap<QDateTime, QString>::iterator ri = recordInfo.begin();
                                QDateTime oldTime = ri.key();
                                QString oldRecord = ri.value();
                                if((dateTime >= oldTime) && (record != oldRecord)) {   // If new record
                                    emit threadLog(QString("New record: %1").arg(record));
                                    emit insertIntoFipsCount(m_getFiles.at(i).trimmed(), dateTime, record);
                                    int gid = recordFields.at(1).trimmed().toInt();
                                    int node = recordFields.at(4).trimmed().toInt();
                                    if(inFilter(node, gid)) {
                                        emit threadLog(QString("Record passed: %1 (%2)").arg(record).arg(m_getFiles.at(i).trimmed()));
                                        emit appendToCloud("<font color=\"green\">" + record + "</font>");
                                        emit incrementCloudCount ();
                                        emit clearCloud ();
                                        if(!m_fipsFilter.contains(gid)) {
                                            emit insertIntoFipsFilter(gid, dateTime);
                                            emit insertIntoSendList(listCounter, dateTime.toString()+"$"+recordFields.at(3).trimmed());
                                            listCounter++;
                                        }
                                        else {
                                            QDateTime prevTime = m_fipsFilter.value(gid); //Get previous time value
                                            if(dateTime > prevTime.addSecs(60)) { //Only concerned about it if enough time has passed
                                                emit insertIntoFipsFilter(gid, dateTime);
                                                emit insertIntoSendList(listCounter, dateTime.toString()+"$"+recordFields.at(3).trimmed());//add to pass list
                                                listCounter++;
                                            }
                                        }
                                    }
                                    else {
                                        emit threadLog(QString("Record NOT passed: %1 (%2)").arg(record).arg(m_getFiles.at(i).trimmed()));
                                        emit appendToCloud("<font color=\"red\">" + record + "</font>");
                                        emit incrementCloudCount ();
                                        emit clearCloud ();
                                    }
                                }
                            }
                        }
                        else emit threadLog(QString("Error opening file %1").arg(filename));
                    }
                    emit sendFips();//Process and send all data in send list
                }
                m_mutex.lock ();
                m_processingFips = false;
                m_mutex.unlock();
            }
            else {
                emit threadLog("Already processing fips!");
            }
            m_mutex.lock ();
            m_condition.wait (&m_mutex);
            m_mutex.unlock ();
        }
    }

signals:

    void threadLog(QString);
    void insertIntoFipsCount(QString, QDateTime, QString);
    void appendToCloud(QString);
    void incrementCloudCount ();
    void clearCloud ();
    void insertIntoFipsFilter(int, QDateTime);
    void insertIntoSendList(int, QString);
    void sendFips();

private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_processingFips, m_abort;
    QStringList m_getFiles;
    QString m_fipsDir;
    QHash<QString, QMap<QDateTime, QString> > m_fipsCount;
    QHash<int, QDateTime> m_fipsFilter;
    QHash<QString, QString> m_filterList;

    bool inFilter(int node, int gid)
    {

        if (m_filterList.size() == 0) return true;

        bool rValue = false;

        QList<QString> wildCard = m_filterList.values("*");
        if(wildCard.contains(QString::number(gid)) || wildCard.contains("*")) rValue = true;

        if(!rValue) {
            QList<QString> vals = m_filterList.values(QString::number(node));
            if(vals.contains(QString::number(gid)) || vals.contains("*")) rValue = true;
        }

        return rValue;
    }
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

    void threadLog(QString);
    void insertIntoFipsCount (QString key, QDateTime dateTime, QString record);
    void appendToCloud(QString);
    void incrementCloudCount ();
    void clearCloud ();
    void insertIntoFipsFilter(int, QDateTime);
    void insertIntoSendList(int, QString);
    void act_sendFips();

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
    CountFipsThread countFipsThread;
    GetFipsThread getFipsThread;
};

class SleeperThread : public QThread
{
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};

#endif // MT500_H
