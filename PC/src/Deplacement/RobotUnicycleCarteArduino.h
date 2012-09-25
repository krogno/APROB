#ifndef ROBOT_UNICYCLE_CARTE_ARDUINO_H
#define ROBOT_UNICYCLE_CARTE_ARDUINO_H

#include "RobotUnicycle.h"
#include "serialib/serialib.h"

/**
Cette classe implémente un robot unicycle réel asservi via une carte arduino avec le sketchbook AsservissementVitesseOmega pilotant
la carte moteur créée par Samuel Quiret en 2011
*/
class RobotUnicycleCarteArduino : public RobotUnicycle
{
public:
    RobotUnicycleCarteArduino();
protected:
    void SetVitesses(double vitesse_avance, double omega);
    void GetDeplacement(double& delta_avance, double& delta_theta);
    void SetMoteursEnModeRouesLibres();

    //Port serie
    serialib serial;
};

#endif
