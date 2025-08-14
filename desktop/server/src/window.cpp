#include "window.h"
#include "setting.h"
#include <csignal>

Window::Window(COMService &_COMHandle) : COMHandle{_COMHandle} {
  QString startLabelText;
  Setting::Signal &settingSignals{Setting::Signal::handle()};

  setWindowTitle("Server");
  setLayout(&mainLayout);
  setWindowFlags(Qt::WindowStaysOnTopHint);

  // Layouts & size properties
  mainLayout.addLayout(&formLayout);
  mainLayout.setSizeConstraint(QLayout::SetFixedSize);

  startLabelText = "Speed";
  startLabelText.append(":");
  formLayout.addRow(startLabelText, &speedSlideBarLayout);

  startLabelText = "Temperature";
  startLabelText.append(":");
  formLayout.addRow(startLabelText, &temperatureSlideBarLayout);

  startLabelText = "Battery";
  startLabelText.append(":");
  formLayout.addRow(startLabelText, &batteryLevelSlideBarLayout);

  startLabelText = "Light signals:";
  formLayout.addRow(startLabelText, &indicatorLayout);
  formLayout.setLabelAlignment(Qt::AlignmentFlag::AlignRight);

  speedSlideBarLayout.addWidget(&speedSlider);
  speedSlideBarLayout.addWidget(&speedLabel);
  speedSlider.setFixedWidth(SLIDER_WIDTH);
  speedLabel.setMinimumWidth(CALC_LABEL_WIDTH(SLIDER_LABEL_CHAR_SIZE));

  temperatureSlideBarLayout.addWidget(&temperatureSlider);
  temperatureSlideBarLayout.addWidget(&temperatureLabel);
  temperatureSlider.setFixedWidth(SLIDER_WIDTH);
  temperatureSlider.setMinimumWidth(CALC_LABEL_WIDTH(SLIDER_LABEL_CHAR_SIZE));

  batteryLevelSlideBarLayout.addWidget(&batteryLevelSlider);
  batteryLevelSlideBarLayout.addWidget(&batteryLevelLabel);
  batteryLevelSlider.setFixedWidth(SLIDER_WIDTH);
  batteryLevelSlider.setMinimumWidth(CALC_LABEL_WIDTH(SLIDER_LABEL_CHAR_SIZE));

  indicatorLayout.addWidget(&indicatorLeft);
  indicatorLayout.addWidget(&indicatorRight);
  indicatorLayout.addWidget(&indicatorWarning);

  // actions & value properties
  speedSlider.setMinimum(settingSignals["speed"].min);
  speedSlider.setMaximum(settingSignals["speed"].max);
  startLabelText.setNum(speedSlider.value());
  startLabelText.append(SPEED_SUFFIX);
  speedLabel.setText(startLabelText);
  connect(&speedSlider, &QSlider::valueChanged, this, &Window::onSlideSpeed);

  temperatureSlider.setMinimum(settingSignals["temperature"].min);
  temperatureSlider.setMaximum(settingSignals["temperature"].max);
  startLabelText.setNum(temperatureSlider.value());
  startLabelText.append(TEMPERATURE_SUFFIX);
  temperatureLabel.setText(startLabelText);
  connect(&temperatureSlider, &QSlider::valueChanged, this,
          &Window::onSlideTemperature);

  batteryLevelSlider.setMinimum(settingSignals["battery"].min);
  batteryLevelSlider.setMaximum(settingSignals["battery"].max);
  startLabelText.setNum(batteryLevelSlider.value());
  startLabelText.append(BATTERY_SUFFIX);
  batteryLevelLabel.setText(startLabelText);
  connect(&batteryLevelSlider, &QSlider::valueChanged, this,
          &Window::onSlideBatteryLevel);

  indicatorLeft.setText("Left");
  connect(&indicatorLeft, &QCheckBox::stateChanged, this,
          &Window::onClickLeftIndicator);

  indicatorRight.setText("Right");
  connect(&indicatorRight, &QCheckBox::stateChanged, this,
          &Window::onClickRightIndicator);

  indicatorWarning.setText("Warning");
  connect(&indicatorWarning, &QCheckBox::stateChanged, this,
          &Window::onClickWarningIndicator);
}

void Window::onSlideSpeed(int _value) {
  QString temp;
  temp.setNum(_value);
  temp.append(SPEED_SUFFIX);
  speedLabel.setText(temp);

  COMHandle.insertSpeed(_value);
}

void Window::onSlideTemperature(int _value) {
  QString temp;
  temp.setNum(_value);
  temp.append(TEMPERATURE_SUFFIX);
  temperatureLabel.setText(temp);

  COMHandle.insertTemperature(_value);
}

void Window::onSlideBatteryLevel(int _value) {
  QString temp;
  temp.setNum(_value);
  temp.append(BATTERY_SUFFIX);
  this->batteryLevelLabel.setText(temp);

  COMHandle.insertBatteryLevel(_value);
}

void Window::onClickLeftIndicator(int _state) {
  if (_state == Qt::CheckState::Checked) {
    indicatorRight.setDisabled(true);
    indicatorRight.setCheckState(Qt::CheckState::Unchecked);
    COMHandle.insertLeftLight(true);
  } else {
    if (indicatorWarning.checkState() == Qt::CheckState::Unchecked) {
      COMHandle.insertLeftLight(false);
    } else {
      ;
    }
    indicatorRight.setDisabled(false);
  }
}

void Window::onClickRightIndicator(int _state) {
  if (_state == Qt::CheckState::Checked) {
    indicatorLeft.setDisabled(true);
    indicatorLeft.setCheckState(Qt::CheckState::Unchecked);
    COMHandle.insertRightLight(true);
  } else {
    if (indicatorWarning.checkState() == Qt::CheckState::Unchecked) {
      COMHandle.insertRightLight(false);
    } else {
      ;
    }
    indicatorLeft.setDisabled(false);
  }
}

void Window::onClickWarningIndicator(int _state) {
  // Warning is on, both lights true.
  if (_state == Qt::CheckState::Checked) {
    COMHandle.insertRightLight(true);
    COMHandle.insertLeftLight(true);
  }
  // When warning turns off, both lights false, or only one depending on if left or right is checked.
  else if (_state == Qt::CheckState::Unchecked) {
    if (indicatorLeft.checkState() == Qt::CheckState::Checked) {
      COMHandle.insertRightLight(false);
    } else if (indicatorRight.checkState() == Qt::CheckState::Checked) {
      COMHandle.insertLeftLight(false);
    } else {
      COMHandle.insertRightLight(false);
      COMHandle.insertLeftLight(false);
    }
  } else {
    ;
  }
}

void Window::closeEvent(QCloseEvent *event) {
  std::raise(SIGINT);
  event->accept();
}
