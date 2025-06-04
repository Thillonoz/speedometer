#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>

#include "canvas.h"

class Window final : public QDialog {
    Canvas *canvas;

public:
    explicit Window(QWidget *parent = nullptr);

    void set_bg_color();

    void set_speed(const int speed) const;
};

#endif
