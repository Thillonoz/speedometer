#ifndef WINDOW_H
#define WINDOW_H

#include <QCloseEvent>
#include <QDialog>
#include <QTimer>

#include "canvas.h"
#include "comservice.h"

class Window final : public QDialog {
    Canvas canvas;
    COMService &comservice;
    QTimer update_timer;

    /**
     * @brief Function to set the background color of the main window
     */
    void set_bg_color();

public:
    Window(QWidget *parent, COMService &comservice);

private slots:
    void update_from_service();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif
