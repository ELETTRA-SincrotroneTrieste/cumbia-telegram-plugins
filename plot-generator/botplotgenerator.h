#ifndef BOTPLOTGENERATOR_H
#define BOTPLOTGENERATOR_H

#include <cubotvolatileoperation.h>
#include <QByteArray>
#include <vector>
#include <cubotplugininterface.h>

class CuData;

class BotPlotGenerator : public QObject, public CuBotVolatileOperation, public CuBotPluginInterface
{
    Q_OBJECT
public:

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "eu.elettra.cumbia-telegram-plot-generator-plugin" FILE "plot-generator-plugin.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(CuFormulaPluginI)
    enum Type { PlotGen = 0x04 };

    BotPlotGenerator(int chat_id, const CuData& data);

    ~BotPlotGenerator();

    QByteArray generate() const;

    // VolatileOperation interface
public:
    void consume(int moduletype);
    int type() const;
    QString name() const;
    void signalTtlExpired();

private:
    std::vector<double> m_data;
    int m_chat_id;
    QString m_source;

    // CuBotModule interface
public:
    void setBotmoduleListener(CuBotModuleListener *l);
    QString description() const;
    QString help() const;
    AccessMode needsDb() const;
    AccessMode needsStats() const;
    void setDb(BotDb *db);
    void setConf(BotConfig *conf);
    void setOption(const QString &key, const QVariant &value);
    int decode(const TBotMsg &msg);
    bool process();
    bool error() const;
    QString message() const;
};

#endif // BOTPLOTGENERATOR_H
