#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>

namespace spike {

/** Display label + locale id (e.g. en_US) — INSTALLER.md launch languages. */
QList<QPair<QString, QString>> supportedLanguages();

/** IANA zones from /usr/share/zoneinfo/zone.tab (+ UTC), sorted. */
QStringList availableTimeZones();

/**
 * XKB layouts from /usr/share/X11/xkb/rules/evdev.lst (fallback: common set).
 * Pair: display "English (US)" → layout id "us".
 */
QList<QPair<QString, QString>> availableKeyboardLayouts();

/** Default XKBLAYOUT for a language code (en_US → us). */
QString suggestKeyboardForLanguage(const QString &langCode);

/** INSTALLER.md timezone → keyboard hint (best-effort). */
QString suggestKeyboardForTimezone(const QString &timezone);

/** Best-effort timezone preselect from language. */
QString suggestTimezoneForLanguage(const QString &langCode);

} // namespace spike
