#include <QColor>
#include <QApplication>
#include <QLabel>
#include <QPalette>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace {

void darkPalette(QApplication &app)
{
  QPalette pal = app.palette();
  pal.setColor(QPalette::Window, QColor(0x1a, 0x1a, 0x2e));
  pal.setColor(QPalette::WindowText, QColor(0xff, 0xff, 0xff));
  pal.setColor(QPalette::Base, QColor(0x22, 0x22, 0x36));
  pal.setColor(QPalette::Text, QColor(0xff, 0xff, 0xff));
  pal.setColor(QPalette::Button, QColor(0x22, 0x22, 0x36));
  pal.setColor(QPalette::ButtonText, QColor(0xff, 0xff, 0xff));
  pal.setColor(QPalette::Highlight, QColor(0x6d, 0x4a, 0xff));
  app.setPalette(pal);
}

} // namespace

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-migration"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.1"));
  QApplication::setOrganizationName(QStringLiteral("BigRangaTech"));
  darkPalette(app);

  QWidget win;
  win.setWindowTitle(QStringLiteral("Move My Files [pre-alpha]"));
  win.resize(520, 320);
  auto *lay = new QVBoxLayout(&win);
  lay->addWidget(new QLabel(
      QStringLiteral(
          "<h2>Move My Files</h2>"
          "<p>Guided migration wizard (SPIKE-MIGRATION.md) is still being built.</p>"
          "<p>For now, use <b>Rescue My Files</b> to recover or restore "
          "personal data with the shared <code>SpikeBackup/</code> layout.</p>"
          "<p>After a backup on USB, run <b>Install Spike</b> and optionally "
          "restore into the new home.</p>"),
      &win));
  auto *rescue = new QPushButton(QStringLiteral("Open Rescue My Files…"), &win);
  auto *install = new QPushButton(QStringLiteral("Open Install Spike…"), &win);
  auto *closeBtn = new QPushButton(QStringLiteral("Close"), &win);
  lay->addWidget(rescue);
  lay->addWidget(install);
  lay->addStretch(1);
  lay->addWidget(closeBtn);
  QObject::connect(rescue, &QPushButton::clicked, &win, []() {
    QProcess::startDetached(QStringLiteral("spike-rescue"), {});
  });
  QObject::connect(install, &QPushButton::clicked, &win, []() {
    QProcess::startDetached(QStringLiteral("spike-installer"), {});
  });
  QObject::connect(closeBtn, &QPushButton::clicked, &win, &QWidget::close);

  win.show();
  return app.exec();
}
