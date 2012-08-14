#ifndef DEF_MYLABEL
#define DEF_MYLABEL

#include <QtGui/QWidget>
#include <QtGui/QtGui>

class MyLabel: public QLabel
{
    Q_OBJECT

    public:
    MyLabel();
    MyLabel(QWidget *parent, Qt::WindowFlags f);

    signals:
    void clicked(int,int);

    protected:
    virtual void mouseReleaseEvent(QMouseEvent *);

};
#endif

