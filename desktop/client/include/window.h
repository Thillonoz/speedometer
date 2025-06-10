#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>
#include <QTimer>
#include <QGridLayout>
#include "canvas.h"
#include "comservice.h"

class Window : public QDialog
{
    QTimer timer;
    Canvas canvas;
    QGridLayout layout;
    COMService &comService;

public:
    Window(COMService &comservice);

private:
    void updateCanvas(void);
};

#endif