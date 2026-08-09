#include "lock/PamAuth.hpp"

#include <QByteArray>
#include <dlfcn.h>

#include <cstdlib>
#include <cstring>

namespace spike {

namespace {

// Minimal PAM ABI (matches Linux-PAM) so we link via dlopen without libpam0g-dev.
struct pam_message {
  int msg_style;
  const char *msg;
};

struct pam_response {
  char *resp;
  int resp_retcode;
};

struct pam_conv {
  int (*conv)(int num_msg, const struct pam_message **msg, struct pam_response **resp,
              void *appdata_ptr);
  void *appdata_ptr;
};

using pam_handle_t = struct pam_handle;

constexpr int PAM_SUCCESS = 0;
constexpr int PAM_PROMPT_ECHO_OFF = 1;
constexpr int PAM_PROMPT_ECHO_ON = 2;
constexpr int PAM_ERROR_MSG = 3;
constexpr int PAM_TEXT_INFO = 4;
constexpr int PAM_CONV_ERR = 19;

struct ConvData {
  QByteArray password;
};

int conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp,
                 void *appdata_ptr)
{
  auto *data = static_cast<ConvData *>(appdata_ptr);
  if (!data || num_msg <= 0) {
    return PAM_CONV_ERR;
  }
  auto *replies = static_cast<pam_response *>(calloc(static_cast<size_t>(num_msg), sizeof(pam_response)));
  if (!replies) {
    return PAM_CONV_ERR;
  }
  for (int i = 0; i < num_msg; ++i) {
    const int style = msg[i]->msg_style;
    if (style == PAM_PROMPT_ECHO_OFF || style == PAM_PROMPT_ECHO_ON) {
      replies[i].resp = strdup(data->password.constData());
      if (!replies[i].resp) {
        for (int j = 0; j < i; ++j) {
          free(replies[j].resp);
        }
        free(replies);
        return PAM_CONV_ERR;
      }
    } else if (style == PAM_ERROR_MSG || style == PAM_TEXT_INFO) {
      replies[i].resp = nullptr;
    } else {
      for (int j = 0; j < i; ++j) {
        free(replies[j].resp);
      }
      free(replies);
      return PAM_CONV_ERR;
    }
  }
  *resp = replies;
  return PAM_SUCCESS;
}

} // namespace

bool pamAuthenticateUser(const QString &password, QString *errorOut)
{
  void *lib = dlopen("libpam.so.0", RTLD_LAZY | RTLD_LOCAL);
  if (!lib) {
    if (errorOut) {
      *errorOut = QStringLiteral("PAM library not available");
    }
    return false;
  }

  using PamStart = int (*)(const char *, const char *, const struct pam_conv *, pam_handle_t **);
  using PamAuth = int (*)(pam_handle_t *, int);
  using PamAcct = int (*)(pam_handle_t *, int);
  using PamEnd = int (*)(pam_handle_t *, int);
  using PamStrError = const char *(*)(pam_handle_t *, int);

  auto pam_start = reinterpret_cast<PamStart>(dlsym(lib, "pam_start"));
  auto pam_authenticate = reinterpret_cast<PamAuth>(dlsym(lib, "pam_authenticate"));
  auto pam_acct_mgmt = reinterpret_cast<PamAcct>(dlsym(lib, "pam_acct_mgmt"));
  auto pam_end = reinterpret_cast<PamEnd>(dlsym(lib, "pam_end"));
  auto pam_strerror = reinterpret_cast<PamStrError>(dlsym(lib, "pam_strerror"));

  if (!pam_start || !pam_authenticate || !pam_acct_mgmt || !pam_end) {
    dlclose(lib);
    if (errorOut) {
      *errorOut = QStringLiteral("PAM symbols missing");
    }
    return false;
  }

  ConvData data;
  data.password = password.toUtf8();
  pam_conv conv{};
  conv.conv = conversation;
  conv.appdata_ptr = &data;

  const QByteArray user = qgetenv("USER");
  if (user.isEmpty()) {
    dlclose(lib);
    if (errorOut) {
      *errorOut = QStringLiteral("USER not set");
    }
    return false;
  }

  pam_handle_t *pamh = nullptr;
  const char *service = "spike-lock";
  int rc = pam_start(service, user.constData(), &conv, &pamh);
  if (rc != PAM_SUCCESS) {
    // Fall back to login if spike-lock is not installed yet.
    rc = pam_start("login", user.constData(), &conv, &pamh);
  }
  if (rc != PAM_SUCCESS || !pamh) {
    if (errorOut) {
      *errorOut = QStringLiteral("pam_start failed");
    }
    dlclose(lib);
    return false;
  }

  rc = pam_authenticate(pamh, 0);
  if (rc == PAM_SUCCESS) {
    rc = pam_acct_mgmt(pamh, 0);
  }

  if (rc != PAM_SUCCESS && errorOut) {
    if (pam_strerror) {
      *errorOut = QString::fromUtf8(pam_strerror(pamh, rc));
    } else {
      *errorOut = QStringLiteral("Authentication failed");
    }
  }

  pam_end(pamh, rc);
  dlclose(lib);
  return rc == PAM_SUCCESS;
}

} // namespace spike
