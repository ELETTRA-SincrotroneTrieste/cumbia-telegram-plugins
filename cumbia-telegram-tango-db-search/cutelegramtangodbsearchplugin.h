#ifndef CUTELEGRAMTANGODBSEARCHPLUGIN_H
#define CUTELEGRAMTANGODBSEARCHPLUGIN_H

#include <QObject>
#include <cubotvolatileoperation.h>
#include <cubotplugininterface.h>

class CuTelegramTangoDbSearchPluginPrivate;

class CuTelegramTangoDbSearchPlugin : public QObject, public CuBotPluginInterface
{
    Q_OBJECT
public:
    enum Type { TangoDbSearch = 30 };

    enum SearchMode { Invalid = 0, DevSearch, AttSearch, ReadFromAttList };

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.CumbiaTelegram.CuTelegramTangoDbSearchPlugin" FILE "cumbia-telegram-tango-db-search.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(CuBotPluginInterface)


    CuTelegramTangoDbSearchPlugin();
    ~CuTelegramTangoDbSearchPlugin();

    void reset();

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

private slots:
    void m_onTgDevListSearchReady(int chat_id, const QStringList &devs, const QString& message);
    void m_onTgAttListSearchReady(int chat_id, const QString &devname, const QStringList &atts, const QString& msg);
    void onVolatileOperationExpired(int,const QString& , const QString& );

private:

    QString m_errorVolatileSequence_msg(const QStringList &seq) const;

    QString tg_devSearchList_msg(const QStringList& devs, const QString &message) const;
    QString tg_attSearchList_msg(const QString &devname, const QStringList& atts, const QString &message) const;

    CuTelegramTangoDbSearchPluginPrivate *d;

    // CuBotModule interface
public:
    int type() const;
    QString name() const;
};

#endif // CUTELEGRAMTANGODBSEARCHPLUGIN_H
