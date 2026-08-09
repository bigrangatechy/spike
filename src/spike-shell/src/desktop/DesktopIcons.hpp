#pragma once

#include <QString>
#include <QVector>
#include <QWidget>

class QGridLayout;
class QToolButton;

namespace spike {

struct DesktopEntry {
  QString path;
  QString name;
  QString icon;
  QString exec;
};

/** Icons for ~/Desktop .desktop launchers (live Install / Rescue / Move My Files). */
class DesktopIcons : public QWidget
{
  Q_OBJECT

public:
  explicit DesktopIcons(QWidget *parent = nullptr);

public slots:
  void reload();

private:
  static QVector<DesktopEntry> scanDesktop();
  static bool ensureSeeded();
  void launch(const DesktopEntry &entry);

  QGridLayout *m_grid = nullptr;
};

} // namespace spike
