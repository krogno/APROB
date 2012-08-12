#include "RobotUnicycleVirtuel.h"
#include "Constantes.h"


RobotUnicycleVirtuel::RobotUnicycleVirtuel(double retard_s)
{
    vitesseDroite=0;
    vitesseGauche=0;
    vitesseGauche_consigne=0;
    vitesseDroite_consigne=0;
    retard=retard_s;
}

void RobotUnicycleVirtuel::SetVitessesAngulairesRoues(double vG, double vD)
{
    vitesseGauche_consigne=vG;
    vitesseDroite_consigne=vD;
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
    vitesseGauche=vitesseGauche+(vitesseGauche_consigne-vitesseGauche)*tempsEcoule/retard;
    vitesseDroite=vitesseDroite+(vitesseDroite_consigne-vitesseDroite)*tempsEcoule/retard;
    //Mise à jour des déplacements
    delta_avance=(ROBOT_UNICYCLE_RAYON_GAUCHE*vitesseGauche+ROBOT_UNICYCLE_RAYON_DROITE*vitesseDroite)/2*tempsEcoule;
    delta_theta=(ROBOT_UNICYCLE_RAYON_DROITE*vitesseDroite-ROBOT_UNICYCLE_RAYON_GAUCHE*vitesseGauche)/ROBOT_UNICYCLE_DISTANCE_ROUES*tempsEcoule;
    horloge.Reset();
}

