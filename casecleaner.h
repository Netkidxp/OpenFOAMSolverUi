#ifndef CASECLEANER_H
#define CASECLEANER_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

class CaseCleaner : public QObject
{
    Q_OBJECT
public:
    explicit CaseCleaner(QObject *parent = nullptr);
    static bool deleteDir(const QString &dirName);
    static bool deleteFile(const QString &fileName);
    static bool deleteElement(const QString &dirName, const QString &exp);
    static void cleanTimeDirectories(const QString &dirName);
    static void cleanDynamicCode(const QString &dirName);
    static void cleanCase(const QString &dirName);
    static void deleteDirOrFile(const QString &name);
    static void cleanProcesorDirs(const QString &dirName);
signals:

public slots:
};

#endif // CASECLEANER_H
