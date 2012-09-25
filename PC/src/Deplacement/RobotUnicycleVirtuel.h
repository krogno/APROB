#ifndef ROBOT_UNICYCLE_VIRTUEL_H
#define ROBOT_UNICYCLE_VIRTUEL_H

#include "RobotUnicycle.h"

/**
Cette classe implémente un robot unicycle virtuel afin de pouvoir tester des algorithmes sans robot connecté.
*/
class RobotUnicycleVirtuel : public RobotUnicycle
{
public:
    //Créé un robot virtuel qui dont l'asservissement en vitesse présente un retard pour plus de réalisme
    RobotUnicycleVirtuel(double retard, double facteur_erreur_vitesse_gauche, double facteur_erreur_vitesse_droite);
protected:
    void SetVitesses(double vitesse_avance, double omega);
    void GetDeplacement(double& delta_avance, double& delta_theta);
    void SetMoteursEnModeRouesLibres();
    //Vitesse effective
    double vitesse_effective;
    double omega_effectif;
    //Consignes de vitesse
    double vitesse_consigne;
    double omega_consigne;
    double retard;
    double facteur_erreur_vitesse_gauche;
    double facteur_erreur_vitesse_droite;
    double vitesse_gauche_consigne;
    double vitesse_droite_consigne;
    double vitesse_gauche;
    double vitesse_droite;
};

#endif
