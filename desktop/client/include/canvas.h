#ifndef CANVAS_H
#define CANVAS_H

#include <QPainter>
#include <QWidget>

class Canvas : public QWidget {
    QTimer *needle_timer{};
    QTimer *battery_timer{};
    QPainter *painter{};

public:
    explicit Canvas(QWidget *parent = nullptr);

    /**
     * @brief A Function to set the current speed
     *
     * @param speed The current speed
     */
    void set_speed(int speed) const;

    /**
     * @brief A Function to set the current temperature
     *
     * @param temperature The current temperature
     */
    void set_temperature(int temperature) const;

    /**
     * @brief A Function to set the current battery in precent
     *
     * @param battery_percent The current battery
     */
    void set_battery(int battery_percent) const;

    /**
     * @brief A Helper Function to calculate the angle of the needle from the current speed
     */
    [[nodiscard]] static int speed_from_angle();

protected:
    /**
     * @brief A Function to draw the needle in GUI
     */
    void show_needle_speed() const;

    /**
     * @brief A Function to draw the speed lines with the corresponding speed number in GUI
     */
    void draw_speed(int &start_angle) const;

    /**
     * @brief A Function the almost full circle in GUI
     */
    void draw_circle() const;

    /**
     * @brief A Function to draw the icon and the speed text below the needle in the GUI
     */
    void show_text_speed() const;

    /**
     * @brief A Function to draw the temperature icon and temperature text in GUI
     */
    void show_temperature();

    /**
     * @brief A Function to draw the battery icon with the battery process text in the GUI
     */
    void show_battery();

    void paintEvent(QPaintEvent *event) override;
};

#endif
