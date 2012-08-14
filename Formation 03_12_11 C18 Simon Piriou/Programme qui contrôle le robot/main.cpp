#include <QtGui/QApplication>
#include "widget.h"

/*
LIBS += -lqextserialportd
 
unix:DEFINES   = _TTY_POSIX_
win32:DEFINES  = _TTY_WIN_
*/


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
