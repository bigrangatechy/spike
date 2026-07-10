Spike End-of-Life Policy
Purpose

This document defines how long each Spike release is supported, what happens when support ends, and how users are guided through transitions. It exists so that users on old hardware — the exact people least equipped to deal with sudden obsolescence — are never surprised by a cutoff.

End of life is not abandonment. It is a managed, communicated, and documented process that gives users time, tools, and a clear path forward.
Policy Principles

1. Old hardware is the project's reason for existing
   ├── EOL must not punish users on the oldest hardware
   ├── The last thing Spike should do is make old laptops obsolete
   └── Support timelines are generous, not minimal

2. No silent obsolescence
   ├── Users are told well in advance when their release is ending
   ├── Multiple notifications across multiple channels
   └── No user should discover EOL by finding their updates stopped

3. Documentation outlasts releases
   ├── Documentation for EOL releases remains available
   ├── Old docs are archived, not deleted
   └── Anyone running an old release can still find their answers

4. Security is the last thing to go
   ├── Security updates continue after feature updates stop
   ├── No release loses security support without a replacement available
   └── The transition window ensures no gap in security coverage

Release Lifecycle Stages

Every Spike release progresses through four lifecycle stages:

Stage 1: ACTIVE SUPPORT
├── Full feature updates, bug fixes, security patches
├── New minor features may be backported (point releases)
├── All documentation maintained and updated
└── Status: Current release

Stage 2: MAINTENANCE
├── No new features
├── Bug fixes continue (regressions, important non-regression bugs)
├── Security patches continue (full)
├── Documentation: maintained (errors fixed, no new content)
└── Status: Previous release (superseded by newer release)

Stage 3: SECURITY-ONLY
├── No new features
├── No bug fixes (unless critical for security)
├── Security patches only
├── Documentation: archived (read-only, no updates)
├── User is notified to upgrade
└── Status: Approaching end of life

Stage 4: END OF LIFE (EOL)
├── No updates of any kind
├── No security patches
├── No support
├── Documentation: archived (still available, clearly marked EOL)
├── User must upgrade or migrate
└── Status: Unsupported

Lifecycle Diagram

Release 1.0.0
    │
    │  ACTIVE SUPPORT
    │  (full updates, features, fixes, security)
    │
    │  Duration: Until next point/major release
    │
    ▼
Release 1.1.0 (or 2.0.0)
    │
    │  1.0.0 enters MAINTENANCE
    │  1.1.0 is ACTIVE SUPPORT
    │
    │  Duration: 12 months (1.0.0 maintenance period)
    │
    ▼
    │
    │  1.0.0 enters SECURITY-ONLY
    │
    │  Duration: 6 months (1.0.0 security-only period)
    │
    ▼
    │
    │  1.0.0 reaches END OF LIFE
    │  No further updates
    │  Users must be on 1.1.0+ (or replacement release)
    │
    ▼

Total support for 1.0.0: Active + 12 months maintenance + 6 months security-only
                        = Approximately 18+ months total (depending on release cadence)

Support Timelines
Standard Support Timeline

Phase              Duration          What's included
──────────────────────────────────────────────────────────────────
Active Support     Until superseded   Full updates: features, fixes, security
Maintenance        12 months          Bug fixes, security patches, no new features
Security-Only      6 months           Security patches only
Total (minimum)    18+ months        From release date to EOL

Minimum guarantees:
├── Every release gets at least 18 months of total support
├── Every release gets at least 6 months of security-only support
├── No release loses security support without a successor available
└── If a successor is delayed: maintenance/security-only period extends

LTS Releases

Major releases that align with Ubuntu LTS bases receive extended support:

LTS-aligned major release (e.g., 1.0.0 on Ubuntu 26.04 LTS):
├── Active Support: Until next major release
├── Maintenance: 24 months (extended from 12)
├── Security-Only: 12 months (extended from 6)
├── Total: 36+ months
└── Aligns with Ubuntu LTS 5-year support for base packages

Non-LTS-aligned releases (e.g., 2.0.0 on Ubuntu 26.10 non-LTS):
├── Active Support: Until next release
├── Maintenance: 12 months (standard)
├── Security-Only: 6 months (standard)
└── Total: 18+ months (standard)

Ubuntu LTS Alignment

Spike is based on Ubuntu Server LTS. The support timeline is tied to Ubuntu's LTS release cadence:

Ubuntu LTS Release Cadence:
├── 24.04 LTS (April 2024) — supported until April 2029
├── 26.04 LTS (April 2026) — supported until April 2031 (target)
├── 28.04 LTS (April 2028) — supported until April 2033 (future)
└── 2-year cycle between LTS releases

Spike Release Alignment:
├── Spike 1.0.0 targets Ubuntu 26.04 LTS (if available by alpha start)
│   ├── Fallback: Ubuntu 24.04 LTS (if 26.04 is delayed)
│   └── See ROADMAP.md for version targeting
├── Spike major version bumps may align with Ubuntu LTS bumps
├── Between LTS releases: point releases ride the same LTS base
└── Spike's support window is a subset of Ubuntu's LTS window
    (Spike cannot provide support past Ubuntu's EOL for base packages)

What Happens When Ubuntu LTS Ends

Ubuntu LTS EOL scenario:
├── Ubuntu 26.04 LTS reaches EOL (April 2031)
├── Spike releases based on 26.04 can no longer receive base package updates
├── Spike must have a release based on 28.04 LTS available BEFORE 26.04 EOL
├── Transition plan:
│   ├── Spike release on 28.04 LTS: at least 12 months before 26.04 EOL
│   ├── Users on 26.04-based Spike: notified to upgrade
│   ├── Migration guide published (MIGRATION-GUIDE.md)
│   └── 26.04-based Spike enters accelerated EOL (6 months after 28.04 release)
└── No user is left without a supported path

Notification Process
EOL Notification Timeline

Users are notified at multiple stages before their release reaches EOL:

Notification 1: MAINTENANCE BEGINNING
├── When: Release enters maintenance (superseded by newer release)
├── Channel: Settings → Software Updates → notification banner
├── Message: "Spike [version] is now in maintenance. A newer version
│   (Spike [newer version]) is available. Consider upgrading."
├── Severity: Informational (yellow indicator)
├── Recurring: Shown once, then dismissible
└── No action required yet

Notification 2: SECURITY-ONLY BEGINNING
├── When: Release enters security-only phase (6 months before EOL)
├── Channel: Settings → Software Updates (persistent banner), forum, website
├── Message: "Spike [version] will reach end of life on [date].
│   Security updates will continue until [date], but no bug fixes
│   are available. Please upgrade to Spike [latest version]."
├── Severity: Warning (orange indicator)
├── Recurring: Shown every 2 weeks, not dismissible until upgrade
├── Website: Banner on spike.bigrangatech.com
└── Forum: Pinned announcement

Notification 3: FINAL SECURITY PERIOD
├── When: 3 months before EOL
├── Channel: All channels (Settings, website, forum, Matrix)
├── Message: "Spike [version] reaches end of life in 3 months.
│   After [date], no updates will be available. Upgrade now
│   to Spike [latest version]."
├── Severity: Urgent (red indicator)
├── Recurring: Shown on every boot (Settings notification), weekly forum reminder
└── Website: Prominent banner with upgrade link

Notification 4: FINAL WARNING
├── When: 1 month before EOL
├── Channel: All channels + email (if registered on forum)
├── Message: "FINAL WARNING: Spike [version] reaches end of life on
│   [date]. After this date, your system will not receive security
│   updates. Your computer may become vulnerable. Please upgrade
│   immediately."
├── Severity: Critical (red, persistent, non-dismissible)
├── Recurring: Every boot, every login
└── Forum: Daily reminder thread (locked, informational)

Notification 5: EOL
├── When: On the EOL date
├── Channel: Settings → Software Updates
├── Message: "Spike [version] has reached end of life. No further
│   updates are available. Your system is no longer receiving
│   security patches. Please upgrade to Spike [latest version]."
├── Severity: Critical (permanent banner, non-dismissible)
├── Actions:
│   ├── spike-update.timer stops running
│   ├── apt sources for Spike-specific packages commented out
│   ├── System still functions (no forced upgrade)
│   └── Security status in Settings → Diagnostics shows "Unsupported"
└── Website: Listed in EOL archive section

Notification Design (Settings Panel)

Settings → Software Updates (for a release in security-only phase):

┌──────────────────────────────────────────────────────────────┐
│  Software Updates                                            │
│                                                              │
│  ⚠ IMPORTANT: Spike 1.0.0 reaches end of life on 2028-06-01 │
│  Security updates will continue until that date, but no      │
│  bug fixes are available. Please upgrade to Spike 1.2.0.    │
│                                                              │
│  [Upgrade Guide]  [Dismiss (reminds in 2 weeks)]             │
│                                                              │
│  ──────────────────────────────────────────────────────────  │
│                                                              │
│  SECURITY UPDATES                                            │
│  Last checked: 2 hours ago                                   │
│  Status: ● Up to date                                        │
│  [Check now]                                                 │
│                                                              │
│  SYSTEM INFORMATION                                          │
│  Spike version: 1.0.0                                        │
│  Support status: Security-only (ends 2028-06-01)            │
│  Latest available: 1.2.0                                    │
│                                                              │
│  [How to upgrade]                                            │
│                                                              │
└──────────────────────────────────────────────────────────────┘

Upgrade Paths
In-Place Upgrade (Point Releases)

Point releases (1.0.0 → 1.1.0) and patch releases (1.0.0 → 1.0.1) are delivered as normal updates:

In-place upgrade (point/patch):
├── Delivered via spike-update (security) or Discover (user-initiated)
├── No reinstallation required
├── User data preserved
├── Settings preserved
├── Typically automatic (part of normal update flow)
├── Reboot may be required (if kernel updated)
└── If upgrade fails: previous state is intact (apt is transactional)

Example flow:
├── User on Spike 1.0.0
├── Spike 1.1.0 released
├── 1.1.0 appears in Software Updates as "upgrade available"
├── User clicks "Install upgrade"
├── apt performs full upgrade (packages, config, kernel)
├── Reboot if needed
├── System is now on 1.1.0
└── No data loss, no reinstallation

Major Version Upgrade (Reinstallation)

Major version releases (1.x → 2.0) involve breaking changes and require a fresh installation:

Major version upgrade (reinstallation):
├── User data must be backed up first
├── Spike Installer's data backup feature handles this:
│   ├── Boot from new version's USB
│   ├── Installer scans for existing Spike installation
│   ├── Detects user data (/home/[user])
│   ├── Copies to USB with SHA256 verification
│   ├── Installs new version
│   ├── Restores user data
│   └── Verifies restoration
├── System settings: NOT migrated (breaking changes)
│   ├── New version applies fresh spike-config defaults
│   ├── User reconfigures settings (appearance, power, network)
│   ├── Network passwords: stored separately (keyfile), may survive
│   └── Browser data: preserved (Flatpak data in /home survives)
├── Migration guide published (MIGRATION-GUIDE.md)
└── Total time: ~30-60 minutes (backup + install + restore)

Upgrade Helper

To assist users through the upgrade process, Spike provides an upgrade helper:

Settings → Software Updates → "Upgrade Spike":

┌──────────────────────────────────────────────────────────────┐
│  Upgrade Spike                                               │
│                                                              │
│  Current version: Spike 1.0.0 (Standard)                    │
│  Available: Spike 1.1.0                                     │
│  Upgrade type: In-place (no reinstallation needed)           │
│                                                              │
│  This upgrade will:                                          │
│  ✓ Update all system packages                                │
│  ✓ Update the kernel (reboot required)                       │
│  ✓ Preserve your files and settings                         │
│  ✓ Preserve your browser data (Firefox, history, bookmarks) │
│                                                              │
│  Estimated download: 450 MB                                  │
│  Estimated time: 15-25 minutes                               │
│  Reboot required: Yes                                        │
│                                                              │
│  Before upgrading:                                           │
│  □ Save your work in all open applications                  │
│  □ Ensure your laptop is plugged in (or battery > 50%)      │
│  □ Ensure you have a stable internet connection              │
│                                                              │
│  [Begin upgrade]  [Not now]  [Read upgrade notes]           │
│                                                              │
└──────────────────────────────────────────────────────────────┘

For major version upgrades (reinstallation required):

┌──────────────────────────────────────────────────────────────┐
│  Upgrade Spike                                               │
│                                                              │
│  Current version: Spike 1.3.0 (Standard)                    │
│  Available: Spike 2.0.0                                     │
│  Upgrade type: Reinstallation (data backup included)         │
│                                                              │
│  This upgrade requires reinstallation because                │
│  Spike 2.0.0 includes significant changes.                 │
│                                                              │
│  Your files will be preserved:                               │
│  ✓ Documents, Pictures, Music, Videos, Downloads            │
│  ✓ Firefox data (history, bookmarks, passwords)             │
│  ✓ Notification history                                      │
│                                                              │
│  Your settings will be reset:                                │
│  ✗ Wallpaper, panel position, appearance                     │
│  ✗ Power settings, display settings                          │
│  ✗ Application permissions (will need to re-grant)           │
│  ✗ Network passwords (may need to re-enter)                 │
│                                                              │
│  Requirements:                                               │
│  □ 8GB+ USB drive (for backup + installer)                  │
│  □ Stable internet connection (for post-install updates)    │
│  □ 30-60 minutes                                             │
│                                                              │
│  [Download new ISO]  [Read migration guide]  [Not now]      │
│                                                              │
└──────────────────────────────────────────────────────────────┘

What If Hardware Becomes Unsupported

Scenario: User on Spike 1.0.0, hardware still supported
├── Normal upgrade path applies
└── User upgrades to latest Spike version

Scenario: User on Spike 1.0.0, latest Spike raises hardware requirements
├── Spike's hardware requirements are NOT raised casually
├── If a future version requires more RAM or newer CPU:
│   ├── The previous version's support window is EXTENDED
│   ├── The user is notified that their hardware cannot run the new version
│   ├── The user is advised to remain on the current (supported) version
│   └── Security updates continue for the old version until its extended EOL
├── Spike never bricks or abandons hardware that was previously supported
└── See "Hardware Support Guarantee" below

Scenario: User on extremely old hardware (below Tier 2)
├── Spike may have never supported this hardware
├── No upgrade path exists
├── User is advised to try a lighter distribution
├── Documentation suggests alternatives (without endorsing specific ones)
└── This is not EOL — the hardware was never in Spike's scope

Hardware Support Guarantee

Spike makes a specific commitment regarding hardware support:

The Hardware Support Guarantee:

Any hardware that was officially supported by a Spike release
remains supported for the lifetime of that release's support window.

If a future Spike release raises hardware requirements:
├── The previous release's support window is extended to cover
│   the gap
├── Users on the old release continue receiving security updates
├── Users are NOT forced to upgrade to a version their hardware
│   can't run
└── The new release clearly states the new minimum requirements

This guarantee is a structural constraint (see GOVERNANCE.md).
It cannot be overridden by the BDFL without the full structural
constraint amendment process.

What this means in practice:
├── If Spike 1.0 supports Celeron N4020 with 4GB RAM
├── And Spike 2.0 requires 6GB RAM (hypothetically)
├── Then Spike 1.0's security support continues until:
│   ├── All users on 1.0 with 4GB RAM have either:
│   │   ├── Upgraded their RAM (and can move to 2.0), OR
│   │   ├── Chosen to stay on 1.0 (supported but EOL eventually), OR
│   │   └── Migrated to a different OS
│   └── A reasonable transition period (12+ months) has passed
└── Nobody is abandoned

Extended Support For Old Releases
Security Backporting

When a security vulnerability is discovered:
├── Fixed in the current (active) release first
├── Then backported to all supported releases (maintenance + security-only)
├── Backport priority:
│   ├── Critical/High: Backported immediately to all supported releases
│   ├── Medium: Backported within 1 week to all supported releases
│   └── Low: Backported in next batch to all supported releases
├── If the fix cannot be backported (architectural change required):
│   ├── The vulnerability is disclosed with mitigation advice
│   ├── Users are advised to upgrade to a newer release
│   └── This is documented in the security advisory
└── Security advisories list all affected releases (by version number)

Documentation Archival

When a release reaches EOL:
├── Documentation for that release is archived:
│   ├── Moved to docs/archive/[version]/
│   ├── Clearly marked as "Archived — Spike [version] (EOL [date])"
│   ├── Read-only (no edits, no PRs accepted)
│   └── Still publicly accessible on the website
├── Current documentation:
│   ├── Always reflects the latest supported release
│   ├── May include notes for differences in older releases
│   └── Version-specific changes noted in MIGRATION-GUIDE.md
└── TROUBLESHOOTING.md:
    ├── Covers current release primarily
    ├── May include notes for older releases where behavior differs
    └── Archived versions have their own troubleshooting snapshot

ISO Availability

EOL release ISOs:
├── Kept available for download (archived)
├── Location: spike.bigrangatech.com/download/archive/
├── Clearly marked as "Spike [version] — End of Life [date]"
├── Warning displayed: "This version is no longer supported.
│   No security updates are available. Install at your own risk."
├── SHA256 checksums preserved
├── GPG signature preserved
└── Use case: restoring an old system to its original state
    (e.g., for data recovery on a bricked old install)

EOL Process Checklist

The following checklist is followed for every release reaching EOL:

12 months before EOL (maintenance beginning):
□ Notify users via Settings notification
□ Publish forum announcement
□ Update website with current/previous release status
□ Ensure migration guide is available or planned
□ Update CHANGELOG.md with status change

6 months before EOL (security-only beginning):
□ Escalate notification frequency (every 2 weeks)
□ Publish detailed upgrade instructions
□ Verify upgrade path works (test on representative hardware)
□ Update MIGRATION-GUIDE.md
□ Forum: pinned announcement with upgrade link
□ Website: banner on homepage

3 months before EOL:
□ Escalate to urgent notification (every boot)
□ Daily forum reminder thread
□ Email to registered forum users
□ Verify: latest release supports all hardware from EOL release
□ Verify: ISO for latest release is available and tested
□ Prepare documentation archival

1 month before EOL:
□ Final warning notification (non-dismissible)
□ Verify migration guide is complete and accurate
□ Prepare ISO archival
□ Prepare documentation archival
□ Prepare EOL announcement

At EOL:
□ Stop spike-update.timer for EOL release (via package update)
□ Comment out Spike-specific apt sources (via package update)
□ Archive documentation to docs/archive/[version]/
□ Archive ISO to download/archive/
□ Publish EOL announcement (website, forum, CHANGELOG.md)
□ Update website to show release as EOL
□ Security status in Settings → Diagnostics shows "Unsupported"
□ Remove from "current releases" list on website
□ Update SUPPORT.md (if applicable)

Release Status Tracking
Current Release Status Table

This table is maintained on the website and in the repository:

Release        Status           Released     EOL Date     Base
──────────────────────────────────────────────────────────────────
(pre-alpha — no releases yet)

When releases exist, this table will show:
Spike 1.0.0    Active Support   2027-XX-XX   —            Ubuntu 26.04 LTS

Future example:
Spike 1.0.0    End of Life      2027-03-01   2028-09-01   Ubuntu 26.04 LTS
Spike 1.1.0    Maintenance      2027-09-01   2029-03-01   Ubuntu 26.04 LTS
Spike 1.2.0    Active Support   2028-03-01   —            Ubuntu 26.04 LTS
Spike 2.0.0    Active Support   2028-09-01   —            Ubuntu 28.04 LTS

Status Indicators

Status              Indicator    Meaning
──────────────────────────────────────────────────────────────────
Active Support      ● Green     Full updates, current release
Maintenance         ● Yellow    Bug fixes + security, previous release
Security-Only       ● Orange    Security patches only, upgrade advised
End of Life         ● Red       No updates, unsupported
Planned             ○ Blue      Announced, not yet released
Cancelled           ✕ Grey      Announced, cancelled before release

Special Circumstances
Emergency EOL (Security Catastrophe)

In the extremely unlikely event that a release has a fundamental security flaw that cannot be patched:

Emergency EOL process:
├── BDFL declares emergency EOL (with rationale in DESIGN-DECISIONS.md)
├── All users notified immediately (all channels, all methods)
├── Security advisory published with urgency
├── Replacement release prioritized (emergency release)
├── Extended support for previous release (if unaffected)
├── Migration path provided (even if rapid)
├── No user left without a supported option
└── Post-incident review documented

Ubuntu LTS Skip

If an Ubuntu LTS release is unsuitable (quality issues, delays):

Ubuntu LTS skip scenario:
├── Spike may skip an LTS and remain on the previous LTS
├── Example: Skip 26.04 (quality issues), stay on 24.04
├── Spike's support window extends to match the skipped LTS's successor
├── Users notified of the decision and rationale
├── No impact on users (they stay on their current Spike release)
├── Next Spike major version targets the next LTS (28.04)
└── Documented in DESIGN-DECISIONS.md

Project Discontinuation

If the entire Spike project is discontinued (all development stops):

Project discontinuation process:
├── Announced 6 months in advance (minimum)
├── All users notified via all channels
├── Final security update batch released
├── Source code remains available (GitLab, GitHub mirror)
├── Documentation archived and accessible
├── ISOs archived and downloadable
├── Community may fork the project (GPLv2+ permits this)
├── Trademark: transferred to community or released for community use
│   (BDFL decision, documented in GOVERNANCE.md)
├── Infrastructure (domains, servers): transitioned to community
│   or shut down with notice
└── No user is left with a suddenly-unsupported system
    (6-month notice + final security batch + fork availability)

Discontinuation is NOT the same as EOL:
├── EOL: A specific release ends, project continues
└── Discontinuation: The entire project ends

Comparison With Other Projects

Project          Typical Support Period    Security-Only Phase
──────────────────────────────────────────────────────────────────
Ubuntu LTS       5 years (standard)        5 years (included)
Ubuntu non-LTS   9 months                   None (direct to EOL)
Fedora           ~13 months                 ~4 weeks
Debian Stable    ~5 years (LTS)             Included
Linux Mint LTS   5 years                    Included
Spike (LTS-based) 36+ months               12 months
Spike (standard)  18+ months               6 months

Spike's support is shorter than Ubuntu LTS because:
├── Spike is a solo-developer project (limited resources)
├── Spike encourages staying current (upgrade path is smooth)
├── Spike's data backup/restore makes reinstallation low-risk
└── 18-36 months is generous for a community project
    while being sustainable for a solo developer

What This Document Does Not Cover

    Release process and versioning: See CHANGELOG.md and dev-guide/17-release-process.md (to be written)
    Upgrade technical details (apt mechanics, Flatpak updates): See SECURITY.md (update model)
    Migration guide for specific version transitions: See MIGRATION-GUIDE.md (to be written)
    Project governance and decision-making: See GOVERNANCE.md
    Roadmap and release timeline: See ROADMAP.md
    Security vulnerability handling: See SECURITY.md and GOVERNANCE.md
    Hardware tier definitions and requirements: See HARDWARE.md (to be written)
    Installer backup/restore process: See INSTALLER.md
    Privacy policy (data collection during EOL): See PRIVACY.md (no data collected, even at EOL)

🐕 BigRangaTech
