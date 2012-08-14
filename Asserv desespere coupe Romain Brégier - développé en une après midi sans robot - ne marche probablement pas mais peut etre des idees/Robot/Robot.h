#ifndef ROBOT_H
#define ROBOT_H

#include <thread>
#include <cmath>
#include <string>
#include "serialib/serialib.h"

//Valeur entre 0 et 128 permettant de regler la vitesse max des moteurs
// elle correspond au nombre de crans que doit decompter un encodeur pendant une periode d asservissement en vitesse
//Donc il faut mettre une valeur suffisamment faible pour que ce soit realisable
#define VITESSE_MAX 10

//Rayon de la zone ou on considere qu'on est au bon endroit (pas clair je sais)
#define RAYON_ZONE_POSITION 20 //memes unites que partout ailleurs (ex mm)

//Demi-angle d'erreur acceptable en orientation, en radian
#define DEMI_ANGLE_MAX_ORIENTATION  0.3

//PID angle
#define K_ANGLE_P   10
#define K_ANGLE_I   0.1
#define K_ANGLE_D   0
#define SATURATION_ANGLE_I 30.0



//ATTENTION A LA COHERENCE DES UNITES (toujours utiliser des radians et la meme unite de longueur)
class Robot// : public Thread
{
    protected :
    //Position
    double x;
    double y;
    double theta;
    //Nombre de crans decomptes par les encodeurs (a la derniere mise a jour)
    int cransGauche;
    int cransDroit;

    //Constantes propres du robot
    double rayonEncodeurGauche;
    double rayonEncodeurDroit;
    double distanceEncodeurs;
    double nombreCransParTour;

    int periode; //Periode d asservissement en position en ms, doit etre bien superieure a la periode d asservissement en vitesse

    //Constantes uniquement la pour eviter de les calculer a chaque fois
    double RAYONGAUCHESURDISTANCEFOIS2PISURNBCRANS;
    double RAYONDROITSURDISTANCEFOIS2PISURNBCRANS;
    double RAYONGAUCHESUR2FOIS2PISURNBCRANS;
    double RAYONDROITSUR2FOIS2PISURNBCRANS;

    //Met a jour la position du robot et les nombres de crans apres reception d'un certain deplacement effectue
    void MiseAJourConstantes();

    //Thread permettant a l asservissement de tourner comme tache de fond
    std::thread *monThread;
    bool isThreadRunning;

    //Communication serie
    serialib LS; // Object of the serialib class

    //Consigne de position et d angle a atteindre, si definies
    double consigne_x;
    double consigne_y;
    double consigne_theta;
    bool isConsignePosition;
    bool isConsigneTheta;


    void MiseAJour(int DeltaG, int DeltaD);

    /***************************************************************************************************
     *      methodes publiques
     ***************************************************************************************************
     */
    public :


    Robot(double rayonMoyen, double ajustement, double distanceEncodeurs, double nombreCransParTour, const char * portserie, int baudrate, int periode);
    ~Robot(){EnvoyerConsigneVitesses(0,0); StopThread();};

    void StartThread(); //Lance l Thread du robot
    void StopThread();//Arrete l Thread du robot (les moteurs vont continuer a la derniere vitesse qui leur a ete fixee)

    void GotoPosition(double x, double y); //Va a la position x,y
    void GotoOrientation(double theta); //Une fois en position, oriente toi suivant theta
    void Stop(); //Dit au robot d'imposer une vitesse nulle aux moteurs, peut importe ou qu'il soit
    void EnvoyerConsigneVitesses(char vg, char vd); //Envoie les consignes correspondantes de vitesse aux moteurs droit et gauche
    //vg et vd entre -128 et 127, 0 voulant dire vitesse nulle
    //NE FONCTIONNERA PAS SI L ASSERVISSEMENT EST LANCE

    bool isBonnePosition(); //true si le robot a atteint la bonne position ou si pas de consigne fixee
    bool isBonneOrientation();//true si le robot a atteint la bonne orientation ou si pas de consigne fixee

    void set_x(double x){this->x=x;};
    double get_x(){return x;};

    void set_y(double y){this->y=y;};
    double get_y(){return y;};

    void set_theta(double theta);
    double get_theta(){return theta;};



    //Regle le rayon des encodeurs : augmenter l ajustement si le robot vire a droite et vice-versa
    void set_rayonEncodeurs(double rayonMoyen, double ajustement);

    //Regle la distance entre les roues codeuses : l'augmenter si le robot tourne trop, la diminuer sinon
    void set_distanceEncodeurs(double distance);

    void set_nombreCransParTour(double nombreCransParTour);

    void AfficherPosition();


    void operator()(); //Boucle "infinie" qui est appelee pour que le robot s asservisse , A NE PAS APPELER DIRECTEMENT, passer par Start
    //Le compilo me fait une erreur si je la passe en protected
};


#endif

