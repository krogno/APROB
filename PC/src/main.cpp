#include <iostream>
#include <SFML/Graphics.hpp>
#include "Carte2D/Carte2D.h"
#include "Deplacement/RobotUnicycleVirtuel.h"

using namespace std;

int main()
{
    //Chargement des images et création des sprites pour l'affichage
    sf::Image imageCroix;
    if (!imageCroix.LoadFromFile("ressources/croix.tga"))
        return EXIT_FAILURE;
    sf::Sprite spriteCroix(imageCroix);
    spriteCroix.SetCenter(4.5,4.5);
    spriteCroix.SetScale(50.0/imageCroix.GetWidth(), 50.0/imageCroix.GetHeight());

    sf::Image imageBete;
    if (!imageBete.LoadFromFile("ressources/bete.png"))
        return EXIT_FAILURE;
    sf::Sprite spriteBete(imageBete);
    spriteBete.SetCenter(40,30);
    //Bete de 350mm  par 300mm
    spriteBete.SetScale(350.0/imageBete.GetWidth(), 300.0/imageBete.GetHeight());

    //Création de la carte
    Carte2D carte("ressources/terrain2012.png", 93,123,500.0/123, 500.0/123);
    carte.Launch();

    /*fauxRobot.Avancer(1000);
    while(!fauxRobot.isArrete())
    {
        sf::Sleep(0.1);
    }
    fauxRobot.Tourner(-M_PI/2);
    while(!fauxRobot.isArrete())
    {
        sf::Sleep(0.1);
    }
    fauxRobot.Orienter(M_PI/2);
    while(!fauxRobot.isArrete())
    {
        sf::Sleep(0.1);
    }
    fauxRobot.Avancer(-500);
    while(!fauxRobot.isArrete())
    {
        sf::Sleep(0.1);
    }
    fauxRobot.Orienter(-M_PI_4+2*M_PI);
    while(!fauxRobot.isArrete())
    {
        sf::Sleep(0.1);
    }*/
    Objet point;
    point.x=0;
    point.y=0;
    carte.AjouterObjet(&point, &spriteCroix);

    //Création d'un faux robot ayant un retard d'asservissement en vitesse typique de 0.5s
    RobotUnicycleVirtuel fauxRobot(0.5);
    fauxRobot.Launch();
    carte.AjouterObjet(&fauxRobot, &spriteBete);

    Objet point2;
    point2.x=1000;
    point2.y=0;
    carte.AjouterObjet(&point2, &spriteCroix);
    fauxRobot.AllerALaPosition(point2.x,point2.y);
    while(!fauxRobot.isArrete())
        sf::Sleep(0.1);
    Objet point3;
    point3.x=1000;
    point3.y=-500;
    carte.AjouterObjet(&point3, &spriteCroix);
    fauxRobot.AllerALaPosition(point3.x,point3.y);
    while(!fauxRobot.isArrete())
        sf::Sleep(0.1);
    Objet point4;
    point4.x=800;
    point4.y=-200;
    carte.AjouterObjet(&point4, &spriteCroix);
    fauxRobot.AllerALaPosition(point4.x,point4.y);
    while(!fauxRobot.isArrete())
        sf::Sleep(0.1);

    char a;
    cout << "Entrer un caractere pour continuer" << endl;
    cin>>a;


    return 0;
}
