#include "RobotUnicycle.h"
#include "Constantes.h"
#include <iostream>


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

void RobotUnicycle::Tourner(double angle)
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=TOURNER;
    angleRestantATourner=angle;
}

void RobotUnicycle::Stopper()
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=STOP;
    SetVitessesAngulairesRoues(0,0);
}

void RobotUnicycle::PasserEnModeRouesLibres()
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=ROUE_LIBRE;
    SetMoteursEnModeRouesLibres();
}

bool RobotUnicycle::isArrete()
{
    return (consigne == STOP) || (consigne == ROUE_LIBRE);
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
                case ROUE_LIBRE:
                    SetMoteursEnModeRouesLibres();
                    break;
                case AVANCER:
                    distanceRestanteAAvancer-=delta_avance;
                    if(distanceRestanteAAvancer<0)
                    {
                        //Si la distance restante est inférieure à la précision on stoppe.
                        if(-distanceRestanteAAvancer <= ROBOT_UNICYCLE_PRECISION_DISTANCE)
                        {
                            consigne=STOP;
                            SetVitessesAngulairesRoues(0,0);
                        }
                        //Si la distance restante est inférieure au seuil, on utilise une rampe décroissante de vitesse
                        else if(-distanceRestanteAAvancer <= ROBOT_UNICYCLE_DISTANCE_SEUIL_VITESSE_MAX)
                        {
                            SetVitessesAngulairesRoues(
                                -ROBOT_UNICYCLE_VITESSE_MOTEUR_GAUCHE_AVANCER_MIN-ROBOT_UNICYCLE_TAUX_ACCROISSEMENT_VITESSE_MOTEUR_GAUCHE_AVANCER*distanceRestanteAAvancer,
                                -ROBOT_UNICYCLE_VITESSE_MOTEUR_DROITE_AVANCER_MIN-ROBOT_UNICYCLE_TAUX_ACCROISSEMENT_VITESSE_MOTEUR_DROITE_AVANCER*distanceRestanteAAvancer);

                        }
                        else
                            SetVitessesAngulairesRoues(-ROBOT_UNICYCLE_VITESSE_MOTEUR_GAUCHE_AVANCER_MAX, -ROBOT_UNICYCLE_VITESSE_MOTEUR_DROITE_AVANCER_MAX);
                    }
                    else
                    {
                        if(distanceRestanteAAvancer <= ROBOT_UNICYCLE_PRECISION_DISTANCE)
                        {
                            consigne=STOP;
                            SetVitessesAngulairesRoues(0,0);
                        }
                        else if(distanceRestanteAAvancer <= ROBOT_UNICYCLE_DISTANCE_SEUIL_VITESSE_MAX)
                        {
                            SetVitessesAngulairesRoues(
                                ROBOT_UNICYCLE_VITESSE_MOTEUR_GAUCHE_AVANCER_MIN+ROBOT_UNICYCLE_TAUX_ACCROISSEMENT_VITESSE_MOTEUR_GAUCHE_AVANCER*distanceRestanteAAvancer,
                                ROBOT_UNICYCLE_VITESSE_MOTEUR_DROITE_AVANCER_MIN+ROBOT_UNICYCLE_TAUX_ACCROISSEMENT_VITESSE_MOTEUR_DROITE_AVANCER*distanceRestanteAAvancer);
                        }
                        else
                            SetVitessesAngulairesRoues(ROBOT_UNICYCLE_VITESSE_MOTEUR_GAUCHE_AVANCER_MAX, ROBOT_UNICYCLE_VITESSE_MOTEUR_DROITE_AVANCER_MAX);
                    }
                    break;
                case TOURNER:
                    angleRestantATourner-=delta_theta;
                    if(angleRestantATourner < 0)
                    {
                        if(angleRestantATourner >= -ROBOT_UNICYCLE_PRECISION_ANGLE)
                        {
                            consigne=STOP;
                            SetVitessesAngulairesRoues(0,0);
                        }
                        //Rampe de vitesse angulaire
                        else if(angleRestantATourner >= -ROBOT_UNICYCLE_ANGLE_SEUIL_VITESSE_MAX)
                        {
                            SetVitessesAngulairesRoues(
                                ROBOT_UNICYCLE_VITESSE_MOTEUR_GAUCHE_TOURNER_MIN-ROBOT_UNICYCLE_TAUX_ACCROISSEMENT_VITESSE_MOTEUR_GAUCHE_TOURNER*angleRestantATourner,
                                -ROBOT_UNICYCLE_VITESSE_MOTEUR_DROITE_TOURNER_MIN+ROBOT_UNICYCLE_TAUX_ACCROISSEMENT_VITESSE_MOTEUR_DROITE_TOURNER*angleRestantATourner);
                        }
                        else
                            SetVitessesAngulairesRoues(ROBOT_UNICYCLE_VITESSE_MOTEUR_GAUCHE_TOURNER_MAX, -ROBOT_UNICYCLE_VITESSE_MOTEUR_DROITE_TOURNER_MAX);
                    }
                    else
                    {
                        if(angleRestantATourner <= ROBOT_UNICYCLE_PRECISION_ANGLE)
                        {
                            consigne=STOP;
                            SetVitessesAngulairesRoues(0,0);
                        }
                        //Rampe de vitesse angulaire
                        else if(angleRestantATourner <= ROBOT_UNICYCLE_ANGLE_SEUIL_VITESSE_MAX)
                        {
                            SetVitessesAngulairesRoues(
                                -ROBOT_UNICYCLE_VITESSE_MOTEUR_GAUCHE_TOURNER_MIN+ROBOT_UNICYCLE_TAUX_ACCROISSEMENT_VITESSE_MOTEUR_GAUCHE_TOURNER*angleRestantATourner,
                                ROBOT_UNICYCLE_VITESSE_MOTEUR_DROITE_TOURNER_MIN-ROBOT_UNICYCLE_TAUX_ACCROISSEMENT_VITESSE_MOTEUR_DROITE_TOURNER*angleRestantATourner);
                        }
                        else
                            SetVitessesAngulairesRoues(-ROBOT_UNICYCLE_VITESSE_MOTEUR_GAUCHE_TOURNER_MAX, +ROBOT_UNICYCLE_VITESSE_MOTEUR_DROITE_TOURNER_MAX);
                    }
                    break;
            }
        }
        else
        {
            sf::Sleep(ROBOT_UNICYCLE_PERIODE_ASSERVISSEMENT-tempsEcoule);
        }
    }
}
