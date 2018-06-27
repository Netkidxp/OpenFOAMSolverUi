#include "runner.h"

QString Runner::getTargetUrl() const
{
    return targetUrl;
}

QString Runner::getLogfileUrl() const
{
    return targetUrl + SEP + logfileName;
}

QString Runner::getErrfileUrl() const
{
    return targetUrl + SEP + errfileName;
}

QProcess *Runner::getProcess() const
{
    return process;
}

void Runner::setTargetUrl(const QString &value)
{
    targetUrl = value;
}

void Runner::setErrfileName(const QString &value)
{
    errfileName = value;
}

void Runner::setLogfileName(const QString &value)
{
    logfileName = value;
}

void Runner::Start()
{


}

void Runner::Stop()
{
    if(getProcessState()==QProcess::NotRunning)
    {
        return;
    }

    else
    {
        process->kill();
        process->waitForFinished();
    }
}


QString Runner::lastError() const
{
    return errors.constLast();
}

void Runner::addError(const QString &value)
{
    if(errors.count()>=MAX_ERROR_COUNT)
        errors.removeFirst();
    errors.append(value);
    emit onProcessError(value);
}

void Runner::Run(const QString app,const QStringList options)
{
    if(getProcessState() == QProcess::NotRunning)
    {
        emit onProcessOutput("target : " + getTargetUrl());
        process->start(app,options);
    }
    else
    {
        addError("solver is running");
        return;
    }
}

void Runner::Run(const QString app)
{
    if(getProcessState() == QProcess::NotRunning)
    {

        //process->setStandardOutputFile(getLogfileUrl());
        //process->setStandardErrorFile(getErrfileUrl());
        emit onProcessOutput("target : " + getTargetUrl());
        process->start(app);
    }
    else
    {
        addError("solver is running");
        return;
    }
}

Runner::Runner()
{
    setTargetUrl(QDir::currentPath());
    process = new QProcess(this);
    connect(process, SIGNAL(started()), this, SLOT(start_process()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(start_read_output()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(start_read_err_output()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finish_process(int, QProcess::ExitStatus)));
}

Runner::~Runner()
{
    if(getProcessState()!=QProcess::NotRunning)
        Stop();
    delete process;
}

void Runner::start_process()
{
    logfile = new QFile(this->getLogfileUrl());
    errfile = new QFile(this->getErrfileUrl());
    logfile->open(QIODevice::WriteOnly|QIODevice::Text);
    errfile->open(QIODevice::WriteOnly|QIODevice::Text);
    emit onProcessStart();
}

void Runner::start_read_output()
{
    QByteArray ba = process->readAllStandardOutput();
    logfile->write(ba.constData(),ba.length());
    logfile->flush();
    emit onProcessOutput(ba);
}

void Runner::start_read_err_output()
{
    QByteArray ba = process->readAllStandardError();
    errfile->write(ba.constData(),ba.length());
    errfile->flush();
    addError(ba);
}

void Runner::finish_process(int exitCode, QProcess::ExitStatus exitStatus)
{
    logfile->close();
    errfile->close();
    logfile = NULL;
    errfile = NULL;
    emit onProcessFinished(exitCode,exitStatus);
}



QProcess::ProcessState Runner::getProcessState()
{
    if(process)
        return process->state();
    else
        return QProcess::NotRunning;
}

bool Runner::isRunning()
{
    return (process->state()!=QProcess::NotRunning);
}

QString Runner::getSolverName() const
{
    QString psControlDict = getTargetUrl() + SEP + "system" + SEP + "controlDict";
    QString solver = "";
    QFile f(psControlDict);
    if(!f.open(QIODevice::ReadOnly))
    {
        return NULL;
    }
    QTextStream s(&f);
    while(!s.atEnd())
    {
        QString buf = s.readLine();
        int i = buf.indexOf("application");
        if(i != -1)
        {
            solver = buf.right(buf.length() - i - 11).trimmed().remove(';');
            break;
        }
    }
    f.close();
    return solver;
}

int Runner::getParallelNodeCount() const
{
    QString psControlDict = getTargetUrl() + SEP + "system" + SEP + "decomposeParDict";
    int np = 0;
    QFile f(psControlDict);
    if(!f.open(QIODevice::ReadOnly))
    {
        return np;
    }
    QTextStream s(&f);
    while(!s.atEnd())
    {
        QString snp = "";
        QString buf = s.readLine();
        int i = buf.indexOf("numberOfSubdomains");
        if(i != -1)
        {
            snp = buf.right(buf.length() - i - 18).trimmed().remove(';');
            np = snp.toInt();
            break;
        }
    }
    f.close();
    return np;
}
