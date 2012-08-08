#include "RobotUnicycle.h"

RobotUnicycle::RobotUnicycle(double rayonG, double rayonD, double dist, float periode)
{
    rayonGauche=rayonG;
    rayonDroite=rayonD;
    distance=dist;
    periodeAsservissement=periode;

    isThreadRunning=false;
}

RobotUnicycle::~RobotUnicycle()
{
    isThreadRunning=false;
    Wait();
}

void RobotUnicycle::Run()
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
                //Mise à jour de la position
                double delta_avance, delta_theta;
                GetDeplacement(delta_avance, delta_theta); //Dépendant de l'implémentation du robot
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
