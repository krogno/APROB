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

void RobotUnicycleVirtuel::Run()
{
    isThreadRunning=true;
    sf::Clock horloge;
    while(isThreadRunning)
    {
        float tempsEcoule=horloge.GetElapsedTime();
        if(tempsEcoule >= periodeAsservissement)
        {
            horloge.Reset();

            {
                sf::Lock lock(mutexMotificationConsignes);
                //Mise à jour de la position (comme c'est un robot virtuel c'est simulé)
                double delta_avance=(rayonGauche*vitesseGauche+rayonDroite*vitesseDroite)/2*tempsEcoule;
                double delta_theta=(rayonDroite*vitesseDroite-rayonGauche*vitesseGauche)/distance*tempsEcoule;
                Deplacer(delta_avance, delta_theta);

                //TODO : Asservissement à mettre ici
            }
        }
        else
        {
            sf::Sleep(periodeAsservissement-tempsEcoule);
        }
    }
}
