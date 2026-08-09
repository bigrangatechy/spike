#pragma once

#include <QWidget>

class QHBoxLayout;

namespace spike {

/**
 * Panel task list: icons for open/minimized windows (restore on click).
 * Fed by KWin script spike-tasklist over D-Bus; wmctrl fallback for X11.
 */
class WindowListApplet : public QWidget
{
  Q_OBJECT

public:
  explicit WindowListApplet(QWidget *parent = nullptr);
  void setEnabledVisible(bool on);
  /** Called from TaskListAdaptor when the KWin script publishes a blob. */
  void applyWindowBlob(const QString &blob);

private slots:
  void refreshFallback();

private:
  struct Win {
    QString uuid;
    QString caption;
    QString resourceClass;
    QString resourceName;
    bool minimized = false;
  };

  void rebuild(const QVector<Win> &wins);
  void activateWindow(const QString &uuid);
  void ensureKwinScript();
  bool registerDbus();
  QIcon iconFor(const Win &w) const;
  QVector<Win> listWindowsWmctrl() const;

  QHBoxLayout *m_lay = nullptr;
  bool m_userVisible = true;
  bool m_dbusReady = false;
  bool m_scriptLoaded = false;
  bool m_usingKwinFeed = false;
};

} // namespace spike
