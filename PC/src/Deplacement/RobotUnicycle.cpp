#include "RobotUnicycle.h"
#include "Constantes.h"
#include "angle.h"
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
    erreurLineaire=-distance;
    erreurLineairePrecedente=erreurLineaire;
}

void RobotUnicycle::Tourner(double angle)
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=TOURNER;
    erreurAngulaire=-angle;
    erreurAngulairePrecedente=erreurAngulaire;
}

void RobotUnicycle::AllerALaPosition(double x, double y, double my_precision, int my_mode, double my_distanceAjustement)
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=POSITION;
    x_objectif=x;
    y_objectif=y;
    mode=my_mode;
    precisionPosition=my_precision;
    distanceAjustement=my_distanceAjustement;

    erreurAngulaire=0;
    erreurLineaire=0;
    erreurAngulairePrecedente=0;
    erreurLineairePrecedente=0;
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

void RobotUnicycle::Orienter(double angle)
{
    //Orienter le robot d'un angle angle revient à le tourner d'un angle angle-theta
    sf::Lock lock(mutexMotificationConsignes);
    consigne=TOURNER;
    //On choisi erreurAngulaire dans ]-PI,PI] pour éviter des tours inutiles
    erreurAngulaire=BornerA_MoinsPi_Pi(theta-angle);
    erreurAngulairePrecedente=erreurAngulaire;
    /*
    Note : puisque mutexMotificationConsignes est verouillé, theta n'est pas mis à jour durant cette boucle et le fait d'utiliser tourner plutôt qu'un test dans Run n'entraine pas d'imprécision supplémentaire
    */
}

bool RobotUnicycle::isArrete()
{
// return (consigne == STOP) || (consigne == ROUE_LIBRE);
    return ((std::abs(vitesse) < ROBOT_UNICYCLE_EPSILON_VITESSE) && (std::abs(omega) < ROBOT_UNICYCLE_EPSILON_OMEGA));
}

Consigne RobotUnicycle::getConsigne()
{
    return consigne;
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
            double delta_t=Deplacer(delta_avance, delta_theta);
            std::cout<<"x "<<x<<"\t\ty "<<y<<"\t\ttheta "<<theta<<std::endl;

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
            {
                erreurLineaire+=delta_avance;
                double correctionLineaire=CorrectionLineaire(erreurLineaire,(erreurLineaire-erreurLineairePrecedente)/delta_t);
                SetVitessesAngulairesRoues(-correctionLineaire/ROBOT_UNICYCLE_RAYON_GAUCHE, -correctionLineaire/ROBOT_UNICYCLE_RAYON_DROITE);
                if(std::abs(erreurLineaire) <= ROBOT_UNICYCLE_PRECISION_ANGLE && isArrete())
                {
                    consigne=STOP;
                    SetVitessesAngulairesRoues(0,0);
                }
                erreurLineairePrecedente=erreurLineaire;
                break;
            }
            case TOURNER:
            {
                erreurAngulaire+=delta_theta;
                double correctionAngulaire=CorrectionAngulaire(erreurAngulaire, (erreurAngulaire-erreurAngulairePrecedente)/delta_t);
                SetVitessesAngulairesRoues(correctionAngulaire/ROBOT_UNICYCLE_RAYON_GAUCHE, -correctionAngulaire/ROBOT_UNICYCLE_RAYON_DROITE);
                if(std::abs(erreurAngulaire) <= ROBOT_UNICYCLE_PRECISION_ANGLE && isArrete())
                {
                    consigne=STOP;
                    SetVitessesAngulairesRoues(0,0);
                }
                erreurAngulairePrecedente=erreurAngulaire;
                break;
            }

            case POSITION:
            {
                double delta_x=x_objectif-x;
                double delta_y=y_objectif-y;

                erreurLineaire=sqrt(delta_x*delta_x+delta_y*delta_y);

                if(erreurLineaire <= precisionPosition && (!(mode & RALENTIR_A_L_ARRIVEE) || isArrete()))
                {
                    consigne=STOP;
                    SetVitessesAngulairesRoues(0,0);
                    break;
                }

                erreurAngulaire=BornerA_MoinsPi_Pi(theta-atan2(delta_y, delta_x));

                if(((mode & MARCHE_AVANT) && erreurAngulaire >= -M_PI/2 && erreurAngulaire <= M_PI/2) || (!(mode & MARCHE_ARRIERE) && erreurLineaire > distanceAjustement))
                {
                    //Si l'objectif est devant, ou si la marche arrière n'est pas autorisée et qu'on n'est pas suffisament près de l'objectif pour considérer ça comme un ajustement de position
                    //On va vers l'objectif en marche avant
                    erreurLineaire=-erreurLineaire;

                    double correctionAngulaire=CorrectionAngulaire(erreurAngulaire, (erreurAngulaire-erreurAngulairePrecedente)/delta_t);
                    if(std::abs(correctionAngulaire) > ROBOT_UNICYCLE_VITESSE_LINEAIRE_ROUE_ROTATION_MAX)
                        std::cout<<"angle "<<correctionAngulaire<<std::endl;
                    double correctionLineaire=(mode & RALENTIR_A_L_ARRIVEE)?
                                              CorrectionLineaire(erreurLineaire, (erreurLineaire-erreurLineairePrecedente)/delta_t)
                                              : -ROBOT_UNICYCLE_VITESSE_TRANSLATION_MAX;

                    double facteur=1-std::abs(correctionAngulaire)/ROBOT_UNICYCLE_VITESSE_LINEAIRE_ROUE_ROTATION_MAX;
                    if(facteur > 0)
                        correctionLineaire*=facteur;
                    else
                        correctionLineaire=0;
                    SetVitessesAngulairesRoues((-correctionLineaire+correctionAngulaire)/ROBOT_UNICYCLE_RAYON_GAUCHE, (-correctionLineaire-correctionAngulaire)/ROBOT_UNICYCLE_RAYON_DROITE);
                }
                else
                {
                    //Sinon on y va en marche arrière

                    erreurAngulaire=BornerA_MoinsPi_Pi(erreurAngulaire-M_PI);

                    double correctionAngulaire=CorrectionAngulaire(erreurAngulaire, (erreurAngulaire-erreurAngulairePrecedente)/tempsEcoule);
                    if(std::abs(correctionAngulaire) > ROBOT_UNICYCLE_VITESSE_LINEAIRE_ROUE_ROTATION_MAX)
                        std::cout<<"angle "<<correctionAngulaire<<std::endl;
                    double correctionLineaire=(mode & RALENTIR_A_L_ARRIVEE)?
                                              CorrectionLineaire(erreurLineaire, (erreurLineaire-erreurLineairePrecedente)/tempsEcoule)
                                              : ROBOT_UNICYCLE_VITESSE_TRANSLATION_MAX;
                    double facteur=1-std::abs(correctionAngulaire)/ROBOT_UNICYCLE_VITESSE_LINEAIRE_ROUE_ROTATION_MAX;
                    if(facteur > 0)
                        correctionLineaire*=facteur;
                    else
                        correctionLineaire=0;
                    SetVitessesAngulairesRoues((-correctionLineaire+correctionAngulaire)/ROBOT_UNICYCLE_RAYON_GAUCHE, (-correctionLineaire-correctionAngulaire)/ROBOT_UNICYCLE_RAYON_DROITE);
                }

                erreurAngulairePrecedente=erreurAngulaire;
                erreurLineairePrecedente=erreurLineaire;
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


double RobotUnicycle::CorrectionAngulaire(double erreur, double derivee_erreur)
{
    double PD=ROBOT_UNICYCLE_KP_ROTATION*erreur+ROBOT_UNICYCLE_KD_ROTATION*derivee_erreur;
    if(PD < -ROBOT_UNICYCLE_VITESSE_LINEAIRE_ROUE_ROTATION_MAX)
        return -ROBOT_UNICYCLE_VITESSE_LINEAIRE_ROUE_ROTATION_MAX;
    else if (PD> ROBOT_UNICYCLE_VITESSE_LINEAIRE_ROUE_ROTATION_MAX)
        return ROBOT_UNICYCLE_VITESSE_LINEAIRE_ROUE_ROTATION_MAX;
    else return PD;
}

double RobotUnicycle::CorrectionLineaire(double erreur, double derivee_erreur)
{
    double PD=ROBOT_UNICYCLE_KP_TRANSLATION*erreur+ROBOT_UNICYCLE_KD_TRANSLATION*derivee_erreur;
    if(PD < -ROBOT_UNICYCLE_VITESSE_TRANSLATION_MAX)
        return -ROBOT_UNICYCLE_VITESSE_TRANSLATION_MAX;
    else if (PD> ROBOT_UNICYCLE_VITESSE_TRANSLATION_MAX)
        return ROBOT_UNICYCLE_VITESSE_TRANSLATION_MAX;
    else return PD;
}

