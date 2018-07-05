#include "casecleaner.h"

CaseCleaner::CaseCleaner(QObject *parent) : QObject(parent)
{

}

bool CaseCleaner::deleteElement(const QString &dirName, const QString &exp)
{
    QDir directory(dirName);
    if (!directory.exists())
    {
        return true;
    }
    QString srcPath = QDir::toNativeSeparators(dirName);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    QRegExp rx(exp);
    bool error =false;
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString fileName = fileNames.at(i);
        if(!rx.exactMatch(fileName))
            continue;
        QString filePath = srcPath + fileName;
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            if(!deleteFile(filePath))
                error = true;
        }
        else if (fileInfo.isDir())
        {
            if (!deleteDir(filePath))
            {
                error = true;
            }
        }
    }
    return error;
}

void CaseCleaner::cleanTimeDirectories(const QString &dirName)
{
    deleteElement(dirName,"^[1-9]\\d*.\\d*|0.\\d*[1-9]\\d*$|^[1-9]\\d*$");
    QDir directory(dirName);
    if (!directory.exists())
    {
        return;
    }
    QString srcPath = QDir::toNativeSeparators(dirName);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString fileName = fileNames.at(i);
        QString filePath = srcPath + fileName;
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
            continue;
        else if (fileInfo.isDir())
        deleteElement(filePath,"^[1-9]\\d*.\\d*|0.\\d*[1-9]\\d*$|^[1-9]\\d*$");
    }
}

void CaseCleaner::cleanDynamicCode(const QString &dirName)
{
    deleteDir(dirName + QDir::separator() + "dynamicCode");
}

void CaseCleaner::cleanCase(const QString &dirName)
{
    cleanTimeDirectories(dirName);
    cleanDynamicCode(dirName);
    deleteElement(dirName,"^processor.*$");
    deleteDir(dirName + QDir::separator() + "postProcessing");
    deleteDir(dirName + QDir::separator() + "TDAC");
    deleteElement(dirName,"^probes.*$");
    deleteElement(dirName,"^forces.*$");
    deleteElement(dirName,"^graphs.*$");
    deleteDirOrFile(dirName + QDir::separator() + "sets");
    deleteDirOrFile(dirName + QDir::separator() + "surfaceSampling");
    deleteDirOrFile(dirName + QDir::separator() + "cuttingPlane");
    deleteDirOrFile(dirName + QDir::separator() + "system" + QDir::separator() + "machines");
    deleteDirOrFile(dirName + QDir::separator() + "content" + QDir::separator() + "cellToRegion");
    deleteElement(dirName + QDir::separator() + "content","^cellLevel.*$");
    deleteElement(dirName + QDir::separator() + "content","^pointLevel.*$");
    deleteDirOrFile(dirName + QDir::separator() + "VTK");
    deleteElement(dirName,"^\\w+[.]{1}log{1}$");
    deleteElement(dirName,"^\\w+[.]{1}err{1}$");
}

void CaseCleaner::deleteDirOrFile(const QString &name)
{
    deleteFile(name);
    deleteDir(name);
}

void CaseCleaner::cleanProcesorDirs(const QString &dirName)
{
    deleteElement(dirName,"^processor.*$");
}

bool CaseCleaner::deleteDir(const QString &dirName)
{
    QDir directory(dirName);
    if (!directory.exists())
    {
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(dirName);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            if (!deleteFile(filePath))
            {
                error = true;
            }
        }
        else if (fileInfo.isDir())
        {
            if (!deleteDir(filePath))
            {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path())))
    {
        qDebug() << "remove dir" << directory.path() << " faild!";
        error = true;
    }
    return !error;
}

bool CaseCleaner::deleteFile(const QString &fileName)
{
    QFile f(fileName);
    if(!f.exists())
        return true;
    QFile::setPermissions(fileName, QFile::WriteOwner);
    if (!QFile::remove(fileName))
    {
        qDebug() << "remove file" << fileName << " faild!";
        return false;
    }
    else
        return true;

}



