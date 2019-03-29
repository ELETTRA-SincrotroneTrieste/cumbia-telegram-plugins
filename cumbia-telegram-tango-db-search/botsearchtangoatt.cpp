#include "botsearchtangoatt.h"
#include <cutelegramtangodbsearchplugin.h> // for enum SearchMode { Invalid = 0, DevSearch, AttSearch, ReadFromAttList };
#include <tango.h>
#include <cutango-world.h>

TgAttSearchThread::TgAttSearchThread(QObject *parent, const QString &devname, const QString& host) : QThread(parent)
{
    m_devname = devname;
    m_host = host;
}

void TgAttSearchThread::run()
{
    errmsg.clear();
    try {
        std::string dname = m_devname.toStdString();
        printf("HOST IN ATT SEACH %s\n", qstoc(m_host));
        if(!m_host.isEmpty()) dname = m_host.toStdString() + "/" + dname;
        printf("HOST IN ATT SEACH %s dname %s\n", qstoc(m_host), dname.c_str());
        Tango::DeviceProxy *dev = new Tango::DeviceProxy(dname);
        Tango::AttributeInfoList *ail = dev->attribute_list_query();
        for(size_t i = 0; i < ail->size(); i++)
            attributes << QString::fromStdString(ail->at(i).name);
    }
    catch(Tango::DevFailed &e) {
        CuTangoWorld tw;
        errmsg = QString::fromStdString(tw.strerror(e));
        perr("TgAttSearchThread.run: %s", qstoc(errmsg));
    }
}

BotSearchTangoAtt::BotSearchTangoAtt(QObject *parent, int chatid) : QObject (parent)
{
    m_chat_id = chatid;
}

BotSearchTangoAtt::~BotSearchTangoAtt()
{
    printf("\e[1;31mx BotSearchTangoAtt %p\e[0m\n", this);
    TgAttSearchThread *th = findChild<TgAttSearchThread *>();
    if(th)
        th->wait();
}

void BotSearchTangoAtt::find(const QString &devname, const QString &host)
{
    m_devname = devname;
    TgAttSearchThread *th = new TgAttSearchThread(this, devname, host);
    connect(th, SIGNAL(finished()), this, SLOT(onSearchFinished()));
    th->start();
}

QString BotSearchTangoAtt::getSourceByIdx(int idx)
{
    QString a = getAttByIdx(idx);
    if(a.length() > 0)
        return m_devname + "/" + a;
    return a;
}

QString BotSearchTangoAtt::getAttByIdx(int idx)
{
    if(idx > 0 && idx - 1 < m_attlist.size())
        return m_attlist.at(idx - 1);
    d_error = true;
    d_msg = QString("BotSearchTangoAtt.getDevByIdx: index %1 out of range").arg(idx);
    return QString();
}

QString BotSearchTangoAtt::devname() const
{
    return m_devname;
}

QStringList BotSearchTangoAtt::attributes() const
{
    return m_attlist;
}

QString BotSearchTangoAtt::message() const
{
    return d_msg;
}

void BotSearchTangoAtt::onSearchFinished()
{
    m_attlist = qobject_cast<TgAttSearchThread *>(sender())->attributes;
    d_msg = qobject_cast<TgAttSearchThread *>(sender())->errmsg;
    sender()->deleteLater();
    emit attListReady(m_chat_id, m_devname, m_attlist, d_msg);
}

void BotSearchTangoAtt::consume(int type)
{
    printf("\e[1;31m*** FIX BotSearchTangoAtt::consume moduletyp\e[0m\n");
    if(type != CuTelegramTangoDbSearchPlugin::ReadFromAttList) {
        d_life_cnt--;
    }
}

bool BotSearchTangoAtt::disposeWhenOver() const
{
    return true;
}

int BotSearchTangoAtt::type() const
{
    return Bot_SearchTangoAtt;
}

QString BotSearchTangoAtt::name() const
{
    return "BotSearchTangoAtt";
}

void BotSearchTangoAtt::signalTtlExpired()
{
    emit volatileOperationExpired(m_chat_id, name(), "attlist " + m_devname);
}
