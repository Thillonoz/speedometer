#include "window.h"

#include "setting.h"

#include <QPalette>

Window::Window(QWidget *parent): QDialog(parent) {
    setWindowTitle("Client"); // it shall take it from the settings
    setFixedSize(800, 600); // it shall take it from the settings
    setWindowFlags(Qt::WindowStaysOnTopHint);

    canvas = new Canvas(this);

    set_bg_color();
}


void Window::set_bg_color() {
    QPalette palette = QPalette();
    palette.setColor(QPalette::Window, QColor(122, 29, 191));

    setAutoFillBackground(true);
    setPalette(palette);
}

void Window::set_speed(const int speed) const {
    canvas->set_speed(speed);
}
