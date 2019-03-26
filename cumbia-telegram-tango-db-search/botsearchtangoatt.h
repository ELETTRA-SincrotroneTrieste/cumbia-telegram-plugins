#ifndef BOTSEARCHTANGOATT_H
#define BOTSEARCHTANGOATT_H

#include "cubotvolatileoperation.h"

#include <QStringList>
#include <QThread>

class TgAttSearchThread : public QThread
{
    Q_OBJECT
public:
    TgAttSearchThread(QObject *parent, const QString& devname);

    void run();

    QStringList attributes;

    QString errmsg;

private:
    QString m_devname;
};

class BotSearchTangoAtt : public QObject, public CuBotVolatileOperation
{
    Q_OBJECT
public:
    enum Type { Bot_SearchTangoAtt = 0x02 };

    BotSearchTangoAtt(QObject *parent, int chatid);

    virtual ~BotSearchTangoAtt();

    void find(const QString& devname);

    QString getAttByIdx(int idx);

    QString getSourceByIdx(int idx);

    QString devname() const;

    QStringList attributes() const;


signals:
    void attListReady(int chat_id, const QString& device, const QStringList& devs);
    void volatileOperationExpired(int chat_id, const QString& name, const QString& text);

private slots:
    void onSearchFinished();

private:
    QStringList m_attlist;
    QString m_devname;
    int m_chat_id;
    bool d_error;
    QString d_msg;


    // CuBotVolatileOperation interface
public:
    void consume(int type);
    bool disposeWhenOver() const;
    int type() const;
    QString name() const;
    void signalTtlExpired();
};

#endif // BOTSEARCHTANGOATT_H
