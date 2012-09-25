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
    AllerALaPosition(x+distance*cos(theta),y+distance*sin(theta));
}

void RobotUnicycle::Tourner(double angle)
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=TOURNER;
    erreurAngulaire=-angle;
    erreurAngulairePrecedente=erreurAngulaire;
    primitiveErreurAngulaire=0;
}

void RobotUnicycle::AllerALaPosition(double x, double y, double my_precision, int my_mode, double my_distanceAjustement)
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=POSITION;
    x_objectif=x;
    y_objectif=y;
    mode=my_mode;
    precisionPositionCarre=my_precision*my_precision;
    distanceAjustementCarre=my_distanceAjustement*my_distanceAjustement;

    erreurAngulaire=0;
    erreurAngulairePrecedente=0;
    primitiveErreurAngulaire=0;

    erreurLineaire=0;
    erreurLineairePrecedente=0;
}

void RobotUnicycle::Stopper()
{
    sf::Lock lock(mutexMotificationConsignes);
    consigne=STOP;
    SetVitesses(0,0);
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
    primitiveErreurAngulaire=0;
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
                SetVitesses(0,0);
                break;
            case ROUE_LIBRE:
                SetMoteursEnModeRouesLibres();
                break;
            case AVANCER:
            {
                erreurLineaire+=delta_avance;
                double correctionLineaire=CorrectionLineaire(delta_t);
                SetVitesses(-correctionLineaire, 0);
                if(std::abs(erreurLineaire) <= ROBOT_UNICYCLE_PRECISION_ANGLE && isArrete())
                {
                    consigne=STOP;
                    SetVitesses(0,0);
                }
                erreurLineairePrecedente=erreurLineaire;
                break;
            }
            case TOURNER:
            {
                erreurAngulaire+=delta_theta;
                std::cout<<"erreur "<<erreurAngulaire<<std::endl;
                double correctionAngulaire=CorrectionAngulaire(delta_t);
                SetVitesses(0, -correctionAngulaire);
                if(std::abs(erreurAngulaire) <= ROBOT_UNICYCLE_PRECISION_ANGLE && isArrete())
                {
                    consigne=STOP;
                    SetVitesses(0,0);
                }
                break;
            }

            case POSITION:
            {
                double delta_x=x_objectif-x;
                double delta_y=y_objectif-y;

                double distanceRestanteCarre=delta_x*delta_x+delta_y*delta_y;

                //Erreur lineaire : projection sur l'axe du robot de la distance restante a parcourir
                erreurLineaire=-(cos(theta)*delta_x+sin(theta)*delta_y);
std::cout<<"lin "<<erreurLineaire<<std::endl;
                if(distanceRestanteCarre <= precisionPositionCarre)
                {
                    SetVitesses(0,0);
                    if(!(mode & RALENTIR_A_L_ARRIVEE) || isArrete())
                    {
                        consigne=STOP;
                    }
                    break;
                }

                erreurAngulaire=BornerA_MoinsPi_Pi(theta-atan2(delta_y, delta_x));

                if(((erreurAngulaire >= -M_PI/2 && erreurAngulaire <= M_PI/2) && ((mode & MARCHE_AVANT) || (distanceRestanteCarre < distanceAjustementCarre))) || !(mode & MARCHE_ARRIERE))
                {
                    //Si l'objectif est devant, ou si la marche arrière n'est pas autorisée et qu'on n'est pas suffisament près de l'objectif pour considérer ça comme un ajustement de position
                    //On va vers l'objectif en marche avant

                    double correctionAngulaire=CorrectionAngulaire(delta_t);
                    double correctionLineaire=(mode & RALENTIR_A_L_ARRIVEE)?
                                              CorrectionLineaire(delta_t)
                                              : -ROBOT_UNICYCLE_VITESSE_MAX;
std::cout<<"cor "<<-correctionLineaire<<std::endl;
                    SetVitesses(-correctionLineaire, -correctionAngulaire);
                }
                else
                {
                    //Sinon on y va en marche arrière

                    erreurAngulaire=BornerA_MoinsPi_Pi(erreurAngulaire-M_PI);

                    double correctionAngulaire=CorrectionAngulaire(delta_t);
                    double correctionLineaire=(mode & RALENTIR_A_L_ARRIVEE)?
                                              CorrectionLineaire(delta_t)
                                              : ROBOT_UNICYCLE_VITESSE_MAX;

                    SetVitesses(-correctionLineaire, -correctionAngulaire);
                }
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


double RobotUnicycle::CorrectionAngulaire(double delta_t)
{

    primitiveErreurAngulaire+=erreurAngulaire*delta_t;
    if(primitiveErreurAngulaire > ROBOT_UNICYCLE_SEUIL_SATURATION_I_ROTATION)
        primitiveErreurAngulaire=ROBOT_UNICYCLE_SEUIL_SATURATION_I_ROTATION;
    else if (primitiveErreurAngulaire<-ROBOT_UNICYCLE_SEUIL_SATURATION_I_ROTATION)
        primitiveErreurAngulaire=-ROBOT_UNICYCLE_SEUIL_SATURATION_I_ROTATION;

    double PID=ROBOT_UNICYCLE_KP_ROTATION*erreurAngulaire
                +ROBOT_UNICYCLE_KI_ROTATION*primitiveErreurAngulaire
                +ROBOT_UNICYCLE_KD_ROTATION*(erreurAngulaire-erreurAngulairePrecedente)/delta_t;

    erreurAngulairePrecedente=erreurAngulaire;

//std::cout<<"erreur "<<erreur<<std::cout<<"\t ang "<<erreurAngulaire<<"\t primitive "<<primitiveErreurAngulaire<<std::endl;
    if(PID < -ROBOT_UNICYCLE_OMEGA_MAX)
        return -ROBOT_UNICYCLE_OMEGA_MAX;
    else if (PID> ROBOT_UNICYCLE_OMEGA_MAX)
        return ROBOT_UNICYCLE_OMEGA_MAX;
    else return PID;


}

double RobotUnicycle::CorrectionLineaire(double delta_t)
{
    double PD=ROBOT_UNICYCLE_KP_TRANSLATION*erreurLineaire+ROBOT_UNICYCLE_KD_TRANSLATION*(erreurLineaire-erreurLineairePrecedente)/delta_t;

    erreurLineairePrecedente=erreurLineaire;
    if(PD < -ROBOT_UNICYCLE_VITESSE_MAX)
        return -ROBOT_UNICYCLE_VITESSE_MAX;
    else if (PD> ROBOT_UNICYCLE_VITESSE_MAX)
        return ROBOT_UNICYCLE_VITESSE_MAX;
    else return PD;
}

