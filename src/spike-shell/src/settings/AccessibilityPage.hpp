#pragma once

class QLabel;
class QWidget;

namespace spike {

class ConfigClient;

/** Settings → Accessibility — sticky/slow/bounce/mouse keys (Plasma-free). */
QWidget *makeAccessibilityPage(QWidget *parent, ConfigClient *config, QLabel *statusBar);

} // namespace spike
