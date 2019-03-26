#ifndef CUTELEGRAM_EXEC_LAST_PLUGIN_H
#define CUTELEGRAM_EXEC_LAST_PLUGIN_H

#include <QStringList>
#include <cubotplugininterface.h>

class CuTgExecLastPluginPrivate;

class CuTgExecLastPlugin : public QObject, public CuBotPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CuBotPluginInterface)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.CumbiaTelegram.CuTgExecLastPlugin" FILE "cumbia-telegram-exec-last.json")
#endif // QT_VERSION >= 0x050000

public:
    enum Type { Bot_ExecLastPlugin = 75 };

    CuTgExecLastPlugin(QObject *parent = nullptr);

    virtual ~CuTgExecLastPlugin();

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

    CuTgExecLastPluginPrivate *d;

};

#endif // CuTgExecLastPLUGIN_H
