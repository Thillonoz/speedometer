#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>

#include "canvas.h"
#include "comservice.h"

class Window final : public QDialog
{
    Canvas *canvas;
    COMService &comservice;

public:
    Window(QWidget *parent, COMService &comservice);

    /**
     * @brief Function to set the background color of the main window
     */
    void set_bg_color();

    /**
     * @brief Function to set the speed
     *
     * @param speed The speed you want to set
     */
    void set_speed(const int speed) const;

    /**
     * @brief Function to set the temperature
     *
     * @param temperature The temperature you want to set
     */
    void set_temperature(const int temperature) const;

    /**
     * @brief Function to set the battery_percent
     *
     * @param battery_percent The battery_percent you want to set
     */
    void set_battery(const int battery_percent) const;

    /**
     * @brief Function to set the blinker_state
     *
     * @param blinker_state The blinker_state you want to set
     */
    void set_blinker(const int blinker_state) const;

    /**
     * @brief Function to set the connection_status
     *
     * @param connection_status The connection_status you want to set
     */
    void set_connection_status(const bool connection_status) const;
};

#endif
