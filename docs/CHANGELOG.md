Spike Changelog
Purpose

This document records all notable changes to Spike across releases. It serves as the chronological record of the project's evolution — what was added, changed, fixed, removed, and why.

This changelog follows the Keep a Changelog format, adapted for Spike's needs.
Format

Each release entry includes:

    Version number (semantic versioning)
    Release date (YYYY-MM-DD)
    Variant applicability (Standard, Plus, or both)
    Categories: Added, Changed, Fixed, Removed, Security, Deprecated, Known Issues

Entry format:

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

Versioning Scheme

Spike uses semantic versioning with a twist — pre-release suffixes indicate the development phase:

0.X.0-alpha     → Alpha releases (incomplete, testing)
0.X.0-beta      → Beta releases (feature-complete, polishing)
1.0.0           → First production release
1.X.0           → Point releases (features + fixes)
1.X.Y           → Patch releases (bug fixes only)
2.0.0           → Major version (breaking changes, see MIGRATION-GUIDE.md)

Breaking Changes

Breaking changes are marked with ⚠️ and require an entry in MIGRATION-GUIDE.md. Breaking changes are only permitted in major version bumps (per the stability constraints in AGENTS.md Section 7).

A "breaking change" is defined as:

    Removal of a user-visible feature
    Change in default behavior without explicit opt-in
    ABI breakage affecting third-party applications
    Removal or rename of a configuration key or file path
    Change to kernel boot parameters that alters system behavior

What Goes In The Changelog

Included:

    User-visible changes (features, settings, UI)
    System behavior changes (boot process, memory management, security)
    Dependency changes that affect users (Flatpak runtime updates, kernel upgrades)
    Documentation changes that alter specifications
    Installer changes (new steps, modified behavior)
    Hardware support additions or removals
    Branding changes (themes, logos, default wallpaper)
    Performance improvements or regressions (with before/after metrics)
    Security patches and policy changes

Not included:

    Internal refactoring with no user-visible effect
    Code style changes
    CI/CD pipeline adjustments
    Translation updates (tracked separately)
    Individual documentation typo fixes

Security Entries

Security fixes follow responsible disclosure. If a vulnerability is fixed before public disclosure, the changelog entry is added at the next release after the disclosure window closes. If already public, the entry is immediate.

Security entry format:

### Security
- Fixed CVE-XXXX-XXXXX: [brief description, no exploit details]
  Impact: [who is affected, what could happen]
  Severity: Critical / High / Medium / Low
  Action required: [what users should do, if anything]

Release History
Unreleased

Changes staged for the next release but not yet shipped.

Nothing yet. Spike is in pre-alpha documentation phase. No code has been written. All 70+ documentation files are being authored before implementation begins (see PHILOSOPHY.md — "Built To Outlast").

Once development begins, in-progress changes will be tracked here before being moved to a versioned release entry.

### Unreleased

### Added
- (nothing yet)

### Changed
- (nothing yet)

### Fixed
- (nothing yet)

### Removed
- (nothing yet)

### Security
- (nothing yet)

### Deprecated
- (nothing yet)

### Known Issues
- (nothing yet)

Planned Version Milestones

These are target milestones, not released versions. Actual content will be determined during development.

## [0.1.0-alpha] — Target: TBD

First alpha release.

Expected scope:
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
├── No telemetry (verified)

Not expected in alpha:
├── All 14 tray applets (core subset only)
├── Translations (English only)
├── Plymouth smooth handoff (may flicker)
├── Full accessibility support
├── Data backup/restore (stretch goal for late alpha)
├── Spike Rescue tool (stretch goal for late alpha)
└── Polish, animations, theme completeness

Hardware validation:
├── Tier 1 (Celeron N4020): Must pass
└── Tier 2 (AMD A4): Should boot

──────────────────────────────────────────────────

## [0.2.0-alpha] — Target: TBD

Expanded hardware testing, more applets, refinement.

Expected additions:
├── All 14 tray applets functional
├── Plymouth smooth handoff
├── Data backup/restore in installer
├── Spike Rescue tool
├── Broad hardware testing (hardware registry opens)
├── Performance baselines populated with real measurements
└── Initial translations (community-driven)

──────────────────────────────────────────────────

## [0.3.0-beta] — Target: TBD

Public beta. Broader testing, polish, translations.

Expected additions:
├── Full settings panel (all custom pages)
├── Accessibility features
├── All translations (target: 11 languages)
├── Performance optimized to meet all baselines
├── Code review enforced (branch protection)
├── Documentation finalized (user guide + dev guide)
└── Branding complete (themes, wallpapers, icons)

──────────────────────────────────────────────────

## [1.0.0] — Target: TBD

First production release.

Criteria:
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

──────────────────────────────────────────────────

## [1.X.0] — Future point releases

Bug fixes, minor features, hardware support improvements.
Point releases maintain backward compatibility within major version.

## [2.0.0] — Future major version

Breaking changes (if any). Requires migration guide.
No current plans — will be evaluated based on project needs.

Changelog Maintenance
When To Update

Update the "Unreleased" section when:
├── A feature is merged (add to "Added")
├── A bug is fixed (add to "Fixed")
├── Default behavior changes (add to "Changed", mark ⚠️ if breaking)
├── A component is removed (add to "Removed", include migration path)
├── A security issue is resolved (add to "Security")
├── Documentation is significantly updated (note in "Changed")
└── A known issue is discovered (add to "Known Issues")

At Release Time

Release process:
1. Review "Unreleased" section — ensure all entries are accurate
2. Create version header: ## [X.Y.Z] — YYYY-MM-DD
3. Move "Unreleased" entries under the new version header
4. Reset "Unreleased" section to empty
5. Update version in AGENTS.md (if applicable)
6. Tag git commit: vX.Y.Z
7. Publish release on GitLab
8. Mirror to GitHub
9. Update website download page
10. Announce on discussion forum / Matrix

Contribution

External contributors should include a changelog entry in their merge request. The entry should be added under the "Unreleased" section in the appropriate category.

Merge request changelog entry example:

In the MR description, include:

## Changelog Entry
### Added
- Battery health monitoring in Settings → Power

If a merge request does not include a changelog entry, reviewers should request one before approving. The only exception is for changes that are purely internal (refactoring, CI/CD, code style).
Archive

Old changelog entries are never deleted. Once a version is released, its changelog entry becomes a permanent historical record. This ensures that any user running an older version can find out what changed in each subsequent release.

For very old releases (2+ major versions behind), entries may be moved to docs/changelog-archive/ to keep the main file readable. A pointer will remain in the main changelog:

## [1.x.x] — YYYY-MM-DD
(Archived to changelog-archive/1.x.md)

What This Document Does Not Cover

    Release process details (signing, CI/CD, publishing): See dev-guide/17-release-process.md (to be written)
    Migration steps for breaking changes: See MIGRATION-GUIDE.md (to be written)
    Roadmap and timeline: See ROADMAP.md (to be written)
    Governance and decision-making: See GOVERNANCE.md (to be written)
    End-of-life policy for old releases: See END-OF-LIFE-POLICY.md (to be written)
    Contribution guidelines: See CONTRIBUTING.md (to be written)

🐕 BigRangaTech
