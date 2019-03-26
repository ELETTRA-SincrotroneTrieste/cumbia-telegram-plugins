#ifndef CUTELEGRAMHISTORYMSGFORMATTER_H
#define CUTELEGRAMHISTORYMSGFORMATTER_H

#include <QString>
#include <historyentry.h>
#include <QList>

class CuTelegramHistoryMsgFormatter
{
public:
    CuTelegramHistoryMsgFormatter();


    QString history_msg(const QList<HistoryEntry> &hel, int ttl, const QString &type) const;
};

#endif // CUTELEGRAMHISTORYMSGFORMATTER_H
