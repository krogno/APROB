typedef struct vecteur3 vecteur3;
struct vecteur3
{
    double x;
    double y;
    double z;
};

typedef struct vecteur2 vecteur2;
struct vecteur2
{
    double x;
    double y;
};


double heron(double x)
{
    double un = 300;
    double vn = x/un;

// Précision de la méthode de héron (à potentiellement changer pour diminuer le temps d'éxécution)
    double e = 0.01;

    while(abs(un-vn) > e)
    {
        un = (un + vn)/2;
        vn = x/un;
    }

    return un;
}



double dist(double x1, double y1, double x2, double y2)
{
    return heron((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}

vecteur2 dichotomie(double x0, double y0, double la, double lo, double d1, double d2, double d3)
{
    double A, B, C, D;

    A = (dist(0  , 0  , x0-la/4, y0-lo/4) - d1)*(dist(0  , 0  , x0-la/4, y0-lo/4) - d1) + (dist(200, 0  , x0-la/4, y0-lo/4) - d2)*(dist(200, 0  , x0-la/4, y0-lo/4) - d2) + (dist(100, 300, x0-la/4, y0-lo/4) - d3)*(dist(100, 300, x0-la/4, y0-lo/4) - d3);
    B = (dist(0  , 0  , x0+la/4, y0-lo/4) - d1)*(dist(0  , 0  , x0+la/4, y0-lo/4) - d1) + (dist(200, 0  , x0+la/4, y0-lo/4) - d2)*(dist(200, 0  , x0+la/4, y0-lo/4) - d2) + (dist(100, 300, x0+la/4, y0-lo/4) - d3)*(dist(100, 300, x0+la/4, y0-lo/4) - d3);
    C = (dist(0  , 0  , x0+la/4, y0+lo/4) - d1)*(dist(0  , 0  , x0+la/4, y0+lo/4) - d1) + (dist(200, 0  , x0+la/4, y0+lo/4) - d2)*(dist(200, 0  , x0+la/4, y0+lo/4) - d2) + (dist(100, 300, x0+la/4, y0+lo/4) - d3)*(dist(100, 300, x0+la/4, y0+lo/4) - d3);
    D = (dist(0  , 0  , x0-la/4, y0+lo/4) - d1)*(dist(0  , 0  , x0-la/4, y0+lo/4) - d1) + (dist(200, 0  , x0-la/4, y0+lo/4) - d2)*(dist(200, 0  , x0-la/4, y0+lo/4) - d2) + (dist(100, 300, x0-la/4, y0+lo/4) - d3)*(dist(100, 300, x0-la/4, y0+lo/4) - d3);

    vecteur2 N;
    N.x = 1;
    N.y = 1;

    if(A > B)
    {
        A = B;
        N.x = 2;
        N.y = 1;
    }
    if(A > C)
    {
        A = C;
        N.x = 2;
        N.y = 2;
    }
    if(A > D)
    {
        N.x =1;
        N.y = 2;
    }

    return N;
}


vecteur2 localisation(double d1, double d2, double d3)
{
    vecteur2 La;
    La.x = 200;
    La.y = 300;

    vecteur2 C;
    C.x = La.x/2;
    C.y = La.y/2;

    int i = 0;
    vecteur2 N;


    for(i = 1 ; i <= 8 ; i++)
    {
        N = dichotomie(C.x, C.y, La.x, La.y, d1, d2, d3);
        La.x /= 2;
        La.y /= 2;
        C.x = C.x - La.x/2 + (N.x-1)*La.x;
        C.y = C.y - La.y/2 + (N.y-1)*La.y;
    }

    return C;
}
