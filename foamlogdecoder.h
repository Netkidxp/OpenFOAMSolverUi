#ifndef FOAMLOGDECODER_H
#define FOAMLOGDECODER_H

#include <QObject>
#include "foamdatapoint.h"
#include <QList>
#include <QString>

class FoamLogDecoder : public QObject
{
    Q_OBJECT

    int findFoamData(const QList<FoamDataPoint> &data, double time,const QString &name);
public:
    explicit FoamLogDecoder(QObject *parent = nullptr);
    QList<FoamDataPoint> decode(const QString &log);

signals:

public slots:
};

#endif // FOAMLOGDECODER_H
