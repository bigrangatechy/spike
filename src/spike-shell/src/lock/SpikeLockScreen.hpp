#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;
class QTimer;
class QShowEvent;
class QKeyEvent;

namespace spike {

/** Fullscreen lock UI (SECURITY.md MVP — PAM unlock). */
class SpikeLockScreen : public QWidget
{
  Q_OBJECT

public:
  static SpikeLockScreen &instance();

  /** Show overlay lock; no-op if already locked. */
  void lock();
  bool isLocked() const { return m_locked; }

signals:
  void unlocked();

protected:
  void showEvent(QShowEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private slots:
  void tryUnlock();
  void updateClock();

private:
  explicit SpikeLockScreen(QWidget *parent = nullptr);
  void applyLayerShell();
  void runPower(const QString &logindMethod, const QString &systemctlVerb);

  bool m_locked = false;
  QLabel *m_clock = nullptr;
  QLabel *m_date = nullptr;
  QLabel *m_status = nullptr;
  QLineEdit *m_password = nullptr;
  QPushButton *m_unlock = nullptr;
  QTimer *m_clockTimer = nullptr;
};

} // namespace spike
