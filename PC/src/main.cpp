/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <SFML/Graphics.hpp>
#include "Carte2D/Carte2D.h"
#include "Deplacement/RobotUnicycleVirtuel.h"
#include "Deplacement/RobotUnicycleCarteArduino.h"

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

    Objet point;
    point.x=0;
    point.y=0;
    carte.AjouterObjet(&point, &spriteCroix);

    //Création d'un faux robot ayant un retard d'asservissement en vitesse typique de 0.5s
    RobotUnicycleVirtuel fauxRobot(1.0,0.9,1.0);
    //RobotUnicycleCarteArduino fauxRobot;
    fauxRobot.Launch();
    carte.AjouterObjet(&fauxRobot, &spriteBete);

    Objet point2;
    point2.x=2000;
    point2.y=0;
    carte.AjouterObjet(&point2, &spriteCroix);


    fauxRobot.AjouterCiblePosition(CiblePosition(point2.x, point2.y, MARCHE_AVANT,50.0));

    fauxRobot.AjouterCibleOrientation(CibleOrientation(-M_PI));
    Objet point3;
    point3.x=2000;
    point3.y=-1000;
    carte.AjouterObjet(&point3, &spriteCroix);
    fauxRobot.AjouterCiblePosition(CiblePosition(point3.x,point3.y, MARCHE_AVANT,50));
    Objet point4;
    point4.x=300;
    point4.y=-1500;
    carte.AjouterObjet(&point4, &spriteCroix);
    fauxRobot.AjouterCiblePosition(CiblePosition(point4.x,point4.y, MARCHE_AVANT,50));
    Objet point5;
    point5.x=800;
    point5.y=-1000;
    carte.AjouterObjet(&point5, &spriteCroix);
    fauxRobot.AjouterCiblePosition(CiblePosition(point5.x,point5.y, MARCHE_ARRIERE,0));

    sf::Sleep(3);
    fauxRobot.Pause();
    sf::Sleep(2);
    fauxRobot.Reprendre();
    sf::Sleep(10);
    fauxRobot.Stop();


    char a;
    cout << "Fini! Entrer un caractere pour continuer" << endl;
    cin>>a;
    fauxRobot.Stop();
    return 0;

}
