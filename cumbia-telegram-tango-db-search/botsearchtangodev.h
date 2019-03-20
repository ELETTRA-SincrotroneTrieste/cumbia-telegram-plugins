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

class BotSearchTangoDev : public QObject, public CuBotVolatileOperation, CuBotPluginInterface
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.CumbiaTelegram.BotSearchTangoDev" FILE "cumbia-telegram-tango-db-search-dev.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(CuBotPluginInterface)

public:
    enum Type { DevSearch = 0 };

    BotSearchTangoDev(QObject *parent  = nullptr);

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

    // VolatileOperation interface
public:
    void consume(int moduletyp);
    int type() const;
    QString name() const;
    void signalTtlExpired();

    // CuBotModule interface
public:
    QString description() const;
    QString help() const;
    int decode(const TBotMsg &msg);
    bool process();
    bool error() const;
    QString message() const;
    bool isVolatileOperation() const;

    // CuBotPluginInterface interface
public:
    void init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf);
};

#endif // BOTSEARCHTANGODEV_H
