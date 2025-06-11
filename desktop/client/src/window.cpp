#include "window.h"

#include "setting.h"

#include <QPalette>

Window::Window(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Client");
    setFixedSize(800, 600);
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

void Window::set_temperature(const int temperature) const {
    canvas->set_temperature(temperature);
}

void Window::set_battery(const int battery_percent) const {
    canvas->set_battery(battery_percent);
}

void Window::set_blinker(const int blinker_state) const {
    canvas->set_blinker(blinker_state);
}

void Window::set_connection_status(const bool connection_status) const {
    canvas->is_connected(connection_status);
};

