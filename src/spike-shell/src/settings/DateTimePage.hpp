#pragma once

class QLabel;
class QWidget;

namespace spike {

/** Settings → Date & Time — timedatectl / systemd-timedated (DESKTOP.md). */
QWidget *makeDateTimePage(QWidget *parent, QLabel *statusBar);

} // namespace spike
