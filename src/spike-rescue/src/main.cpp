#include "MainWindow.hpp"
#include "RescueEngine.hpp"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QEventLoop>
#include <QFile>
#include <QPalette>
#include <QTextStream>

namespace {

bool partitionOnDisk(const QString &part, const QString &disk)
{
  if (disk.isEmpty() || part.isEmpty()) {
    return false;
  }
  if (part == disk) {
    return true;
  }
  // NVMe / mmc: /dev/nvme0n1p2, /dev/mmcblk0p1
  if (part.startsWith(disk + QLatin1String("p")) && part.size() > disk.size() + 1) {
    return part.at(disk.size() + 1).isDigit();
  }
  // SCSI/SATA: /dev/sda1
  if (part.startsWith(disk) && part.size() > disk.size()) {
    return part.at(disk.size()).isDigit();
  }
  return false;
}

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

int batchListSystems()
{
  spike::RescueEngine engine;
  // Installer / migration only need partition + OS + users — skip find-files inventory
  // (that walk hung the installer GUI for minutes on large home trees).
  engine.setIncludeInventoryOnScan(false);
  bool ok = false;
  QObject::connect(&engine, &spike::RescueEngine::scanFinished, [&](bool s) { ok = s; });
  QEventLoop loop;
  QObject::connect(&engine, &spike::RescueEngine::scanFinished, &loop, &QEventLoop::quit);
  QTextStream err(stderr);
  QObject::connect(&engine, &spike::RescueEngine::scanProgress, [&](const QString &m, int p) {
    err << QStringLiteral("[%1%] %2\n").arg(p).arg(m);
    err.flush();
  });
  engine.scanSystems();
  loop.exec();
  if (!ok) {
    err << QStringLiteral("ERROR: scan failed: %1\n").arg(engine.lastError());
    return 1;
  }
  QTextStream out(stdout);
  const auto systems = engine.systems();
  out << QStringLiteral("systems=%1\n").arg(systems.size());
  for (int i = 0; i < systems.size(); ++i) {
    const auto &s = systems.at(i);
    // Build without multi-arg .arg() — labels can contain % and break place markers.
    QString line = QString::number(i);
    line += QLatin1Char('\t');
    line += s.partition.path;
    line += QLatin1Char('\t');
    line += s.osLabel;
    line += QLatin1Char('\t');
    line += s.partition.fstype;
    line += QLatin1Char('\t');
    line += s.users.join(QLatin1Char(','));
    out << line << QLatin1Char('\n');
  }
  out.flush();
  if (!engine.lastScanSummary().isEmpty()) {
    err << engine.lastScanSummary() << QLatin1Char('\n');
  }
  return systems.isEmpty() ? 2 : 0;
}

int batchRecover(const QString &dest, int systemIndex, const QString &excludeDisk,
                 const QString &partitionPath)
{
  QTextStream err(stderr);
  QTextStream out(stdout);
  if (dest.isEmpty()) {
    err << QStringLiteral("ERROR: --dest is required\n");
    return 1;
  }

  spike::RescueEngine engine;
  engine.setIncludeInventoryOnScan(false);
  QObject::connect(&engine, &spike::RescueEngine::scanProgress, [&](const QString &m, int p) {
    err << QStringLiteral("[%1%] %2\n").arg(p).arg(m);
    err.flush();
  });
  QObject::connect(&engine, &spike::RescueEngine::copyProgress,
                   [&](const QString &f, qint64 done, qint64 total, qint64, qint64) {
                     err << QStringLiteral("copy %1/%2 %3\n").arg(done).arg(total).arg(f);
                     err.flush();
                   });

  bool scanOk = false;
  {
    QEventLoop loop;
    QObject::connect(&engine, &spike::RescueEngine::scanFinished, &loop, &QEventLoop::quit);
    QObject::connect(&engine, &spike::RescueEngine::scanFinished, [&](bool s) { scanOk = s; });
    engine.scanSystems();
    loop.exec();
  }
  if (!scanOk) {
    err << QStringLiteral("ERROR: scan failed: %1\n").arg(engine.lastError());
    return 1;
  }

  QList<int> eligible;
  for (int i = 0; i < engine.systems().size(); ++i) {
    const QString &path = engine.systems().at(i).partition.path;
    if (partitionOnDisk(path, excludeDisk)) {
      err << QStringLiteral("skip (excluded disk): %1\n").arg(path);
      err.flush();
      continue;
    }
    eligible.append(i);
  }

  if (eligible.isEmpty()) {
    if (!excludeDisk.isEmpty()) {
      err << QStringLiteral("WARN: no recoverable systems outside %1 — skipping backup\n")
                 .arg(excludeDisk);
      out << QStringLiteral("SKIPPED=no-eligible-systems\n");
      out.flush();
      return 0;
    }
    err << QStringLiteral("ERROR: no recoverable systems found\n");
    if (!engine.lastScanSummary().isEmpty()) {
      err << engine.lastScanSummary() << QLatin1Char('\n');
    }
    return 2;
  }

  int pickEligible = systemIndex;
  if (!partitionPath.isEmpty()) {
    pickEligible = -1;
    for (int e = 0; e < eligible.size(); ++e) {
      if (engine.systems().at(eligible.at(e)).partition.path == partitionPath) {
        pickEligible = e;
        break;
      }
    }
    if (pickEligible < 0) {
      err << QStringLiteral("ERROR: --partition %1 not in eligible systems\n").arg(partitionPath);
      return 1;
    }
  } else if (systemIndex < 0 || systemIndex >= eligible.size()) {
    err << QStringLiteral("ERROR: --system %1 out of range (0..%2 eligible)\n")
               .arg(systemIndex)
               .arg(eligible.size() - 1);
    return 1;
  }
  const int realIndex = eligible.at(pickEligible);

  bool invOk = false;
  {
    QEventLoop loop;
    QObject::connect(&engine, &spike::RescueEngine::inventoryFinished, &loop, &QEventLoop::quit);
    QObject::connect(&engine, &spike::RescueEngine::inventoryFinished, [&](bool s) { invOk = s; });
    engine.inventorySystem(realIndex);
    loop.exec();
  }
  if (!invOk) {
    err << QStringLiteral("ERROR: inventory failed: %1\n").arg(engine.lastError());
    engine.cleanupMounts();
    return 1;
  }

  bool copyOk = false;
  {
    QEventLoop loop;
    QObject::connect(&engine, &spike::RescueEngine::copyFinished, &loop, &QEventLoop::quit);
    QObject::connect(&engine, &spike::RescueEngine::copyFinished, [&](bool s) { copyOk = s; });
    engine.startCopy(realIndex, dest);
    loop.exec();
  }
  engine.cleanupMounts();

  const auto result = engine.lastCopy();
  if (!copyOk || result.destRoot.isEmpty()) {
    err << QStringLiteral("ERROR: recover failed: %1\n")
               .arg(engine.lastError().isEmpty() ? QStringLiteral("copy failed")
                                                 : engine.lastError());
    return 1;
  }
  out << QStringLiteral("OK\n");
  out << QStringLiteral("SESSION_PATH=%1\n").arg(result.destRoot);
  out << QStringLiteral("COPIED=%1\n").arg(result.copied);
  out << QStringLiteral("BYTES=%1\n").arg(result.bytesCopied);
  out.flush();
  return 0;
}

int batchRestore(const QString &session, const QString &home)
{
  QTextStream err(stderr);
  QTextStream out(stdout);
  if (session.isEmpty() || home.isEmpty()) {
    err << QStringLiteral("ERROR: --session and --home are required\n");
    return 1;
  }

  spike::RescueEngine engine;
  QObject::connect(&engine, &spike::RescueEngine::copyProgress,
                   [&](const QString &f, qint64 done, qint64 total, qint64, qint64) {
                     err << QStringLiteral("restore %1/%2 %3\n").arg(done).arg(total).arg(f);
                     err.flush();
                   });

  bool ok = false;
  {
    QEventLoop loop;
    QObject::connect(&engine, &spike::RescueEngine::copyFinished, &loop, &QEventLoop::quit);
    QObject::connect(&engine, &spike::RescueEngine::copyFinished, [&](bool s) { ok = s; });
    engine.startRestoreFromPath(session, home);
    loop.exec();
  }

  const auto result = engine.lastCopy();
  if (!ok) {
    err << QStringLiteral("ERROR: restore failed: %1\n")
               .arg(engine.lastError().isEmpty() ? QStringLiteral("restore failed")
                                                 : engine.lastError());
    return 1;
  }
  out << QStringLiteral("OK\n");
  out << QStringLiteral("COPIED=%1\n").arg(result.copied);
  out << QStringLiteral("BYTES=%1\n").arg(result.bytesCopied);
  out.flush();
  return 0;
}

} // namespace

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("spike-rescue"));
  QApplication::setApplicationVersion(QStringLiteral("0.0.11"));
  QApplication::setOrganizationName(QStringLiteral("BigRangaTech"));

  QCommandLineParser parser;
  parser.setApplicationDescription(
      QStringLiteral("Spike Rescue — recover/restore personal files (DISASTER-RECOVERY.md)"));
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption listSystemsOpt(QStringLiteral("list-systems"),
                                    QStringLiteral("Scan and print recoverable OS indexes"));
  QCommandLineOption batchRecoverOpt(QStringLiteral("batch-recover"),
                                     QStringLiteral("Non-GUI recover into SpikeBackup/ on --dest"));
  QCommandLineOption batchRestoreOpt(QStringLiteral("batch-restore"),
                                     QStringLiteral("Non-GUI restore session into --home"));
  QCommandLineOption destOpt(QStringLiteral("dest"), QStringLiteral("Writable mount for SpikeBackup/"),
                             QStringLiteral("path"));
  QCommandLineOption systemOpt(QStringLiteral("system"), QStringLiteral("System index from --list-systems"),
                               QStringLiteral("index"), QStringLiteral("0"));
  QCommandLineOption partitionOpt(
      QStringLiteral("partition"),
      QStringLiteral("Prefer this block device path over --system index"),
      QStringLiteral("path"));
  QCommandLineOption excludeDiskOpt(
      QStringLiteral("exclude-disk"),
      QStringLiteral("Skip partitions on this disk (installer wipe target)"),
      QStringLiteral("disk"));
  QCommandLineOption sessionOpt(QStringLiteral("session"),
                                QStringLiteral("SpikeBackup session path for restore"),
                                QStringLiteral("path"));
  QCommandLineOption homeOpt(QStringLiteral("home"), QStringLiteral("Target home directory for restore"),
                             QStringLiteral("path"));

  parser.addOption(listSystemsOpt);
  parser.addOption(batchRecoverOpt);
  parser.addOption(batchRestoreOpt);
  parser.addOption(destOpt);
  parser.addOption(systemOpt);
  parser.addOption(partitionOpt);
  parser.addOption(excludeDiskOpt);
  parser.addOption(sessionOpt);
  parser.addOption(homeOpt);
  parser.process(app);

  const bool batch = parser.isSet(listSystemsOpt) || parser.isSet(batchRecoverOpt) ||
                     parser.isSet(batchRestoreOpt);
  if (batch) {
    if (parser.isSet(listSystemsOpt)) {
      return batchListSystems();
    }
    if (parser.isSet(batchRecoverOpt)) {
      bool ok = false;
      const int idx = parser.value(systemOpt).toInt(&ok);
      return batchRecover(parser.value(destOpt), ok ? idx : 0, parser.value(excludeDiskOpt),
                          parser.value(partitionOpt));
    }
    if (parser.isSet(batchRestoreOpt)) {
      return batchRestore(parser.value(sessionOpt), parser.value(homeOpt));
    }
  }

  darkPalette(app);
  loadStyle(app);

  spike::MainWindow win;
  win.show();
  return app.exec();
}
