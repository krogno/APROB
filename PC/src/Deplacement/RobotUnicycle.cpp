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
    SetCorrecteurVitesse(1,0,0,1);
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

void RobotUnicycle::Run()
{
    isThreadRunning=true;
    sf::Clock horloge;
    sf::Clock horlogeDeplacement;
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
            double delta_t=horlogeDeplacement.GetElapsedTime();
            horlogeDeplacement.Reset();

            double vitesse=delta_avance/delta_t;
            double omega=delta_theta/delta_t;

            std::cout<<"x "<<x<<"\t\ty "<<y<<"\t\ttheta "<<theta<<std::endl;

            //Si le robot n'est pas en pause et s'il y a une cible a atteindre de definie : asservissement
            if(!(pause || cibles.empty()))
            {
                //Calcul des consignes de vitesse des moteurs
                double consigneOmega, consigneVitesse;

                switch(cibles.front()->getType())
                {
                //CONSIGNE D ORIENTATION
                case ORIENTATION:
                {
                    CibleOrientation *cible=(CibleOrientation*) cibles.front();
                    double erreurAngulaire=BornerA_MoinsPi_Pi(cible->getTheta()-theta);

                    consigneOmega=CalculerConsigneOmega(erreurAngulaire, delta_t);
                    consigneVitesse=CalculerConsigneVitesse(0,0,delta_t);

                    //Test de si la cible a ete atteinte : erreurAngulaire suffisamment faible
                    if(std::abs(erreurAngulaire) <= ROBOT_UNICYCLE_PRECISION_ANGLE)
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
                //CONSIGNE DE POSITION
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
                        consigneOmega=CalculerConsigneOmega(0, delta_t);
                        consigneVitesse=CalculerConsigneVitesse(0, 0, delta_t);

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
                        //Si on n'est pas dans la cible : calcul des consignes de vitesses

                        //Erreur lineaire : projection sur l'axe du robot de la distance restante a parcourir
                        double erreurLineaire=(cos(theta)*delta_x+sin(theta)*delta_y);
                        double erreurAngulaire=BornerA_MoinsPi_Pi(atan2(delta_y, delta_x)-theta);

                        /*
                        Si la marche avant n est pas autorisee et que la cible est devant et est trop loin pour qu on puisse considerer un ajustement en marche avant
                        ou
                        Si la cible est derriere et (la marche arriere est autorisee ou est est suffisamment proche de la cible pour accepter d ajuster sa position par marche arriere)
                        => on va en marche arriere
                        */
                        if((!cible->isMarcheAvantAutorisee() && erreurLineaire >= 0 && (distanceRestanteCarre > ROBOT_UNICYCLE_DISTANCE_AJUSTEMENT*ROBOT_UNICYCLE_DISTANCE_AJUSTEMENT))
                            || ((erreurLineaire < 0) && (cible->isMarcheArriereAutorisee() || (distanceRestanteCarre < ROBOT_UNICYCLE_DISTANCE_AJUSTEMENT*ROBOT_UNICYCLE_DISTANCE_AJUSTEMENT))))
                        {
                            //redefinition de l erreur angulaire
                            erreurAngulaire=BornerA_MoinsPi_Pi(erreurAngulaire-M_PI);
                        }

                        //Mise a jour de la consigne de omega
                        consigneOmega=CalculerConsigneOmega(erreurAngulaire, delta_t);

                        //Mise a jour de la consigne de vitesse
                        consigneVitesse=CalculerConsigneVitesse(erreurLineaire, cible->getVitessePassage(), delta_t);
                    }

                }
                break;
                }

                //Calcul de la correction en omega
                erreurOmega=consigneOmega-omega;
                double correctionOmega=CorrectionOmega(delta_t);

                //Calcul de la correction en vitesse
                erreurVitesse=consigneVitesse-vitesse;
                double correctionVitesse=CorrectionVitesse(delta_t);

                //Envoi des consignes corrigees aux moteurs
                SetVitesses(correctionVitesse, correctionOmega);
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


double RobotUnicycle::CorrectionOmega(double delta_t)
{

    primitiveErreurOmega+=erreurOmega*delta_t;

    if(primitiveErreurOmega > ROBOT_UNICYCLE_SEUIL_SATURATION_I_OMEGA)
        primitiveErreurOmega=ROBOT_UNICYCLE_SEUIL_SATURATION_I_OMEGA;
    else if(primitiveErreurOmega < -ROBOT_UNICYCLE_SEUIL_SATURATION_I_OMEGA)
        primitiveErreurOmega=-ROBOT_UNICYCLE_SEUIL_SATURATION_I_OMEGA;

//std::cout<<"er omega"<<erreurOmega<<"\tprimitive omega "<<primitiveErreurOmega<<std::endl;

    double PI=ROBOT_UNICYCLE_KP_OMEGA*erreurOmega
              +ROBOT_UNICYCLE_KI_OMEGA*primitiveErreurOmega;

    return PI;
}

double RobotUnicycle::CorrectionVitesse(double delta_t)
{
    /*L estimation de la derivee de l erreur de vitesse est trop sensible aux differents bruits de mesure et de calcul:
    Il suffit que pour un intervalle de temps de nombreux crans aient ete decomptes et que pour celui suivant ca ne soit pas le cas pour generer des valeurs anormalement elevee de derivee de l' erreur
    Aussi ici un PI est utilise et pas un PID
    */
    primitiveErreurVitesse+=erreurVitesse*delta_t;

    if(primitiveErreurVitesse > Ksaturation_vitesse)
        primitiveErreurVitesse=Ksaturation_vitesse;
    else if(primitiveErreurVitesse < -Ksaturation_vitesse)
        primitiveErreurVitesse=-Ksaturation_vitesse;

//        std::cout<<"er vit"<<erreurVitesse<<"\tprimitive vit"<<primitiveErreurVitesse<<std::endl;


    double PI=KP_vitesse*erreurVitesse
              +KI_vitesse*primitiveErreurVitesse;
std::cout<<PI<<std::endl;
    return PI;
}

double RobotUnicycle::CalculerConsigneVitesse(const double& erreurLineaire, const double& vitesse_passage,const double& delta_t)
{
    static double consigneVitessePrecedente=0;

    double consigneVitesse;

    if(erreurLineaire==0)
        consigneVitesse=0;
    else if(erreurLineaire >0)
    {
        //Marche avant
        //Dans le cas general on accelere
        consigneVitesse=consigneVitessePrecedente+ROBOT_UNICYCLE_ACCELERATION_MAX*delta_t;
        //saturation
        if(consigneVitesse> ROBOT_UNICYCLE_VITESSE_MAX)
            consigneVitesse=ROBOT_UNICYCLE_VITESSE_MAX;

        //Sauf si on est suffisamment pres de la cible auquel cas il y a une rampe de vitesse de la vitesse max jusqu a la vitesse de passage
        if(erreurLineaire < ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT)
        {
            double consigneVitesseFreinage=vitesse_passage+(ROBOT_UNICYCLE_VITESSE_MAX-vitesse_passage)*erreurLineaire*(1.0/ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT);
            //On prend la valeur la plus petite
            if(consigneVitesseFreinage < consigneVitesse)
                consigneVitesse=consigneVitesseFreinage;
        }


    }
    else
    {
        //Marche arriere
        //Dans le cas general on accelere vers l arriere
        consigneVitesse=consigneVitessePrecedente-ROBOT_UNICYCLE_ACCELERATION_MAX*delta_t;
        //saturation
        if(consigneVitesse< -ROBOT_UNICYCLE_VITESSE_MAX)
            consigneVitesse=-ROBOT_UNICYCLE_VITESSE_MAX;
        //Sauf si on est suffisamment pres de la cible auquel cas on adapte sa vitesse progressivement jusqu a la vitesse de passage
        if(erreurLineaire > -ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT)
        {
            double consigneVitesseFreinage=-vitesse_passage-(-ROBOT_UNICYCLE_VITESSE_MAX+vitesse_passage)*erreurLineaire*(1.0/ROBOT_UNICYCLE_DISTANCE_RALENTISSEMENT);
            //On prend la valeur la plus grande
            if(consigneVitesseFreinage > consigneVitesse)
                consigneVitesse=consigneVitesseFreinage;
        }
    }

    //Mise a jour de la variable de consigne de vitesse precedente
    consigneVitessePrecedente=consigneVitesse;

    return consigneVitesse;
}

double RobotUnicycle::CalculerConsigneOmega(const double& erreurAngulaire, const double &delta_t)
{
    static double consigneOmegaPrecedente=0;

    double consigneOmega;

    if(erreurAngulaire==0)
        consigneOmega=0;
    else if(erreurAngulaire > 0)
    {
        //il faut tourner a gauche
        //Dans le cas general on accelere la rotation
        consigneOmega=consigneOmegaPrecedente+ROBOT_UNICYCLE_ACCELERATION_ANGULAIRE_MAX*delta_t;
        //Saturation
        if(consigneOmega > ROBOT_UNICYCLE_OMEGA_MAX)
            consigneOmega=ROBOT_UNICYCLE_OMEGA_MAX;
        //Sauf si on est suffisamment pres de l angle cible auquel cas on adapte omega progressivement jusqu a 0
        if(erreurAngulaire < ROBOT_UNICYCLE_ANGLE_RALENTISSEMENT)
        {
            double consigneOmegaFreinage=ROBOT_UNICYCLE_OMEGA_MIN+(ROBOT_UNICYCLE_OMEGA_MAX-ROBOT_UNICYCLE_OMEGA_MIN)/ROBOT_UNICYCLE_ANGLE_RALENTISSEMENT*erreurAngulaire;
            //On prend la valeur la plus petite
            if(consigneOmegaFreinage < consigneOmega)
                consigneOmega=consigneOmegaFreinage;
        }
    }
    else
    {
        //il faut tourner a droite
        //Dans le cas general on accelere la rotation dans l autre sens
        consigneOmega=consigneOmegaPrecedente-ROBOT_UNICYCLE_ACCELERATION_ANGULAIRE_MAX*delta_t;
        //Saturation
        if(consigneOmega < -ROBOT_UNICYCLE_OMEGA_MAX)
            consigneOmega=-ROBOT_UNICYCLE_OMEGA_MAX;
        //Sauf si on est suffisamment pres de l angle cible auquel cas on adapte omega progressivement jusqu a 0
        if(erreurAngulaire > -ROBOT_UNICYCLE_ANGLE_RALENTISSEMENT)
        {
            double consigneOmegaFreinage=-ROBOT_UNICYCLE_OMEGA_MIN-(-ROBOT_UNICYCLE_OMEGA_MAX+ROBOT_UNICYCLE_OMEGA_MIN)/ROBOT_UNICYCLE_ANGLE_RALENTISSEMENT*erreurAngulaire;
            //On prend la valeur la plus grande
            if(consigneOmegaFreinage > consigneOmega)
                consigneOmega=consigneOmegaFreinage;
        }
    }

    //Mise a joure de la variable de consigne de omega precedente
    consigneOmegaPrecedente=consigneOmega;

    return consigneOmega;
}

void RobotUnicycle::SetCorrecteurVitesse(double P, double I, double D, double saturation)
{
    KP_vitesse=P;
    KI_vitesse=I;
    KD_vitesse=D;
    Ksaturation_vitesse=saturation;
}

void RobotUnicycle::ReinitialiserErreurs()
{
    erreurOmega=0;
    primitiveErreurOmega=0;

    erreurVitesse=0;
    primitiveErreurVitesse=0;
}
