#ifndef BOTSEARCHTANGODEV_H
#define BOTSEARCHTANGODEV_H

#include "cubotvolatileoperation.h"
#include <QStringList>
#include <QThread>
#include <cubotplugininterface.h>

class TgDevSearchThread : public QThread
{
    Q_OBJECT
public:
    TgDevSearchThread(QObject *parent, const QString& pattern, const QString& host);

    void run();

    bool error() const;

    QString message() const;

    QStringList devices;

private:
    QString m_pattern, m_host, m_errmsg;
};

class BotSearchTangoDev : public QObject, public CuBotVolatileOperation
{
    Q_OBJECT

public:
    enum Type { Bot_SearchTangoDev = 0 };

    BotSearchTangoDev(QObject *parent, int chat_id);

    virtual ~BotSearchTangoDev();

    void find(const QString& patter, const QString& host);

    QString getDevByIdx(int idx);

    QStringList devices() const;


signals:
    void devListReady(int chat_id, const QStringList& devs, const QString& msg);

    void volatileOperationExpired(int chat_id, const QString& name, const QString& text);

private slots:
    void onSearchFinished();

private:
    QStringList m_devlist;
    int m_chat_id;
    QString m_pattern, m_host;
    bool d_error;
    QString d_msg;


    // CuBotVolatileOperation interface
public:
    void consume(int module_type);
    bool disposeWhenOver() const;
    int type() const;
    QString name() const;
    void signalTtlExpired();
};

#endif // BOTSEARCHTANGODEV_H
