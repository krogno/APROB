#include "RobotUnicycleVirtuel.h"
#include "Constantes.h"

RobotUnicycleVirtuel::RobotUnicycleVirtuel()
{
    vitesseDroite=0;
    vitesseGauche=0;
}

void RobotUnicycleVirtuel::SetVitessesAngulairesRoues(double vG, double vD)
{
    vitesseGauche=vG;
    vitesseDroite=vD;
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
    delta_avance=(ROBOT_UNICYCLE_RAYON_GAUCHE*vitesseGauche+ROBOT_UNICYCLE_RAYON_DROITE*vitesseDroite)/2*tempsEcoule;
    delta_theta=(ROBOT_UNICYCLE_RAYON_DROITE*vitesseDroite-ROBOT_UNICYCLE_RAYON_GAUCHE*vitesseGauche)/ROBOT_UNICYCLE_DISTANCE_ROUES*tempsEcoule;
    horloge.Reset();
}

