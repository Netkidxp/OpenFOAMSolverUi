#ifndef DECOMPOSERUNNER_H
#define DECOMPOSERUNNER_H
#include "runner.h"

class DecomposeRunner: public Runner
{
public:
    DecomposeRunner();

    // Runner interface
public:
    void Start();
};

#endif // DECOMPOSERUNNER_H
