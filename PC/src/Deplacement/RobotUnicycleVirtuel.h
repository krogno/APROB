#ifndef ROBOT_UNICYCLE_VIRTUEL_H
#define ROBOT_UNICYCLE_VIRTUEL_H

#include "RobotUnicycle.h"

/**
Cette classe implémente un robot unicycle virtuel, d'inertie nulle et d'asservissement idéal afin de pouvoir tester des algorithmes sans robot connecté.
*/
class RobotUnicycleVirtuel : public RobotUnicycle
{
public:
    //Créé un robot virtuel qui dont l'asservissement en vitesse présente un retard pour plus de réalisme
    RobotUnicycleVirtuel(double retard);
protected:
    void SetVitessesAngulairesRoues(double vitesseGauche, double vitesseDroite);
    void GetDeplacement(double& delta_avance, double& delta_theta);
    void SetMoteursEnModeRouesLibres();
    //Vitesse effective
    double vitesseGauche;
    double vitesseDroite;
    //Consignes de vitesse
    double vitesseGauche_consigne;
    double vitesseDroite_consigne;
    double retard;
};

#endif
