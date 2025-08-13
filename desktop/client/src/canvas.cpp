#include "canvas.h"
#include "setting.h"

#include <QtMath>
#include <QTimer>
#include <QFont>
#include <QPainter>
#include <QTime>

// Include Settngs
static Setting::Signal &SETTINGS{Setting::Signal::handle()};

// Window Size Settings
static constexpr int offset = 40;

static constexpr int a_offset_width = 80;
static constexpr int a_offset_top = 70;
static constexpr int line_offset_angle = 2;

static float _h, _w, _d;

// Circle Parameter
static constexpr int circle_multiplayer = 16;
static constexpr int circle_start_angle = -40;
static constexpr int circle_end_angle = 220;
static constexpr int full_circle = 360;

// Painter Parameter
static constexpr float pen_width = 6.0f;
static constexpr float line_length = 20.0f;
static constexpr float medium_line_length = 10.0f;
static constexpr float small_line_length = 5.0f;
static constexpr float line_gap = 10.0f;

static constexpr float arrow_circle_outline_radius = 40.0f;

static const QPen pen_white(Qt::white, pen_width);
static const QPen pen_blue(Qt::blue, pen_width);
static const QPen pen_yellow(Qt::yellow, pen_width);
static const QPen pen_green(Qt::green, pen_width);
static const QPen pen_red(Qt::red, pen_width);

static constexpr QChar speed_icon(0xe9e4);
static constexpr QChar temperature_icon(0xe1ff);
static constexpr QChar battery_icon(0xebdc);
static constexpr QChar disconnect_icon(0xe628);

// Speed Settings
static int max_speed = SETTINGS["speed"].max;
static int min_speed = SETTINGS["speed"].min;

static constexpr float speed_icon_top_offset = 0.33f;
static constexpr float speed_icon_size = 40.0f;
static constexpr float speed_text_size_w = 200.0f;
static constexpr float speed_text_size_h = 40.0f;
static constexpr float speed_text_offset = 0.07f;

static constexpr int speed_step_substraction = 20;
static int current_speed = 0;

// Needle Settings
static constexpr float needle_offset = 40.0f;
static constexpr float needle_width = 10.0f;

// Txt Settings
static constexpr int text_font_size = 24;
static constexpr float text_offset = 15.0f;
static constexpr int text_step = 5;

// Temperature Settings
static constexpr float temperature_icon_width = 40.0f;
static constexpr float temperature_icon_height = 85.0f;
static constexpr float temperature_icon_top_offset = 0.4f;
static constexpr float temperature_icon_width_offset = 9.0f;
static constexpr float temperature_icon_font_size = 60.0f;
static constexpr float temperature_text_font_size = 12.0f;
static constexpr float temperature_text_top_offset = 0.12f;

static int max_temperature = SETTINGS["temperature"].max;
static int min_temperature = SETTINGS["temperature"].min;

static int current_temperature = 0;

// Battery Settings
static constexpr float battery_icon_height = 100.0f;
static constexpr float battery_icon_width = 40.0f;
static constexpr float battery_icon_width_offset = 9.0f;
static constexpr float battery_icon_top_offset = 30.4f;
static constexpr float battery_text_top_offset = 70.4f;
static constexpr float fill_margin_ratio = 0.90f;

static float current_battery_fill = 0;
static float target_battery_fill = 100;
static int current_battery = 0;

static int max_battery = SETTINGS["battery"].max;
static int min_battery = SETTINGS["battery"].min;

// Helper angles
static QPointF center;
static QPointF arc_center;

static int start_angle;
static int end_angle;
static float radius;

static float current_angle_deg = qDegreesToRadians(static_cast<float>(circle_end_angle + line_offset_angle));
static float target_angle_deg = current_angle_deg;

static int blinker_position = 0;

// Connection
static constexpr float disconnect_icon_size = 80.0f;
static constexpr float disconnect_font_size = 50.0f;

static bool current_connection_status = false; // True if connected, false if not

Canvas::Canvas(QWidget *parent)
{
    // Setup Window Size
    setParent(parent);
    setFixedSize(800 - offset, 600 - offset);

    painter = nullptr; // Init painter

    current_speed = max_speed; // Set current speed to max speed
}

void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    // Setup Painter
    QPainter localPainter(this);
    QMediaPlayer localMediaPlayer(this);

    painter = &localPainter;
    mediaPlayer = &localMediaPlayer;

    painter->setRenderHints(QPainter::Antialiasing, true);

    // Calculate height, width and diameter
    _h = static_cast<float>(this->height());
    _w = _h;
    _d = qMin(_w, _h) + 100;

    radius = _d / 2.0f;
    center = QPointF(_w / 2.0f, _h / 2.0f); // Get the center of the CANVAS screen

    // Call all draw functions
    draw_circle();
    draw_speed(start_angle);
    show_needle_speed();
    current_connection_status == true ? show_text_speed() : show_disconnect_warning();
    show_temperature();
    show_battery();
    blinker();
}

void Canvas::update_all(const int speed, const int temperature, const int battery,
                        const int left_blinker, const int right_blinker, const bool connected)
{
    set_speed(speed);
    set_temperature(temperature);
    set_battery(battery);

    if (left_blinker && right_blinker)
        set_blinker(3);
    else if (left_blinker)
        set_blinker(2);
    else if (right_blinker)
        set_blinker(1);
    else
        set_blinker(0);

    is_connected(connected);

    update();
}

void Canvas::draw_circle() const
{
    arc_center = QPointF(center.x() + a_offset_width, center.y() + a_offset_top);
    QRectF arcRect((center.x() - radius) + a_offset_width,
                   (center.y() - radius) + a_offset_top,
                   _d, _d);

    painter->setPen(pen_white);
    painter->drawArc(arcRect,
                     circle_start_angle * circle_multiplayer,
                     (circle_end_angle - circle_start_angle) * circle_multiplayer);
}

void Canvas::draw_speed(int &start_angle) const
{
    painter->setPen(pen_white);

    const int segment = max_speed / text_step;
    start_angle = circle_start_angle + line_offset_angle;
    end_angle = circle_end_angle - line_offset_angle;

    const QFont font("Arial", text_font_size, QFont::Bold);
    painter->setFont(font);

    for (int i = 0; i <= segment; ++i)
    {
        const float angle_deg = start_angle + (static_cast<float>(i) / static_cast<float>(segment)) *
                                                  (end_angle - start_angle);
        const float angle_rad = qDegreesToRadians(angle_deg);

        int current_line_length = small_line_length;
        if (i % 4 == 0)
        {
            current_line_length = line_length;
        }
        else if (i % 2 == 0)
        {
            current_line_length = medium_line_length;
        }

        QPointF p1 = arc_center + QPointF((radius - line_gap) * std::cos(angle_rad),
                                          -(radius - line_gap) * std::sin(angle_rad));
        QPointF p2 = arc_center + QPointF(((radius - line_gap) - current_line_length) * std::cos(angle_rad),
                                          -((radius - line_gap) - current_line_length) * std::sin(angle_rad));

        if (i % 4 == 0)
        {
            QPointF vec = p2 - arc_center;
            QFontMetrics fm(painter->font());
            QRect textRect = fm.boundingRect(QString::number(max_speed));

            QPointF text_pos = p2 - (vec / std::hypot(vec.x(), vec.y())) * (text_offset + textRect.width() / 2.0f);
            text_pos.rx() -= textRect.width() / 2.0f;
            text_pos.ry() += textRect.height() / 2.0f;

            const int speed = -(((i * speed_step_substraction) / 4) - max_speed);
            painter->drawText(text_pos, QString::number(speed));
            painter->setPen(pen_red);
        }
        else
        {
            painter->setPen(pen_white);
        }

        painter->drawLine(p1, p2);
    }
}

void Canvas::show_needle_speed() const
{
    const double current_angle = current_angle_deg;

    // Draw needle base
    painter->setBrush(Qt::white);
    painter->setPen(pen_white);

    QRectF innerCircleRect(
        arc_center.x() - arrow_circle_outline_radius / 2.0f,
        arc_center.y() - arrow_circle_outline_radius / 2.0f,
        arrow_circle_outline_radius,
        arrow_circle_outline_radius);
    painter->drawArc(innerCircleRect, 0, full_circle * circle_multiplayer);
    painter->drawEllipse(innerCircleRect);

    // Draw center needle circle
    painter->setPen(pen_red);
    painter->setBrush(Qt::red);

    QRectF needleCircleRect(
        arc_center.x() - arrow_circle_outline_radius / 4.0f,
        arc_center.y() - arrow_circle_outline_radius / 4.0f,
        arrow_circle_outline_radius - 20.0f,
        arrow_circle_outline_radius - 20.0f);
    painter->drawArc(needleCircleRect, 0, full_circle * circle_multiplayer);
    painter->drawEllipse(needleCircleRect);

    // Draw needle pointer
    const QPointF needle_center = arc_center;
    const float needle_length = radius - line_gap - needle_offset;

    const QPointF needle_tip = needle_center + QPointF(needle_length * std::cos(current_angle),
                                                       -needle_length * std::sin(current_angle));

    const QPointF needle_left = needle_center + QPointF(needle_width * std::cos(current_angle + M_PI_2),
                                                        -needle_width * std::sin(current_angle + M_PI_2));
    const QPointF needle_right = needle_center + QPointF(needle_width * std::cos(current_angle - M_PI_2),
                                                         -needle_width * std::sin(current_angle - M_PI_2));

    QPolygonF needle;
    needle << needle_tip << needle_left << needle_right;
    painter->drawPolygon(needle);

    painter->setBrush(Qt::NoBrush);
}

void Canvas::show_text_speed() const
{
    const QFont font("Arial", text_font_size, QFont::Bold);
    painter->setPen(pen_white);
    painter->setFont(font);

    // draw speed icon
    const QRectF icon_rect(
        arc_center.x() - speed_icon_size / 2.0, // center horizontally
        arc_center.y() + arrow_circle_outline_radius / speed_icon_top_offset,
        speed_icon_size,
        speed_icon_size);

    // draw speed txt
    const QRectF text_rect(
        arc_center.x() - speed_text_size_w / 2.0, // center horizontally
        arc_center.y() + arrow_circle_outline_radius / (speed_icon_top_offset - speed_text_offset),
        speed_text_size_w,
        speed_text_size_h);

    painter->drawText(icon_rect, Qt::AlignCenter, speed_icon);
    painter->drawText(text_rect, Qt::AlignCenter, QString::number(speed_from_angle()) + QString(" km/h"));
}

void Canvas::show_disconnect_warning() const
{
    const QFont font_icon("Arial", text_font_size, QFont::Bold);
    const QFont font("Arial", disconnect_font_size, QFont::Bold);
    painter->setPen(pen_red);

    // draw disconnect icon
    const QRectF icon_rect(
        arc_center.x() - disconnect_icon_size / 2.0, // center horizontally
        arc_center.y() + arrow_circle_outline_radius / 0.5,
        disconnect_icon_size,
        disconnect_icon_size);

    // draw disconnect txt
    const QRectF text_rect(
        arc_center.x() - speed_text_size_w * 0.7, // center horizontally
        arc_center.y() + arrow_circle_outline_radius / (speed_icon_top_offset - speed_text_offset),
        speed_text_size_w + disconnect_icon_size,
        speed_text_size_h);

    painter->setFont(font);
    painter->drawText(icon_rect, Qt::AlignCenter, disconnect_icon);
    painter->setFont(font_icon);
    painter->drawText(text_rect, Qt::AlignCenter, QString("Connection Error!"));
}

int Canvas::speed_from_angle()
{
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

void Canvas::show_temperature()
{
    const QFont font("Material Icons", temperature_icon_font_size);
    painter->setFont(font);

    // set pen color depending on temperature
    if (current_temperature < 5)
    {
        painter->setPen(pen_white);
    }
    else if (current_temperature < 40)
    {
        painter->setPen(pen_blue);
    }
    else
    {
        painter->setPen(pen_red);
    }

    // draw temperature icon
    const QRectF icon_rect(
        arc_center.x() + temperature_icon_width * temperature_icon_width_offset,
        arc_center.y() + arrow_circle_outline_radius / temperature_icon_top_offset,
        temperature_icon_width,
        temperature_icon_height);

    painter->drawText(icon_rect, Qt::AlignCenter, temperature_icon);
    const QFont txt_font("Arial", temperature_text_font_size);

    painter->setPen(pen_white);
    painter->setFont(txt_font);

    const QRectF text_rect(
        arc_center.x() + temperature_icon_width * (temperature_icon_width_offset),
        arc_center.y() + arrow_circle_outline_radius / (temperature_icon_top_offset - temperature_text_top_offset),
        temperature_icon_width,
        temperature_icon_height);

    painter->drawText(text_rect, Qt::AlignCenter, QString::number(current_temperature) + QString("°C"));
}

void Canvas::show_battery()
{
    const QFont font("Material Icons", 90);

    QPen current_pen;
    painter->setFont(font);

    if (current_battery < 25)
    {
        current_pen = pen_red;
        painter->setBrush(Qt::red);
    }
    else if (current_battery < 50)
    {
        current_pen = pen_yellow;
        painter->setBrush(Qt::yellow);
    }
    else
    {
        current_pen = pen_green;
        painter->setBrush(Qt::green);
    }

    const QRectF icon_rect(
        arc_center.x() + battery_icon_width * battery_icon_width_offset,
        arc_center.y() - battery_icon_top_offset,
        battery_icon_width,
        battery_icon_height);

    const float usable_fill_height = battery_icon_height * fill_margin_ratio;
    const float fill_height = (current_battery_fill / 100.0f) * usable_fill_height;

    const QRectF fill_rect(
        icon_rect.x(),
        icon_rect.y() + (battery_icon_height - fill_height),
        icon_rect.width(),
        fill_height);

    const QRectF icon_fill_rect(fill_rect);
    painter->setPen(Qt::NoPen);
    painter->drawRect(icon_fill_rect);
    painter->setBrush(Qt::NoBrush);

    painter->setPen(current_pen);
    painter->drawText(icon_rect, Qt::AlignCenter, battery_icon);
    const QFont txt_font("Arial", temperature_text_font_size);

    painter->setPen(pen_white);
    painter->setFont(txt_font);

    const QRectF text_rect(
        arc_center.x() + battery_icon_width * battery_icon_width_offset,
        arc_center.y() - (battery_icon_top_offset - battery_text_top_offset),
        battery_icon_width,
        battery_icon_height);

    painter->drawText(text_rect, Qt::AlignCenter, QString::number(current_battery_fill) + QString("%"));
}

void Canvas::blinker()
{
    QFont iconFont = painter->font();
    iconFont.setPointSize(60);
    painter->setFont(iconFont);
    QColor color = Qt::white;
    int ms;

    if (blinker_position == 1) // right blinker
    {
        ms = QTime::currentTime().msec();

        color = (ms < 500) ? Qt::green : Qt::transparent;
        painter->setPen(color);
        painter->drawText(QPointF(580.0f, 90.0f), QChar(0xe5c8));
        if (ms < 500)
            playBlinkerSound(true);
    }
    else if (blinker_position == 2) // left blinker
    {
        ms = QTime::currentTime().msec();

        color = (ms < 500) ? Qt::green : Qt::transparent;
        painter->setPen(color);
        painter->drawText(QPointF(60.0f, 90.0f), QChar(0xe5c4));
        if (ms < 500)
            playBlinkerSound(true);
    }
    else if (blinker_position == 3) // warning lights
    {
        ms = QTime::currentTime().msec();
        color = (ms < 500) ? Qt::green : Qt::transparent;
        painter->setPen(color);
        painter->drawText(QPointF(580.0f, 90.0f), QChar(0xe5c8));

        painter->drawText(QPointF(60.0f, 90.0f), QChar(0xe5c4));
        if (ms < 500)
            playBlinkerSound(true);
    }
    else // no blinker
    {
        color = Qt::transparent;
        playBlinkerSound(false);
    }
}

void Canvas::playBlinkerSound(bool _isActive)
{
    if (_isActive == true)
    {
        if (QMediaPlayer::MediaStatus::EndOfMedia == mediaPlayer->mediaStatus() ||
            QMediaPlayer::PlaybackState::PlayingState != mediaPlayer->playbackState())
        {
            mediaPlayer->setSource(QUrl());
            mediaPlayer->setSource(QUrl::fromLocalFile("sound.wav"));
            mediaPlayer->play();
        }
    }
    else
    {
        mediaPlayer->stop();
    }
}

// Set parameters:

void Canvas::set_speed(int speed)
{
    speed = qBound(min_speed, speed, max_speed);

    const float angle_range = (circle_end_angle - circle_start_angle) - 2 * line_offset_angle;
    const float percentage = static_cast<float>(speed) / max_speed;

    const float angle_deg = circle_end_angle - line_offset_angle - (percentage * angle_range);
    target_angle_deg = qDegreesToRadians(angle_deg);

    current_angle_deg = target_angle_deg;
}

void Canvas::set_temperature(const int temperature)
{
    current_temperature = qBound(min_temperature, temperature, max_temperature);
}

void Canvas::set_battery(const int battery_percent)
{
    current_battery = qBound(min_battery, battery_percent, max_battery);
    target_battery_fill = static_cast<float>(current_battery);
    current_battery_fill = target_battery_fill;
}

void Canvas::set_blinker(const int blinker_state)
{
    blinker_position = blinker_state;
}

void Canvas::is_connected(const bool status)
{
    current_connection_status = status;
}
