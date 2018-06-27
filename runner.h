#ifndef RUNNER_H
#define RUNNER_H
#include <qstring.h>
#include <QList>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QDebug>

class Runner : public QObject
{
    Q_OBJECT

    QString logfileName = "Runner.log";
    QString errfileName = "Runner.err";
    const int MAX_ERROR_COUNT = 100;
    QFile *logfile;
    QFile *errfile;
    QString targetUrl;
    QList<QString> errors;
    QProcess *process;

    protected:
    void addError(const QString &value);
    virtual void Run(const QString app,const QStringList options);
    virtual void Run(const QString app);

    private slots:
    void start_process();
    void start_read_output();
    void start_read_err_output();
    void finish_process(int exitCode, QProcess::ExitStatus exitStatus);


    public:
    const QString SEP = QDir::separator();
    Runner();
    ~Runner();
    void setTargetUrl(const QString &value);
    void setErrfileName(const QString &value);
    void setLogfileName(const QString &value);
    virtual void Start();
    void Stop();
    QString getTargetUrl() const;
    QString getLogfileUrl() const;
    QString getErrfileUrl() const;
    QProcess* getProcess() const;
    QString lastError() const;
    QProcess::ProcessState getProcessState();
    bool isRunning();
    QString getSolverName() const;
    int getParallelNodeCount() const;

    signals:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(const QString &value);
    void onProcessOutput(const QString &value);
    void onProcessStart();
};

#endif // RUNNER_H
