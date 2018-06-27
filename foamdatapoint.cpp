#include "foamdatapoint.h"
double FoamDataPoint::getX() const
{
    return x;
}

void FoamDataPoint::setX(double value)
{
    x = value;
}

double FoamDataPoint::getY() const
{
    return y;
}

void FoamDataPoint::setY(double value)
{
    y = value;
}

QString FoamDataPoint::getName() const
{
    return name;
}

void FoamDataPoint::setName(const QString &value)
{
    name = value;
}

FoamDataPoint::FoamDataPoint()
{

}

FoamDataPoint::FoamDataPoint(const QString &name, double x, double y)
{
    setName(name);
    setX(x);
    setY(y);
}
