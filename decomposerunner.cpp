#include "decomposerunner.h"

DecomposeRunner::DecomposeRunner()
{
    this->setLogfileName("decompose.log");
    this->setErrfileName("decompose.err");
}

void DecomposeRunner::Start()
{
    if(getParallelNodeCount()>1)
    {
        QStringList options;
        options << "-case";
        options << getTargetUrl();
        Run("decomposePar",options);
    }

    else
    {
        emit onProcessOutput("no decomposeParDict");
        emit onProcessFinished(0,QProcess::CrashExit);
    }

}
