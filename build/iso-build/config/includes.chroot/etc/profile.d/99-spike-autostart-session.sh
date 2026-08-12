# Spike live: start the graphical session after tty1 autologin.
# Guards keep SSH / other VTs / nested shells usable for debug.
# Sourced from /etc/profile (login shells only).

case "$-" in
  *i*) ;;
  *) return 0 ;;
esac

grep -Eqw 'boot=casper' /proc/cmdline 2>/dev/null || return 0
[ "$(id -u)" -ne 0 ] || return 0
[ -z "${WAYLAND_DISPLAY:-}" ] || return 0
[ -z "${DISPLAY:-}" ] || return 0
[ -n "${XDG_SESSION_ID:-}" ] || return 0
[ -z "${SPIKE_SESSION_STARTED:-}" ] || return 0

_tty="$(tty 2>/dev/null || true)"
case "$_tty" in
  /dev/tty1) ;;
  *) return 0 ;;
esac

command -v spike-session >/dev/null 2>&1 || return 0

# Never nest: Desktop Terminal= launches / login shells must not start a second
# KWin on top of an already-running session for this user.
if command -v pgrep >/dev/null 2>&1; then
  pgrep -u "$(id -u)" -x kwin_wayland >/dev/null 2>&1 && return 0
fi
if [ -n "${XDG_RUNTIME_DIR:-}" ]; then
  for _sock in "${XDG_RUNTIME_DIR}/wayland"-*; do
    [ -S "$_sock" ] && return 0
  done
fi

export SPIKE_SESSION_STARTED=1
echo "spike: starting desktop session (Ctrl+Alt+F2 for a text console)…"
exec spike-session
