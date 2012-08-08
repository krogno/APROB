#include "RobotUnicycleVirtuel.h"

RobotUnicycleVirtuel::RobotUnicycleVirtuel(double rayonG, double rayonD, double dist, float periode):
    RobotUnicycle(rayonG, rayonD, dist, periode) //Initialise les parametres hérités de RobotUnicycle
{
    vitesseDroite=0;
    vitesseGauche=0;
}

void RobotUnicycleVirtuel::SetVitessesAngulairesRoues(double vG, double vD)
{
    sf::Lock lock(mutexMotificationConsignes);
    vitesseGauche=vG;
    vitesseDroite=vD;
}

void RobotUnicycleVirtuel::GetDeplacement(double& delta_avance, double& delta_theta)
{
    static  sf::Clock horloge;
    float tempsEcoule=horloge.GetElapsedTime();
    delta_avance=(rayonGauche*vitesseGauche+rayonDroite*vitesseDroite)/2*tempsEcoule;
    delta_theta=(rayonDroite*vitesseDroite-rayonGauche*vitesseGauche)/distance*tempsEcoule;
    horloge.Reset();
}

