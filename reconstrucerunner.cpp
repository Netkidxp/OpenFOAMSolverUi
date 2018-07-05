#include "reconstrucerunner.h"

ReconstruceRunner::ReconstruceRunner()
{
    this->setLogfileName("reconstruct.log");
    this->setErrfileName("reconstruct.err");
}

void ReconstruceRunner::Start()
{
    if(getParallelNodeCount()>1)
    {
        QStringList options;
        options << "-case";
        options << getTargetUrl();
        Run("reconstructPar",options);
    }
    else
    {
        emit onProcessOutput("no reconstructParDict");
        emit onProcessFinished(0,QProcess::CrashExit);
    }
}
