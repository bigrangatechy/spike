#pragma once

#include <QWidget>

namespace spike {

class DesktopIcons;

/** Full-screen layer-shell background for wallpaper + desktop icons (DESKTOP.md). */
class DesktopBackground : public QWidget
{
  Q_OBJECT

public:
  explicit DesktopBackground(QWidget *parent = nullptr);

  static DesktopBackground *instance();

  void applyLayerShell();
  bool setImagePath(const QString &path);
  QString imagePath() const { return m_path; }
  void reloadIcons();

protected:
  void paintEvent(QPaintEvent *event) override;
  void showEvent(QShowEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  void reloadPixmap();
  void layoutIcons();

  QString m_path;
  QPixmap m_pixmap;
  DesktopIcons *m_icons = nullptr;
  static DesktopBackground *s_instance;
};

} // namespace spike
