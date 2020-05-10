#ifndef CUTELEGRAM_COMMENTS_PLUGIN_H
#define CUTELEGRAM_COMMENTS_PLUGIN_H

#include <QStringList>
#include <cubotplugininterface.h>

class CuTgCommentsPluginPrivate;

class CuTgCommentsPlugin : public QObject, public CuBotPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CuBotPluginInterface)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.CumbiaTelegram.CuTgCommentsPlugin" FILE "cumbia-telegram-comments.json")
#endif // QT_VERSION >= 0x050000

public:
    enum Type { Bot_CommentsPlugin = 1 };

    CuTgCommentsPlugin(QObject *parent = nullptr);

    virtual ~CuTgCommentsPlugin();

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

    CuTgCommentsPluginPrivate *d;

};

#endif // CuTgExecLastPLUGIN_H
