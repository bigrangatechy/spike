#include "InstallLocale.hpp"

#include <QFile>
#include <QHash>
#include <QRegularExpression>
#include <QSet>
#include <QTextStream>

namespace spike {
namespace {

QString zoneTabPath()
{
  return QStringLiteral("/usr/share/zoneinfo/zone.tab");
}

QString xkbRulesPath()
{
  return QStringLiteral("/usr/share/X11/xkb/rules/evdev.lst");
}

} // namespace

QList<QPair<QString, QString>> supportedLanguages()
{
  // INSTALLER.md — 11 launch languages (locale without .UTF-8; engine adds it).
  return {
      {QStringLiteral("English (United States)"), QStringLiteral("en_US")},
      {QStringLiteral("English (United Kingdom)"), QStringLiteral("en_GB")},
      {QStringLiteral("Français"), QStringLiteral("fr_FR")},
      {QStringLiteral("Deutsch"), QStringLiteral("de_DE")},
      {QStringLiteral("Español"), QStringLiteral("es_ES")},
      {QStringLiteral("Italiano"), QStringLiteral("it_IT")},
      {QStringLiteral("Português (Brasil)"), QStringLiteral("pt_BR")},
      {QStringLiteral("Nederlands"), QStringLiteral("nl_NL")},
      {QStringLiteral("Polski"), QStringLiteral("pl_PL")},
      {QStringLiteral("Русский"), QStringLiteral("ru_RU")},
      {QStringLiteral("日本語"), QStringLiteral("ja_JP")},
  };
}

QStringList availableTimeZones()
{
  QSet<QString> zones;
  zones.insert(QStringLiteral("UTC"));

  QFile f(zoneTabPath());
  if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&f);
    while (!in.atEnd()) {
      const QString line = in.readLine().trimmed();
      if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) {
        continue;
      }
      // zone.tab: code coords TZ [comments] — fields separated by tabs
      const QStringList parts = line.split(QLatin1Char('\t'));
      if (parts.size() >= 3) {
        const QString tz = parts.at(2).trimmed();
        if (!tz.isEmpty() && tz.contains(QLatin1Char('/'))) {
          zones.insert(tz);
        }
      }
    }
  }

  // Fallback if zone.tab missing (unusual on live ISO)
  if (zones.size() <= 1) {
    for (const QString &z : {
             QStringLiteral("Africa/Cairo"),
             QStringLiteral("Africa/Johannesburg"),
             QStringLiteral("America/Chicago"),
             QStringLiteral("America/Denver"),
             QStringLiteral("America/Los_Angeles"),
             QStringLiteral("America/Mexico_City"),
             QStringLiteral("America/New_York"),
             QStringLiteral("America/Sao_Paulo"),
             QStringLiteral("America/Toronto"),
             QStringLiteral("Asia/Dubai"),
             QStringLiteral("Asia/Hong_Kong"),
             QStringLiteral("Asia/Kolkata"),
             QStringLiteral("Asia/Seoul"),
             QStringLiteral("Asia/Shanghai"),
             QStringLiteral("Asia/Singapore"),
             QStringLiteral("Asia/Tokyo"),
             QStringLiteral("Australia/Adelaide"),
             QStringLiteral("Australia/Brisbane"),
             QStringLiteral("Australia/Melbourne"),
             QStringLiteral("Australia/Perth"),
             QStringLiteral("Australia/Sydney"),
             QStringLiteral("Europe/Amsterdam"),
             QStringLiteral("Europe/Berlin"),
             QStringLiteral("Europe/London"),
             QStringLiteral("Europe/Madrid"),
             QStringLiteral("Europe/Moscow"),
             QStringLiteral("Europe/Paris"),
             QStringLiteral("Europe/Rome"),
             QStringLiteral("Europe/Warsaw"),
             QStringLiteral("Pacific/Auckland"),
         }) {
      zones.insert(z);
    }
  }

  QStringList out = zones.values();
  out.sort(Qt::CaseInsensitive);
  // Keep UTC first for discoverability
  out.removeAll(QStringLiteral("UTC"));
  out.prepend(QStringLiteral("UTC"));
  return out;
}

QList<QPair<QString, QString>> availableKeyboardLayouts()
{
  QList<QPair<QString, QString>> layouts;
  QFile f(xkbRulesPath());
  if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&f);
    bool inLayout = false;
    while (!in.atEnd()) {
      const QString line = in.readLine();
      if (line.startsWith(QLatin1String("! "))) {
        inLayout = line.trimmed() == QLatin1String("! layout");
        continue;
      }
      if (!inLayout || line.trimmed().isEmpty()) {
        continue;
      }
      // "  us              English (US)"
      const QString trimmed = line.trimmed();
      const int sp = trimmed.indexOf(QRegularExpression(QStringLiteral("\\s")));
      if (sp <= 0) {
        continue;
      }
      const QString id = trimmed.left(sp).trimmed();
      const QString name = trimmed.mid(sp).trimmed();
      if (id.isEmpty()) {
        continue;
      }
      layouts.append({name.isEmpty() ? id : QStringLiteral("%1 (%2)").arg(name, id), id});
    }
  }

  if (layouts.isEmpty()) {
    layouts = {
        {QStringLiteral("English (US) (us)"), QStringLiteral("us")},
        {QStringLiteral("English (UK) (gb)"), QStringLiteral("gb")},
        {QStringLiteral("French (fr)"), QStringLiteral("fr")},
        {QStringLiteral("German (de)"), QStringLiteral("de")},
        {QStringLiteral("Spanish (es)"), QStringLiteral("es")},
        {QStringLiteral("Italian (it)"), QStringLiteral("it")},
        {QStringLiteral("Portuguese (Brazil) (br)"), QStringLiteral("br")},
        {QStringLiteral("Dutch (nl)"), QStringLiteral("nl")},
        {QStringLiteral("Polish (pl)"), QStringLiteral("pl")},
        {QStringLiteral("Russian (ru)"), QStringLiteral("ru")},
        {QStringLiteral("Japanese (jp)"), QStringLiteral("jp")},
    };
  }
  return layouts;
}

QString suggestKeyboardForLanguage(const QString &langCode)
{
  static const QHash<QString, QString> map = {
      {QStringLiteral("en_US"), QStringLiteral("us")},
      {QStringLiteral("en_GB"), QStringLiteral("gb")},
      {QStringLiteral("fr_FR"), QStringLiteral("fr")},
      {QStringLiteral("de_DE"), QStringLiteral("de")},
      {QStringLiteral("es_ES"), QStringLiteral("es")},
      {QStringLiteral("it_IT"), QStringLiteral("it")},
      {QStringLiteral("pt_BR"), QStringLiteral("br")},
      {QStringLiteral("nl_NL"), QStringLiteral("nl")},
      {QStringLiteral("pl_PL"), QStringLiteral("pl")},
      {QStringLiteral("ru_RU"), QStringLiteral("ru")},
      {QStringLiteral("ja_JP"), QStringLiteral("jp")},
  };
  return map.value(langCode, QStringLiteral("us"));
}

QString suggestKeyboardForTimezone(const QString &timezone)
{
  const QString tz = timezone;
  if (tz.startsWith(QLatin1String("America/")) || tz == QLatin1String("UTC")) {
    if (tz.contains(QLatin1String("Toronto")) || tz.contains(QLatin1String("Vancouver")) ||
        tz.contains(QLatin1String("Montreal"))) {
      return QStringLiteral("us");
    }
    if (tz.contains(QLatin1String("Mexico")) || tz.contains(QLatin1String("Argentina")) ||
        tz.contains(QLatin1String("Santiago")) || tz.contains(QLatin1String("Bogota")) ||
        tz.contains(QLatin1String("Lima")) || tz.contains(QLatin1String("Caracas"))) {
      return QStringLiteral("latam");
    }
    if (tz.contains(QLatin1String("Sao_Paulo")) || tz.contains(QLatin1String("Bahia")) ||
        tz.contains(QLatin1String("Fortaleza")) || tz.contains(QLatin1String("Manaus"))) {
      return QStringLiteral("br");
    }
    return QStringLiteral("us");
  }
  if (tz.startsWith(QLatin1String("Europe/London")) || tz.contains(QLatin1String("Dublin")) ||
      tz.contains(QLatin1String("Isle_of_Man")) || tz.contains(QLatin1String("Jersey")) ||
      tz.contains(QLatin1String("Guernsey"))) {
    return QStringLiteral("gb");
  }
  if (tz.contains(QLatin1String("Berlin")) || tz.contains(QLatin1String("Vienna")) ||
      tz.contains(QLatin1String("Zurich")) || tz.contains(QLatin1String("Vaduz"))) {
    return QStringLiteral("de");
  }
  if (tz.contains(QLatin1String("Paris")) || tz.contains(QLatin1String("Brussels")) ||
      tz.contains(QLatin1String("Luxembourg"))) {
    return QStringLiteral("fr");
  }
  if (tz.contains(QLatin1String("Madrid")) || tz.contains(QLatin1String("Canary"))) {
    return QStringLiteral("es");
  }
  if (tz.contains(QLatin1String("Rome")) || tz.contains(QLatin1String("Vatican")) ||
      tz.contains(QLatin1String("San_Marino"))) {
    return QStringLiteral("it");
  }
  if (tz.contains(QLatin1String("Amsterdam"))) {
    return QStringLiteral("nl");
  }
  if (tz.contains(QLatin1String("Warsaw"))) {
    return QStringLiteral("pl");
  }
  if (tz.contains(QLatin1String("Moscow"))) {
    return QStringLiteral("ru");
  }
  if (tz.startsWith(QLatin1String("Asia/Tokyo")) || tz.contains(QLatin1String("Osaka"))) {
    return QStringLiteral("jp");
  }
  if (tz.startsWith(QLatin1String("Australia/")) || tz.startsWith(QLatin1String("Pacific/Auckland"))) {
    return QStringLiteral("us"); // common AU/NZ default; users often use us or gb
  }
  return QString();
}

QString suggestTimezoneForLanguage(const QString &langCode)
{
  static const QHash<QString, QString> map = {
      {QStringLiteral("en_US"), QStringLiteral("America/New_York")},
      {QStringLiteral("en_GB"), QStringLiteral("Europe/London")},
      {QStringLiteral("fr_FR"), QStringLiteral("Europe/Paris")},
      {QStringLiteral("de_DE"), QStringLiteral("Europe/Berlin")},
      {QStringLiteral("es_ES"), QStringLiteral("Europe/Madrid")},
      {QStringLiteral("it_IT"), QStringLiteral("Europe/Rome")},
      {QStringLiteral("pt_BR"), QStringLiteral("America/Sao_Paulo")},
      {QStringLiteral("nl_NL"), QStringLiteral("Europe/Amsterdam")},
      {QStringLiteral("pl_PL"), QStringLiteral("Europe/Warsaw")},
      {QStringLiteral("ru_RU"), QStringLiteral("Europe/Moscow")},
      {QStringLiteral("ja_JP"), QStringLiteral("Asia/Tokyo")},
  };
  return map.value(langCode, QStringLiteral("UTC"));
}

} // namespace spike
