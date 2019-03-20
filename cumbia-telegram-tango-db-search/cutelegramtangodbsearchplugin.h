#ifndef CUTELEGRAMTANGODBSEARCHPLUGIN_H
#define CUTELEGRAMTANGODBSEARCHPLUGIN_H

#include <QObject>
#include <cubotvolatileoperation.h>
#include <cubotplugininterface.h>

class CuTelegramTangoDbSearchPlugin : public QObject, public CuBotVolatileOperation, CuBotPluginInterface
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

    // VolatileOperation interface
public:
    void consume(int moduletyp);
    int type() const;
    QString name() const;
    void signalTtlExpired();
    bool disposeWhenOver() const;

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
    void m_onTgDevListSearchReady(int chat_id, const QStringList &devs);
    void m_onTgAttListSearchReady(int chat_id, const QString &devname, const QStringList &atts);

signals:
    void volatileOperationExpired(int chat_id, const QString& name, const QString& text);

private:

    QString m_errorVolatileSequence_msg(const QStringList &seq) const;

    QString tg_devSearchList_msg(const QStringList& devs) const;
    QString tg_attSearchList_msg(const QString &devname, const QStringList& atts) const;

    SearchMode m_mode;
    QString m_devnam, m_msg;
    int m_index;
    TBotMsg m_tbotmsg;
    bool m_err;
};

#endif // CUTELEGRAMTANGODBSEARCHPLUGIN_H
