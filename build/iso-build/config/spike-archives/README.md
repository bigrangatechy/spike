# Tracked third-party APT sources for live-build / installed image.
#
# `build-iso.sh` copies Mozilla into `config/archives/` before `lb build`
# (live-build's empty `config/archives/` is often root/nobody-owned and
# listed under iso-build ignore rules for generated junk).
#
# mozilla.* — packages.mozilla.org (Firefox + Thunderbird real .debs).
#
# spike.* — Spike component packages (shell/config/rescue/installer/migration).
#   - spike.key          public signing key (commit)
#   - spike.list         template URL (do NOT stage into archives until host is up)
#   - spike.pref         pin spike-* when the archive is enabled
#   Installed path: includes.chroot/etc/apt/sources.list.d/spike.sources
#   (Enabled: no until https://packages.bigrangatech.com/spike is online)
#
# Publish: ./scripts/generate-spike-apt-key.sh && ./scripts/publish-spike-apt-repo.sh
# See docs/UPDATES.md
