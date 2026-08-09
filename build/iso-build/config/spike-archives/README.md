# Tracked third-party APT sources for live-build.
#
# `build-iso.sh` copies these into `config/archives/` before `lb build`
# (live-build's empty `config/archives/` is often root/nobody-owned and
# listed under iso-build ignore rules for generated junk).
#
# mozilla.* — packages.mozilla.org (Firefox + Thunderbird real .debs).
