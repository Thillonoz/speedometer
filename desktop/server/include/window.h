#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>
#include <QFormLayout>
#include <QString>
#include <QVBoxLayout>

#include <QCheckBox>
#include <QLabel>
#include <QSlider>

#define SPEED_SUFFIX " Km/h"
#define TEMPERATURE_SUFFIX " °C"
#define BATTERY_SUFFIX " %"

#define SLIDER_WIDTH (300)
#define SLIDER_LABEL_CHAR_SIZE 8
#define CALC_LABEL_WIDTH(x) (8 * (x + 1))

class Window : public QDialog {
  // --- temp
  int speedValue{0};
  int temperatureValue{0};
  int BatteryLevelValue{0};
  enum indicatorValue {
    none,
    left,
    right,
    warning
  } indicatorValue{indicatorValue::none};
  // ---

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

  void onSlideSpeed(int);
  void onSlideTemperature(int);
  void onSlideBatteryLevel(int);
  void onClickLeftIndicator(int);
  void onClickRightIndicator(int);
  void onClickWarningIndicator(int);

public:
  Window();
};

#endif