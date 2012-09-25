#include "RobotUnicycleVirtuel.h"
#include "Constantes.h"


RobotUnicycleVirtuel::RobotUnicycleVirtuel(double retard_s,double facteur_erreur_vitesse_gauche_param, double facteur_erreur_vitesse_droite_param)
{
    vitesse_effective=0;
    omega_effectif=0;
    vitesse_consigne=0;
    omega_consigne=0;
    retard=retard_s;
    facteur_erreur_vitesse_gauche=facteur_erreur_vitesse_gauche_param;
    facteur_erreur_vitesse_droite=facteur_erreur_vitesse_droite_param;
}


void RobotUnicycleVirtuel::SetVitesses(double vitesse_avance, double omega)
{
    double part_vitesse_omega=omega*ROBOT_UNICYCLE_DISTANCE_ROUES/2.0;
    vitesse_gauche_consigne=floor(vitesse_avance-part_vitesse_omega);
    vitesse_droite_consigne=floor(vitesse_avance+part_vitesse_omega);

    //Saturation des consignes de vitesse
    if(vitesse_gauche_consigne>ROBOT_UNICYCLE_VITESSE_MAX)
        vitesse_gauche_consigne=ROBOT_UNICYCLE_VITESSE_MAX;
    else if(vitesse_gauche_consigne<-ROBOT_UNICYCLE_VITESSE_MAX)
        vitesse_gauche_consigne=-ROBOT_UNICYCLE_VITESSE_MAX;

    if(vitesse_droite_consigne>ROBOT_UNICYCLE_VITESSE_MAX)
        vitesse_droite_consigne=ROBOT_UNICYCLE_VITESSE_MAX;
    else if(vitesse_droite_consigne<-ROBOT_UNICYCLE_VITESSE_MAX)
        vitesse_droite_consigne=-ROBOT_UNICYCLE_VITESSE_MAX;
}

void RobotUnicycleVirtuel::SetMoteursEnModeRouesLibres()
{
    /*
    Puisqu'il s'agit d'un robot sans inertie, le robot est censé pouvoir partir en vrille lorsqu'il est laissé en roues libre
    ici on se contente de ne rien faire
    */
    SetVitesses(0,0);
}

void RobotUnicycleVirtuel::GetDeplacement(double& delta_avance, double& delta_theta)
{
    static  sf::Clock horloge;
    float tempsEcoule=horloge.GetElapsedTime();
    //Mise à jour des consignes de vitesse (la manière dont c'est calculé est un peu pifométrique je l'accorde, mais j'avais pas envie de trainer des exponentiells)
    /*vitesse_effective=vitesse_effective+(vitesse_consigne-vitesse_effective)*tempsEcoule/retard;
    omega_effectif=omega_effectif+(omega_consigne-omega_effectif)*tempsEcoule/retard;*/
    vitesse_gauche=vitesse_gauche+(vitesse_gauche_consigne*facteur_erreur_vitesse_gauche-vitesse_gauche)/retard;
    vitesse_droite=vitesse_droite+(vitesse_droite_consigne*facteur_erreur_vitesse_droite-vitesse_droite)/retard;

    delta_avance=(vitesse_gauche+vitesse_droite)/2*tempsEcoule;
    delta_theta=(vitesse_droite-vitesse_gauche)/ROBOT_UNICYCLE_DISTANCE_ROUES*tempsEcoule;

    //Mise à jour des déplacements
    /*delta_avance=vitesse_effective*tempsEcoule;
    delta_theta=omega_effectif*tempsEcoule;*/
    horloge.Reset();
}

