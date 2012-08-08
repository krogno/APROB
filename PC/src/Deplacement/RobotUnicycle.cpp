#include "RobotUnicycle.h"
#include "Constantes.h"

RobotUnicycle::RobotUnicycle()
{
    isThreadRunning=false;
    consigne=ROUE_LIBRE;
}

RobotUnicycle::~RobotUnicycle()
{
    isThreadRunning=false;
    Wait();
}

void RobotUnicycle::Avancer(double distance)
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=AVANCER;
    distanceRestanteAAvancer=distance;
}

void RobotUnicycle::Stopper()
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=STOP;
    SetVitessesAngulairesRoues(0,0);
}

void RobotUnicycle::Run()
{
    isThreadRunning=true;
    sf::Clock horloge;
    while(isThreadRunning)
    {
        float tempsEcoule=horloge.GetElapsedTime();
        if(tempsEcoule >= ROBOT_UNICYCLE_PERIODE_ASSERVISSEMENT)
        {
            horloge.Reset();
            sf::Lock lock(mutexMotificationConsignes);
            //Mise à jour de la position
            double delta_avance, delta_theta;
            GetDeplacement(delta_avance, delta_theta); //Dépendant de l'implémentation du robot
            Deplacer(delta_avance, delta_theta);

            //Asservissement et mise à jour des consignes de vitesse des moteurs
            switch(consigne)
            {
                case STOP:
                    SetVitessesAngulairesRoues(0,0);
                    break;
                case AVANCER:
                    distanceRestanteAAvancer-=delta_avance;
                    if(abs(distanceRestanteAAvancer) <= ROBOT_UNICYCLE_PRECISION_DISTANCE)
                    {
                        consigne=STOP;
                        SetVitessesAngulairesRoues(0,0);
                    }
                    else
                    {
                        if(distanceRestanteAAvancer<0)
                            SetVitessesAngulairesRoues(-ROBOT_UNICYCLE_VITESSE_AVANCER_MAX, -ROBOT_UNICYCLE_VITESSE_AVANCER_MAX);
                        else
                            SetVitessesAngulairesRoues(ROBOT_UNICYCLE_VITESSE_AVANCER_MAX, ROBOT_UNICYCLE_VITESSE_AVANCER_MAX);
                    }
                    break;
                case ROUE_LIBRE:
                    break;
            }
        }
        else
        {
            sf::Sleep(ROBOT_UNICYCLE_PERIODE_ASSERVISSEMENT-tempsEcoule);
        }
    }
}
