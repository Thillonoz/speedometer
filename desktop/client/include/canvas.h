#ifndef CANVAS_H
#define CANVAS_H

#include <QPainter>
#include <QWidget>

constexpr int width = 800;
constexpr int height = 600;

class Canvas : public QWidget {
    QTimer *needle_timer{};
    QTimer *battery_timer{};
    QPainter *painter{};

    int offset = 40;

public:
    explicit Canvas(QWidget *parent = nullptr);

    void set_speed(int speed) const;

    void set_temperature(int temperature) const;

    void set_battery(int battery_percent) const;


    [[nodiscard]] static int speed_from_angle();

protected:
    void show_needle_speed();

    void draw_speed(int &start_angle);

    void draw_circle(float _d);

    void show_text_speed();

    void show_temperature();

    void show_battery();

    void paintEvent(QPaintEvent *event) override;
};

#endif
