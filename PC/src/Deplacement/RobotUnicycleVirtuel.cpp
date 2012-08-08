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

void RobotUnicycleVirtuel::GetDeplacement(double& delta_avance, double& delta_theta)
{
    static  sf::Clock horloge;
    float tempsEcoule=horloge.GetElapsedTime();
    delta_avance=(ROBOT_UNICYCLE_RAYON_GAUCHE*vitesseGauche+ROBOT_UNICYCLE_RAYON_DROITE*vitesseDroite)/2*tempsEcoule;
    delta_theta=(ROBOT_UNICYCLE_RAYON_DROITE*vitesseDroite-ROBOT_UNICYCLE_RAYON_GAUCHE*vitesseGauche)/ROBOT_UNICYCLE_DISTANCE_ROUES*tempsEcoule;
    horloge.Reset();
}

