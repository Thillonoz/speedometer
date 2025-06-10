#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>

#include "canvas.h"

class Window final : public QDialog
{
    Canvas *canvas;

public:
    explicit Window(QWidget *parent = nullptr);

    void set_bg_color();

    void set_speed(const int speed) const;

    void set_temperature(const int temperature) const;

    void set_battery(const int battery_percent) const;

    void set_blinker(const int position) const;
};

#endif
