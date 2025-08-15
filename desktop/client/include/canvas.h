#ifndef CANVAS_H
#define CANVAS_H

#include <QPainter>
#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>

class Canvas : public QWidget {
    QPainter *painter{};

private:
    QMediaPlayer mediaPlayer;
    QAudioOutput audioOutput;

    /**
     * @brief A Function to set the current speed
     *
     * @param speed The current speed
     */
    void set_speed(int speed);

    /**
     * @brief A Function to set the current temperature
     *
     * @param temperature The current temperature
     */
    static void set_temperature(int temperature);

    /**
     * @brief A Function to set the current battery in precent
     *
     * @param battery_percent The current battery
     */
    static void set_battery(int battery_percent);

    /**
     * @brief Function to turn on [left, right, warning] blinkers or off
     *
     * @param blinker_state The state of the blinker, ether on [left, right, warning] or off
     */
    void set_blinker(int blinker_state);

    /**
     * @brief Plays blinker sound effect.
     *
     * @param _isActive True if the blinker is active, false to stop the sound.
     */
    void playBlinkerSound(bool _isActive);

    /**
     * @brief Function to display the disconnect warning
     *
     * @param status The status of connection its ether true [connected] or false [not connected]
     */
    static void is_connected(bool status);

    /**
     * @brief A Helper Function to calculate the angle of the needle from the current speed
     */
    [[nodiscard]] static int speed_from_angle();

public:
    explicit Canvas(QWidget *parent = nullptr);

    /**
     * @brief Function to update all variables in the same time
     *
     * @param speed New speed in kmh
     * @param temperature New Temperature in Celsius
     * @param battery New Battery precent
     * @param left_blinker 1 if left blinker on / 1 if warning on / 0 if off
     * @param right_blinker 1 if right blinker on / 1 if warning on / 0 if off
     * @param connected True if connected successfully and false if not
     */
    void update_all(int speed, int temperature, int battery, int left_blinker, int right_blinker, bool connected);

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

    /**
     * @brief Function to draw the blinkers
     */
    void blinker() const;

    /**
     * @brief Function to draw the disconnect warning
     */
    void show_disconnect_warning() const;

    void paintEvent(QPaintEvent *event) override;
};

#endif
