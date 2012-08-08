#ifndef ROBOT_UNICYCLE_VIRTUEL_H
#define ROBOT_UNICYCLE_VIRTUEL_H

#include "RobotUnicycle.h"

/**
Cette classe implémente un robot unicycle virtuel, d'inertie nulle et d'asservissement idéal afin de pouvoir tester des algorithmes sans robot connecté.
*/
class RobotUnicycleVirtuel : public RobotUnicycle
{
public:
    RobotUnicycleVirtuel(double rayonG, double rayonD, double dist, float periode);
    void SetVitessesAngulairesRoues(double vitesseGauche, double vitesseDroite);
protected:
    void GetDeplacement(double& delta_avance, double& delta_theta);
    double vitesseGauche;
    double vitesseDroite;
};

#endif
