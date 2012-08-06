#include "RobotUnicycle.h"

RobotUnicycle::RobotUnicycle(double rayonG, double rayonD, double dist, float periode)
{
    rayonGauche=rayonG;
    rayonDroite=rayonD;
    distance=dist;
    periodeAsservissement=periode;

    isThreadRunning=false;
}

RobotUnicycle::~RobotUnicycle()
{
    isThreadRunning=false;
    Wait();
}
