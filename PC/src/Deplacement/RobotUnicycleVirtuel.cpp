#include "RobotUnicycleVirtuel.h"
#include "Constantes.h"


RobotUnicycleVirtuel::RobotUnicycleVirtuel(double retard_s)
{
    vitesse_effective=0;
    omega_effectif=0;
    vitesse_consigne=0;
    omega_consigne=0;
    retard=retard_s;
}

void RobotUnicycleVirtuel::SetVitesses(double vitesse_avance, double omega)
{
    vitesse_consigne=vitesse_avance;
    omega_consigne=omega;
}

void RobotUnicycleVirtuel::SetMoteursEnModeRouesLibres()
{
    /*
    Puisqu'il s'agit d'un robot sans inertie, le robot est censé pouvoir partir en vrille lorsqu'il est laissé en roues libre
    ici on se contente de ne rien faire
    */
}

void RobotUnicycleVirtuel::GetDeplacement(double& delta_avance, double& delta_theta)
{
    static  sf::Clock horloge;
    float tempsEcoule=horloge.GetElapsedTime();
    //Mise à jour des consignes de vitesse (la manière dont c'est calculé est un peu pifométrique je l'accorde, mais j'avais pas envie de trainer des exponentiells)
    vitesse_effective=vitesse_effective+(vitesse_consigne-vitesse_effective)*tempsEcoule/retard;
    omega_effectif=omega_effectif+(omega_consigne-omega_effectif)*tempsEcoule/retard;
    //Mise à jour des déplacements
    delta_avance=vitesse_effective*tempsEcoule;
    delta_theta=omega_effectif*tempsEcoule;
    horloge.Reset();
}

