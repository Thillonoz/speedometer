#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>
#include <QFormLayout>
#include <QString>
#include <QVBoxLayout>

#include <QCheckBox>
#include <QLabel>
#include <QSlider>

#include "comservice.h"

class Window : public QDialog
{
  static constexpr const char *SPEED_SUFFIX{" Km/h"};
  static constexpr const char *TEMPERATURE_SUFFIX{" °C"};
  static constexpr const char *BATTERY_SUFFIX{" %"};
  static constexpr int SLIDER_WIDTH{500};
  static constexpr int SLIDER_LABEL_CHAR_SIZE{8};

private:
  COMService &COMHandle;

private:
  QVBoxLayout mainLayout;
  QFormLayout formLayout;

  QBoxLayout speedSlideBarLayout{QBoxLayout::Direction::LeftToRight};
  QBoxLayout temperatureSlideBarLayout{QBoxLayout::Direction::LeftToRight};
  QBoxLayout batteryLevelSlideBarLayout{QBoxLayout::Direction::LeftToRight};
  QLabel speedLabel, temperatureLabel, batteryLevelLabel;
  QSlider speedSlider{Qt::Orientation::Horizontal};
  QSlider temperatureSlider{Qt::Orientation::Horizontal};
  QSlider batteryLevelSlider{Qt::Orientation::Horizontal};

  QBoxLayout indicatorLayout{QBoxLayout::Direction::LeftToRight};
  QLabel indicatorLeftLabel, indicatorRightLabel, indicatorWarningLabel;
  QCheckBox indicatorLeft;
  QCheckBox indicatorRight;
  QCheckBox indicatorWarning;

private:
  void onSlideSpeed(int);
  void onSlideTemperature(int);
  void onSlideBatteryLevel(int);
  void onClickLeftIndicator(int);
  void onClickRightIndicator(int);
  void onClickWarningIndicator(int);

private:
  static int CALC_LABEL_WIDTH(int x) { return (8 * (x + 1)); };

public:
  Window(COMService &_COMHandle);
};

#endif