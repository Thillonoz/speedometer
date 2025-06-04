#include "canvas.h"

#include <QtMath>
#include <QTimer>
#include <QFont>
#include <QPainter>

static constexpr int a_offset_width = 80;
static constexpr int a_offset_top = 70;
static constexpr int line_offset_angle = 2;

static constexpr int circle_multiplayer = 16;
static constexpr int circle_start_angle = -40;
static constexpr int circle_end_angle = 220;
static constexpr int full_circle = 360;

static constexpr float pen_width = 6.0f;
static constexpr float line_length = 20.0f;
static constexpr float medium_line_length = 10.0f;
static constexpr float small_line_length = 5.0f;
static constexpr float line_gap = 10.0f;

static constexpr float arrow_circle_outline_radius = 40.0f;

static constexpr int max_speed = 240;
static int current_speed = 0;
static constexpr int speed_step_substraction = 20;

static constexpr float needle_offset = 40.0f;
static constexpr float needle_width = 10.0f;

static constexpr int text_font_size = 24;
static constexpr float text_offset = 15.0f;
static constexpr int text_step = 5;

static constexpr int interval = 16U; // ~60fps

static const QPen pen_white(Qt::white, pen_width);
static const QPen pen_red(Qt::red, pen_width);

static constexpr QChar speed_icon = QChar(0xe9e4);

static constexpr float speed_icon_top_offset = 0.33f;
static constexpr float speed_icon_size = 40.0f;
static constexpr float speed_text_size_w = 200.0f;
static constexpr float speed_text_size_h = 40.0f;
static constexpr float speed_text_offset = 0.07f;

static QPointF center;
static QPointF arc_center;

static int start_angle;
static int end_angle;
static float radius;

static float current_angle_deg = qDegreesToRadians(static_cast<float>(circle_end_angle + line_offset_angle));
static float target_angle_deg = current_angle_deg;

Canvas::Canvas(QWidget *parent) {
    setParent(parent);
    setFixedSize(parent->width() - offset, parent->height() - offset);

    current_speed = max_speed;

    needle_timer = new QTimer(this);
    needle_timer->setInterval(interval);

    connect(needle_timer, &QTimer::timeout, this, [this]() {
        const float step = qDegreesToRadians(1.5f); // Convert degrees to radians for smooth motion

        if (qAbs(current_angle_deg - target_angle_deg) < step) {
            current_angle_deg = target_angle_deg;
            needle_timer->stop();
        } else {
            current_angle_deg += (current_angle_deg < target_angle_deg) ? step : -step;
        }
        update(); // trigger paintEvent()
    });
}

void Canvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    const float _h = static_cast<float>(this->height());
    const float _w = _h;
    const float _d = qMin(_w, _h) + 100;

    radius = _d / 2.0f;
    center = QPointF(_w / 2.0f, _h / 2.0f);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);


    draw_circle(_d, painter);
    draw_speed(start_angle, painter);
    show_needle_speed(painter);
    show_text_speed(painter);
}

void Canvas::draw_circle(const float _d, QPainter &painter) {
    arc_center = QPointF(center.x() + a_offset_width, center.y() + a_offset_top);
    QRectF arcRect((center.x() - radius) + a_offset_width,
                   (center.y() - radius) + a_offset_top,
                   _d, _d);

    painter.setPen(pen_white);
    painter.drawArc(arcRect,
                    circle_start_angle * circle_multiplayer,
                    (circle_end_angle - circle_start_angle) * circle_multiplayer);
}

void Canvas::draw_speed(int &start_angle, QPainter &painter) {
    painter.setPen(pen_white);

    const int segment = max_speed / text_step;
    start_angle = circle_start_angle + line_offset_angle;
    end_angle = circle_end_angle - line_offset_angle;

    const QFont font("Arial", text_font_size, QFont::Bold);
    painter.setFont(font);

    for (int i = 0; i <= segment; ++i) {
        const float angle_deg = start_angle + (static_cast<float>(i) / static_cast<float>(segment)) *
                                (end_angle - start_angle);
        const float angle_rad = qDegreesToRadians(angle_deg);

        int current_line_length = small_line_length;
        if (i % 4 == 0) {
            current_line_length = line_length;
        } else if (i % 2 == 0) {
            current_line_length = medium_line_length;
        }

        QPointF p1 = arc_center + QPointF((radius - line_gap) * std::cos(angle_rad),
                                          -(radius - line_gap) * std::sin(angle_rad));
        QPointF p2 = arc_center + QPointF(((radius - line_gap) - current_line_length) * std::cos(angle_rad),
                                          -((radius - line_gap) - current_line_length) * std::sin(angle_rad));

        if (i % 4 == 0) {
            QPointF vec = p2 - arc_center;
            QFontMetrics fm(painter.font());
            QRect textRect = fm.boundingRect(QString::number(max_speed));

            QPointF text_pos = p2 - (vec / std::hypot(vec.x(), vec.y())) * (
                                   text_offset + textRect.width() / 2.0f);
            text_pos.rx() -= textRect.width() / 2.0f;
            text_pos.ry() += textRect.height() / 2.0f;

            const int speed = -(((i * speed_step_substraction) / 4) - max_speed);
            painter.drawText(text_pos, QString::number(speed));
            painter.setPen(pen_red);
        } else {
            painter.setPen(pen_white);
        }

        painter.drawLine(p1, p2);
    }
}

void Canvas::show_needle_speed(QPainter &painter) {
    const double current_angle = current_angle_deg;

    // Draw needle base
    painter.setBrush(Qt::white);
    painter.setPen(pen_white);

    QRectF innerCircleRect(
        arc_center.x() - arrow_circle_outline_radius / 2.0f,
        arc_center.y() - arrow_circle_outline_radius / 2.0f,
        arrow_circle_outline_radius,
        arrow_circle_outline_radius
    );
    painter.drawArc(innerCircleRect, 0, full_circle * circle_multiplayer);
    painter.drawEllipse(innerCircleRect);

    // Draw center needle circle
    painter.setPen(pen_red);
    painter.setBrush(Qt::red);

    QRectF needleCircleRect(
        arc_center.x() - arrow_circle_outline_radius / 4.0f,
        arc_center.y() - arrow_circle_outline_radius / 4.0f,
        arrow_circle_outline_radius - 20.0f,
        arrow_circle_outline_radius - 20.0f
    );
    painter.drawArc(needleCircleRect, 0, full_circle * circle_multiplayer);
    painter.drawEllipse(needleCircleRect);

    // Draw needle pointer
    const QPointF needle_center = arc_center;
    const float needle_length = radius - line_gap - needle_offset;

    QPointF needle_tip = needle_center + QPointF(needle_length * std::cos(current_angle),
                                                 -needle_length * std::sin(current_angle));

    QPointF needle_left = needle_center + QPointF(needle_width * std::cos(current_angle + M_PI_2),
                                                  -needle_width * std::sin(current_angle + M_PI_2));
    QPointF needle_right = needle_center + QPointF(needle_width * std::cos(current_angle - M_PI_2),
                                                   -needle_width * std::sin(current_angle - M_PI_2));

    QPolygonF needle;
    needle << needle_tip << needle_left << needle_right;
    painter.drawPolygon(needle);

    painter.setBrush(Qt::NoBrush);
}

void Canvas::show_text_speed(QPainter &painter) {
    const QFont font("Arial", text_font_size, QFont::Bold);
    painter.setPen(pen_white);
    painter.setFont(font);

    const QRectF icon_rect(
        arc_center.x() - speed_icon_size / 2.0, // center horizontally
        arc_center.y() + arrow_circle_outline_radius / speed_icon_top_offset,
        speed_icon_size,
        speed_icon_size
    );

    const QRectF text_rect(
        arc_center.x() - speed_text_size_w / 2.0, // center horizontally
        arc_center.y() + arrow_circle_outline_radius / (speed_icon_top_offset - speed_text_offset),
        speed_text_size_w,
        speed_text_size_h
    );

    painter.drawText(icon_rect, Qt::AlignCenter, speed_icon);
    painter.drawText(text_rect, Qt::AlignCenter, QString::number(speed_from_angle()) + QString(" km/h"));
}

void Canvas::set_speed(int speed) const {
    speed = qBound(0, speed, max_speed);

    const float angle_range = (circle_end_angle - circle_start_angle) - 2 * line_offset_angle;
    const float percentage = static_cast<float>(speed) / max_speed;

    const float angle_deg = circle_end_angle - line_offset_angle - (percentage * angle_range);
    target_angle_deg = qDegreesToRadians(angle_deg);

    needle_timer->start();
}

int Canvas::speed_from_angle() {
    const float angle_range = (circle_end_angle - circle_start_angle) - 2 * line_offset_angle;
    const float angle_deg = qRadiansToDegrees(current_angle_deg);
    float normalized_angle = circle_end_angle - line_offset_angle - angle_deg;

    if (normalized_angle < 0)
        normalized_angle = 0;
    if (normalized_angle > angle_range)
        normalized_angle = angle_range;

    float percentage = normalized_angle / angle_range;
    int speed = static_cast<int>(percentage * max_speed);
    return speed;
}
