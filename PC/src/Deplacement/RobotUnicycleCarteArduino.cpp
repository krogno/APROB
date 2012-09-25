#include "Deplacement/RobotUnicycleCarteArduino.h"

#include <stdexcept>
#include <stdio.h>


RobotUnicycleCarteArduino::RobotUnicycleCarteArduino()
{
    //Ouverture du port serie
    if(serial.Open(ROBOT_UNICYCLE_PORT_SERIE, ROBOT_UNICYCLE_PORT_SERIE_FREQUENCE)!=1)
    {
        //Renvoi d'une exception si erreur
        char buffer[50];
        sprintf(buffer,"Erreur d'ouverture du port serie : %s a la frequence %i",ROBOT_UNICYCLE_PORT_SERIE,ROBOT_UNICYCLE_PORT_SERIE_FREQUENCE);
     //   throw std::runtime_error(buffer);
    }
}


void RobotUnicycleCarteArduino::SetVitesses(double vitesse_avance, double omega)
{
    double part_vitesse_omega=omega*ROBOT_UNICYCLE_DISTANCE_ROUES/2.0;
    int vitesse_gauche=vitesse_avance-part_vitesse_omega;
    int vitesse_droite=vitesse_avance+part_vitesse_omega;

    //Saturation des consignes de vitesse
    if(vitesse_gauche>255)
        vitesse_gauche=255;
    else if(vitesse_gauche<-255)
        vitesse_gauche=-255;

    if(vitesse_droite>255)
        vitesse_droite=255;
    else if(vitesse_droite<-255)
        vitesse_droite=-255;

    char buffer[100];
    sprintf(buffer,"v %i %i\n",vitesse_gauche, vitesse_droite);
    std::cout<<buffer<<std::endl;
    serial.WriteString(buffer);
}


bool SerialToInt32(int32_t & retour, serialib *serial)
{
  retour=0;

  char temp;
  int timeout=100;
  bool negatif=false;
  do
  {
      if(serial->ReadChar(&temp, timeout)==1)
      {
        if(temp=='-')
            negatif=true;
        else if(temp>='0' && temp <='9')
            retour=retour*10+(temp-'0');
      }
      else
        return false;
      //Si il y a un autre caractere on devrait trouver le moyen de generer une erreur
  } while(temp!='\n' && temp!=' ');

  if(negatif)
    retour=-retour;
  return true;
}


void RobotUnicycleCarteArduino::GetDeplacement(double& delta_avance, double& delta_theta)
{
    static long old_cransGauche=0;
    static long old_cransDroite=0;

    static const double facteur_avance=2.0*M_PI/ROBOT_UNICYCLE_NOMBRE_CRANS_PAR_TOUR*ROBOT_UNICYCLE_RAYON_ROUE/2.0;
    static const double facteur_theta=2.0*M_PI/ROBOT_UNICYCLE_NOMBRE_CRANS_PAR_TOUR*ROBOT_UNICYCLE_RAYON_ROUE/ROBOT_UNICYCLE_DISTANCE_ROUES;


    serial.WriteString("?\n");
    std::cout<<"Demande valeurs encodeurs"<<std::endl;

    int32_t cransGauche;
    int32_t cransDroite;

    if(SerialToInt32(cransGauche, &serial) && SerialToInt32(cransDroite, &serial))
    {
        std::cout<<"gauche:"<<cransGauche<<"\tdroite:"<<cransDroite<<std::endl;
        //La lecture des crans decomptes par les encodeurs s'est bien deroulee
        int32_t deltaGauche=cransGauche-old_cransGauche;
        int32_t deltaDroite=cransDroite-old_cransDroite;

        delta_avance=(deltaGauche+deltaDroite)*facteur_avance;
        delta_theta=(deltaDroite-deltaGauche)*facteur_theta;

        old_cransGauche=cransGauche;
        old_cransDroite=cransDroite;
    }
    else
    {
        std::cout<<"Erreur lors de la recuperation de la valeur des encodeurs"<<std::endl;
    }
}

void RobotUnicycleCarteArduino::SetMoteursEnModeRouesLibres()
{
    serial.WriteString("v 0 0\n");
}
