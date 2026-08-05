

# Spike Support

## Purpose

This document defines how users get help with Spike Linux. It covers support channels, what kind of help is available, how to ask for help effectively, how to report bugs, how to request features, and what to expect in terms of response.

Spike is a small open-source project built by a few people in their spare time. This document is honest about that. Setting realistic expectations prevents frustration on both sides — users know what to expect, and contributors aren't burned out by impossible demands.

Support is a community activity. The forum is the heart of Spike's support ecosystem. Documentation is the first line of defense. The goal is for most users to find their answer before they need to ask a question. When they do need to ask, the community and the documentation work together to resolve it quickly.

## Support Philosophy

**Principles:**

```
├── Documentation first: every common problem should be solvable
│   by reading the docs, not waiting for a human response
├── Community-powered: volunteers provide most support on the forum
│   — this is not a paid help desk
├── No question is too basic: if someone asks, they need help
│   — respond with patience, not condescension
├── Bugs and user errors are both valid: a user who can't figure
│   something out has found a UX problem, even if the code works
├── Every support interaction is an opportunity to improve docs:
│   if a question is asked twice, the docs should answer it
└── Response time is "when someone can" not "immediately"
```

**What support IS:**

```
├── Helping users solve problems they encounter
├── Explaining how features work
├── Reporting and fixing genuine bugs
├── Guiding users through troubleshooting steps
├── Improving documentation based on gaps found during support
└── Building a knowledgeable community where users help users
```

**What support is NOT:**

```
├── 24/7 live chat (there is no paid staff)
├── Phone support (there is no phone number)
├── Remote desktop assistance (no one will remote into your machine)
├── Guaranteed response within a timeframe (community volunteers)
├── Training sessions or tutorials beyond documentation
├── Support for unrelated software (Firefox, LibreOffice, etc. — see below)
├── Support for hardware problems (broken screen, dead battery, etc.)
└── Emotional support for frustration with technology (we sympathize,
    but we can only fix software)
```

## Support Channels

### Channel Overview

```
┌──────────────────────────────────────────────────────────────────────┐
│                                                                      │
│  CHANNEL                  WHO IT'S FOR            RESPONSE TIME      │
│  ──────────────────────── ─────────────────────── ────────────────── │
│  Documentation            All users               Instant (self-serve)│
│  (shipped + website)      Every user starts here  Read the docs first │
│                                                                      │
│  Forum                    All users               1-3 days typical    │
│  (forum.bigrangatech.com) Community Q&A            Could be longer     │
│                                                                      │
│  Bug Tracker              Contributors, devs      1-7 days (triage)   │
│  (GitLab Issues)          Structured bug reports   Prioritized        │
│                                                                      │
│  In-App Help              All users               Instant (self-serve)│
│  (Settings → Help)        Context-aware help      Links to docs       │
│                                                                      │
│  Community Chat           Developers, advanced    Informal, not        │
│  (Matrix — see below)     contributors            guaranteed response  │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
```

**Priority order (use in this order):**

1. Read the relevant documentation (FAQ, Troubleshooting, User Guide) 

2. Search the forum for similar questions 

3. Ask on the forum if no answer found 

4. File a bug report on GitLab if it's a genuine software defect 

5. Use in-app help for quick reference on specific features 

### Documentation (Self-Service)

Documentation is the primary support channel. Most problems are solvable without asking anyone.

**Shipped documentation (installed with Spike):**

```
├── /usr/share/spike/docs/
│   ├── FAQ.md                  → Common questions, plain language
│   ├── TROUBLESHOOTING.md      → Symptom-based problem solving
│   ├── MIGRATION-GUIDE.md      → Switching from Windows/macOS/Linux
│   ├── docs/user-guide/        → How to use every feature (17 files)
│   └── (technical docs are on the website, not shipped)
├── Accessed via:
│   ├── Settings → Help → "Open Documentation"
│   ├── Launcher → search "Help"
│   └── File manager → navigate to the docs folder
└── Format: Markdown (readable in any text editor, rendered in browser)
```

**Website documentation** (`spike.bigrangatech.com/docs`):

```
├── All shipped docs (mirrored)
├── Technical docs (architecture, development, contribution)
├── Searchable (full-text search across all docs)
├── Versioned (docs for each Spike release)
└── Updated between releases (website is more current than shipped docs)
```

**In-app contextual help:**

```
├── Settings → Help (question mark icon in each settings category)
│   ├── Opens the relevant documentation page
│   ├── No generic "help" — it's contextual to where you are
│   └── Example: in Settings → Sound, help opens the Sound docs
├── Tooltips on hover (brief explanations of UI elements)
└── Settings → About → "Report a Problem" (shortcut to bug reporting)
```

**Documentation maintenance:**

```
├── Docs are source-controlled (same GitLab repo as code)
├── Docs updates can be contributed by anyone (merge requests)
├── Docs are reviewed by maintainers (same as code)
├── Docs improvements don't require a release cycle (website updates anytime)
└── If you find a doc error or gap: report it (see "Improving Documentation")
```

### Forum (Community Support)

The forum is the primary community support channel.

**URL:** `forum.bigrangatech.com` **Software:** Discourse (modern, mobile-friendly, good search)

**Forum structure:**

```
├── Announcements
│   ├── Official announcements from BigRangaTech
│   ├── Release announcements
│   ├── Important changes (end-of-life notices, security advisories)
│   └── Read-only (only moderators/admins post)
├── Help & Support
│   ├── Post questions about using Spike
│   ├── Search before posting (your question may be answered)
│   ├── Tag your post (e.g., "wi-fi", "audio", "boot", "installer")
│   └── Mark as solved when resolved (helps future users)
├── Hardware Compatibility
│   ├── Reports of what works on specific laptop models
│   ├── "Will Spike work on my [model]?" questions
│   ├── Hardware Registry results — users submit compatibility reports
│   │   that are compiled into a public database of tested hardware
│   └── GPU/driver compatibility discussions
├── Tips & Tricks
│   ├── Share discoveries, shortcuts, workflows
│   ├── Customization within Spike's limits
│   ├── App recommendations
│   └── Not official guidance — community knowledge
├── Feature Requests
│   ├── Suggest new features
│   ├── Discuss feasibility with community
│   ├── Popular requests inform roadmap
│   └── No guarantee of implementation
├── Off-Topic
│   ├── General Linux discussion
│   ├── Other open-source projects
│   ├── Completely unrelated (keep it civil)
│   └── Politics/religion discouraged (plenty of internet for that)
└── Development
    ├── Contributor discussion
    ├── Build/compile questions
    ├── Architecture discussion
    └── Dev-guide questions
```

**Forum guidelines:**

```
├── Be respectful — to volunteers, newcomers, and each other
├── Search before posting — duplicates clutter the forum
├── Use clear titles — "Wi-Fi not working on Acer Aspire" not "HELP!!!"
├── Provide details — laptop model, Spike version, Standard/Plus variant
├── Mark solutions — when your problem is solved, mark the answer
├── Don't cross-post — post in one category only
├── Don't bump — if no one responds in 3 days, add more detail, don't spam
├── English is primary — but other languages welcome if community supports
├── No spam, advertising, or self-promotion (except open-source projects)
└── Moderators are volunteers — respect their decisions
```

**Forum accounts:**

```
├── Account required to post (read access without account)
├── Register with email — no social login required
├── Accounts are free — no payment or subscription
├── No data collection beyond forum activity (see PRIVACY.md)
└── Account deletion: contact moderators (GDPR compliant)
```

### Bug Tracker (Structured Reporting)

The bug tracker is for software defects, not general questions.

**URL:** `git.bigrangatech.com` (GitLab CE Issues) **GitHub mirror:** `github.com` (read-only issues, synced from GitLab)

**Bug tracker is for:**

```
├── Software crashes (application exits unexpectedly)
├── Features that don't work as documented
├── Incorrect behavior (setting doesn't apply, button does nothing)
├── Visual defects (overlapping UI, missing icons, wrong colors)
├── Performance regressions (slower than baseline without explanation)
├── Security vulnerabilities (see SECURITY.md for disclosure process)
└── Documentation errors (wrong instructions, missing steps)
```

**Bug tracker is NOT for:**

```
├── General questions ("How do I...") → use the forum
├── Feature requests → use the forum (Feature Requests category)
├── Hardware compatibility questions → use the forum
├── Application-specific bugs (Firefox, LibreOffice) → report to upstream
├── Cosmetic preferences ("I wish the panel were taller") → forum
├── Non-Spike software issues → report to respective projects
└── Emotional complaints → the forum exists, but bugs need facts
```

**Filing a bug report** (see "Bug Reporting Process" below for details):

```
├── Search existing issues first (avoid duplicates)
├── Use the bug report template (provided in the issue tracker)
├── Include: Spike version, variant, hardware, steps to reproduce
├── Include diagnostic report (Settings → Diagnostics → Generate Report)
├── Attach screenshots if relevant
├── Be patient — triage may take days
└── Respond to maintainer questions if asked
```

### Community Chat (Matrix)

**Matrix room:** `#spike-linux:matrix.org`

> **Status:** The Matrix room is configured and available. Activity depends on community participation — it may be quiet during early development phases.

**Purpose:**

```
├── Real-time discussion (not real-time support)
├── Developer coordination
├── Quick questions from contributors
├── Build/test coordination
└── Community bonding
```

**Important limitations:**

```
├── Chat is NOT a support channel
├── Questions in chat may go unanswered (people are AFK, different timezones)
├── No searchable history (unlike forum)
├── No obligation for anyone to respond
├── Not monitored by BigRangaTech for support purposes
└── If you need help: use the forum or documentation
```

**Who should use chat:**

```
├── Contributors discussing code
├── Advanced users with quick technical questions
├── People coordinating testing or development work
└── NOT first-time users needing help with basic issues
```

**Rules:** Same as forum guidelines (respect, patience, no spam).

## What Support Covers

### Spike-Supported Software

Spike provides support for:

**System components:**

```
├── Spike Shell (desktop, panel, launcher, notifications, settings)
├── SDDM login screen (Spike theme configuration)
├── GRUB bootloader (Spike theme and boot configuration)
├── Plymouth boot splash
├── KWin compositor (as configured by Spike)
├── spike-config (configuration system)
├── Update system (Settings → Updates)
├── ZRAM and memory management (as configured by Spike)
├── Power management (as configured by Spike)
├── Network management (NetworkManager, as presented by Spike)
├── Bluetooth (BlueZ, as presented by Spike)
├── Audio (PipeWire, as configured by Spike)
├── Flatpak management (Discover, as configured by Spike)
├── File manager (Dolphin, as configured by Spike)
├── Terminal (Konsole — functional questions, not scripting help)
├── User account management
├── Accessibility features (Orca, magnifier, on-screen keyboard, etc.)
├── Backup and recovery (Spike Rescue)
└── Installation process (Spike Installer)
```

**Support scope for these components:**

```
├── How to use them
├── Why they behave a certain way
├── Fixing bugs in them
├── Configuration through the Settings GUI (NOT manual config editing)
└── Integration between them (how they work together)
```

### Third-Party Application Support

Spike does NOT provide support for third-party applications:

**Applications installed via Discover/Flatpak:**

```
├── Firefox: support at support.mozilla.org
├── LibreOffice: support at ask.libreoffice.org
├── GIMP: support at gimp.org
├── Steam: support at help.steampowered.com
├── Discord: support at discord.com
├── Spotify: support at spotify.com
├── Zoom: support at zoom.us
├── Thunderbird: support at support.mozilla.org/thunderbird
└── Any other Flatpak application: contact the application's maintainer
```

**What Spike WILL help with regarding third-party apps:**

```
├── "I can't install an app from Discover" → Spike issue (package management)
├── "The app won't launch" → Spike may investigate (sandbox/permissions issue)
├── "The app doesn't appear in the launcher" → Spike issue (desktop integration)
├── "The app can't access my files" → Spike issue (Flatpak permissions/portals)
├── "The app looks wrong (dark theme not applied)" → Spike may investigate (Qt theme)
└── "The app has a bug in its own functionality" → NOT a Spike issue
```

**The boundary:**

```
├── Spike is responsible for: the platform, the delivery mechanism,
│   the integration, the permissions, the theming
├── Application authors are responsible for: the application's
│   functionality, bugs, features, and user interface design
└── If unsure whether an issue is Spike or the app: ask on the forum
```

### Hardware Support Scope

**Spike provides support for:**

```
├── Hardware that Spike detects and configures (drivers, firmware)
├── Hardware compatibility questions ("Will my laptop work?")
├── Driver configuration issues (GPU, Wi-Fi, audio — as managed by Spike)
├── Peripheral connectivity (USB devices, external displays, Bluetooth)
└── Hardware that worked during install but stopped after update
```

**Spike does NOT provide support for:**

```
├── Physical hardware failures (cracked screen, dead battery, broken hinge)
├── Hardware repair advice (take it to a repair shop)
├── BIOS/UEFI configuration (consult your laptop manual)
├── Hardware upgrades (RAM installation, SSD replacement — consult manufacturer docs)
├── Driver issues caused by user modification (blacklisting, manual driver installs)
└── Hardware not in the target range (ARM devices, single-core CPUs, etc.)
```

**When hardware issues overlap with software:**

```
├── Wi-Fi not detected → Spike support (driver/firmware issue)
├── Wi-Fi hardware switch off → user responsibility (physical switch)
├── Audio not working → Spike support (configuration/driver)
├── Speakers physically broken → user responsibility
├── Battery not charging → could be either; forum can help diagnose
├── Battery physically degraded → user responsibility (replace battery)
└── When unclear: start on the forum, community will help triage
```

## Bug Reporting Process

### Before Filing A Bug

**1. Update Spike**

```
├── Settings → Updates → Check for Updates
├── Install any available updates
├── Restart
├── Retest the issue
└── Your bug may already be fixed in a newer version
```

**2. Search existing reports**

```
├── Search GitLab Issues: git.bigrangatech.com
├── Search GitHub mirror: github.com (if accessible)
├── Search the forum: forum.bigrangatech.com
├── Use multiple search terms (the bug may have a different title)
└── If you find your bug: add a comment confirming it, don't duplicate
```

**3. Determine if it's actually a bug**

```
├── Is the behavior unexpected? (compared to documentation)
├── Is it a crash? (application exits on its own)
├── Is it a regression? (worked before, broken now)
├── Is it only on your hardware? (could be hardware-specific)
├── Is it reproducible? (can you make it happen again)
└── If you can't reproduce it: still report it, but note it's intermittent
```

**4. Gather information**

```
├── Spike version: Settings → About → Version
├── Spike variant: Standard or Plus (Settings → About)
├── Hardware: Settings → About → Hardware (or Diagnostics report)
├── Steps to reproduce: write them down step by step
├── Expected behavior: what SHOULD happen
├── Actual behavior: what ACTUALLY happens
├── Screenshots: if the issue is visual
├── Logs: if applicable (see "Collecting Logs" below)
└── Diagnostic report: Settings → Diagnostics → Generate Report
```

### Collecting Logs

Logs are essential for debugging. Spike makes log collection easy:

**Settings → Diagnostics → "Generate Diagnostic Report":**

```
├── Produces a single text file with:
│   ├── System information (version, variant, kernel)
│   ├── Hardware summary (CPU, RAM, GPU, storage, network)
│   ├── Spike configuration (state.json, minus sensitive data)
│   ├── Recent system logs (journalctl — last 100 boot entries)
│   ├── Spike Shell logs (last 500 lines)
│   ├── SDDM logs (last login attempts)
│   ├── NetworkManager logs (connection history)
│   ├── PipeWire/audio logs
│   ├── Flatpak application list
│   └── Kernel messages (dmesg — last 200 lines)
├── Sensitive data is automatically removed:
│   ├── Passwords: never included
│   ├── Wi-Fi passwords: never included
│   ├── Private file paths: excluded
│   ├── MAC addresses: randomized/hashed
│   ├── Serial numbers: excluded (except storage model for SMART)
│   └── Usernames: included (needed for log parsing)
├── Saved to: ~/Documents/spike-diagnostic-report.txt
├── User can review the file before sharing (it's plain text)
└── User can redact further if desired (edit the text file)
```

**Manual log collection** (for advanced users or specific issues):

```
├── System journal: journalctl -b (current boot) or journalctl -b -1 (previous)
├── Kernel messages: dmesg
├── Spike Shell: journalctl --user -u spike-shell
├── SDDM: journalctl -u sddm
├── NetworkManager: journalctl -u NetworkManager
├── PipeWire: journalctl --user -u pipewire
├── Bluetooth: journalctl -u bluetooth
├── Boot: journalctl -b (includes full boot sequence)
├── Plymouth: journalctl -b | grep plymouth
└── Flatpak: flatpak list --columns=application,version,branch
```

**Privacy note:**

```
├── Diagnostic reports contain NO personal data
├── Reports contain hardware specs, logs, and config — nothing else
├── Users should review before attaching to a public bug report
├── If a log contains a username or path the user doesn't want public:
│   ├── They can edit the report file manually
│   └── Redact with [REDACTED] or similar
└── Reports should NEVER contain passwords or encryption keys
    (the generator is designed to exclude these, but review is wise)
```

### Filing The Bug Report

Bug report template (available in GitLab Issue tracker):

```
─── BUG REPORT TEMPLATE ───────────────────────────────────────
**Spike version:** [e.g., 0.3.1-alpha]
**Variant:** [Standard / Plus]
**Hardware:** [e.g., Acer Aspire 1, Celeron N4020, 4GB RAM]
**Description:**
Briefly describe the problem in one or two sentences.
**Steps to reproduce:**
1. [First step]
2. [Second step]
3. [Third step]
**Expected behavior:**
What you thought would happen.
**Actual behavior:**
What actually happened. Include error messages if any.
**Screenshots:**
Attach screenshots if the issue is visual. (Drag and drop images.)
**Diagnostic report:**
Attach the diagnostic report file. See Settings → Diagnostics.
**How often does this happen?**
[Always / Sometimes / Once]
**Did this work before?**
[Yes — in version X / No — never worked / Not sure]
**Additional context:**
Anything else that might help (recent changes, related issues, etc.)
─── END TEMPLATE ──────────────────────────────────────────────
```

**Tips for effective bug reports:**

```
├── One bug per report — don't group multiple issues
├── Be specific — "Settings crashes" is useless; "Settings crashes
│   when clicking Sound → Output → Device dropdown" is actionable
├── Include EVERY step — maintainers need to reproduce it
├── Don't assume the maintainer knows your setup
├── Attach the diagnostic report — it saves 5 back-and-forth messages
├── If you found a workaround: include it
├── If the bug is a security vulnerability: do NOT post it publicly
│   └── See SECURITY.md for responsible disclosure process
└── Screenshots: PNG preferred, include the full window or screen
```

**Bug lifecycle:**

```
├── Submitted: issue created with template
├── Triaged: maintainer reviews, labels added
│   ├── Labels: bug, enhancement, question, duplicate, wontfix
│   ├── Priority: low, medium, high, critical
│   ├── Component: shell, installer, settings, kernel, etc.
│   └── If not a bug: closed with explanation
├── Confirmed: maintainer reproduces the issue
├── Assigned: a developer takes ownership
├── In progress: fix is being developed
├── Fixed: fix committed, linked to issue
├── Verified: reporter confirms fix in next release
└── Closed: issue resolved
```

**Response expectations:**

```
├── Triage (acknowledgment): 1-7 days
├── Confirmation (reproduced): depends on complexity
├── Fix: depends on severity and developer availability
├── Critical bugs (system won't boot, data loss): prioritized
├── Minor bugs (cosmetic, edge case): may wait for next release
└── Some bugs may be marked "wontfix" with explanation
```

## Feature Requests

Feature requests are welcome but have no guarantee of implementation.

**Where to submit:**

```
├── Forum: forum.bigrangatech.com → Feature Requests category
├── NOT the bug tracker (bugs are for broken things, not missing things)
└── For substantial features: discuss on forum before filing a design proposal
```

**What makes a good feature request:**

```
├── Describe the problem, not the solution
│   ├── GOOD: "I can't easily switch between audio outputs"
│   ├── BAD: "Add a dropdown in the panel with a specific icon that..."
│   └── Let the design emerge from the problem
├── Explain who benefits (just you? many users?)
├── Explain why existing features don't solve it
├── Be open to discussion — your idea may evolve
└── Check if it violates any structural constraints:
    ├── Does it require telemetry? → No (structural constraint)
    ├── Does it require terminal usage? → No (Golden Rule)
    ├── Does it require config file editing? → No (Golden Rule)
    ├── Does it abandon old hardware? → No (structural constraint)
    └── If it violates these: it won't be implemented regardless of popularity
```

**Feature request lifecycle:**

```
├── Posted on forum
├── Community discusses (feasibility, demand, alternatives)
├── If demand is clear and feasible:
│   ├── Added to ROADMAP.md (future release)
│   ├── Design document may be written
│   └── Implementation scheduled (if contributor available)
├── If demand is limited:
│   ├── Acknowledged, kept in community knowledge
│   └── May be revisited if interest grows
├── If infeasible or violates constraints:
│   ├── Explanation provided
│   └── Closed with reason
└── The BDFL has final say on all feature decisions (see GOVERNANCE.md)
```

## Improving Documentation

Documentation is code — it lives in the same repository and follows the same contribution process.

**How to improve documentation:**

```
├── Found an error? → Report it (bug tracker, label: "documentation")
├── Found a gap? → Report it (or better, write a fix)
├── Found unclear wording? → Report it with suggested alternative
├── Found a missing cross-reference? → Report it
└── Found a broken link? → Report it
```

**Contributing documentation changes:**

```
├── Fork the repository (GitLab)
├── Edit the .md file(s) directly (GitLab web editor works)
├── Submit a merge request
├── Follow the same DCO process as code contributions (see CONTRIBUTING.md)
├── Documentation MRs are typically reviewed faster than code MRs
├── No programming knowledge needed for documentation contributions
└── Style: follow existing document conventions (see house style in AGENTS.md)
```

**What makes good documentation:**

```
├── Plain language (target audience is non-technical)
├── Step-by-step instructions (numbered, sequential)
├── Screenshots or diagrams where words aren't enough
├── "What this does NOT cover" sections (manage expectations)
├── Cross-references to related documents
├── No assumptions about prior Linux knowledge
└── No jargon without explanation
```

## Community Support Guidelines

### For People Asking For Help

**Before you ask:**

```
├── Read the relevant documentation (FAQ first, then Troubleshooting)
├── Search the forum (use search, not just scrolling)
├── Try the obvious fixes (restart, check for updates)
└── Gather your information (version, hardware, what happened)
```

**When you ask:**

```
├── Use a clear title: "Wi-Fi won't connect after update to 0.4"
│   not "HELP!!!" or "It doesn't work"
├── Be specific: describe what you did and what happened
├── Include your details: Spike version, variant, laptop model
├── Attach a diagnostic report if the issue is technical
├── Be patient: responses come when volunteers have time
├── Don't cross-post: one question, one place
├── Don't private-message individuals for support: use the public forum
│   (public answers help everyone with the same problem)
└── When solved: mark the answer as solution (helps future users)
```

**After you ask:**

```
├── Check back periodically (you'll get a notification if you subscribed)
├── Respond to questions from people trying to help
├── Try the suggested solutions and report back
├── If it works: say "that worked, thank you" and mark as solved
├── If it doesn't: say "that didn't work, here's what happened instead"
└── Pay it forward: if you see a question you can answer, answer it
```

### For People Providing Help

Community support is volunteering. Here's how to do it well:

**Responding to questions:**

```
├── Welcome newcomers — they may be using Linux for the first time
├── Assume good faith — a "stupid question" means the docs failed, not the user
├── Be patient — not everyone has the same technical background
├── Link to documentation when it exists (don't reinvent the answer)
├── Write answers that help future readers, not just the original asker
├── If the problem is a bug: guide them to file a report (with details)
├── If the problem is a third-party app: redirect them to the right place
├── If you don't know: say so — don't guess and spread misinformation
└── If you're frustrated: step away — don't take it out on the user
```

**Escalation:**

```
├── If a problem seems like a genuine bug: ask them to file a report
├── If a problem is a security issue: direct them to SECURITY.md process
├── If a problem is beyond forum-level help (deep debugging):
│   ├── Suggest filing a bug report with diagnostic report
│   └── A developer will investigate through the issue tracker
├── If a user is disruptive or abusive:
│   ├── Don't engage in arguments
│   ├── Flag the post for moderator attention
│   └── Moderators will handle it
└── If a user needs something Spike can't provide:
    ├── Be honest (don't promise things that won't happen)
    ├── Suggest alternatives if they exist
    └── Thank them for trying Spike
```

**Tone:**

```
├── Friendly, not formal
├── Helpful, not condescending
├── Direct, not verbose (get to the answer)
├── Encouraging, not dismissive
└── If someone is struggling: acknowledge it, then help
```

## Code Of Conduct

All support channels operate under the Spike Code of Conduct (see `CODE_OF_CONDUCT.md` for the full text).

**Summary:**

```
├── Be respectful to everyone — regardless of skill level, background,
│   language, gender, age, or experience
├── Harassment, discrimination, and personal attacks are not tolerated
├── Technical disagreements are fine — personal attacks are not
├── Newcomers are valued — condescension toward beginners is unacceptable
├── English may not be everyone's first language — be patient with language
├── "Just Google it" is not a support response
├── Moderators have the authority to remove posts, lock threads, and
│   ban users who violate the code of conduct
├── Bans are progressive (warning → temporary → permanent)
└── Appeals: contact moderators via private message on the forum
```

**Enforcement:**

```
├── Forum moderators: appointed by BigRangaTech
├── Moderators are volunteers (not paid staff)
├── Decisions are made in good faith
├── If you disagree with a moderation decision: appeal privately
├── Public arguments about moderation will result in further action
└── BigRangaTech (BDFL) is the final authority on moderation disputes
```

## Support Expectations By Severity

Honest expectations — no SLAs, this is a community project:

| **Severity** | **Forum Response** | **Bug Triage** | **Fix Timeline** | **Workaround** |
| :-: | :-: | :-: | :-: | :-: |
| **Critical** (system won't boot) | 1-2 days | 1-3 days | Days to weeks | Likely provided quickly (recovery boot). See `DISASTER-RECOVERY.md` |
| **High** (feature broken — no Wi-Fi, no audio) | 1-3 days | 3-7 days | 1-4 weeks | Attempted quickly (alternative driver, config reset) |
| **Medium** (intermittent crash, visual glitch) | 2-5 days | 1-2 weeks | 4-8 weeks or next release | May or may not exist |
| **Low** (cosmetic issue, minor annoyance) | 3-7 days | 2-4 weeks | Next release or backlog | Unlikely needed |
| **Question** ("How do I...") | 1-3 days (or instant via docs) | N/A | N/A | N/A |

> **Note:** "Fix timeline" depends entirely on volunteer developer availability. These are expectations, not guarantees. Major bugs affecting many users get faster attention. Obscure issues on rare hardware may take longer.

**If no response within 7 days:**

```
├── Reply to your own post with additional detail (don't just "bump")
├── Verify your post is in the right category
├── Check if your question was answered elsewhere
├── It's okay to ask again with more context
└── Understand: sometimes the right person hasn't seen it yet
```

## Supporting Spike (Giving Back)

If Spike helped you, here's how you can help Spike:

### Contribute Time

```
├── Answer questions on the forum (even basic ones)
├── Improve documentation (errors, gaps, clarity)
├── Test new releases and report bugs
├── Help test on different hardware (if you have spare machines)
├── Translate documentation or UI into your language
├── Write tutorials or guides for the community
├── Review merge requests (if you have expertise)
└── Moderate the forum (if invited by BigRangaTech)
```

### Contribute Code

```
├── See CONTRIBUTING.md for the contribution process
├── See docs/dev-guide/ for architecture and development docs
├── Start with "good first issue" tagged items in the bug tracker
├── DCO (Developer Certificate of Origin) required for all commits
├── No CLA (Contributor License Agreement) required
└── All code is GPLv2+
```

### Contribute Hardware

```
├── Donate or loan target hardware for testing (contact via forum)
├── Report hardware compatibility results (Hardware Registry)
├── If you have a laptop that Spike doesn't support yet:
│   ├── Report what doesn't work
│   ├── Provide diagnostic information
│   └── Developers may try to fix it
└── Hardware donations help developers test on real devices
```

### Spread The Word

```
├── Tell friends with old laptops about Spike
├── Share on social media (if you use it)
├── Write a blog post about your experience
├── Mention Spike in relevant communities (don't spam)
├── If Spike revived your old laptop: that's a story worth telling
└── Word of mouth is how small open-source projects grow
```

### Financial Support

```
├── Spike is free and always will be

├── BigRangaTech will never ask for money

│   └── Currently there is no legal mechanism to receive donations

├── If this ever changes:

│   ├── You will see it documented here (SUPPORT.md)

│   └── You will see it documented on the website and in other docs

├── No "pro" version — all features are in all versions

└── If donation links or payment tiers appear anywhere else:

    └── They are NOT authorized by BigRangaTech


## What This Document Does Not Cover

- **Code of Conduct full text:** See `CODE_OF_CONDUCT.md` 

- **Contribution process and code standards:** See `CONTRIBUTING.md` 

- **Technical architecture for developers:** See `docs/dev-guide/` 

- **User guide for specific features:** See `docs/user-guide/` 

- **Bug triage process for maintainers:** See `docs/dev-guide/` 

- **Security vulnerability reporting process:** See `SECURITY.md` 

- **Privacy policy for forum and website:** See `PRIVACY.md` 

- **Governance and project decision-making:** See `GOVERNANCE.md` 

- **End-of-life support expectations:** See `END-OF-LIFE-POLICY.md` 

- **Troubleshooting specific issues:** See `TROUBLESHOOTING.md` 

- **Frequently asked questions:** See `FAQ.md` 

- **Disaster recovery and boot failure:** See `DISASTER-RECOVERY.md` 

- **Roadmap and future development:** See `ROADMAP.md` 

- **Hardware compatibility details:** See `HARDWARE.md` 

- **Migration from other operating systems:** See `MIGRATION-GUIDE.md` 

🐕 BigRangaTech


