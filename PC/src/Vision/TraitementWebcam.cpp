#include "TraitementWebcam.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "aruco/aruco.h"
#include <vector>

TraitementWebcam::TraitementWebcam(int numero, const char * filenameCameraParameters, bool affichage)
{
    numeroWebcam=numero;
    affichageImage=affichage;

    if(filenameCameraParameters!=NULL)
        theCameraParameters.readFromXMLFile(filenameCameraParameters);

    continuer=true;
    dimension_marqueur=0.075;
}

void TraitementWebcam::Run()
{
    cv::VideoCapture capture(numeroWebcam);
    if(!capture.isOpened())  // check if we succeeded
    {
        std::cout<<"Impossible d ouvrir en capture la webcam "<<numeroWebcam<<std::endl;
        return;
    }

    //Si demande d affichage, creation d une fenetre
    if(affichageImage)
        cv::namedWindow( "cam", CV_WINDOW_AUTOSIZE );

    //Matrice dans laquelle est stockee l image captee par la webcam
    cv::Mat imgWebcam;

    //Detecteur de marqueurs
    aruco::MarkerDetector markerDetector;

    //Passage de l argument 3 pour un mode de detection rapide, mais pouvant inclure des faux positifs
    //markerDetector.setDesiredSpeed(3);

    //Travaille avec une image de taille divisee par deux 1 fois pour accelerer le processus
    //markerDetector.pyrDown(1);

    //Vecteur des marqueurs detectes
    std::vector<aruco::Marker> markersDetectes;

    cv::Scalar couleurMarqueurs(255,0,0);

    while(continuer)
    {
        //Decimation par 5 : on lit 5 fois l image de la webcam, mais on ne la traite qu une fois
        //for(int j=0;j<5;j++)
        //{
        capture >> imgWebcam;
        //}

        //detection de marqueurs dans l image
        markerDetector.detect(imgWebcam,markersDetectes,theCameraParameters,dimension_marqueur);

        //Iteration sur les marqueurs detectes
        for(std::vector<aruco::Marker>::iterator marker=markersDetectes.begin(); marker!=markersDetectes.end(); marker++)
        {
            std::cout<<"***********marqueur "<<marker->id;//<<"\nTvec "<<std::endl<<marker->Tvec<<"\nRvec "<<marker->Rvec<<std::endl;
            std::cout<<" x "<<balises[marker->id].x<<" size "<<marker->ssize<<std::endl;
            marker->calculateExtrinsics(dimension_marqueur,theCameraParameters);
            std::cout<<marker->Tvec<<std::endl;
        }


        if(affichageImage)
        {
            //Dessin du contour des eventuels marqueurs sur  l image
            for(std::vector<aruco::Marker>::iterator marker=markersDetectes.begin(); marker!=markersDetectes.end(); marker++)
            {
                marker->draw(imgWebcam,couleurMarqueurs);
            }
            //Affichage de l image
            cv::imshow("cam", imgWebcam);
        }
        cv::waitKey(1);
    }

}

void TraitementWebcam::Stop()
{
    continuer=false;
    Wait();
}

TraitementWebcam::~TraitementWebcam()
{
    Stop();
}
