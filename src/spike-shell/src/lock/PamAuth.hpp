#pragma once

#include <QString>

namespace spike {

/** Authenticate the current user via PAM (service spike-lock, else login). */
bool pamAuthenticateUser(const QString &password, QString *errorOut = nullptr);

/** Authenticate an arbitrary username (login greeter). Uses PAM service "login". */
bool pamAuthenticateLogin(const QString &username, const QString &password,
                          QString *errorOut = nullptr);

} // namespace spike
