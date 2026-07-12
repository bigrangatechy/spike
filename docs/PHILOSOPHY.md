# Spike Philosophy

## Why Spike Exists

Every year, millions of laptops are thrown away. Not because they're broken. Not because they can't do what their owners need. They're thrown away because the software running on them has become too heavy, too slow, and too demanding for the hardware it was designed for.

A Celeron laptop with 4GB of RAM bought in 2019 is perfectly capable of browsing the web, writing documents, watching videos, and handling email. But the operating system it shipped with has grown heavier with every update until the machine feels unusable. The owner assumes the laptop is dead. It goes in a drawer. Eventually it goes to landfill.

That laptop isn't dead. It's being suffocated.

Spike exists to unsuffocate it.

## The Mission

**Let's Make Tech Repairable Again.**

Spike is a Linux distribution built to rescue and revive older laptops that have been abandoned not because they're broken, but because the software outgrew them. It is designed to run on hardware that other operating systems and distributions have abandoned — Celeron processors, 4GB of RAM, aging storage — and make it feel usable again.

Not tolerable. Not "good enough." Usable. Responsive. Dignified.

## Who Spike Is For

Spike is for the person who found a laptop in a closet and wants it to work. They don't know what Linux is. They don't know what a terminal is. They don't know what a partition is. They know their old laptop is slow and they heard that something called "Linux" might fix it.

Spike is not for power users. It's not for developers. It's not for people who want to configure every aspect of their system. It's for people who want their computer to work and then want to get on with their lives.

**If someone needs to open a terminal to change a setting, that's a design failure.**


## Golden Rules

These ten rules govern every design decision in Spike. They are non-negotiable and apply to every component: the installer, the desktop shell, settings, recovery tools, and all documentation.

> **Note:** These rules may expand as the project evolves. Any addition must be logged in `DECISIONS.md` and require explicit BDFL approval.

| **\#** | **Golden Rule** | **Purpose** | **Spec Reference** |
| :-: | :-: | :-: | :-: |
| 1 | Users never edit config files | Config files hidden behind Settings GUI | `PHILOSOPHY.md`, `CONFIGURATION.md` |
| 2 | CLI tools exist for developers only | CLI reserved for dev tooling only | `PHILOSOPHY.md` |
| 3 | If it's not documented, it doesn't exist | Documentation-first methodology | `PHILOSOPHY.md`, `AGENTS.md` |
| 4 | The user never has to touch the terminal | No terminal for system tasks | `PHILOSOPHY.md` |
| 5 | If a GUI doesn't exist either make one or it doesn't exist | GUI completeness guarantee | `PHILOSOPHY.md` |
| 6 | Privacy is the default, not a toggle | Privacy-by-design (no privacy mode needed) | `PRIVACY.md` |
| 7 | Your personal files are always recoverable | Recovery guarantee (4-layer model) | `DISASTER-RECOVERY.md` |
| 8 | The laptop belongs to the user. The data belongs to the user. | User sovereignty (no cloud lock-in) | `PRIVACY.md` |
| 9 | Build software that outlasts the creator | Long-term maintainability | `PHILOSOPHY.md`, `GOVERNANCE.md` |
| 10 | Lightweight doesn't mean stripped down | Performance + polish balance | `PHILOSOPHY.md` |

### Detailed Explanation

#### Rule 1: Users Never Edit Config Files

Every user-facing setting has a graphical interface in Settings. If a setting exists and a user might want to change it, there is a GUI for it. Config files still exist on disk, but they are managed by the system. A user should never need to open a text editor to change how their system behaves.

If a setting has no GUI, it is not user-facing. That is a deliberate design choice, not an oversight.

Examples of this rule in action:

| **Setting** | **Has GUI?** | **Where** |
| :-: | :-: | :-: |
| Wi-Fi password | Yes | Settings → Network or tray applet |
| Display brightness | Yes | Settings → Display or brightness applet |
| Swap size | No | Install-time decision based on hardware |
| Module blacklisting | Read-only | Settings → Advanced → Kernel Modules |
| Swappiness | Yes (SSD) | Settings → Memory (increase only) |
| GRUB timeout | Yes | Settings → Advanced → Boot |
| CPU governor | Yes | Settings → Power |

#### Rule 2: CLI Tools Exist for Developers Only

Command-line interfaces are provided for developers, contributors, and power users who opt in. However, normal system administration, configuration, and troubleshooting must never require CLI usage.

The CLI exists as a developer tool, not a user tool.

Examples of this rule in action:

| **Tool** | **Intended User** | **Purpose** |
| :-: | :-: | :-: |
| `spike-config` | Developers | Configuration engine (CLI for automation/dev tooling) |
| `apt` | Developers/system | Package management backend (users use Discover) |
| `journalctl` | Developers | Log inspection (users use Diagnostics GUI) |
| `networkmanager` CLI | Developers | Network backend (users use Settings/Network applet) |

#### Rule 3: If It's Not Documented, It Doesn't Exist

Documentation-first methodology: Discuss until convergent → write docs → then code. No undocumented features. Every architectural decision is recorded with its rationale so future maintainers understand not just what was built, but why.

If it's not in a spec document, it's not part of Spike — even if code exists for it.

Examples of this rule in action:

| **Scenario** | **Status** |
| :-: | :-: |
| Feature implemented, spec written | ✅ Part of Spike |
| Feature implemented, no spec | ❌ Remove or document immediately |
| Feature in discussion, spec drafting | 📝 In progress (not yet active) |
| Feature in spec, not implemented | 🔲 Planned (acceptable) |

#### Rule 4: The User Never Has to Touch the Terminal

No normal system task — installing software, changing settings, troubleshooting, connecting to Wi-Fi, updating, recovering files, or managing the system — should ever require opening a terminal and typing a command.

If the only way to accomplish a task is through the terminal, that is a bug, not a feature. It should be reported and fixed with a GUI.

Examples of this rule in action:

| **Task** | **Terminal Required?** | **GUI Alternative** |
| :-: | :-: | :-: |
| Install software | No | Discover |
| Change network settings | No | Settings → Network / tray applet |
| Update system | No | Discover update page |
| Check disk usage | No | Settings → Advanced → Storage |
| Recover files | No | Spike Rescue tool |
| Manage boot options | No | Settings → Advanced → Boot |
| Change language | No | Settings → Language |
| Configure Bluetooth | No | Settings → Bluetooth / tray applet |
| Check system info | No | Settings → About |
| View logs | No | Settings → Advanced → Diagnostics |

**Exceptions:**

- **Developers** — Those contributing to Spike development naturally need the terminal for building, debugging, and testing 

- **Power users who opt in** — The terminal is available for users who want to explore advanced features on their own 

- **Educational contexts** — If a user learns terminal commands for themselves, they're free to use them — but the system should still work without them 

#### Rule 5: If a GUI Doesn't Exist Either Make One or It Doesn't Exist

Every built-in Spike system tool and every system setting must have a corresponding GUI in the Settings application or an appropriate applet. Developer tools and user-installed applications are excluded from this requirement.

Scope:

- **Required:** Spike-provided system tools and settings (installed with the OS) 

- **Not required:** Developer tools (spike-config CLI, debugging utilities, dev-guide tooling) 

- **Not required:** User-installed applications (Flatpak apps manage their own UI) 

This rule ensures a cohesive, complete desktop experience — no terminal required for any system task.

Examples of this rule in action:

| **Feature** | **Has GUI?** | **Status** |
| :-: | :-: | :-: |
| Memory/ZRAM configuration | Yes | Settings → Memory |
| Power profiles | Yes | Settings → Power / tray applet |
| Firewall rules | Yes | Settings → Network → Firewall |
| App permissions (portals) | Yes | Settings → Privacy |
| Notification history | Yes | Settings → Privacy → Notifications |
| Network diagnostics | Yes | Settings → Diagnostics → Network |
| Kernel module management | Yes (read-only) | Settings → Advanced → Kernel Modules |
| Developer CLI (spike-config) | No | Excluded (developer tool) |
| User-installed GIMP | No (its own UI) | Excluded (user app) |

#### Rule 6: Privacy Is the Default, Not a Toggle

Privacy is not a mode you enable. It is the baseline. There is no "privacy mode" because privacy is always on. Zero telemetry, zero analytics, zero crash reports, zero "anonymous" statistics, zero opt-out mechanisms.

This is a non-negotiable constraint. If a future contributor proposes adding telemetry, the answer is no.

Examples of this rule in action:

| **Feature** | **Privacy Behavior** |
| :-: | :-: |
| Telemetry | None exists |
| Analytics | None exists |
| Crash reports | None exists |
| Usage stats | None exists |
| Cloud sync | None exists |
| Camera/mic indicators | Always visible, cannot be disabled |
| Data collection | Zero — absolute and non-negotiable |

#### Rule 7: Your Personal Files Are Always Recoverable

The recovery promise that shapes the entire 4-layer recovery model. From boot failure counter to Spike Rescue to reinstall + restore — the commitment is that personal files are always recoverable, even if the system is completely broken.

This is why there's no LUKS encryption (encrypted disks are unrecoverable without the password), why backups are simple and automated, and why Spike Rescue reads the broken system as read-only.

Examples of this rule in action:

| **Scenario** | **Recovery Path** |
| :-: | :-: |
| 3 consecutive boot failures | Layer 1: Boot failure counter → GRUB shows automatically |
| Config change broke boot | Layer 2: Recovery mode → `spike-config --rollback` |
| System won't boot at all | Layer 3: Live ISO Rescue → copy data to USB |
| Dead hard drive | Layer 3: Live ISO → recover to new drive |
| Forgotten password | Layer 2: Recovery mode → root shell → passwd |
| Failed upgrade | Layer 4: Reinstall + restore from SpikeBackup |

#### Rule 8: The Laptop Belongs to the User. The Data Belongs to the User.

No cloud lock-in. No forced accounts. No "we decided what's best for your files." No data leaving the device. No cloud sync, cloud backup, cloud authentication, or cloud settings sync.

The laptop is a standalone device. The user owns it. The data on it stays on it.

Examples of this rule in action:

| **Feature** | **User Ownership** |
| :-: | :-: |
| Accounts | Local only (no cloud accounts) |
| Settings sync | None (per-device only) |
| Backup | Manual/local (Spike Rescue to USB) |
| Authentication | Local password only |
| "Sign in with..." | Does not exist |
| Cloud clients | User can install Flatpaks (their choice) |

#### Rule 9: Build Software That Outlasts the Creator

Documentation comes first, before any code. Every architectural decision is recorded with its rationale. Governance model includes a succession plan. License (GPLv2+) ensures the code remains free forever.

This project is not built for the next two years. It's built for the next twenty.

Examples of this rule in action:

| **Aspect** | **Longevity Measure** |
| :-: | :-: |
| Documentation | 70 documents before alpha release |
| Governance | BDFL succession plan documented |
| License | GPLv2+ (copyleft, perpetual freedom) |
| Artwork license | CC-BY-SA 4.0 (free forever) |
| Living docs | Specs evolve with implementation |
| Dependency map | INDEX.md tracks all cross-references |

#### Rule 10: Lightweight Doesn't Mean Stripped Down

Spike is designed to run efficiently on old hardware while still feeling like a complete, polished, "real" operating system. Not a stripped-down afterthought. Not a "tolerable" experience. A dignified one.

The goal is resource efficiency, not feature reduction. Animations can be disabled (Standard variant) while keeping full functionality. Background services can be trimmed while keeping full capabilities.

Examples of this rule in action:

| **Feature** | **Standard (Efficient)** | **Plus (Full)** |
| :-: | :-: | :-: |
| Animations | Off (saves CPU) | On (visual polish) |
| Compositor effects | Minimal | Full (blur/transparency) |
| Idle RAM target | \<400MB | \<800MB |
| Memory budget | ~280–370MB | ~420–650MB |
| Functionality | Identical between variants | Identical between variants |
| Desktop shell | Same | Same |
| Settings app | Same | Same |


## Core Values

### Respect The Hardware

Spike respects the hardware it runs on. It doesn't demand more than the machine can give. It adapts to what's available — detecting CPU capability, storage type, and memory capacity at install time and configuring itself accordingly. If the CPU is too weak for ZRAM compression, Spike doesn't use ZRAM. If the storage is a spinning hard drive, Spike adjusts swap behavior and warns the user about upgrading. The system conforms to the hardware, not the other way around.

*(Supported by Golden Rule 9 — builds software that adapts and endures)*

### Simplicity Is A Feature

Every decision Spike makes should reduce complexity for the user, not add it. One download, not two. One installer that handles everything, not a partitioning screen that demands technical knowledge. One settings application, not a scattered collection of config files. One software center, not a choice between five package managers.

When faced with the choice between "powerful but complex" and "simple but sufficient," Spike chooses simple. Every time.

*(Supported by Golden Rules 1, 4, and 5 — GUI completeness, no terminal required)*

### Nothing Should Require A Terminal (Reinforcement Of Golden Rule 4)

This is Golden Rule 4, restated as a value: if a user needs to open a terminal to configure something, that's a design failure. The terminal exists for those who want it. It is never required for normal system use, configuration, or troubleshooting.

### Every Notification Matters

When a user hears a notification sound, they should be able to find what it was. Always. Without exception. A missed notification is a trust failure. If the user can't find it in the history, they stop trusting the system. Spike's notification daemon writes every notification to disk before displaying it. History survives crashes. Nothing is ever lost.

*(Supported by Golden Rule 7 — data persistence/reliability)*

### Old Hardware Deserves Dignity

A cheap Celeron laptop from 2019 cost someone their hard-earned money. It served them for years. It doesn't deserve to be thrown in a landfill because software companies decided it wasn't profitable to support anymore. Spike treats old hardware with respect — not as a burden to be tolerated, but as a resource to be utilized.

*(Core purpose of Spike — tied to Golden Rule 10)*


## What Spike Is Not

- Spike is not a power-user distribution 

- Spike is not a minimal tiling window manager setup 

- Spike is not a "learn Linux" educational tool 

- Spike is not a server operating system 

- Spike is not a developer workstation 

- Spike is not a clone of any existing OS brand 

- Spike is not a distribution where config files are exposed to users (Golden Rule 1) 

- Spike is not a distribution that requires terminal usage for system tasks (Golden Rule 4) 

- Spike is not a distribution with telemetry or cloud lock-in (Golden Rules 6 and 8) 

Spike is a complete, coherent operating system that feels designed — not assembled. The difference matters.


## What Spike Is

- Spike is a complete, beginner-friendly operating system 

- Spike is designed for old hardware that other systems have abandoned 

- Spike is a cohesive desktop experience that feels designed, not assembled 

- Spike is a system that adapts to the hardware it runs on 

- Spike is an OS where every setting has a graphical interface (Golden Rule 5) 

- Spike is an OS where no normal task requires the terminal (Golden Rule 4) 

- Spike is a distribution that makes software installation simple through Discover and Flatpak 

- Spike is a project that treats old hardware with respect and dignity 

- Spike is a community where beginners are welcomed, not judged 

- Spike is built to outlast its creator through documentation and governance (Golden Rule 9) 

- Spike is an environmental project that keeps laptops out of landfills 

- Spike is a rescue tool for computers that were thrown away too soon 

- Spike is a privacy-first system where the user owns their data (Golden Rules 6 and 8) 

- Spike is a lightweight system that doesn't sacrifice completeness (Golden Rule 10) 

- Spike is a documentation-first project where nothing ships without specs (Golden Rule 3) 


## The Name

Spike is named after a real dog. A loyal, scrappy companion who doesn't quit. The logo is his likeness, surrounded by circuitry, because the mission is about bringing technology back to life. The dog represents loyalty, endurance, and care. The circuitry represents the technology being saved.

Together, they represent the belief that technology — like a good dog — deserves to stick around.


## The Variant Philosophy

Spike comes in two variants:

- **Spike Standard** — For laptops with 4GB RAM, Celeron-class CPUs, and older hardware 

- **Spike Plus** — For computers with 8GB+ RAM and more capable processors 

Both variants share the same philosophy, the same codebase, the same shell, and the same user experience. Spike Plus is not "advanced" or "for power users." It is the same beginner-friendly system with relaxed resource constraints. Animations are on. More background services run. More Flatpak runtimes are pre-seeded. But the user experience is identical.

A user of Spike Standard and a user of Spike Plus should feel like they're using the same operating system. Because they are.

*(Supports Golden Rule 10 — lightweight doesn't mean stripped down)*


## Built To Outlast

Spike is designed to outlast its creator. The documentation comes first, before any code. Every architectural decision is recorded with its rationale so that future maintainers understand not just what was built, but why. The governance model includes a succession plan. The license (GPLv2+) ensures the code remains free forever.

This project is not built for the next two years. It's built for the next twenty.

*(Golden Rule 9 — explicitly stated)*


## Community

Spike's community is built on a simple principle: kindness is mandatory. The target users are beginners who are already intimidated by technology. A community that mocks, gates, or belittles beginners directly contradicts the mission of the project.

There is zero tolerance for bullying. One warning. One chance. Then permanent removal.

The goal is keeping computers out of landfills. Anyone who undermines that goal by driving people away from the community is working against the mission.


## Environmental Impact

Every laptop that Spike rescues is one less piece of electronic waste. Every year of extended life for a Celeron laptop is a year of avoided manufacturing, shipping, and disposal. Spike is an environmental project as much as a software project.

The carbon footprint of keeping an old laptop running is dramatically lower than manufacturing a new one. Spike makes that choice accessible to people who don't have the technical knowledge to install Linux on their own.

**Let's Make Tech Repairable Again.**

🐕 BigRangaTech

