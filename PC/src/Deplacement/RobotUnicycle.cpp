#include "RobotUnicycle.h"
#include "Constantes.h"
#include "angle.h"
#include <iostream>

RobotUnicycle::RobotUnicycle()
{
    sf::Lock lock(mutexMotificationConsignes);
    isThreadRunning=false;
    pause=false;
    ReinitialiserErreurs();
}

RobotUnicycle::~RobotUnicycle()
{
    isThreadRunning=false;
    Wait();
}


void RobotUnicycle::AjouterCiblePosition(CiblePosition cible)
{
    sf::Lock lock(mutexMotificationConsignes);
    cibles.push_back(new CiblePosition(cible));
}


void RobotUnicycle::Pause()
{
    sf::Lock lock(mutexMotificationConsignes);
    pause=true;
}

void RobotUnicycle::Reprendre()
{
    sf::Lock lock(mutexMotificationConsignes);
    pause=false;
}

void RobotUnicycle::Stop()
{
    sf::Lock lock(mutexMotificationConsignes);
    //Vide la liste de cibles
    while(!cibles.empty())
    {
        delete cibles.front();
        cibles.pop_front();
    }
    SetVitesses(0,0);
}

void RobotUnicycle::AjouterCibleOrientation(const CibleOrientation& cible)
{
    sf::Lock lock(mutexMotificationConsignes);
    cibles.push_back(new CibleOrientation(cible));
}

bool RobotUnicycle::isArrete()
{
    return ((std::abs(vitesse) < ROBOT_UNICYCLE_EPSILON_VITESSE) && (std::abs(omega) < ROBOT_UNICYCLE_EPSILON_OMEGA));
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


            //Si le robot n'est pas en pause et s'il y a une cible a atteindre de definie : asservissement
            if(!(pause || cibles.empty()))
            {
                //Asservissement et mise à jour des consignes de vitesse des moteurs
                switch(cibles.front()->getType())
                {
                case ORIENTATION:
                {
                    CibleOrientation *cible=(CibleOrientation*) cibles.front();
                    erreurAngulaire=BornerA_MoinsPi_Pi(theta-cible->getTheta());
                    double correctionAngulaire=CorrectionAngulaire(delta_t);
                    SetVitesses(0, -correctionAngulaire);

                    //Test de si la cible a ete atteinte : erreurAngulaire et vitesse de rotation suffisamment faible
                    if(std::abs(erreurAngulaire) <= ROBOT_UNICYCLE_PRECISION_ANGLE && isArrete())
                    {
                        //S'il y a une cible qui suit, on supprime la courante pour passer a la suivante
                        if(cibles.size() > 1)
                        {
                            delete cible;
                            cibles.pop_front();
                            ReinitialiserErreurs();
                        }
                    }

                }
                break;
                case POSITION:
                {
                    CiblePosition* cible=(CiblePosition*) cibles.front();

                    double delta_x, delta_y, distanceRestanteCarre;

                    delta_x=cible->getX()-x;
                    delta_y=cible->getY()-y;

                    distanceRestanteCarre=delta_x*delta_x+delta_y*delta_y;

                    /*Test de si la cible courante est atteinte. Les criteres sont :
                    - la distance a la cible est inferieure a la precision souhaitee
                    - le robot est arrete si la vitesse de passage souhaitee etait 0
                    */
                    if(distanceRestanteCarre <= cible->getPrecisionCarre())
                    {
                        //On est dans la zone cible : envoi de consignes nulles aux moteurs
                        SetVitesses(0, 0);

                        //S'il y a encore une cible sur la liste apres, on supprime la cible courante et on passe a la suivante
                        if(cibles.size() > 1)
                        {
                            //Suppression de la cible courante
                            delete cible;
                            cibles.pop_front();
                            ReinitialiserErreurs();


                        }
                        else
                        {
                            /*Sinon, etant donne qu'il faut bien faire quelque chose,
                            alteration de la cible : on doit l'atteindre avec une vitesse nulle (i.e. s arreter)*/
                            cible->setVitessePassage(0);
                        }

                    }
                    else
                    {
                        //Si on n'est pas dans la cible : asservissement des moteurs

                        //Erreur lineaire : projection sur l'axe du robot de la distance restante a parcourir
                        double erreurLineaire=(cos(theta)*delta_x+sin(theta)*delta_y);

                        erreurAngulaire=BornerA_MoinsPi_Pi(theta-atan2(delta_y, delta_x));
                        double cosinusErreurAngulaire=cos(erreurAngulaire);

                        if(((erreurLineaire >=0) && (cible->isMarcheAvantAutorisee() || (distanceRestanteCarre < ROBOT_UNICYCLE_DISTANCE_AJUSTEMENT))) || !cible->isMarcheArriereAutorisee() )
                        {
                            //Si l'objectif est devant et que (la marche avant est autorisee ou si on est suffisamment pres pour considerer qu'on aie le droit d'ajuster la position en marche avant)
                            //ou sinon si la marche arriere n'est pas autorisee (et du coup soit la cible est derriere ou on est trop loin pour considerer que ce serait qu'un ajustement)
                            //On y va en marche avant
                            //Note: on suppose que si la marche arriere n'est pas autorisee, la marche avant l'est

                            //Mise a jour de la consigne de vitesse
                            double consigneVitesse;
                            //Dans le cas general on accelere
                            consigneVitesse=consigneVitessePrecedente+ROBOT_UNICYCLE_ACCELERATION_MAX*delta_t;
                            //saturation
                            if(consigneVitesse> ROBOT_UNICYCLE_VITESSE_MAX)
                                consigneVitesse=ROBOT_UNICYCLE_VITESSE_MAX;
                            //Sauf si on est suffisamment pres de la cible auquel cas il y a une rampe de vitesse de la vitesse max jusqu a la vitesse de passage
                            if(erreurLineaire < ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT)
                            {
                                double consigneVitesseFreinage=cible->getVitessePassage()+(ROBOT_UNICYCLE_VITESSE_MAX-cible->getVitessePassage())*erreurLineaire/ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT;
                                //On prend la valeur la plus petite
                                if(consigneVitesseFreinage < consigneVitesse)
                                    consigneVitesse=consigneVitesseFreinage;
                            }

                            //Calcul de l erreur de vitesse
                            erreurVitesse=consigneVitesse-vitesse;


                            //Mise a jour de la variable de consigne de vitesse precedente
                            consigneVitessePrecedente=consigneVitesse;
                        }
                        else
                        {
                            //Sinon on y va en marche arrière
                            //redefinition de l erreur angulaire
                            erreurAngulaire=BornerA_MoinsPi_Pi(erreurAngulaire-M_PI);
                            cosinusErreurAngulaire=-cosinusErreurAngulaire;

                            //Mise a jour de la consigne de vitesse
                            double consigneVitesse;
                            //Dans le cas general on accelere vers l arriere
                            consigneVitesse=consigneVitessePrecedente-ROBOT_UNICYCLE_ACCELERATION_MAX*delta_t;
                            //saturation
                            if(consigneVitesse< -ROBOT_UNICYCLE_VITESSE_MAX)
                                consigneVitesse=-ROBOT_UNICYCLE_VITESSE_MAX;
                            //Sauf si on est suffisamment pres de la cible auquel cas on adapte sa vitesse progressivement jusqu a la vitesse de passage
                            if(erreurLineaire < ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT)
                            {
                                double consigneVitesseFreinage=-cible->getVitessePassage()-(-ROBOT_UNICYCLE_VITESSE_MAX+cible->getVitessePassage())*erreurLineaire/ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT;
                                //On prend la valeur la plus grande
                                if(consigneVitesseFreinage > consigneVitesse)
                                    consigneVitesse=consigneVitesseFreinage;
                            }

                            //Calcul de l erreur de vitesse
                            erreurVitesse=consigneVitesse-vitesse;


                            //Mise a jour de la variable de consigne de vitesse precedente
                            consigneVitessePrecedente=consigneVitesse;
                        }


                        double correctionAngulaire=CorrectionAngulaire(delta_t);
                        double correctionVitesse=CorrectionVitesse(delta_t);

                        //std::cout<<"CONSIGNE "<<consigneVitesse<<std::endl<<"\tVITESSE "<<vitesse<<"\tERREUR "<<erreurVitesse<<"\tCORRECTION "<<correctionVitesse<<std::endl;

                        SetVitesses(correctionVitesse, -correctionAngulaire);
                    }

                }
                break;
                default:
                    break;
                }
            }
            else
            {
                //S'il n'y a pas de cibles, consigne de vitesse nulle des 2 moteurs
                SetVitesses(0,0);
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
    if(PID < -ROBOT_UNICYCLE_CONSIGNE_OMEGA_MAX)
        return -ROBOT_UNICYCLE_CONSIGNE_OMEGA_MAX;
    else if (PID> ROBOT_UNICYCLE_CONSIGNE_OMEGA_MAX)
        return ROBOT_UNICYCLE_CONSIGNE_OMEGA_MAX;
    else return PID;


}

double RobotUnicycle::CorrectionVitesse(double delta_t)
{
    /*L estimation de la derivee de l erreur de vitesse est trop sensible aux differents bruits de mesure et de calcul:
    Il suffit que pour un intervalle de temps de nombreux crans aient ete decomptes et que pour celui suivant ca ne soit pas le cas pour generer des valeurs anormalement elevee de derivee de l' erreur
    Aussi ici un PI est utilise et pas un PID
    */

    primitiveErreurVitesse+=erreurVitesse*delta_t;

    if(primitiveErreurVitesse > ROBOT_UNICYCLE_SEUIL_SATURATION_I_VITESSE)
        primitiveErreurVitesse=ROBOT_UNICYCLE_SEUIL_SATURATION_I_VITESSE;
    else if(primitiveErreurVitesse < -ROBOT_UNICYCLE_SEUIL_SATURATION_I_VITESSE)
        primitiveErreurVitesse=-ROBOT_UNICYCLE_SEUIL_SATURATION_I_VITESSE;


    double PI=ROBOT_UNICYCLE_KP_VITESSE*erreurVitesse
              +ROBOT_UNICYCLE_KI_VITESSE*primitiveErreurVitesse;

    erreurVitessePrecedente=erreurVitesse;

    return PI;
}

void RobotUnicycle::ReinitialiserErreurs()
{
    erreurAngulaire=0;
    erreurAngulairePrecedente=0;
    primitiveErreurAngulaire=0;

    erreurVitesse=0;
    erreurVitessePrecedente=0;
    primitiveErreurVitesse=0;
}
