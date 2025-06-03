#include "window.h"
#include "setting.h"

Window::Window() {
  setWindowTitle("Server");
  setLayout(&mainLayout);

  // Layouts & size properties
  mainLayout.addLayout(&formLayout);
  mainLayout.setSizeConstraint(QLayout::SetFixedSize);

  formLayout.addRow("Speed:", &speedSlideBarLayout);
  formLayout.addRow("Temperature:", &temperatureSlideBarLayout);
  formLayout.addRow("Battery level:", &batteryLevelSlideBarLayout);
  formLayout.addRow("Light signals:", &indicatorLayout);
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
  QString startLabelText;
  speedSlider.setMinimum(SPEEDMIN);
  speedSlider.setMaximum(SPEEDMAX);
  startLabelText.setNum(speedSlider.value());
  startLabelText.append(SPEED_SUFFIX);
  speedLabel.setText(startLabelText);
  connect(&speedSlider, &QSlider::valueChanged, this, &Window::onSlideSpeed);

  temperatureSlider.setMinimum(TEMPMIN);
  temperatureSlider.setMaximum(TEMPDMAX);
  startLabelText.setNum(temperatureSlider.value());
  startLabelText.append(TEMPERATURE_SUFFIX);
  temperatureLabel.setText(startLabelText);
  connect(&temperatureSlider, &QSlider::valueChanged, this,
          &Window::onSlideTemperature);

  batteryLevelSlider.setMinimum(BATTMIN);
  batteryLevelSlider.setMaximum(BATTMAX);
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

  // Send communication?
}

void Window::onSlideTemperature(int _value) {
  QString temp;
  temp.setNum(_value);
  temp.append(TEMPERATURE_SUFFIX);
  temperatureLabel.setText(temp);

  // Send communication?
}

void Window::onSlideBatteryLevel(int _value) {
  QString temp;
  temp.setNum(_value);
  temp.append(BATTERY_SUFFIX);
  this->batteryLevelLabel.setText(temp);

  // Send communication?
}

void Window::onClickLeftIndicator(int _state) {
  if (_state == Qt::CheckState::Checked) {
    indicatorRight.setDisabled(true);
    indicatorRight.setCheckState(Qt::CheckState::Unchecked);
  } else {
    indicatorRight.setDisabled(false);
  }

  if (indicatorWarning.checkState() == Qt::CheckState::Unchecked) {
    // Send communication?
  } else {
    ;
  }
}

void Window::onClickRightIndicator(int _state) {
  if (_state == Qt::CheckState::Checked) {
    indicatorLeft.setDisabled(true);
    indicatorLeft.setCheckState(Qt::CheckState::Unchecked);
  } else {
    indicatorLeft.setDisabled(false);
  }

  if (indicatorWarning.checkState() == Qt::CheckState::Unchecked) {
    // Send communication?
  } else {
    ;
  }
}

void Window::onClickWarningIndicator(int _state) {
  // Send communication?
}
