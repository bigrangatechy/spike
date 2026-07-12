# Contributing to Spike Linux

## Purpose

This document explains how to contribute to Spike Linux. It covers code, documentation, translations, testing, and community contributions. It defines the process for submitting changes, the standards those changes must meet, and what happens after you submit.

Spike follows a "docs-first" methodology. Every feature, every component, every behavior is specified in documentation before code is written. This means documentation is not a second-class contribution — it is the foundation the entire project is built on. A documentation contribution that improves a specification affects every line of code that follows it.

Whether you're fixing a typo in a README or architecting a new subsystem, the process is the same. Small or large, your contribution goes through the same pipeline. The pipeline exists to protect the project's coherence — one person's quick hack can undo weeks of careful design if it bypasses review.

## Who Can Contribute

Anyone. No exceptions.

- First-time contributors: welcome — start with "good first issue" tagged items in the GitLab issue tracker 

- Non-coders: welcome — documentation, translations, testing, design feedback, community support are all contributions 

- Experienced developers: welcome — but Spike's conventions may differ from what you're used to (read `DESIGN-DECISIONS.md`) 

- People who have never contributed to open source before: welcome — this document assumes no prior experience 

- People who disagree with Spike's direction: see "When You Disagree With the Project Direction" below 

**Requirements:**

```
`├── A GitLab account (git.bigrangatech.com) — free, self-hosted`

`├── An email address (for DCO sign-off and communication)`

`├── Willingness to follow the process (this document)`

`├── Willingness to follow the Code of Conduct (see CODE\_OF\_CONDUCT.md)`

`└── That's it. No credentials, no prior contributions, no`

`    minimum skill level.`
```

## What You Can Contribute

### Documentation

Documentation is the most valuable contribution you can make to Spike right now. The project is in the documentation phase — code has not begun. Every document improvement shapes the system that will be built.

**Types of documentation contributions:**

```
`├── Fixing errors (typos, incorrect facts, broken links)`

`├── Improving clarity (rewording confusing passages, adding examples)`

`├── Filling gaps (adding sections that are referenced but missing)`

`├── Writing new documents (user-guide/ and dev-guide/ files)`

`├── Adding diagrams (ASCII diagrams in the house style — see AGENTS.md)`

`├── Translating documents into supported languages`

`├── Reviewing documentation for accuracy against other documents`

`│   (cross-reference validation)`

`└── Improving the "What This Document Does Not Cover" sections`
```

**Documentation house style (see AGENTS.md for full conventions):**

```
`├── Markdown format (no reStructuredText, no AsciiDoc)`

`├── ASCII diagrams (not images — text-based, reproducible, diffable)`

`├── Structured tree notation for hierarchical information`

`├── Cross-references to other documents (See DOCUMENTNAME.md)`

`├── "What This Document Does Not Cover" section at the end`

`├── 🐕 BigRangaTech footer at the end of every document`

`├── Plain language for user-facing docs (target audience is non-technical)`

`├── Technical precision for dev-guide docs (target audience is developers)`

`└── No assumptions about prior Linux knowledge in user-facing docs`
```

### Code

Code contributions will become central once the documentation phase completes and the alpha phase begins. Until then, code contributions are limited to:

```
`├── Tooling (build scripts, CI configuration, deployment automation)`

`├── Spike documentation site (if a website is built for docs)`

`├── Infrastructure (Dockerfiles for CI, GitLab CI pipelines)`

`└── Experimental prototypes (if explicitly sanctioned by the BDFL`

`    — do not start building Spike Shell, the installer, or other`

`    core components without coordination)`
```

When the alpha phase begins, code contributions will include:

```
`├── Spike Shell (Qt6/QML desktop components)`

`├── spike-config (configuration system)`

`├── Installer (Calamares configuration or custom)`

`├── KWin configuration and window decoration`

`├── SDDM theme`

`├── GRUB theme and configuration`

`├── Plymouth themes`

`├── Qt stylesheet (spike.qss)`

`├── Icon theme modifications`

`├── Build system (live-build or custom ISO builder)`

`├── CI/CD pipelines`

`├── Automated tests`

`└── Packaging scripts`
```

Code must match the specification in the documentation. If the specification is wrong, fix the specification first, then write code that matches the corrected specification. This is the docs-first principle: the docs are the source of truth, not the code.

### Translations

Spike supports 11 languages. Translations are needed for:

```
`├── UI strings (Spike Shell, Settings, installer, SDDM theme)`

`├── Documentation (user guide, FAQ, troubleshooting)`

`├── Website copy`

`├── Installer messages`

`└── System notification templates`
```

**Translation process (to be established when alpha begins):**

```
`├── Translation platform: to be determined (possibly Weblate, self-hosted)`

`├── Source language: English (all documentation and UI strings`

`│   written in English first)`

`├── Target languages: to be confirmed (likely aligned with the 11`

`│   supported languages)`

`├── Translation memory: maintained on the translation platform`

`├── Review: native speakers review translations`

`└── Updates: translations updated when source strings change`
```

Until the translation infrastructure is set up:

```
`├── Translation contributions are not yet actionable`

`├── If you want to help: express interest on the forum`

`│   (Development category)`

`└── When infrastructure is ready: we'll announce it`
```

### Testing

Testing is critical and will become a major contribution area once alpha ISOs are available.

**Types of testing:**

```
`├── Hardware testing: boot Spike on your laptop, report what works`

`│   and what doesn't`

`├── Regression testing: verify that something that worked in`

`│   version X still works in version Y`

`├── Edge case testing: unusual configurations, low disk space,`

`│   multiple monitors, Bluetooth combos, etc.`

`├── Accessibility testing: test with screen reader, on-screen`

`│   keyboard, high contrast, large text`

`├── Performance testing: measure boot time, memory usage, app`

`│   launch time against baselines (see PERFORMANCE-BASELINES.md)`

`├── Usability testing: watch a non-technical person use Spike`

`│   and report what confuses them`

`└── Update testing: install an update and verify nothing breaks`
```

**Testing contributions:**

```
`├── No code or documentation skill required`

`├── Need: hardware (ideally Tier 1 or Tier 2 — see HARDWARE.md)`

`├── Need: willingness to follow test procedures and report results`

`├── Report format: GitLab issue with test results template`

`├── Include: hardware specs, Spike version, variant, test results`

`└── Every hardware report helps — even "it worked fine" is valuable`
```

### Community

Community contributions are real contributions:

```
`├── Answering questions on the forum (see SUPPORT.md)`

`├── Welcoming newcomers (see CODE\_OF\_CONDUCT.md — Beginner Protection)`

`├── Triaging bug reports (if you have relevant knowledge)`

`├── Organizing community events or testing days`

`├── Writing blog posts or tutorials about Spike`

`├── Creating videos showing how to use Spike`

`├── Spreading the word (responsibly, no spam)`

`└── Being a positive presence in the community`
```

These contributions are not tracked in GitLab but are valued equally with code and documentation. A community member who helps 100 beginners on the forum has contributed as much as someone who wrote 100 lines of code.

## Before You Start

### Read The Right Documents

Before contributing, read (at minimum):

**For ALL contributors:**

```
`├── This document (CONTRIBUTING.md)`

`├── CODE\_OF\_CONDUCT.md — you agreed to it by participating`

`├── AGENTS.md — master project reference (all 70 documents, phases, conventions)`

`└── PHILOSOPHY.md — understand WHY before HOW`
```

**For documentation contributors:**

```
`├── AGENTS.md (house style, document list, phase tracking)`

`├── The document you're editing (read it fully before changing it)`

`├── Documents that cross-reference yours (ensure consistency)`

`└── DESIGN-DECISIONS.md (understand why things are the way they are)`
```

**For code contributors:**

```
`├── ARCHITECTURE.md (system overview)`

`├── DESIGN-DECISIONS.md (rationale for technical choices)`

`├── The specification for the component you're working on`

`│   (e.g., DESKTOP.md for Spike Shell, INSTALLER.md for installer)`

`├── KERNEL.md (kernel configuration)`

`├── dev-guide/ documents (when available — technical implementation details)`

`└── PERFORMANCE-BASELINES.md (performance targets your code must meet)`
```

**For testing contributors:**

```
`├── HARDWARE.md (hardware tiers and detection)`

`├── PERFORMANCE-BASELINES.md (what to measure)`

`├── TROUBLESHOOTING.md (known issues and how to report)`

`└── SUPPORT.md (how to report bugs effectively)`
```

### Understand The Structural Constraints

Spike has structural constraints that cannot be overridden by any contributor, including the BDFL acting alone. These are foundational decisions that define what Spike IS. Overriding any of them requires 5+ contributors (see GOVERNANCE.md).

The structural constraints are:

**1. Zero telemetry**

- No contribution may add data collection of any kind. No analytics, no crash reports (without explicit opt-in), no usage statistics, no hardware scanning without consent. 

**2. Golden Rule 1: No config file editing for any user setting**

- If a user needs to change something, there must be a GUI. Contributions that add features requiring manual config file editing are rejected — the feature must include a GUI path. 

**3. Golden Rule 2: No terminal required for any user task**

- If a user needs to do something, there must be a GUI method. Terminal-based tools are fine for developers and power users, but no user-facing task should require the terminal. 

**4. Old-hardware focus**

- Contributions that degrade performance on Tier 1 hardware (Celeron N4020, 4GB RAM) are rejected. Features must work on the minimum target hardware or not be included. 

**5. GPLv2+ license**

- All code must be GPLv2+ compatible. No proprietary dependencies. No GPL-incompatible licenses in the codebase. 

**6. Flatpak-only application model**

- User applications are delivered via Flatpak. No Snap, no apt-installed user applications (system packages only for the base system and core components). 

If your contribution violates any of these, it will be rejected. Not because it's bad, but because it contradicts what Spike is. If you believe a structural constraint should change, see GOVERNANCE.md for the override process (requires 5+ contributors).

### Set Up Your Environment

**GitLab account:**

```
`├── Register at git.bigrangatech.com (self-hosted GitLab CE)`

`├── Account is free`

`├── No social login required (email registration)`

`├── Enable two-factor authentication (recommended, not required)`

`└── Your GitLab username is your contributor identity`
```

**Fork the repository:**

```
`├── Navigate to the Spike Linux repository on GitLab`

`├── Click "Fork" (creates your own copy of the repository)`

`├── Clone your fork locally:`

`│   ├── git clone https://git.bigrangatech.com/\<your-username\>/spike-linux.git`

`│   └── cd spike-linux`

`├── Add the upstream remote:`

`│   ├── git remote add upstream https://git.bigrangatech.com/bigrangatech/spike-linux.git`

`│   └── git fetch upstream`

`└── Keep your fork in sync:`

`    ├── git checkout main`

`    ├── git fetch upstream`

`    ├── git merge upstream/main`

`    └── git push origin main`
```

**Git configuration:**

```
`├── Set your name: git config --global user.name "Your Name"`

`├── Set your email: git config --global user.email "your@email.com"`

`├── This name and email appear on every commit (public record)`

`├── Use a real name or a consistent pseudonym`

`├── Do not use someone else's name or a disposable email`

`└── These must match your GitLab account email for DCO verification`
```

**Tools:**

```
`├── Git (command line or GUI client — your choice)`

`├── A text editor (any editor that handles Markdown)`

`├── For code: Qt6 development environment (when alpha begins)`

`├── For testing: a spare laptop (Tier 1 or Tier 2 hardware)`

`└── No specific IDE required — use what you're comfortable with`
```

## DCO: Developer Certificate Of Origin

### What It Is

Spike uses the Developer Certificate of Origin (DCO) for all contributions. The DCO is a simple attestation that you wrote the contribution yourself (or have the right to submit it) and that you are licensing it under the project's license (GPLv2+).

The DCO is NOT a Contributor License Agreement (CLA).

```
`├── CLA: you grant additional rights to the project owner`

`│   (often a broad copyright assignment or license grant)`

`├── DCO: you confirm you have the right to contribute and`

`│   that the contribution is licensed under GPLv2+`

`└── Spike uses DCO, not CLA. You retain your copyright.`
```

**The DCO text (Version 1.1):**

```
`Developer Certificate of Origin`

`Version 1.1`


`Copyright (C) 2004, 2006 The Linux Foundation and its contributors.`


`Everyone is permitted to copy and distribute verbatim copies of this`

`license document, but changing it is not allowed.`


`Developer's Certificate of Origin 1.1`


`By making a contribution to this project, I certify that:`


`(a) The contribution was created in whole or in part by me and I`

`    have the right to submit it under the open source license`

`    indicated in the file; or`


`(b) The contribution is based upon previous work that, to the best`

`    of my knowledge, is covered under an appropriate open source`

`    license and I have the right under that license to submit that`

`    work with modifications, whether created in whole or in part`

`    by me, under the same open source license (unless I am`

`    permitted to submit under a different license), as indicated`

`    in the file; or`


`(c) The contribution was provided directly to me by some other`

`    person who certified (a), (b) or (c) and I have not modified it.`


`(d) I understand and agree that this project and the contribution`

`    are public and that a record of the contribution (including all`

`    personal information I submit with it, including my sign-off) is`

`    maintained indefinitely and may be redistributed consistent with`

`    this project or the open source license(s) involved.`
```

### How To Sign Off

Every commit must include a Signed-off-by line. This is the DCO attestation. Commits without it will be rejected by the CI pipeline.

**Method 1: Add -s (or --signoff) to your git commit command:**

```
`├── git commit -s -m "Fix typo in INSTALLER.md partition section"`

`├── This adds: Signed-off-by: Your Name \<your@email.com\>`

`└── The name and email come from your git config`
```

**Method 2: Add the sign-off manually to your commit message:**

```
`├── git commit -m "Fix typo in INSTALLER.md partition section`


`    Signed-off-by: Your Name \<your@email.com\>"`

`└── Ensure the name and email match your git config`
```

**Verification:**

```
`├── Check: git log -1 --format="%B"`

`├── You should see the Signed-off-by line in every commit`

`├── CI pipeline enforces DCO: commits without sign-off fail the pipeline`

`└── Merge requests with unsigned commits cannot be merged`
```

**If you forget to sign off:**

```
`├── Amend the commit: git commit --amend -s`

`├── Or: git rebase with --signoff across multiple commits`

`├── Force push to your fork: git push --force-with-lease`

`└── The MR will re-check after force push`
```

**Multiple contributors on one commit:**

```
`├── Each person who authored part of the commit signs off`

`├── Multiple Signed-off-by lines are acceptable`

`├── Use Co-authored-by: for additional authors`

`│   └── git commit -s -m "message`


`        Co-authored-by: Other Person \<other@email.com\>"`

`└── All authors must have their own Signed-off-by or Co-authored-by line`
```

## Commit Conventions

### Commit Messages

**Format:**

```
`├── First line: short summary (max 72 characters)`

`├── Blank line`

`├── Body: explanation (wrap at 72 characters per line)`

`├── Blank line`

`├── Signed-off-by line (DCO — see above)`
```

**Rules:**

```
`├── First line: imperative mood ("Fix" not "Fixed" or "Fixes")`

`│   ├── "Fix typo in INSTALLER.md partition section"`

`│   ├── "Add high-contrast cursor variant to BRANDING.md"`

`│   ├── "Document ZRAM swappiness logic in MEMORY.md"`

`│   └── NOT "Fixed the typo" or "Updated documentation"`

`├── First line: lowercase (no capital letter, no period at end)`

`│   └── Spike does not capitalize commit message first lines`

`│       (matches many large open-source projects)`

`├── First line: specific enough to understand without opening the diff`

`│   ├── GOOD: "fix typo in INSTALLER.md partition table section"`

`│   ├── BAD: "fix typo"`

`│   ├── GOOD: "add USB modem detection to HARDWARE.md"`

`│   └── BAD: "update hardware doc"`

`├── Body: explain WHAT and WHY (not HOW — the diff shows how)`

`│   ├── For documentation: what changed and why`

`│   ├── For code: what the code does and why it's needed`

`│   ├── Reference issues: "Closes \#123" or "Refs \#456"`

`│   └── If the change is non-obvious: explain the reasoning`

`├── No AI-generated commit messages without review`

`│   ├── If you use an AI tool to draft a message: review it,`

`│   │   edit it, and make sure it accurately describes YOUR change`

`│   └── Commit messages are part of the permanent project record`

`└── Sign-off is mandatory (see DCO section)`
```

**Example — documentation fix:**

```
`fix incorrect ZRAM size formula in MEMORY.md`


`The ZRAM size calculation referenced "RAM × compression\_ratio"`

`but the actual formula uses min(RAM, max\_zram) for Standard`

`variant and RAM for Plus variant. Updated both formulas and`

`added a worked example for 4GB RAM at 3.5:1 compression.`


`Refs \#42`


`Signed-off-by: Jane Smith \<jane@example.com\>`
```

**Example — code contribution (when alpha begins):**

```
`add CPU bogomips detection to spike-config`


`Implements CPU performance estimation using bogomips as`

`described in HARDWARE.md section "CPU Detection and`

`Classification". The detection runs during install and stores`

`results in state.json under hardware.cpu.bogomips\_total.`


`ZRAM capability benchmark also implemented (compression test`

`with 100ms timeout) as specified in HARDWARE.md.`


`Closes \#87`


`Signed-off-by: Jane Smith \<jane@example.com\>`
```

### Commit Granularity

**One logical change per commit:**

```
`├── GOOD: one commit fixes one typo in one file`

`├── GOOD: one commit adds one section to one document`

`├── GOOD: one commit implements one detection function`

`├── BAD: one commit fixes typos in three unrelated documents`

`├── BAD: one commit adds a feature AND fixes a bug AND reformats code`

`└── If a change spans multiple files: it's still one commit IF`

`    the changes are logically related (e.g., updating a term across`

`    all documents that reference it)`
```

**Commit size:**

```
`├── Small commits are better than large commits`

`├── A one-line fix is a valid commit`

`├── A 500-line documentation addition is a valid commit`

`├── A 2000-line code dump is NOT a valid commit — split it`

`├── If your commit touches more than 3-4 files: consider splitting`

`└── Reviewers need to understand each commit independently`
```

**Squashing:**

```
`├── If you have many small commits ("fix typo", "fix another typo",`

`│   "actually fix the typo"): squash before submitting the MR`

`├── git rebase -i upstream/main → squash related commits`

`├── The final commit history should tell a clean story`

`└── Reviewers should not have to read through "work in progress"`

`    commits to find the actual change`
```

### Branch Naming

**Branch names:**

```
`├── Use descriptive names (not "my-changes" or "patch-1")`

`├── Prefix with the type of change:`

`│   ├── docs/ for documentation changes`

`│   ├── fix/ for bug fixes`

`│   ├── feat/ for new features (when code begins)`

`│   ├── test/ for test additions or changes`

`│   ├── refactor/ for code restructuring`

`│   ├── ci/ for CI/CD changes`

`│   └── i18n/ for translation changes`

`├── Follow with a brief description:`

`│   ├── docs/fix-installer-partition-typo`

`│   ├── fix/zram-size-calculation`

`│   ├── feat/cpu-bogomips-detection`

`│   └── i18n/fr-faq-translation`

`├── Lowercase, hyphens between words, no spaces`

`├── Keep branch names under 50 characters`

`└── Delete your branch after the MR is merged (keeps the fork clean)`
```

**Working on your branch:**

```
`├── Create branch from up-to-date main:`

`│   ├── git checkout main`

`│   ├── git fetch upstream`

`│   ├── git merge upstream/main`

`│   ├── git checkout -b docs/fix-installer-partition-typo`

`│   └── Make your changes`

`├── Keep your branch rebased on latest main:`

`│   ├── git fetch upstream`

`│   ├── git rebase upstream/main`

`│   └── Resolve conflicts if any (then force push to your fork)`

`└── Don't let branches go stale — rebase regularly`
```

## Submitting A Merge Request

### Before You Submit

**Checklist before creating a merge request:**

```
`├── \[ \] Your branch is based on the latest upstream main`

`├── \[ \] All commits are signed off (DCO: -s flag)`

`├── \[ \] Commit messages follow the convention (see above)`

`├── \[ \] One logical change per commit (squash if needed)`

`├── \[ \] Your change doesn't violate structural constraints`

`│       (zero telemetry, Golden Rules, old-hardware focus, GPLv2+, Flatpak-only)`

`├── \[ \] You've tested your change:`

`│       ├── Documentation: proofread, verified cross-references, checked formatting`

`│       ├── Code: compiles, passes tests, meets performance targets`

`│       └── Tests: actually ran them, not just "should work"`

`├── \[ \] You've read the document/file you're modifying (fully)`

`├── \[ \] Your changes don't contradict another document`

`│       (if they do: you need to update BOTH documents in the same MR)`

`├── \[ \] No AI-generated content without review`

`│       (AI tools are fine for assistance, but you must read, understand,`

`│       and take responsibility for every line you submit)`

`├── \[ \] No secrets, credentials, or personal data in your commits`

`│       (check your diff before pushing)`

`└── \[ \] You've searched existing MRs to avoid duplicates`
```

### Creating The Merge Request

**1. Push your branch to your fork:**

```
`├── git push origin docs/fix-installer-partition-typo`

`└── If rebased: git push --force-with-lease origin docs/...`
```

**2. Go to GitLab:**

```
`├── Navigate to your fork on git.bigrangatech.com`

`├── GitLab will show a "Create merge request" button for your`

`  pushed branch — click it`

`├── Set the target:`

`│   ├── Source: your fork, your branch`

`│   └── Target: bigrangatech/spike-linux, main branch`

`└── Title: same format as commit first line (imperative, specific)`
```

**3. MR description template:**

```
`├── GitLab provides a template (fill it in — don't leave it blank)`

`├── Include:`

`│   ├── What changed and why`

`│   ├── What document(s) or component(s) are affected`

`│   ├── How you tested the change`

`│   ├── Any breaking implications (does this change affect other docs?)`

`│   ├── Screenshots (if visual/UI change)`

`│   └── Issue references (Closes \#123, Refs \#456)`
```

**4. MR description example:**

```
`\#\# What this changes`

`Fixes an incorrect formula in MEMORY.md's ZRAM size calculation section.`

`The Standard variant formula referenced "RAM × compression\_ratio" but`

`the actual algorithm uses min(RAM, max\_zram) where max\_zram is capped`

`at 4GB for Standard variant.`


`\#\# Why`

`The incorrect formula would lead developers implementing spike-config`

`to calculate ZRAM sizes that exceed the Standard variant cap, violating`

`the specification in VARIANT-DIFFERENCES.md.`


`\#\# Testing`

`- Proofread the updated section`

`- Verified cross-references: VARIANT-DIFFERENCES.md §ZRAM caps,`

`  CONFIGURATION.md §ZRAM configuration`

`- Checked ASCII diagram formatting in a Markdown renderer`

`- Confirmed the worked example produces correct values for 4GB RAM`

`  at 3.5:1 compression ratio`


`\#\# Related issues`

`Closes \#42`
```

**5. Labels:**

```
`├── Add appropriate labels if you have permission:`

`│   ├── documentation (for doc changes)`

`│   ├── code (for code changes, when alpha begins)`

`│   ├── i18n (for translations)`

`│   ├── testing (for test additions)`

`│   └── good first issue (if the MR resolves one)`

`└── If you don't have permission to label: a maintainer will add labels during review`
```

**6. Reviewers:**

```
`├── Don't assign specific reviewers (maintainers triage MRs)`

`├── If a specific maintainer has been discussing the change with`

`│   you: you can mention them (@username) in the description`

`└── Otherwise: let the triage process handle assignment`
```

**7. Submit:**

```
`├── Click "Create merge request"`

`├── CI pipeline runs automatically (DCO check, linting, etc.)`

`├── If CI fails: fix the issues, push again`

`└── Wait for review (see "Review Process" below)`
```

### Keeping Your MR Updated

While your MR is being reviewed:

```
`├── Maintainers may request changes`

`├── Upstream main may advance (new commits merged)`

`├── You need to keep your MR up to date:`
```

**Updating for requested changes:**

```
`├── Make the requested changes on your branch`

`├── Commit with proper sign-off: git commit -s -m "address review feedback"`

`├── Push: git push origin docs/...`

`├── Add a comment in the MR: "Updated — please re-review"`

`└── Do NOT squash while the MR is under active review`

`    (reviewers need to see what changed since their last review)`
```

**Rebasing on new upstream main:**

```
`├── git fetch upstream`

`├── git rebase upstream/main`

`├── Resolve conflicts`

`├── git push --force-with-lease origin docs/...`

`└── Leave a comment: "Rebased on latest main"`
```

**MR staleness:**

```
`├── If your MR has no activity for 30 days:`

`│   ├── A maintainer will ping you: "Is this still relevant?"`

`│   ├── If you respond and continue: MR stays open`

`│   ├── If no response within 14 days: MR is closed (stale)`

`│   └── You can reopen it later if you return`

`└── Life happens — closing stale MRs is not a punishment,`

`    it's housekeeping`
```

## Review Process

### What Reviewers Look For

**Documentation review checklist:**

```
`├── Accuracy: does the content match other documents?`

`├── Clarity: would a non-technical user understand this? (user-facing docs)`

`├── Completeness: are all referenced sections present?`

`├── Cross-references: do "See X.md" references point to real documents?`

`├── Formatting: does Markdown render correctly? Are ASCII diagrams intact?`

`├── House style: follows AGENTS.md conventions?`

`├── No contradictions: does this document contradict another?`

`├── "What This Document Does Not Cover" section present and accurate?`

`├── 🐕 BigRangaTech footer present?`

`├── No broken internal links`

`└── No copyrighted content without permission`
```

**Code review checklist (when alpha begins):**

```
`├── Matches specification: does the code implement what the docs describe?`

`├── Structural constraints: no telemetry, no config-file requirements,`

`│   no terminal-only paths, works on Tier 1 hardware, GPLv2+ compatible`

`├── Correctness: does it do what it claims?`

`├── Edge cases: what happens with empty input, null, extreme values?`

`├── Error handling: are errors caught and handled gracefully?`

`├── Performance: does it meet baselines on Tier 1 hardware?`

`├── Memory: does it stay within memory budget on 4GB RAM?`

`├── Security: no injection, no unsafe operations, no hardcoded secrets`

`├── Style: follows coding conventions (see AGENTS.md / dev-guide)`

`├── Tests: are there tests for the new code?`

`├── DCO: all commits signed off`

`└── No commented-out code or debug prints left in`
```

### Review Feedback

**How reviewers provide feedback:**

```
`├── Inline comments on specific lines (preferred)`

`├── General comments in the MR discussion`

`├── Categorized as: blocking (must fix), suggestion (should consider),`

`│   question (need clarification), nit (minor, optional)`

`├── Blocking issues MUST be resolved before merge`

`├── Suggestions are recommendations, not requirements`

`├── Nits are trivial — fix them if convenient, don't worry if not`

`└── If a reviewer is wrong: it's OK to push back (politely, with evidence)`
```

**How to respond to review feedback:**

```
`├── Read all comments before responding`

`├── Address blocking issues first`

`├── If you disagree with a blocking issue:`

`│   ├── Explain your reasoning (calmly, with evidence)`

`│   ├── Reference the specification or design decisions if relevant`

`│   ├── If the reviewer maintains their position: defer to the maintainer`

`│   └── Do NOT argue extensively — state your case once, then accept`

`│       the outcome`

`├── For suggestions:`

`│   ├── If you agree: implement and push`

`│   ├── If you disagree: briefly explain why and move on`

`│   └── Don't fight over suggestions — they're optional`

`├── For questions: answer clearly and completely`

`├── For nits: fix them if easy, ignore if not worth the effort`

`├── Mark resolved threads as "resolved" in GitLab`

`└── Push changes and comment "Updated" to trigger re-review`
```

**Timeline:**

```
`├── Initial review: within 7 days of submission (for most MRs)`

`├── Subsequent reviews (after your updates): within 5 days`

`├── Large or complex MRs: may take longer`

`├── Simple MRs (typo fixes, small doc improvements): may be reviewed`

`│   within 1-2 days`

`├── No response after 14 days: ping the MR (add a comment)`

`├── No response after 21 days: email the maintainer list`

`└── If still no response: post on the forum (Development category)`
```

### Approval And Merge

**Approval:**

```
`├── At least one maintainer must approve the MR`

`├── For documentation MRs: one maintainer approval is sufficient`

`├── For code MRs (when alpha begins):`

`│   ├── One maintainer approval required for small changes`

`│   ├── Two maintainer approvals required for significant changes`

`│   │   (new features, architectural changes, changes to specs)`

`│   └── The BDFL may override (approve or reject) any MR`

`├── CI pipeline must pass (DCO check, linting, tests if applicable)`

`├── All blocking review issues must be resolved`

`└── The branch must be up to date with main (no merge conflicts)`
```

**Merge:**

```
`├── Maintainer merges the MR (not the contributor)`

`├── Merge method: squash merge (combines all commits into one)`

`│   └── Keeps the commit history clean (one commit per MR)`

`├── The squashed commit retains the original commit message`

`│   (ensure your commit messages are clean before squash)`

`├── After merge:`

`│   ├── Your branch on your fork can be deleted`

`│   ├── The change appears in main`

`│   ├── CI runs on main to verify integration`

`│   └── You are credited as the author (GitLab tracks this)`

`└── If the merge causes a CI failure on main:`

`    ├── The maintainer who merged investigates`

`    ├── May be reverted if it breaks main`

`    └── You'll be asked to fix and resubmit`
```

## Coding Conventions

### General Principles

Spike's coding conventions will be detailed in dev-guide/ documents when they are written. Until then, the following principles apply:

**1. The documentation is the specification**

- Code must match what the docs describe 

- If the docs are wrong: fix the docs first (same MR) 

- If the docs are silent: ask before implementing 

- "The code is the spec" is NOT how Spike works 

**2. Readable over clever**

- Code is read more than it is written 

- Prefer explicit over implicit 

- Prefer boring over novel 

- Avoid clever tricks that require a comment to understand 

- Comments explain WHY, not WHAT (the code shows what) 

- A new contributor should understand your code without asking you questions 

**3. No silent failures**

- Errors must be handled (not swallowed) 

- Empty catch blocks are not acceptable 

- Logging an error and continuing is fine (if appropriate) 

- Crashing is better than silently corrupting data 

- User-facing errors must have user-friendly messages (no raw stack traces to the user — see Golden Rule 2) 

**4. Performance is a feature**

- Code must meet performance baselines on Tier 1 hardware 

- Memory usage matters (4GB RAM is the target) 

- Avoid unnecessary allocations 

- Avoid blocking operations on the main thread 

- Lazy-load: don't initialize what isn't needed yet 

- Benchmark if you're not sure (compare to baseline) 

**5. No magic**

- No magic numbers (use named constants) 

- No hidden state (state in state.json, not scattered globals) 

- No implicit behavior (side effects must be explicit) 

- No "it works because of this undocumented dependency" 

- If something is non-obvious: document it (in code AND in docs) 

### Language-Specific Conventions

**C/C++ (kernel modules, core tools):**

- Follow the Linux kernel coding style (see KERNEL.md) 

- No C++ exceptions in kernel code 

- Tabs for indentation (kernel convention) 

- snake\_case for functions and variables 

- All code GPLv2+ (header comments required) 

**Qt6/QML (Spike Shell, SDDM theme):**

- C++: follow Qt coding style (see Qt documentation) 

- QML: camelCase for properties, PascalCase for components 

- Use Qt6 APIs (not Qt5 compatibility) 

- Signal/slot connections: explicit (no string-based connect) 

- No JavaScript in QML for logic (use C++ models, QML for presentation) 

- QML files: one component per file 

- Resources: use Qt resource system (.qrc), not absolute paths 

**Python (tooling, scripts, spike-config):**

- Python 3 (no Python 2) 

- PEP 8 style (with line length 100) 

- Type hints required for public functions 

- snake\_case for functions and variables 

- Docstrings for all public functions (Google style) 

- No global mutable state (use classes or module-level constants) 

- Scripts must be executable (\#!/usr/bin/env python3) 

**Shell scripts (init, hooks, utilities):**

- POSIX sh where possible (not bash-specific unless needed) 

- set -euo pipefail (strict mode) 

- No bare cd without checking (use && || handle errors) 

- Quote all variables ("VAR) 

- Use printf, not echo (portability) 

- Comments for non-obvious logic 

- Exit codes: 0 = success, non-zero = failure (with meaning) 

### File Organization

**Repository structure (will be defined in detail in dev-guide/):**

```
`├── Documentation lives at the root level (top-level .md files)`

`├── user-guide/ — user-facing documentation`

`├── dev-guide/ — developer documentation`

`├── src/ — source code (when alpha begins)`

`│   ├── shell/ — Spike Shell (Qt6/QML)`

`│   ├── config/ — spike-config`

`│   ├── installer/ — installer configuration`

`│   ├── themes/ — QSS, GRUB, Plymouth, SDDM themes`

`│   ├── icons/ — icon theme modifications`

`│   ├── wallpapers/ — default wallpapers`

`│   ├── sounds/ — system sounds`

`│   └── branding/ — logo files and brand assets`

`├── tools/ — build scripts, CI config, utility scripts`

`├── tests/ — automated tests`

`└── packaging/ — ISO build configuration, package definitions`
```

**File naming:**

```
`├── Documentation: UPPERCASE-WITH-HYPHENS.md (e.g., BOOT-PROCESS.md)`

`├── Code files: lowercase\_with\_underscores (C/C++, Python)`

`├── QML files: PascalCase.qml (e.g., PanelButton.qml)`

`├── Shell scripts: lowercase-with-hyphens.sh`

`├── Config files: lowercase (e.g., state.json, spike.qss)`

`├── No spaces in filenames`

`├── No special characters except - and \_`

`└── Filenames should be descriptive (not abbreviations unless`

`    universally known)`
```

## Issue Tracking

### Reporting Issues

Bug reports and feature requests go in the GitLab issue tracker: `git.bigrangatech.com` (Issues)

**Before creating an issue:**

```
`├── Search existing issues (avoid duplicates)`

`├── Determine: is this a bug or a feature request?`

`│   ├── Bug: something doesn't work as documented`

`│   ├── Feature request: something is missing`

`│   └── Question: you don't understand how something works`

`├── Bugs → create issue in GitLab`

`├── Feature requests → discuss on the forum first (see SUPPORT.md)`

`├── Questions → ask on the forum (see SUPPORT.md)`

`└── Don't use issues for questions — the forum is faster`
```

**Bug report format (see SUPPORT.md for full template):**

```
`├── Spike version`

`├── Variant (Standard/Plus)`

`├── Hardware (CPU, RAM, GPU, storage)`

`├── Steps to reproduce`

`├── Expected behavior`

`├── Actual behavior`

`├── Diagnostic report (Settings → Diagnostics → Generate Report)`

`├── Screenshots (if visual)`

`└── Frequency (always / sometimes / once)`
```

**Security vulnerabilities:**

```
`├── Do NOT report security issues in the public issue tracker`

`├── See SECURITY.md for responsible disclosure process`

`├── Report privately to the security contact listed in SECURITY.md`

`└── This is the one exception to "report in the open"`
```

### Working On Issues

**Finding issues to work on:**

```
`├── "good first issue" label: beginner-friendly tasks`

`├── "help wanted" label: tasks that need a contributor`

`├── Unassigned issues: claim them by commenting "I'll work on this"`

`├── Assigned issues: don't work on these without coordinating`

`│   with the assignee`

`└── Check the issue is not already being worked on (check MRs too)`
```

**Claiming an issue:**

```
`├── Comment on the issue: "I'd like to work on this"`

`├── A maintainer will assign it to you (if appropriate)`

`├── If the issue is complex: a maintainer may want to discuss`

`│   the approach first before you start`

`├── If no one responds within 3 days: ask on the forum`

`└── Don't claim more issues than you can work on simultaneously`
```

**Closing issues:**

```
`├── Issues are closed when the MR that fixes them is merged`

`│   ├── Use "Closes \#123" in the MR description or commit message`

`│   └── GitLab auto-closes the issue when the MR is merged`

`├── If an issue is a duplicate: close with "Duplicate of \#456"`

`├── If an issue is invalid: close with explanation`

`├── If an issue is a feature request that won't be implemented:`

`│   ├── Close with "wontfix" label and explanation`

`│   └── Direct the requester to the forum for discussion`

`└── Only maintainers close issues (contributors can request closure)`
```

## Maintainers

### Maintainer Role

Maintainers are contributors with the ability to:

```
`├── Review and approve merge requests`

`├── Merge approved MRs into main`

`├── Triage and label issues`

`├── Close issues and MRs`

`├── Edit documentation for clarity (with MR, not directly to main)`

`├── Moderate the forum (if also a forum moderator)`

`├── Participate in design discussions with weight (not veto —`

`│   that's the BDFL)`

`└── Represent the project publicly`
```

Maintainers are NOT:

```
`├── Above the Code of Conduct (they're held to a higher standard)`

`├── Allowed to push directly to main (all changes via MR, including`

`│   their own)`

`├── Able to override structural constraints`

`├── Paid (this is a volunteer role)`

`├── Permanent (maintainership can be revoked by BDFL)`

`└── Dictators (maintainers propose, BDFL decides on disagreements)`
```

### Becoming A Maintainer

Maintainers are appointed by the BDFL. Criteria:

```
`├── Sustained contribution quality (not quantity)`

`├── Understanding of Spike's architecture and philosophy`

`├── Respect for the structural constraints`

`├── Constructive participation in design discussions`

`├── Helpful and respectful interaction with community members`

`│   (especially beginners)`

`├── Demonstrated review capability (providing useful feedback on`

`│   others' MRs)`

`├── Reliability (following through on commitments)`

`├── Code of Conduct compliance (no active warnings)`

`└── Alignment with Spike's mission (old laptops, non-technical users)`
```

**Process:**

```
`├── BDFL identifies potential maintainers from active contributors`

`├── Existing maintainers may recommend candidates`

`├── BDFL appoints (no election, no voting — BDFL model)`

`├── Announcement on the forum (Announcements category)`

`├── Probationary period: 3 months (can review and comment, cannot`

`│   merge to main)`

`├── After 3 months: full maintainer privileges (if no issues)`

`└── Maintainers can step down at any time (no obligation)`
```

**How NOT to become a maintainer:**

```
`├── Asking for it directly (contribute first, recognition follows)`

`├── Submitting many low-quality MRs (quantity ≠ quality)`

`├── Being technically skilled but dismissive to beginners`

`├── Arguing with existing maintainers about process`

`└── Trying to "campaign" for the role`
```

## When You Disagree With The Project Direction

You will disagree with some of Spike's decisions. Everyone does. Spike makes unusual choices: Flatpak-only, no terminal for user tasks, no config file editing, BDFL governance, strict enforcement.

**Healthy disagreement:**

```
`├── Express it on the forum (Development category or Feature Requests)`

`├── Provide reasoning and alternatives`

`├── Accept that the BDFL makes final decisions`

`├── If your argument is compelling, it may influence future decisions`

`├── Structural constraints require 5+ contributors to override (see`

`│   GOVERNANCE.md) — one person cannot change them`

`└── Forking is always an option (GPLv2+ permits it)`
```

**Unhealthy disagreement:**

```
`├── Relitigating settled decisions repeatedly (disruptive)`

`├── Attacking the decision-maker instead of the decision (CoC violation)`

`├── Trying to circumvent decisions by submitting MRs that violate`

`│   constraints (will be rejected)`

`├── Recruiting others to "vote" against a decision (Spike is not a`

`│   democracy — it's a BDFL project)`

`├── Insisting that Spike should be like other distros (it's not, by`

`│   design)`

`└── "If you don't change this, I'm leaving" (that's your choice,`

`    and it's fine, but it's not leverage)`
```

**The boundary:**

```
`├── "I disagree with this decision and here's why" → welcome`

`├── "I disagree and I'll keep bringing it up until you change" → disruptive`

`├── "I disagree so I'm going to fork" → your right, good luck`

`└── "I disagree so I'm going to undermine the project" → banned (CoC)`
```

## AI-Assisted Contributions

Spike allows the use of AI tools (LLMs, code assistants) to help with contributions, with conditions:

**Allowed:**

```
`├── Using AI to help draft documentation (then reviewing and editing)`

`├── Using AI to help write code (then reviewing and testing)`

`├── Using AI to help understand unfamiliar code or concepts`

`├── Using AI to help write commit messages (then reviewing)`

`├── Using AI to help translate documentation (then human review)`

`└── Using AI tools as a learning aid`
```

**Required:**

```
`├── You must understand every line you submit`

`│   ├── If you can't explain what a line does: don't submit it`

`│   ├── If the AI introduced something you don't understand: remove it`

`│   └── "The AI wrote it" is not an excuse for bugs or violations`

`├── You are responsible for the contribution (DCO sign-off means YOU`

`│   certify the work)`

`│   ├── You must have the right to submit it (DCO clause a, b, or c)`

`│   ├── AI-generated content that reproduces copyrighted code is`

`│   │   YOUR responsibility (check for this)`

`│   └── Plagiarism via AI is still plagiarism`

`├── You must test the contribution (AI code can be subtly wrong)`

`├── You must ensure it follows Spike's conventions (AI doesn't know`

`│   Spike's house style — you do, or you learn it)`

`└── AI-generated commit messages must be reviewed and edited to`

`    accurately describe the actual change`
```

**Not allowed:**

```
`├── Bulk-generating documentation with AI and submitting without`

`│   reading it`

`├── Submitting AI-generated code that you cannot explain`

`├── Using AI to circumvent review (submitting AI-reviewed code as`

`│   if it were human-reviewed)`

`├── Claiming AI-generated work as "entirely my own" (if AI wrote`

`│   significant portions, that's fine — just don't lie about it)`

`└── Letting AI introduce copyrighted material from other projects`

`    (check AI output for copied code — this is YOUR responsibility)`
```

**Why these rules exist:**

```
`├── AI can produce convincing-looking but subtly incorrect content`

`├── AI doesn't know Spike's constraints (it might suggest telemetry,`

`│   config file editing, or terminal-only solutions)`

`├── The DCO requires YOU to certify the work — you can't delegate`

`│   that to an AI`

`└── Spike's quality standard is "a human understands and stands`

`    behind this work" — AI is a tool, not a contributor`
```

## Attribution

Contributors are attributed in multiple ways:

```
`├── Git history (author name and email on commits)`

`├── GitLab MR history (linked to your profile)`

`├── Release notes (significant contributions mentioned by name`

`│   in CHANGELOG.md, if you consent)`

`├── Contributors list (if maintained — to be decided)`

`└── No "contributor license agreement" — you keep your copyright`
```

**Release notes attribution:**

```
`├── For significant contributions (new features, major docs, critical`

`│   fixes): contributor name appears in CHANGELOG.md`

`├── Consent required: if you don't want your name in release notes,`

`│   tell a maintainer (use your pseudonym or "Anonymous")`

`├── Minor contributions (typo fixes, small tweaks): not individually`

`│   attributed (too many to list)`

`└── The Git history always shows who did what (this cannot be`

`    removed — it's the project's permanent record)`
```

## Quick Reference

```
`CONTRIBUTION QUICK REFERENCE`


`1. Read: CONTRIBUTING.md, CODE\_OF\_CONDUCT.md, AGENTS.md`

`2. Fork the repo on GitLab`

`3. Create a branch: docs/fix-..., fix/..., feat/...`

`4. Make your changes`

`5. Commit with -s (DCO sign-off)`

`   git commit -s -m "imperative description of change"`

`6. Push to your fork`

`7. Create merge request on GitLab`

`8. Fill in the MR description template`

`9. Respond to review feedback`

`10. Maintainer merges when approved`


`REMEMBER:`

`├── Documentation is the most valuable contribution now`

`├── Code follows documentation (docs-first)`

`├── DCO sign-off on EVERY commit (no exceptions)`

`├── No CLA — you keep your copyright`

`├── Structural constraints are non-negotiable`

`└── Be excellent to each other (see CODE\_OF\_CONDUCT.md)`
```

## What This Document Does Not Cover

- Code of Conduct (behavioral standards): See `CODE\_OF\_CONDUCT.md` 

- Governance and decision-making (BDFL, structural constraints): See `GOVERNANCE.md` 

- Master project reference (document list, phases, house style): See `AGENTS.md` 

- Architecture and system design: See `ARCHITECTURE.md` 

- Design rationale (why decisions were made): See `DESIGN-DECISIONS.md` 

- Coding conventions (detailed, per-language): See `dev-guide/` (to be written) 

- Build system and tooling: See `dev-guide/` (to be written) 

- Testing framework and procedures: See `dev-guide/` (to be written) 

- Bug reporting (user-facing): See `SUPPORT.md` 

- Security vulnerability reporting: See `SECURITY.md` 

- Privacy policy (data collection): See `PRIVACY.md` 

- License details: See `LICENSE` (to be written) 

- Roadmap and project phases: See `ROADMAP.md` 

- Project philosophy and values: See `PHILOSOPHY.md` 

🐕 BigRangaTech

