#ifndef CUTELEGRAMHISTORYPLUGIN_H
#define CUTELEGRAMHISTORYPLUGIN_H

#include <QStringList>
#include <cubotplugininterface.h>

class CuTgHistoryMsgDecoder {
public:
    enum Type { Undefined = 0, ReadHistory, MonitorHistory, AlertHistory };

    Type decode(const TBotMsg &msg);

    QString toHistoryTableType(CuTgHistoryMsgDecoder::Type t) const;
private:
    Type m_type;
};

class CuTelegramHistoryPluginPrivate;

class CuTelegramHistoryPlugin : public QObject, public CuBotPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CuBotPluginInterface)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.CumbiaTelegram.CuTelegramHistoryPlugin" FILE "cumbia-telegram-history.json")
#endif // QT_VERSION >= 0x050000

public:
    enum Type { Bot_HistoryPlugin = 15 };

    CuTelegramHistoryPlugin(QObject *parent = nullptr);

    virtual ~CuTelegramHistoryPlugin();

    void reset();


    // CuBotModule interface
public:
    int type() const;
    QString name() const;
    QString description() const;
    QString help() const;
    int decode(const TBotMsg &msg);
    bool process();
    bool error() const;
    QString message() const;

    // CuBotPluginInterface interface
public:
    void init(CuBotModuleListener *listener, BotDb *db, BotConfig *bot_conf);

private:
    QList<HistoryEntry> m_prepareHistory(int uid, CuTgHistoryMsgDecoder::Type t);

    CuTelegramHistoryPluginPrivate *d;

};

#endif // CUTELEGRAMHISTORYPLUGIN_H
