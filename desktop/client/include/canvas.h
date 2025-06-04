#ifndef CANVAS_H
#define CANVAS_H

#include <QPainter>
#include <QWidget>

constexpr int width = 800;
constexpr int height = 600;

class Canvas : public QWidget {
    QTimer *needle_timer{};

    int offset = 40;

public:
    explicit Canvas(QWidget *parent = nullptr);

    void set_speed(int speed) const;

    [[nodiscard]] static int speed_from_angle();

protected:
    static void show_needle_speed(QPainter &painter);

    static void draw_speed(int &start_angle, QPainter &painter);

    static void draw_circle(float _d, QPainter &painter);

    static void show_text_speed(QPainter &painter);

    void paintEvent(QPaintEvent *event) override;
};

#endif
