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
    TgDevSearchThread(QObject *parent, const QString& pattern);

    void run();

    QStringList devices;

private:
    QString m_pattern;
};

class BotSearchTangoDev : public QObject
{
    Q_OBJECT

public:
    enum Type { DevSearch = 0 };

    BotSearchTangoDev(QObject *parent, int chat_id);

    virtual ~BotSearchTangoDev();

    void find(const QString& pattern);

    QString getDevByIdx(int idx);

    QStringList devices() const;


signals:
    void devListReady(int chat_id, const QStringList& devs);

    void volatileOperationExpired(int chat_id, const QString& name, const QString& text);

private slots:
    void onSearchFinished();

private:
    QStringList m_devlist;
    int m_chat_id;
    QString m_pattern;
    bool d_error;
    QString d_msg;

};

#endif // BOTSEARCHTANGODEV_H
