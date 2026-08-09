#pragma once

#include <QString>

namespace spike {

/** Authenticate the current user via PAM (service spike-lock, else login). */
bool pamAuthenticateUser(const QString &password, QString *errorOut = nullptr);

} // namespace spike
