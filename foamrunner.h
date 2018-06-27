#ifndef FOAMRUNNER_H
#define FOAMRUNNER_H
#include "runner.h"

class FoamRunner:public Runner
{
public:
    FoamRunner();

    // Runner interface
public:
    void Start();
    void StartSerially();
    void StartParallelly(int np);
    void StartParallelly(int np, const QString &hostfile);

};

#endif // FORMRUNNER_H
