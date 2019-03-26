#ifndef CUTELEGRAMEXECHISTORYLINKSPLUGIN_H
#define CUTELEGRAMEXECHISTORYLINKSPLUGIN_H

#include <QStringList>
#include <cubotplugininterface.h>

class CuTgExHistLnkMsgDecoder {
public:

    int decode(const TBotMsg &msg);
    QString type;
private:
};

class CuTgExHistLnkPluginPrivate;

class CuTgExHistLnkPlugin : public QObject, public CuBotPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CuBotPluginInterface)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.CumbiaTelegram.CuTgExHistLnkPlugin" FILE "cumbia-telegram-exec-history-links.json")
#endif // QT_VERSION >= 0x050000

public:
    enum Type { Bot_ExecHistoryLinkPlugin = 45 };

    CuTgExHistLnkPlugin(QObject *parent = nullptr);

    virtual ~CuTgExHistLnkPlugin();

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

    QString m_history_op_msg(const QDateTime& dt, const QString& name) const;

    CuTgExHistLnkPluginPrivate *d;

};

#endif // CuTgExHistLnkPLUGIN_H
