#include "setting.h"
#include "window.h"

#include <QVBoxLayout>
#include <QPalette>
#include <csignal>

static constexpr QColor bg_color = QColor(122, 29, 191);

Window::Window(COMService &comservice) : comservice(comservice) // <-- initialize here
{
    setWindowTitle("Client");
    setFixedSize(800, 600);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    set_bg_color();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    canvas.setParent(this);
    layout->addWidget(&canvas);

    setLayout(layout);

    // Set timer
    connect(&update_timer, &QTimer::timeout, this, &Window::update_from_service);
    update_timer.start(Setting::INTERVAL);
}

void Window::set_bg_color() {
    QPalette palette = QPalette();
    palette.setColor(QPalette::Window, bg_color);

    setAutoFillBackground(true);
    setPalette(palette);
}

void Window::closeEvent(QCloseEvent *event) {
    std::raise(SIGINT);
    event->accept();
}

void Window::update_from_service() {
    canvas.update_all(
        comservice.getSpeed(),
        comservice.getTemperature(),
        comservice.getBatteryLevel(),
        comservice.getLeftLight(),
        comservice.getRightLight(),
        comservice.getStatus()
    );
}
