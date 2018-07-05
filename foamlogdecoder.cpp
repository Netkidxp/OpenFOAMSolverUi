#include "foamlogdecoder.h"
#include <QDebug>
int FoamLogDecoder::findFoamData(const QList<FoamDataPoint> &data, double time, const QString &name)
{
    int ir =-1;
    for(int i=0;i<data.size();i++)
    {
        FoamDataPoint p = data.at(i);
        if(p.getName()==name && p.getX()==time)
            ir = i;
    }
    return ir;
}

FoamLogDecoder::FoamLogDecoder(QObject *parent) : QObject(parent)
{

}

QList<FoamDataPoint> FoamLogDecoder::decode(const QString &log)
{
    QList<FoamDataPoint> list;
    QStringList tl = log.split(QString("\nTime = "));
    for(int i=0;i<tl.size();i++)
    {
        QString s = tl.at(i);
        s = s.trimmed();
        if(s.length()==0)
            continue;
        QByteArray strByteArray = s.toLatin1();
        const char *chString = strByteArray.data();
        if(*chString>'9'||*chString<'0')                  //judge zhe sub string start with digit
            continue;
        QStringList sublist = s.split("\n");
        QString stime = sublist.first();                //get time
        double time = stime.toDouble();
        QList<FoamDataPoint> tmp;
        for(int j=1;j<sublist.size();j++)
        {
            QString subs = sublist.at(j);
            subs = subs.trimmed();
            if(subs.size()==0 || subs == "\n")
                continue;
            if(subs.startsWith("time step continuity errors : "))
            {
                subs = subs.replace("time step continuity errors : ","");
                QStringList ssublist = subs.split(",");
                for(int k=0;k<ssublist.size();k++)
                {
                    QString ssubs = ssublist.at(k);
                    ssubs = ssubs.simplified();
                    QStringList eslist = ssubs.split("=");
                    QString name("continuity.");
                    QString sn = eslist.at(0);
                    sn = sn.simplified();
                    QString sv = eslist.at(1);
                    sv = sv.simplified();
                    name += sn;
                    double value = sv.toDouble();
                    int ir = findFoamData(tmp,time,name);
                    if(ir==-1)
                        tmp.append(FoamDataPoint(name,time,value));
                    else
                        tmp.replace(ir,FoamDataPoint(name,time,value));
                }
            }
            if(subs.contains("Solving for"))
            {
                subs = subs.simplified();
                QStringList ssublist = subs.split(",");
                QString sbegin = ssublist.at(0);
                sbegin = sbegin.simplified();
                QStringList ebs = sbegin.split(":");
                QString en = ebs.at(1);
                en = en.replace("Solving for ","").simplified();

                for(int k=1;k<ssublist.size()-1;k++)
                {
                    QString name = en + ".";
                    QString ssubs = ssublist.at(k);
                    ssubs = ssubs.simplified();
                    QStringList eslist = ssubs.split("=");
                    QString sn = eslist.at(0);
                    sn = sn.simplified();
                    QString sv = eslist.at(1);
                    sv = sv.simplified();
                    name += sn;
                    double value = sv.toDouble();
                    int ir = findFoamData(tmp,time,name);
                    if(ir==-1)
                        tmp.append(FoamDataPoint(name,time,value));
                    else
                        tmp.replace(ir,FoamDataPoint(name,time,value));
                }
            }
        }
        foreach (FoamDataPoint p, tmp) {
            list.append(p);
        }
    }
    return list;
}

