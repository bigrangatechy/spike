#include "MainWindow.hpp"

#include <QApplication>
#include <QFile>
#include <QPalette>

namespace {

void loadStyle(QApplication &app)
{
  QFile f(QStringLiteral(":/styles/spike-rescue.qss"));
  if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    app.setStyleSheet(QString::fromUtf8(f.readAll()));
  }
}

void darkPalette(QApplication &app)
{
  QPalette pal = app.palette();
  const QColor bg(0x1a, 0x1a, 0x2e);
  const QColor panel(0x22, 0x22, 0x36);
  const QColor text(0xff, 0xff, 0xff);
  const QColor accent(0x6d, 0x4a, 0xff);
  pal.setColor(QPalette::Window, bg);
  pal.setColor(QPalette::WindowText, text);
  pal.setColor(QPalette::Base, panel);
  pal.setColor(QPalette::Text, text);
  pal.setColor(QPalette::Button, panel);
  pal.setColor(QPalette::ButtonText, text);
  pal.setColor(QPalette::Highlight, accent);
  pal.setColor(QPalette::HighlightedText, text);
  app.setPalette(pal);
}

} // namespace

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-rescue"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.7"));
  QApplication::setOrganizationName(QStringLiteral("BigRangaTech"));

  darkPalette(app);
  loadStyle(app);

  spike::MainWindow win;
  win.show();
  return app.exec();
}
