#include "foamrunner.h"

FoamRunner::FoamRunner()
{
    this->setLogfileName("foam.log");
    this->setErrfileName("foam.err");
}

void FoamRunner::Start()
{
    int np = getParallelNodeCount();
    if(np == 0)
        StartSerially();
    else
    {
        QString qsHostfile = getTargetUrl() + SEP + "system" + SEP + "hostfile";
        QFile f(qsHostfile);
        if(f.exists())
        {
            StartParallelly(np,qsHostfile);
        }
        else
            StartParallelly(np);
    }
}

void FoamRunner::StartSerially()
{
    QString solver = getSolverName();
    if(solver == NULL)
    {
        addError("read ControlDict error, please make sure your ControlDict  file is existed");
        return;
    }
    if(solver == "")
    {
        addError("get solver name error, please make sure your ControlDict format is corrected");
        return;
    }
    QStringList options;
    options << "-case";
    options << getTargetUrl();
    Run(solver,options);

}

void FoamRunner::StartParallelly(int np)
{

    QString solver = getSolverName();
    if(solver == NULL)
    {
        addError("read ControlDict error, please make sure your ControlDict  file is existed");
        return;
    }
    if(solver == "")
    {
        addError("get solver name error, please make sure your ControlDict format is corrected");
        return;
    }
    QStringList options;
    QString application;
#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    application = "mpiexec";
    options << "-n";
#else
    application = "mpirun";
    options << "-np";
#endif
    options << QString::number(np, 10);
    options << solver;
    options << "-parallel";
    options << "-case";
    options << getTargetUrl();
    Run(application,options);
}



void FoamRunner::StartParallelly(int np, const QString &hostfile)
{
    QString solver = getSolverName();
    if(solver == NULL)
    {
        addError("read ControlDict error, please make sure your ControlDict  file is existed");
        return;
    }
    if(solver == "")
    {
        addError("get solver name error, please make sure your ControlDict format is corrected");
        return;
    }

    QStringList options;
    QString application;
#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    application = "mpiexec";
    options << "-n";
#else
    application = "mpirun";
    options << "-np";
#endif
    options << QString::number(np, 10);
    options << "--hostfile";
    options << hostfile;
    options << solver;
    options << "-parallel";
    options << "-case";
    options << getTargetUrl();
    Run(application,options);
}


