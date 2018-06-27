#ifndef FOAMDATAPOINT_H
#define FOAMDATAPOINT_H
#include <QPointF>
#include <QString>
#include <QList>

class FoamDataPoint
{
    double x;
    double y;
    QString name;
public:
    FoamDataPoint();
    FoamDataPoint(const QString &name,double x,double y);
    double getX() const;
    void setX(double value);
    double getY() const;
    void setY(double value);
    QString getName() const;
    void setName(const QString &value);
};

#endif // FOAMDATAPOINT_H
