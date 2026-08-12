# Spike Changelog

## Purpose

This document records all notable changes to Spike across releases. It serves as the chronological record of the project's evolution — what was added, changed, fixed, removed, and why.

This changelog follows the Keep a Changelog format, adapted for Spike's needs.

## Format

Each release entry includes:

- Version number (semantic versioning) 

- Release date (YYYY-MM-DD) 

- Variant applicability (Standard, Plus, or both) 

- Categories: Added, Changed, Fixed, Removed, Security, Deprecated, Known Issues 

**Entry format:**

```
## [VERSION] — YYYY-MM-DD
### Variant: Standard / Plus / Both
### Added
- New features, capabilities, or documentation
### Changed
- Modifications to existing behavior (includes breaking changes, marked ⚠️)
### Fixed
- Bug fixes and resolved issues
### Removed
- Features or components removed (must include migration path)
### Security
- Security-related changes (patches, hardening, policy changes)
### Deprecated
- Features slated for removal in a future version
### Known Issues
- Acknowledged problems not fixed in this release
```

## Versioning Scheme

Spike uses semantic versioning with a twist — pre-release suffixes indicate the development phase:

| **Version** | **Phase** | **Description** |
| :-: | :-: | :-: |
| 0.X.0-alpha | Alpha | Incomplete, testing |
| 0.X.0-beta | Beta | Feature-complete, polishing |
| 1.0.0 | Production | First stable release |
| 1.X.0 | Point | Features + fixes |
| 1.X.Y | Patch | Bug fixes only |
| 2.0.0 | Major | Breaking changes (see MIGRATION-GUIDE.md) |

## Breaking Changes

Breaking changes are marked with ⚠️ and require an entry in MIGRATION-GUIDE.md. Breaking changes are only permitted in major version bumps (per the stability constraints in AGENTS.md Section 7).

A "breaking change" is defined as:

```
├── Removal of a user-visible feature
├── Change in default behavior without explicit opt-in
├── ABI breakage affecting third-party applications
├── Removal or rename of a configuration key or file path
└── Change to kernel boot parameters that alters system behavior
```

## What Goes In The Changelog

**Included:**

```
├── User-visible changes (features, settings, UI)
├── System behavior changes (boot process, memory management, security)
├── Dependency changes that affect users (Flatpak runtime updates, kernel upgrades)
├── Documentation changes that alter specifications
├── Installer changes (new steps, modified behavior)
├── Hardware support additions or removals
├── Branding changes (themes, logos, default wallpaper)
├── Performance improvements or regressions (with before/after metrics)
└── Security patches and policy changes
```

**Not included:**

```
├── Internal refactoring with no user-visible effect
├── Code style changes
├── CI/CD pipeline adjustments
├── Translation updates (tracked separately)
└── Individual documentation typo fixes
```

## Security Entries

Security fixes follow responsible disclosure. If a vulnerability is fixed before public disclosure, the changelog entry is added at the next release after the disclosure window closes. If already public, the entry is immediate.

**Security entry format:**

```
### Security
- Fixed CVE-XXXX-XXXXX: [brief description, no exploit details]
  Impact: [who is affected, what could happen]
  Severity: Critical / High / Medium / Low
  Action required: [what users should do, if anything]
```

## Release History

### Unreleased

**Alpha** (opened 2026-08-09): installer E2E met. See `agent-ops/STATE.md` / `DECISIONS.md`. Next: preinstalled apps (Firefox / media / email).

**Added:**

```
├── Live-build ISO path + spike-shell / spike-config / spike-rescue packages
├── Spike Rescue MVP: recover + restore; SpikeBackup/<stamp>/<label>/ layout
├── src/spike-common shared SpikeBackup helpers
├── Spike Tools desktop category (X-Spike-Tools)
├── Living docs/dev-guide/ (repo map, rescue/shell/build internals)
├── spike-installer **0.0.1**: 10-step wizard + SpikeBackup session pick (no wipe yet)
├── Settings → Power (shell **0.0.24** + config **0.0.5**): custom page + logind drop-in
├── Settings finish (shell **0.0.25**): Advanced forms, About, live Appearance, Diagnostics/VPN/Sources
├── spike-installer **0.0.2**: privileged install-helper (partition/squashfs/grub) + ERASE confirm
├── spike-installer **0.0.3**: CLI args + UEFI grub pkgs + installed session autostart
├── spike-installer **0.0.4**: grub.cfg guarantee + Wi‑Fi nmcli page
├── spike-installer **0.0.5** + rescue **0.0.10** + migration **0.0.2**: Step 7 + Layer 4 + Move My Files Mode A/B
├── spike-installer **0.0.6** + rescue **0.0.11** + config **0.0.9**: local (non-casper) installed boot; Step 7 `--exclude-disk`
├── spike-installer **0.0.7** + shell **0.0.30** + config **0.0.10**: installed DRM groups/seatd; Panel/tray live Apply echo
├── spike-installer **0.0.8** + config **0.0.11** + migration **0.0.3**: install-time blacklist; Step 7 honesty; Move My Files wizard shell
├── spike-installer **0.0.9**: full timezone (`zone.tab`) + 11 languages + XKB keyboard → `/etc/default/keyboard`
├── spike-installer **0.0.10** + rescue **0.0.12**: async Step 7 system scan; `--list-systems` skips find-files inventory
├── spike-shell **0.0.31** + installer **0.0.11**: post-install first-run wizard + `/var/lib/spike/first-boot`
├── spike-shell **0.0.32**: SpikeLockScreen (PAM), block sleep/locking inhibit, brightness via logind
├── Default apps seed: Mozilla Firefox/Thunderbird `.deb` + VLC + LibreOffice (next ISO)
├── spike-config **0.0.12** + shell **0.0.33** + installer **0.0.12**: power/input live apply; Wi‑Fi NM handoff
├── shell **0.0.34** + ISO AppArmor stubs: Firefox/Thunderbird profile-load fix (Mozilla `.deb` vs Ubuntu confinement)
├── shell **0.0.35** + installer **0.0.13**: XDG home seed, Discover Flatpak/AppStream, LibreOffice profile dirs, Spike kscreenlocker QML
├── shell **0.0.36**: panel open-app icons (KWin task list), Discover index refresh, plasma-systemmonitor
├── shell **0.0.37**: idle lock disabled; Spike LockScreen for greeter; Night Light via NightColor+preview
├── shell **0.0.38**: Discover refresh via `pkgcli` (Ubuntu resolute has no `packagekit-tools`)
├── installer **0.0.14**: Step 7 backs up wipe-disk OS; NM Wi‑Fi keyfile handoff; auto-login opt-in (default off)
├── shell **0.0.39**: Night Light Mode=Constant + preview retries; session Sleep; Users getty auto-login Apply
├── shell **0.0.40** + installer **0.0.15**: Desktop **Copy Spike Logs to USB** (`spike-save-logs`); slim live audio capture
├── shell **0.0.41** + installer **0.0.16**: night-light.log; install-from-live.log on target; drop live capture binary
├── shell **0.0.42** + installer **0.0.17**: UDisks2 USB hotplug/notify; Storage Mount/Eject; spike-greeter on boot
├── shell **0.0.43**: desktop/hardware helpers as Depends; ISO list + verify hook require them (no Recommends-only gaps)
├── installer **0.0.26** + branding **0.0.2**: Plymouth default via update-alternatives (Ubuntu)
├── shell **0.0.51**: Fn volume/brightness via evdev KEY_* (/dev/input); Meta+L/Space still KWin
├── shell **0.0.50** + branding **0.0.1** + installer **0.0.25** + config **0.0.13**: greeter stay-visible; Plymouth/GRUB logos; kglobalacceld
├── installer **0.0.24**: SpikeBackup to writable partition root (not /var/log); Layer 4 SESSION_PATH drain
├── rescue **0.0.15** + installer **0.0.23**: fix --list-systems hang (quit before exec → empty OS list)
├── rescue **0.0.14** + installer **0.0.22**: backup scan skip live USB disk; safer list-systems parse/log
├── shell **0.0.49**: Fn volume/brightness/media + Meta+L/Space (KWin spike-shortcuts → D-Bus)
├── shell **0.0.48**: greeter stays visible (After=getty; KD_GRAPHICS; no VT deallocate on getty stop)
├── shell **0.0.47** + installer **0.0.20**: Spike APT plumbing (Enabled: no); Updates Apply → unattended-upgrades
├── installer **0.0.21**: Step 7 disk scan only when backup checked; “can take a while” message
├── rescue **0.0.13** + installer **0.0.19** + shell **0.0.46**: backup timeouts; greeter waits for Plymouth (quiet boot)
├── **Alpha opened** (2026-08-09): installer E2E + blacklist smoke confirmed

├── spike-migration **0.0.1** + live Desktop: Install Spike / Rescue My Files / Move My Files
├── Full tray + Settings → PANEL (shell **0.0.27** + config **0.0.7**)
├── Desktop icons layer (shell **0.0.28**)
└── Coastal-Run default wallpaper stretch-to-fit (shell **0.0.29** + config **0.0.8**)
```

**Changed:**

```
├── Writable backups prefer LABEL=writable at partition root (not casper /var/log)
├── Product docs aligned for recover/restore everywhere + shared layout
├── Power Apply (shell **0.0.26**): do not restart systemd-logind mid-session
└── Appearance panel geometry moved to Settings → Panel (**0.0.27**)
```

**Fixed / Known:** installed black screen (missing DRM groups) fixed in **0.0.7**; post-install live initramfs in **0.0.6**; Panel/tray live Apply in shell **0.0.30**; otherwise SESSION_LOG/STATE.

#### Unreleased (structured)

**Added:** live ISO packages; Rescue recover+restore; spike-common; Spike Tools; installer wizard 0.0.1; Settings → Power; full tray applets + PANEL Settings; dev-guide core

**Changed:** SpikeBackup destination policy; docs status (prototyping in progress); Power Apply logind policy; Appearance vs Panel split

**Fixed:** spike-config **0.0.6** — dbus-python 1.4 had no `dbus.Variant` (Settings Apply crash); shell **0.0.26** — Power Apply session tear-down via logind restart; shell **0.0.34** + AppArmor stubs — Firefox/Thunderbird “profile cannot be loaded”; shell **0.0.35** — Discover AppStream/Flatpak, LibreOffice user install dirs, KWin “screen locker is broken” (minimal breeze LockScreen.qml); ISO — drop nonexistent `packagekit-tools` (use `packagekit`/`pkgcli`); installer **0.0.14** — Step 7 excluded wipe disk (reinstall backup always empty); NM `connection export` VPN-only so Wi‑Fi never copied

**Removed:** (nothing)

**Security:** (nothing)

**Deprecated:** (nothing)

**Known Issues:** installer not E2E; migration wizard not shipped; N4020 audio reference issues

### Planned Version Milestones

These are target milestones, not released versions. Actual content will be determined during development.


## [0.1.0-alpha] — Target: TBD

**First alpha release.**

**Expected scope:**

```
├── Spike Shell boots to a usable desktop (panel, launcher, clock)
├── Installer functions end-to-end (all 10 steps, all 20 installation tasks)
├── NetworkManager connects via Wi-Fi
├── PipeWire produces audio
├── GRUB2 installs and boots with hidden menu
├── Boot failure counter works
├── Settings panel opens (custom pages + KCM modules)
├── Notification daemon writes to disk before display
├── Memory management configured at install (ZRAM, swap, earlyoom)
├── Module blacklist generated at install
├── Security baseline (ufw, AppArmor, root locked)
└── No telemetry (verified)
```

**Not expected in alpha:**

```
├── All 14 tray applets (core subset only)
├── Translations (English only)
├── Plymouth smooth handoff (may flicker)
├── Full accessibility support
├── Installer Layer 4 restore (stretch until installer ships)
├── spike-migration Move My Files wizard (scaffold only in pre-alpha)
└── Polish, animations, theme completeness
```

Note: Spike Rescue recover+restore and installer Layer 4 restore ship on the live ISO in Alpha; polish and migration inventory UI continue.

**Hardware validation:**

```
├── Tier 1 (Celeron N4020): Must pass
└── Tier 2 (AMD A4): Should boot
```


## [0.2.0-alpha] — Target: TBD

**Expanded hardware testing, more applets, refinement.**

**Expected additions:**

```
├── All 14 tray applets functional
├── Plymouth smooth handoff
├── Data backup/restore in installer (Step 7 + Layer 4)
├── spike-migration Move My Files wizard
├── Broad hardware testing (hardware registry opens)
├── Performance baselines populated with real measurements
└── Initial translations (community-driven)
```


## [0.3.0-beta] — Target: TBD

**Public beta. Broader testing, polish, translations.**

**Expected additions:**

```
├── Full settings panel (all custom pages)
├── Accessibility features
├── All translations (target: 11 languages)
├── Performance optimized to meet all baselines
├── Code review enforced (branch protection)
├── Documentation finalized (user guide + dev guide)
└── Branding complete (themes, wallpapers, icons)
```


## [1.0.0] — Target: TBD

**First production release.**

**Criteria:**

```
├── All Tier 1 baselines met on physical hardware
├── Tier 2 boots and functions
├── All 69+ documentation files complete and accurate
├── All Golden Rules enforced (verified)
├── All critical constraints satisfied (verified)
├── Security audit passed
├── No known critical or high-severity bugs
├── Installer tested on diverse hardware
├── Update mechanism tested (apt + Flatpak)
├── Disaster recovery tested (all 4 layers)
└── Community guidelines enforced
```


## [1.X.0] — Future point releases

Bug fixes, minor features, hardware support improvements.

Point releases maintain backward compatibility within major version.

## [2.0.0] — Future major version

Breaking changes (if any). Requires migration guide.

No current plans — will be evaluated based on project needs.

## Changelog Maintenance

### When To Update

Update the "Unreleased" section when:

```
├── A feature is merged (add to "Added")
├── A bug is fixed (add to "Fixed")
├── Default behavior changes (add to "Changed", mark ⚠️ if breaking)
├── A component is removed (add to "Removed", include migration path)
├── A security issue is resolved (add to "Security")
├── Documentation is significantly updated (note in "Changed")
└── A known issue is discovered (add to "Known Issues")
```

### At Release Time

Release process:

1. Review "Unreleased" section — ensure all entries are accurate 

2. Create version header: `## [X.Y.Z] — YYYY-MM-DD` 

3. Move "Unreleased" entries under the new version header 

4. Reset "Unreleased" section to empty 

5. Update version in AGENTS.md (if applicable) 

6. Tag git commit: `vX.Y.Z` 

7. Publish release on GitLab 

8. Mirror to GitHub 

9. Update website download page 

10. Announce on discussion forum / Matrix 

### Contribution

External contributors should include a changelog entry in their merge request. The entry should be added under the "Unreleased" section in the appropriate category.

**Merge request changelog entry example:**

In the MR description, include:

```
## Changelog Entry
### Added
- Battery health monitoring in Settings → Power
```

If a merge request does not include a changelog entry, reviewers should request one before approving. The only exception is for changes that are purely internal (refactoring, CI/CD, code style).

## Archive

Old changelog entries are never deleted. Once a version is released, its changelog entry becomes a permanent historical record. This ensures that any user running an older version can find out what changed in each subsequent release.

For very old releases (2+ major versions behind), entries may be moved to `docs/changelog-archive/` to keep the main file readable. A pointer will remain in the main changelog:

```
## [1.x.x] — YYYY-MM-DD
(Archived to changelog-archive/1.x.md)
```

## What This Document Does Not Cover

- Release process details (signing, CI/CD, publishing): See dev-guide/17-release-process.md (to be written) 

- Migration steps for breaking changes: See MIGRATION-GUIDE.md 

- Governance and decision-making: See GOVERNANCE.md 

- End-of-life policy for old releases: See END-OF-LIFE-POLICY.md 

🐕 BigRangaTech

