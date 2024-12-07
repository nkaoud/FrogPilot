#include "selfdrive/ui/qt/onroad/onroad_home.h"

#include <QApplication>
#include <QPainter>
#include <QStackedLayout>

#ifdef ENABLE_MAPS
#include "selfdrive/ui/qt/maps/map_helpers.h"
#include "selfdrive/ui/qt/maps/map_panel.h"
#endif

#include "selfdrive/ui/qt/util.h"

OnroadWindow::OnroadWindow(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *main_layout  = new QVBoxLayout(this);
  main_layout->setMargin(UI_BORDER_SIZE);
  QStackedLayout *stacked_layout = new QStackedLayout;
  stacked_layout->setStackingMode(QStackedLayout::StackAll);
  main_layout->addLayout(stacked_layout);

  nvg = new AnnotatedCameraWidget(VISION_STREAM_ROAD, this);

  QWidget * split_wrapper = new QWidget;
  split = new QHBoxLayout(split_wrapper);
  split->setContentsMargins(0, 0, 0, 0);
  split->setSpacing(0);
  split->addWidget(nvg);

  if (getenv("DUAL_CAMERA_VIEW")) {
    CameraWidget *arCam = new CameraWidget("camerad", VISION_STREAM_ROAD, true, this);
    split->insertWidget(0, arCam);
  }

  if (getenv("MAP_RENDER_VIEW")) {
    CameraWidget *map_render = new CameraWidget("navd", VISION_STREAM_MAP, false, this);
    split->insertWidget(0, map_render);
  }

  stacked_layout->addWidget(split_wrapper);

  alerts = new OnroadAlerts(this);
  alerts->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  stacked_layout->addWidget(alerts);

  // setup stacking order
  alerts->raise();

  setAttribute(Qt::WA_OpaquePaintEvent);
  QObject::connect(uiState(), &UIState::uiUpdate, this, &OnroadWindow::updateState);
  QObject::connect(uiState(), &UIState::offroadTransition, this, &OnroadWindow::offroadTransition);
  QObject::connect(uiState(), &UIState::primeChanged, this, &OnroadWindow::primeChanged);

  // FrogPilot variables
  QObject::connect(&clickTimer, &QTimer::timeout, [this]() {
    clickTimer.stop();
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, timeoutPoint, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::postEvent(this, event);
  });
}

void OnroadWindow::updateState(const UIState &s) {
  try {
    if (!s.scene.started) {
      return;
    }

    if (s.scene.map_on_left || s.scene.full_map) {
      split->setDirection(QBoxLayout::LeftToRight);
    } else {
      split->setDirection(QBoxLayout::RightToLeft);
    }

    alerts->updateState(s);
    nvg->updateState(alerts->alert_height, s);

    bool shouldUpdate = false;

    QColor bgColor = bg_colors[s.status];
    if (bg != bgColor) {
      // repaint border
      bg = bgColor;
      shouldUpdate = true;
    }

    // FrogPilot variables
    const UIScene &scene = s.scene;

    accelerationJerk = scene.acceleration_jerk;
    accelerationJerkDifference = scene.acceleration_jerk_difference;
    blindSpotLeft = scene.blind_spot_left;
    blindSpotRight = scene.blind_spot_right;
    currentAcceleration = scene.acceleration;
    fps = scene.fps;
    friction = scene.friction;
    hasLead = scene.has_lead;
    latAccel = scene.lat_accel;
    liveValid = scene.live_valid;
    showBlindspot = scene.show_blind_spot && (blindSpotLeft || blindSpotRight);
    showFPS = scene.show_fps;
    showJerk = scene.jerk_metrics;
    showSignal = scene.signal_metrics && (turnSignalLeft || turnSignalRight);
    showSteering = scene.steering_metrics;
    showTuning = scene.lateral_tuning_metrics;
    speedJerk = scene.speed_jerk;
    speedJerkDifference = scene.speed_jerk_difference;
    steer = scene.steer;
    steeringAngleDeg = scene.steering_angle_deg;
    turnSignalLeft = scene.turn_signal_left;
    turnSignalRight = scene.turn_signal_right;

    if (showBlindspot || showFPS || (showJerk && hasLead) || showSignal || showSteering || showTuning) {
      shouldUpdate = true;
    }

    if (shouldUpdate) {
      update();
    }
  } catch (const std::exception &e) {
    std::cerr << "Exception in OnroadWindow::updateState: " << e.what() << std::endl;
    params_memory.putBool("DebugUI", true);
  } catch (...) {
    std::cerr << "Unknown exception in OnroadWindow::updateState." << std::endl;
    params_memory.putBool("DebugUI", true);
  }
}

void OnroadWindow::mousePressEvent(QMouseEvent* e) {
  try {
    // FrogPilot variables
    UIState *s = uiState();
    UIScene &scene = s->scene;

    // FrogPilot clickable widgets
    QPoint pos = e->pos();

    if (scene.speed_limit_changed && nvg->newSpeedLimitRect.contains(pos)) {
      params_memory.putBool("SLCConfirmed", true);
      return;
    }

    if (scene.experimental_mode_via_tap && pos != timeoutPoint) {
      if (clickTimer.isActive()) {
        clickTimer.stop();

        if (scene.conditional_experimental) {
          int override_value = (scene.conditional_status >= 1 && scene.conditional_status <= 6) ? 0 : (scene.conditional_status >= 7 ? 5 : 6);
          params_memory.putInt("CEStatus", override_value);
        } else {
          params.putBoolNonBlocking("ExperimentalMode", !params.getBool("ExperimentalMode"));
        }

      } else {
        clickTimer.start(500);
      }
      return;
    }

  #ifdef ENABLE_MAPS
    if (map != nullptr) {
      // Switch between map and sidebar when using navigate on openpilot
      bool sidebarVisible = geometry().x() > 0;
      bool show_map = scene.navigate_on_openpilot ? sidebarVisible : !sidebarVisible;
      map->setVisible(show_map && !map->isVisible());
      if (scene.big_map) {
        map->setFixedWidth(width());
      } else {
        map->setFixedWidth(topWidget(this)->width() / 2 - UI_BORDER_SIZE);
      }
    }
  #endif
    // propagation event to parent(HomeWindow)
    QWidget::mousePressEvent(e);
  } catch (const std::exception &e) {
    std::cerr << "Exception in OnroadWindow::mousePressEvent: " << e.what() << std::endl;
    params_memory.putBool("DebugUI", true);
  } catch (...) {
    std::cerr << "Unknown exception in OnroadWindow::mousePressEvent." << std::endl;
    params_memory.putBool("DebugUI", true);
  }
}

void OnroadWindow::createMapWidget() {
#ifdef ENABLE_MAPS
  auto m = new MapPanel(get_mapbox_settings());
  map = m;
  QObject::connect(m, &MapPanel::mapPanelRequested, this, &OnroadWindow::mapPanelRequested);
  QObject::connect(nvg->map_settings_btn, &MapSettingsButton::clicked, m, &MapPanel::toggleMapSettings);
  nvg->map_settings_btn->setEnabled(true);

  m->setFixedWidth(topWidget(this)->width() / 2 - UI_BORDER_SIZE);
  split->insertWidget(0, m);
  // hidden by default, made visible when navRoute is published
  m->setVisible(false);
#endif
}

void OnroadWindow::offroadTransition(bool offroad) {
#ifdef ENABLE_MAPS
  if (!offroad) {
    if (map == nullptr && (uiState()->hasPrime() || !MAPBOX_TOKEN.isEmpty())) {
      createMapWidget();
    }
  }
#endif
  alerts->clear();
}

void OnroadWindow::primeChanged(bool prime) {
#ifdef ENABLE_MAPS
  if (map && (!prime && MAPBOX_TOKEN.isEmpty())) {
    nvg->map_settings_btn->setEnabled(false);
    nvg->map_settings_btn->setVisible(false);
    map->deleteLater();
    map = nullptr;
  } else if (!map && (prime || !MAPBOX_TOKEN.isEmpty())) {
    createMapWidget();
  }
#endif
}

void OnroadWindow::paintEvent(QPaintEvent *event) {
  try {
    QPainter p(this);

    // FrogPilot variables
    UIState *s = uiState();
    SubMaster &sm = *(s->sm);

    QRect rect = this->rect();
    QColor bgColor(bg.red(), bg.green(), bg.blue(), 255);
    p.fillRect(rect, bgColor);

    if (showSteering) {
      static float smoothedSteer = 0.0;

      smoothedSteer = 0.1 * std::abs(steer) + 0.9 * smoothedSteer;

      if (std::abs(smoothedSteer - steer) < 0.01) {
        smoothedSteer = steer;
      }

      int visibleHeight = rect.height() * smoothedSteer;

      QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
      gradient.setColorAt(0.0, bg_colors[STATUS_TRAFFIC_MODE_ACTIVE]);
      gradient.setColorAt(0.15, bg_colors[STATUS_EXPERIMENTAL_MODE_ACTIVE]);
      gradient.setColorAt(0.5, bg_colors[STATUS_CONDITIONAL_OVERRIDDEN]);
      gradient.setColorAt(0.85, bg_colors[STATUS_ENGAGED]);
      gradient.setColorAt(1.0, bg_colors[STATUS_ENGAGED]);

      QBrush brush(gradient);
      int fillWidth = UI_BORDER_SIZE;

      if (steeringAngleDeg != 0) {
        QRect rectToFill, rectToHide;
        if (steeringAngleDeg < 0) {
          rectToFill = QRect(rect.x(), rect.y() + rect.height() - visibleHeight, fillWidth, visibleHeight);
          rectToHide = QRect(rect.x(), rect.y(), fillWidth, rect.height() - visibleHeight);
        } else {
          rectToFill = QRect(rect.x() + rect.width() - fillWidth, rect.y() + rect.height() - visibleHeight, fillWidth, visibleHeight);
          rectToHide = QRect(rect.x() + rect.width() - fillWidth, rect.y(), fillWidth, rect.height() - visibleHeight);
        }
        p.fillRect(rectToFill, brush);
        p.fillRect(rectToHide, bgColor);
      }
    }

    if (showBlindspot) {
      QColor blindspotColorLeft = bgColor;
      QColor blindspotColorRight = bgColor;

      if (blindSpotLeft) {
        blindspotColorLeft = bg_colors[STATUS_TRAFFIC_MODE_ACTIVE];
      }

      if (blindSpotRight) {
        blindspotColorRight = bg_colors[STATUS_TRAFFIC_MODE_ACTIVE];
      }

      int xLeft = rect.x();
      int xRight = rect.x() + rect.width() / 2;
      QRect blindspotRectLeft(xLeft, rect.y(), rect.width() / 2, rect.height());
      QRect blindspotRectRight(xRight, rect.y(), rect.width() / 2, rect.height());

      p.fillRect(blindspotRectLeft, blindspotColorLeft);
      p.fillRect(blindspotRectRight, blindspotColorRight);
    }

    if (showSignal) {
      static int signalFramesLeft = 0;
      static int signalFramesRight = 0;

      bool blindSpotActive = (blindSpotLeft && turnSignalLeft) || (blindSpotRight && turnSignalRight);
      bool turnSignalActive = (turnSignalLeft && signalFramesLeft > 0) || (turnSignalRight && signalFramesRight > 0);

      QColor signalBorderColorLeft = bg;
      QColor signalBorderColorRight = bg;

      if (blindSpotLeft) {
        signalBorderColorLeft = bg_colors[STATUS_TRAFFIC_MODE_ACTIVE];
      }

      if (blindSpotRight) {
        signalBorderColorRight = bg_colors[STATUS_TRAFFIC_MODE_ACTIVE];
      }

      if (sm.frame % 20 == 0 || blindSpotActive || turnSignalActive) {
        QColor activeColor = bg_colors[STATUS_CONDITIONAL_OVERRIDDEN];

        if (turnSignalLeft) {
          signalFramesLeft = sm.frame % 10 == 0 && blindSpotActive ? 5 : sm.frame % 20 == 0 ? 10 : signalFramesLeft - 1;
          if (signalFramesLeft > 0) {
            signalBorderColorLeft = activeColor;
          }
        }

        if (turnSignalRight) {
          signalFramesRight = sm.frame % 10 == 0 && blindSpotActive ? 5 : sm.frame % 20 == 0 ? 10 : signalFramesRight - 1;
          if (signalFramesRight > 0) {
            signalBorderColorRight = activeColor;
          }
        }
      }

      int xLeft = rect.x();
      int xRight = rect.x() + rect.width() / 2;
      QRect signalRectLeft(xLeft, rect.y(), rect.width() / 2, rect.height());
      QRect signalRectRight(xRight, rect.y(), rect.width() / 2, rect.height());

      if (turnSignalLeft) {
        p.fillRect(signalRectLeft, signalBorderColorLeft);
      }

      if (turnSignalRight) {
        p.fillRect(signalRectRight, signalBorderColorRight);
      }
    }

    QString logicsDisplayString;

    if (showJerk) {
      static float maxAcceleration = 0.0f;
      maxAcceleration = std::max(currentAcceleration * nvg->accelerationConversion, maxAcceleration);
      logicsDisplayString += QString("Acceleration: %1 %3 - Max: %2 %3 | ")
                               .arg(currentAcceleration * nvg->accelerationConversion, 0, 'f', 1)
                               .arg(maxAcceleration, 0, 'f', 1)
                               .arg(nvg->accelerationUnit);
    }

    std::function<void(QString&, const QString&, int, int)> appendJerkInfo = [](QString& displayString, const QString& label, int value, int difference) {
      displayString += QString("%1: %2").arg(label).arg(value);
      if (difference != 0) {
        displayString += QString(" (%1%2)").arg(difference > 0 ? "-" : "").arg(difference);
      }
      displayString += " | ";
    };

    if (showJerk) {
      appendJerkInfo(logicsDisplayString, "Acceleration Jerk", accelerationJerk, accelerationJerkDifference);
      appendJerkInfo(logicsDisplayString, "Speed Jerk", speedJerk, speedJerkDifference);
    }

    if (showTuning) {
      if (liveValid) {
        logicsDisplayString += QString("Friction: %1 | Lateral Acceleration: %2").arg(friction, 0, 'f', 3).arg(latAccel, 0, 'f', 3);
      } else {
        logicsDisplayString += "Friction: Calculating... | Lateral Acceleration: Calculating...";
      }
    }

    if (logicsDisplayString.endsWith(" | ")) {
      logicsDisplayString.chop(3);
    }

    if (!logicsDisplayString.isEmpty()) {
      p.setFont(InterFont(28, QFont::DemiBold));
      p.setRenderHint(QPainter::TextAntialiasing);
      p.setPen(Qt::white);

      int logicsWidth = p.fontMetrics().horizontalAdvance(logicsDisplayString);
      int logicsX = (rect.width() - logicsWidth) / 2;
      int logicsY = rect.top() + 27;

      QStringList parts = logicsDisplayString.split(" | ");
      int currentX = logicsX;

      for (int i = 0; i < parts.size(); ++i) {
        const QString &part = parts[i];
        p.drawText(currentX, logicsY, part);
        currentX += p.fontMetrics().horizontalAdvance(part);

        if (i < parts.size() - 1) {
          QString separator = " | ";
          p.drawText(currentX, logicsY, separator);
          currentX += p.fontMetrics().horizontalAdvance(separator);
        }
      }
    }

    if (showFPS) {
      qint64 currentMillis = QDateTime::currentMSecsSinceEpoch();
      static std::queue<std::pair<qint64, float>> fpsQueue;

      static float avgFPS = 0.0;
      static float maxFPS = 0.0;
      static float minFPS = 99.9;

      minFPS = std::min(minFPS, fps);
      maxFPS = std::max(maxFPS, fps);

      fpsQueue.push({currentMillis, fps});

      while (!fpsQueue.empty() && currentMillis - fpsQueue.front().first > 60000) {
        fpsQueue.pop();
      }

      if (!fpsQueue.empty()) {
        float totalFPS = 0.0;
        for (auto tempQueue = fpsQueue; !tempQueue.empty(); tempQueue.pop()) {
          totalFPS += tempQueue.front().second;
        }
        avgFPS = totalFPS / fpsQueue.size();
      }

      QString fpsDisplayString = QString("FPS: %1 | Min: %3 | Max: %4 | Avg: %5")
          .arg(qRound(fps))
          .arg(qRound(minFPS))
          .arg(qRound(maxFPS))
          .arg(qRound(avgFPS));

      p.setFont(InterFont(28, QFont::DemiBold));
      p.setRenderHint(QPainter::TextAntialiasing);
      p.setPen(Qt::white);

      int textWidth = p.fontMetrics().horizontalAdvance(fpsDisplayString);
      int xPos = (rect.width() - textWidth) / 2;
      int yPos = rect.bottom() - 5;

      p.drawText(xPos, yPos, fpsDisplayString);
    }
  } catch (const std::exception &e) {
    std::cerr << "Exception in OnroadWindow::paintEvent: " << e.what() << std::endl;
    params_memory.putBool("DebugUI", true);
  } catch (...) {
    std::cerr << "Unknown exception in OnroadWindow::paintEvent." << std::endl;
    params_memory.putBool("DebugUI", true);
  }
}
