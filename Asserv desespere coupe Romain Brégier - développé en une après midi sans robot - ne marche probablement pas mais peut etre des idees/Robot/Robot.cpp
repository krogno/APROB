#include "Robot.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include "angle_toolbox.h"

using namespace std;

#define SERIAL_TIMEOUT 200 //Nombre de ms ou on attend au max pour recevoir la valeur des encodeurs



#define RAYON_ZONE_POSITION_CARRE RAYON_ZONE_POSITION*RAYON_ZONE_POSITION

inline double PID_Angle(double epsilon)
{
    static double old_epsilon=0, integrale=0, derivee=0;

    derivee=epsilon-old_epsilon;
    integrale=max(min(integrale+epsilon,SATURATION_ANGLE_I),-SATURATION_ANGLE_I);

    old_epsilon=epsilon;

    return K_ANGLE_P*epsilon+K_ANGLE_I*integrale+K_ANGLE_D*derivee;
}



void Robot::operator()()
{
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::milliseconds milliseconds;
    Clock::time_point last_time = Clock::now();

    char buffer[128]="";

    while(isThreadRunning)
    {
        Clock::time_point current_time = Clock::now();
        milliseconds ms = std::chrono::duration_cast<milliseconds>(current_time-last_time);

        //Temps qu il reste a attendre avant la prochaine boucle en ms
        int delta_t=periode-ms.count();
        //cout<<delta_t<<" "<<periode<<" "<<clock()<<" "<<last_time<<endl;

        //On teste si il s est ecoule une periode ou plus
        if(delta_t<0)
        {
            //Si oui
            last_time=current_time;

            //Demande la valeur des encodeurs
            if(LS.Write("e\r\n",3)!=1)
                cout<<"Erreur lors de l envoi d'une requete de valeur des encodeurs"<<endl;
            //Reception de la valeur
            if(LS.ReadString(buffer,'\n',sizeof(buffer),SERIAL_TIMEOUT)<=0)
                cout<<"Pas de reponse suffisamment rapide a la requete de valeur des encodeurs"<<endl;
            else
            {
                //Lit du buffer la valeur des encodeurs
                string myString =buffer;
                istringstream sstr(myString);
                int new_cransGauche=0, new_CransDroit=0;
                sstr >>new_cransGauche>>new_CransDroit;
                cout<<"Encodeurs : "<<new_cransGauche<<" "<<new_CransDroit<<endl;

                //Met a jour la position et les valeurs des encodeurs en memoire
                MiseAJour(new_cransGauche-cransGauche,new_CransDroit-cransDroit);

                if(isBonnePosition())
                {
                    if(isBonneOrientation())
                    {
                        EnvoyerConsigneVitesses(0,0);
                    }
                    else
                    {//On est bien positionne, faut s orienter
                       double deltaTheta=DoubleToAngle(consigne_theta-theta);
                       double correctionAngle=PID_Angle(deltaTheta); //Si deltaTheta est positif, le moteur gauche doit aller en avant
                       double vg=correctionAngle;
                       double vd=-correctionAngle;
                       EnvoyerConsigneVitesses(vg,vd);

                    }


                }
                else
                {
                    //Vecteur dont on doit se deplacer pour etre bien positionne
                    double delta_x=consigne_x-x;
                    double delta_y=consigne_y-y;

                    double thetaObjectif=atan2(delta_y,delta_x);

                    //Erreur d angle pour etre en face de l objectif
                    double deltaTheta=DoubleToAngle(thetaObjectif-theta);


                    if(abs(deltaTheta)<=M_PI/2)
                    {//L objectif est devant
                        //On y va en marche avant

                        double correctionAngle=PID_Angle(deltaTheta); //deltaTheta positif veut dire qu'il faut que le moteur gauche aille plus vite
                        double vg=VITESSE_MAX+correctionAngle;
                        double vd=VITESSE_MAX-correctionAngle;
                        double normaliseur=max(vg,vd);
                        vg/=normaliseur;
                        vd/=normaliseur;

                        EnvoyerConsigneVitesses(vg, vd);
                    }
                    else
                    {//L objectif est derriere
                        //On y va en marche arriere

                        double correctionAngle=PID_Angle(DoubleToAngle(deltaTheta+M_PI)); //L angle de consigne est donc decale de PI
                        //s'il est positif le moteur gauche doit aller plus vite (en valeur absolue)
                        double vg=-(VITESSE_MAX+correctionAngle);
                        double vd=-(VITESSE_MAX-correctionAngle);
                        double normaliseur=max(vg,vd);
                        vg/=normaliseur;
                        vd/=normaliseur;

                        EnvoyerConsigneVitesses(vg, vd);
                    }

                }




            }
        }
        else
        {
            //sinon on attend
            std::this_thread::sleep_for(std::chrono::milliseconds(delta_t));

        }

    }
}



bool Robot::isBonnePosition()
{
    return (!isConsignePosition || (x-consigne_x)*(x-consigne_x)+(y-consigne_y)*(y-consigne_y)<RAYON_ZONE_POSITION_CARRE);
}

bool Robot::isBonneOrientation()
{
    if(isConsigneTheta)
    {
        double deltaTheta=DoubleToAngle(consigne_theta-theta);
        if(abs(deltaTheta)<DEMI_ANGLE_MAX_ORIENTATION)
            return true;
        else return false;
    }
    else return false;
}


void Robot::GotoPosition(double x, double y)
{
    isConsignePosition=true;
    consigne_x=x;
    consigne_y=y;
}

void Robot::GotoOrientation(double theta)
{
    consigne_theta=DoubleToAngle(theta);
    isConsigneTheta=true;

}

void Robot::EnvoyerConsigneVitesses(char vg, char vd)
{
    char consigne[5];
    consigne[0]='g';
    consigne[1]=vg;
    consigne[2]=vd;
    consigne[3]='\r';
    consigne[4]='\n';

    if(LS.Write(consigne,5)!=1)
        cout<<"Erreur lors de l envoi d'une consigne de vitesse"<<endl;
}


void Robot::Stop()
{
    isConsignePosition=false;
    isConsigneTheta=false;
}




void Robot::StartThread()
{
    //Lance un thread de la methode operator()
    if(!isThreadRunning)
    {
        isThreadRunning=true;
        monThread=new std::thread(std::ref(*this));
    }
}

void Robot::StopThread()
{
    if(isThreadRunning)
    {
        isThreadRunning=false;
        monThread->join(); //Attend que le thread soit fini
        delete monThread;
    }
}

//Reste dans la plage [0, 2PI]
void Robot::set_theta(double theta)
{
    this->theta=DoubleToAngle(theta);
}


void Robot::set_rayonEncodeurs(double rayonMoyen, double ajustement)
{
    if(rayonMoyen-ajustement>0.0)
    {
        rayonEncodeurDroit=rayonMoyen+ajustement;
        rayonEncodeurGauche=rayonMoyen-ajustement;
    }
    else
    {
        rayonEncodeurDroit=rayonEncodeurGauche=100;
    }
    MiseAJourConstantes();
}

void Robot::set_distanceEncodeurs(double distance)
{
    if(distance>0.0)
        distanceEncodeurs=distance;
    else distanceEncodeurs=100;
    MiseAJourConstantes();
}

void Robot::set_nombreCransParTour(double nombreCransParTour)
{
    if(nombreCransParTour>0.0)
        this->nombreCransParTour=nombreCransParTour;
    else this->nombreCransParTour=100;
    MiseAJourConstantes();
}

void Robot::MiseAJourConstantes()
{
    RAYONDROITSURDISTANCEFOIS2PISURNBCRANS=rayonEncodeurDroit/distanceEncodeurs*2*M_PI/nombreCransParTour;
    RAYONGAUCHESURDISTANCEFOIS2PISURNBCRANS=rayonEncodeurGauche/distanceEncodeurs*2*M_PI/nombreCransParTour;
    RAYONDROITSUR2FOIS2PISURNBCRANS=rayonEncodeurDroit*M_PI/nombreCransParTour;
    RAYONGAUCHESUR2FOIS2PISURNBCRANS=rayonEncodeurGauche*M_PI/nombreCransParTour;

}

Robot::Robot(double rayonMoyen, double ajustement, double distanceEncodeurs,double nombreCransParTour, const char * portserie, int baudrate, int periode)
{
    x=0;
    y=0;
    theta=0;
    cransGauche=0;
    cransDroit=0;
    set_rayonEncodeurs(rayonMoyen, ajustement);
    set_distanceEncodeurs(distanceEncodeurs);
    set_nombreCransParTour(nombreCransParTour);
    isThreadRunning=false;
    this->periode=periode;
    isConsignePosition=false;
    isConsigneTheta=false;

    if(LS.Open(portserie,baudrate)!=1)      // Open serial link
    {
        cout<<"Erreur lors de l ouverture du port serie, plus qu a reesayer"<<endl;
    }
}

void Robot::MiseAJour(int DeltaG, int DeltaD)
{
    double DeltaTheta=RAYONDROITSURDISTANCEFOIS2PISURNBCRANS*DeltaD-RAYONGAUCHESURDISTANCEFOIS2PISURNBCRANS*DeltaG;
    double DeltaAvance=RAYONDROITSUR2FOIS2PISURNBCRANS*DeltaD+RAYONGAUCHESUR2FOIS2PISURNBCRANS*DeltaG;
    double DELTATHETASUR2=DeltaTheta/2;

    theta+=DELTATHETASUR2;
    x+=DeltaAvance*cos(theta);
    y+=DeltaAvance*sin(theta);
    set_theta(theta+DELTATHETASUR2);

    cransGauche+=DeltaG;
    cransDroit+=DeltaD;

    AfficherPosition();
}

void Robot::AfficherPosition()
{
    cout<<"[ x="<<x<<"\t y="<<y<<"\t theta="<<theta<<" ]"<<endl;
}

