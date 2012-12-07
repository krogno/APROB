#include "TraitementWebcam.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "aruco/aruco.h"
#include <vector>

TraitementWebcam::TraitementWebcam(int numero, const char * filenameCameraParameters, bool affichage, bool detectionMarker)
{
    numeroWebcam=numero;
    affichageImage=affichage;
    this->detectionMarker=detectionMarker;

    if(filenameCameraParameters!=NULL)
        theCameraParameters.readFromXMLFile(filenameCameraParameters);

    continuer=true;
}

void TraitementWebcam::Run()
{
    cv::VideoCapture capture(numeroWebcam);
    if(!capture.isOpened())  // check if we succeeded
    {
        std::cout<<"Impossible d ouvrir en capture la webcam "<<numeroWebcam<<std::endl;
        return;
    }
    capture.set(CV_CAP_PROP_FRAME_WIDTH,320);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);


    //Si demande d affichage, creation d une fenetre
    if(affichageImage)
        cv::namedWindow( "cam", CV_WINDOW_AUTOSIZE );

    //Matrice dans laquelle est stockee l image
    cv::Mat imgWebcam;

    //Detecteur de marqueurs
    aruco::MarkerDetector markerDetector;
    //Mode de detection rapide, mais pouvant inclure des faux positifs
    markerDetector.setDesiredSpeed(3);
    //cv::Point3f position=theCameraParameters.getCameraLocation();
    //std::cout<<"position "<<position<<std::endl;

    //Vecteur des marqueurs detectes
    std::vector<aruco::Marker> markersDetectes;

    cv::Scalar couleurMarqueurs(255,0,0);

    while(continuer)
    {
        capture >> imgWebcam;
        //std::cout<<"CV_CAP_PROP_POS_FRAMES " <<capture.get(CV_CAP_PROP_POS_FRAMES)<<std::endl;

        //Eventuelle detection de marqueurs dans l image
        if(detectionMarker)
        {
            markerDetector.detect(imgWebcam,markersDetectes,theCameraParameters,10);
            for(std::vector<aruco::Marker>::iterator marker=markersDetectes.begin(); marker!=markersDetectes.end(); marker++)
            {
                std::cout<<"***********marqueur "<<marker->id<<"\nTvec "<<std::endl<<marker->Tvec<<"\nRvec "<<marker->Rvec<<std::endl;
            }

        }



        if(affichageImage)
        {
            for(std::vector<aruco::Marker>::iterator marker=markersDetectes.begin(); marker!=markersDetectes.end(); marker++)
            {
                marker->draw(imgWebcam,couleurMarqueurs);
            }
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
