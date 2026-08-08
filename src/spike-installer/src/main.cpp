#include "InstallWizard.hpp"

#include <QApplication>
#include <QPalette>

namespace {

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
  QApplication::setApplicationName(QStringLiteral("spike-installer"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.1"));
  QApplication::setOrganizationName(QStringLiteral("BigRangaTech"));
  darkPalette(app);

  spike::InstallWizard wiz;
  wiz.show();
  return app.exec();
}
