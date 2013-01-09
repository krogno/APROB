/*
Copyright (c) 2012, Romain BREGIER
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    Neither the name of the Atelier Piston Robotique (APROB) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef TRAITEMENT_WEBCAM
#define TRAITEMENT_WEBCAM

#include <SFML/System.hpp>
#include "aruco/cameraparameters.h"
#include "2D/Objet.h"
#include <map>


/**
Classe gérant la lecture en temps "reel" d un flux video par webcam et pouvant realiser des traitements dessus
**/
class TraitementWebcam : public sf::Thread
{
    public :
    /**
    Instancie une classe TraitementWebcam
    numero : le numero de la webcam
    affichage : l image de la camera doit elle etre affichee?
    detectionMarker : la detecton des fiducial markers est elle activee?
    **/
    TraitementWebcam(int numero, const char * fichierParametresIntrinseques, bool affichage=true);

    ///Position de la webcam dans le repere du robot
    Objet positionWebcam;

    ///Map des balises (la balise balises[124] correspond a la balise dont le numero est 124)
    std::map<int, Objet> balises;

    ///Dimension des marqueurs ARUCO utilisés (en metres de meme que la calibration camera. Sinon meme avec les memes unites que celles de la calibration de la camera les resultats seront faux)
    double dimension_marqueur;

    ~TraitementWebcam();

    ///Lancer le thread de traitement
    void Run();
    ///Arreter le thread
    void Stop();

    protected:
    int numeroWebcam;
    bool affichageImage;

    bool continuer;

    //Parametres de la camera
    aruco::CameraParameters theCameraParameters;
};

#endif
