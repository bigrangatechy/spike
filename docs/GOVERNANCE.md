# Spike Governance

## Purpose

This document defines how decisions are made in the Spike project, who has authority to make them, how the community participates, and what happens if the project leadership changes. It exists to ensure the project can outlast any single contributor — including its founder.

Governance is not bureaucracy. It is the set of agreements that allow a solo-developer project to become a community project without losing coherence. Every rule in this document exists because something would break without it.

## Governance Model

### Benevolent Dictator For Life (BDFL)

Spike operates under a BDFL governance model. The project founder (BigRangaTech) holds final decision-making authority over all aspects of the project.

**BDFL authority covers:**

```
├── Feature acceptance or rejection
├── Architectural decisions
├── Release timing and version numbering
├── Code merges (final approval)
├── Documentation standards
├── Trademark and branding usage
├── Community moderation decisions
├── Contributor access levels
├── Project direction and roadmap
├── License interpretation
└── Dispute resolution
```

The word "benevolent" is intentional. The BDFL's role is to serve the project's mission — making old laptops useful — not to serve personal preference. Decisions are made with the mission, the Golden Rules, and the critical constraints as the guiding framework.

### Why BDFL?

BDFL model is appropriate when:

```
├── The project has a clear, specific mission (not general-purpose)
├── Consistency of vision is critical to the product
├── The project is early-stage with few contributors
├── Design decisions have cascading consequences
└── A single coherent voice is needed for direction
```

Alternative models considered (see **DESIGN-DECISIONS.md**):

```
├── Meritocratic council: Too much overhead for current stage
├── Pure democracy: Leads to design-by-committee, violates vision coherence
├── No governance: Leads to forks, inconsistency, and stagnation
└── BDFL chosen: Maximum coherence, minimum overhead, clear accountability
```

### BDFL Responsibilities

The BDFL is responsible for:

```
├── Maintaining the project's mission and vision
├── Enforcing the Golden Rules and critical constraints
├── Making final decisions on disputed proposals
├── Ensuring documentation stays complete and accurate
├── Reviewing and merging code (or delegating to trusted reviewers)
├── Setting release criteria and timing
├── Managing project infrastructure (GitLab, website, domains)
├── Protecting the project's privacy and security posture
├── Responding to security vulnerabilities
├── Moderating community spaces (or appointing moderators)
├── Approving or rejecting trademark usage requests
└── Planning for succession (see below)
```

### BDFL Limitations

The BDFL is not unlimited. The following are binding constraints on the BDFL's authority:

**The BDFL cannot:**

```
├── Change the license from GPLv2+ to a non-free license
├── Remove the privacy guarantee (zero telemetry is non-negotiable)
├── Violate the Golden Rules (no terminal required, no config file editing)
├── Change the target hardware focus away from old/low-spec machines
├── Add telemetry, analytics, or data collection of any kind
├── Close the source code
├── Require a CLA (contributor license agreement) — only DCO is used
├── Remove the documentation-first methodology requirement
├── Sell exclusive rights or proprietary control to any party
└── Override the critical constraints in AGENTS.md Section 7 without
    documenting the rationale in DESIGN-DECISIONS.md
```

These constraints are structural — they define what Spike IS. Changing them creates a different project, not a new version of Spike.

## Decision-Making Process

### How Decisions Are Made

**Decision types and process:**

**Type 1: Routine (documentation edits, bug fixes, minor features)**

```
├── Contributor proposes change (issue or merge request)
├── BDFL or delegated reviewer approves
├── Decision time: Days
└── No formal process needed
```

**Type 2: Significant (new feature, behavior change, new dependency)**

```
├── Contributor opens issue with proposal
├── Discussion period: minimum 7 days
├── Proposal evaluated against:
│   ├── Does it comply with Golden Rules?
│   ├── Does it comply with critical constraints?
│   ├── Does it work on Tier 1 hardware?
│   ├── Does it increase idle memory? If so, is it justified?
│   ├── Does it require terminal or config file editing? If so, rejected.
│   ├── Does it add telemetry or data collection? If so, rejected.
│   └── Does it serve a beginner? If not, it doesn't belong.
├── BDFL makes decision (approve, reject, request changes)
├── Decision documented (issue comment, DESIGN-DECISIONS.md if architectural)
└── Decision time: 1-4 weeks
```

**Type 3: Architectural (changes to core systems, breaking changes)**

```
├── Proposal must be documented before discussion
├── Discussion period: minimum 14 days
├── Requires DESIGN-DECISIONS.md entry with full rationale
├── BDFL decision required (cannot be delegated)
├── If breaking change: MIGRATION-GUIDE.md entry required
└── Decision time: 2-8 weeks
```

**Type 4: Foundational (license change, governance change, mission change)**

```
├── Requires community consultation (forum, issue, blog post)
├── Discussion period: minimum 30 days
├── BDFL decision, but community feedback is weighed heavily
├── Requires update to this document (GOVERNANCE.md)
└── Decision time: 1-3 months
```

### Proposal Evaluation Criteria

Every proposal — feature, change, or addition — is evaluated against this checklist:

```
┌──────────────────────────────────────────────────────────────┐
│  Proposal Evaluation Checklist                               │
│                                                              │
│  [ ] Complies with Golden Rule 1 (no config file editing)    │
│  [ ] Complies with Golden Rule 2 (no terminal required)      │
│  [ ] Works on Tier 1 hardware (Celeron N4020, 4GB RAM)      │
│  [ ] Does not increase idle memory above target              │
│      (Standard: <400MB / Plus: <800MB)                      │
│  [ ] Does not add telemetry or data collection               │
│  [ ] Does not require a network service (SSH, web server)   │
│  [ ] Does not require Snap or snapd                          │
│  [ ] Serves a beginner, not just a power user               │
│  [ ] Has documentation (or documentation plan)               │
│  [ ] Does not conflict with existing architecture            │
│  [ ] Does not introduce a new runtime dependency             │
│      (GTK, Electron, Python app framework, etc.)            │
│  [ ] Is testable on target hardware                          │
│                                                              │
│  If ANY box is unchecked, the proposal requires              │
│  explicit BDFL approval with documented rationale.          │
│                                                              │
│  If "does not add telemetry" is unchecked, the proposal     │
│  is automatically rejected. No exceptions.                  │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

### Design Decisions Registry

All architectural decisions are recorded in **DESIGN-DECISIONS.md** with:

**Entry format:**

```
├── Decision ID (sequential: DD-001, DD-002, etc.)
├── Date decided
├── Title (short description)
├── Context (why this decision was needed)
├── Options considered (alternatives evaluated)
├── Decision (what was chosen)
├── Rationale (why this option was chosen)
├── Consequences (what this decision implies)
├── Status (Accepted / Superseded by DD-XXX / Deprecated)
└── BDFL sign-off
```

The registry is append-mostly. Decisions can be superseded by newer decisions, but the original entry is never deleted. This creates a permanent record of the project's reasoning — anyone joining the project can understand not just what was decided, but why.

## Contributor Roles

### Role Definitions

| **Role** | **Description** | **How to earn** |
| :-: | :-: | :-: |
| Community Member | Anyone who uses Spike or participates in discussions | Join the forum or Matrix (no special access) |
| Reporter | Submits bug reports or feature suggestions | Open a quality issue with reproduction steps |
| Contributor | Submits code or documentation that is accepted | One merged merge request (DCO required) |
| Trusted Contributor | Consistently high-quality contributions, given merge access to specific areas | 5+ merged MRs, trusted by BDFL, no broken merges |
| Reviewer | Can review MRs in specific areas (cannot merge to main) | Trusted Contributor + demonstrated review skill |
| Maintainer | Can merge to main in specific areas (not all areas) | BDFL appointment |
| BDFL | Final authority on all matters | Project founder |

### Access Levels (GitLab)

**GitLab access mapped to roles:**

| **Role** | **GitLab Access** |
| :-: | :-: |
| Community Member | No GitLab access (public viewer) |
| Reporter | No GitLab access (issues are public) |
| Contributor | Guest role (can open MRs) |
| Trusted Contributor | Reporter role (can see internal issues) |
| Reviewer | Developer role (can review MRs, push to branches) |
| Maintainer | Maintainer role (can merge to main, manage labels) |
| BDFL | Owner role (full control) |

### Area Ownership

As the project grows, contributors may be given ownership of specific areas:

**Potential areas (as project grows):**

```
├── Spike Shell (panel, launcher, notifications)
├── Spike Installer
├── spike-config (configuration engine)
├── spike-update (update mechanism)
├── Documentation (user guide, dev guide)
├── Translations (per-language)
├── Branding (themes, assets)
├── Hardware testing (per-hardware-tier)
├── Packaging (ISO build, Flatpak manifests)
└── CI/CD (pipeline, automated tests)
```

Area owners are appointed by the BDFL based on demonstrated expertise and reliability. Area owners can review and merge MRs within their area. Cross-area changes require coordination between area owners or BDFL adjudication.

### Access Revocation

**Access can be revoked for:**

```
├── Violating the Code of Conduct
├── Repeatedly introducing bugs or regressions
├── Ignoring the Golden Rules or critical constraints
├── Adding telemetry or data collection (immediate revocation)
├── Abusing access (force-pushing main, deleting branches)
├── Inactivity (6+ months without contribution or communication)
└── Violating license or trademark terms
```

**Revocation process:**

```
├── BDFL notifies contributor with reason
├── Access reduced to previous level
├── Contributor can appeal via issue (public) or email (private)
└── BDFL's decision is final
```

## Licensing

### Software License

Spike is licensed under the GNU General Public License v2 or later (GPLv2+).

**What this means:**

```
├── Source code is freely available
├── Anyone can modify and redistribute
├── Derivatives must also be GPLv2+ (copyleft)
├── No warranty (as-is)
├── Commercial use is permitted
├── Cannot be relicensed as proprietary
└── "v2 or later" allows future GPL versions (v3, etc.) at user's option
```

**Why GPLv2+ (see DESIGN-DECISIONS.md for full rationale):**

**GPLv2+ chosen because:**

```
├── Consistent with Linux kernel license (v2-only)
├── Compatible with Qt6 (GPLv2+ / LGPLv3 / commercial)
├── Strong copyleft — derivatives must stay open
├── "v2 or later" provides flexibility for future GPL versions
├── Simple and well-understood in the Linux community
└── Avoids GPLv3-specific clauses that could complicate hardware
    partnerships or distribution
```

### Contributor Licensing

Spike uses Developer Certificate of Origin (DCO), not a Contributor License Agreement (CLA).

**DCO:**

```
├── Contributors attest they wrote the code (or have right to submit it)
├── Signed-off-by line in each commit: "Signed-off-by: Name <email>"
├── No copyright assignment to BigRangaTech
├── Contributors retain their copyright
├── Code is licensed under the project's GPLv2+ license
└── No CLA — contributors are not granting special rights to BigRangaTech
```

**Why DCO instead of CLA:**

```
├── CLA grants the project owner additional rights (often the right
│   to relicense). Spike does not need this and never will relicense
│   to a non-free license.
├── DCO is lighter weight (no legal agreement to sign)
├── DCO is standard in Linux kernel, Git, and many major projects
├── DCO protects against contributions of code the contributor
│   doesn't have rights to (but doesn't prevent it entirely)
└── BDFL commitment: Spike will never require a CLA
```

**How to sign off (contributors):**

```
├── git commit -s (adds Signed-off-by automatically)
├── Or manually add: Signed-off-by: Your Name <your.email@example.com>
└── Every commit must be signed off (not just the MR)
```

### Third-Party Code

**Third-party code included in Spike:**

```
├── Ubuntu packages: Under their respective licenses (mostly GPL, LGPL, MIT, BSD)
├── KDE applications: Under their respective licenses (mostly GPL, LGPL)
├── Qt6: Under GPLv2+ / LGPLv3 / commercial (Spike uses the open-source license)
├── Flatpak applications: Under their respective licenses
├── Flatpak runtimes: Under their respective licenses (KDE/GNOME runtime licenses)
└── All third-party licenses are compatible with GPLv2+
```

**Third-party code in the Spike repository:**

```
├── Vendored code (if any) must include original license text
├── Attribution must be preserved
├── License compatibility must be verified before inclusion
├── BDFL approves all third-party code inclusion
└── LICENSE file lists all included third-party licenses
```

## Trademark

### Trademark Ownership

The "Spike" name, the Spike dog logo, and all associated branding are trademarks of BigRangaTech.

**Trademark covers:**

```
├── The name "Spike" (as applied to a Linux distribution)
├── The Spike dog mascot/logo
├── The Spike color scheme (#6d4aff purple, #00bcd4 cyan)
├── The Spike Shell name
├── The "bigRangaTech" name and logo
└── All branding assets defined in BRANDING.md
```

**Trademark does NOT cover:**

```
├── The source code (covered by GPLv2+)
├── The architecture or design (documented openly)
├── The concept of a lightweight Linux for old hardware
└── Individual components (KWin, NetworkManager, etc.) — these are
    their respective projects' trademarks
```

### Trademark Usage Policy

**Permitted (no permission needed):**

```
├── Forking the code and building for personal use
├── Referencing Spike in reviews, articles, or tutorials
├── Linking to the official Spike website or repository
├── Using the Spike name in bug reports or technical discussions
├── Creating fan art or community content (non-commercial)
└── Distributing unmodified official ISO (with attribution)
```

**Requires permission:**

```
├── Distributing a modified ISO under the "Spike" name
├── Using the Spike logo on a product or service
├── Creating a commercial product based on Spike
├── Using "Spike" in the name of a derivative project
├── Hosting a website with "spike" in the domain name (other than
│   official domains)
└── Representing yourself as an official Spike project
```

**Prohibited:**

```
├── Claiming the Spike name or logo as your own
├── Distributing modified software under the Spike name without
│   clearly distinguishing it from official Spike
├── Using Spike branding to imply endorsement without authorization
└── Registering Spike-related trademarks in any jurisdiction
```

### Forks And Derivatives

Forks are welcome (GPLv2+ permits this). A fork must:

```
├── Use a different name (not "Spike" or anything confusingly similar)
├── Use different branding (not the Spike logo or color identity)
├── Respect the GPLv2+ license (share source code)
├── Clearly state it is a derivative of Spike, not official Spike
├── Not imply endorsement by BigRangaTech
└── Not use the bigrangatech.com domain or Spike infrastructure
```

A fork that violates trademark terms will be asked to rebrand. Persistent violation may result in legal action (last resort only).

### Trademark Enforcement

**Enforcement approach:**

```
├── Education first: contact the party, explain the policy
├── Request voluntary compliance (rename, rebrand)
├── Escalation only if voluntary compliance is refused
├── Legal action is a last resort, not a first response
├── BDFL makes all enforcement decisions
└── Community-driven reporting: report violations via GitLab issue
    or email to trademark@bigrangatech.com
```

## Community Spaces

### Official Channels

**Official Spike community spaces:**

**GitLab (git.bigrangatech.com/spike/spike)**

```
├── Issues: bug reports, feature requests, discussion
├── Merge Requests: code contributions
├── Wiki: community-maintained guides (if needed)
└── Primary hub for all project activity
```

**Website (spike.bigrangatech.com)**

```
├── Download page
├── Documentation
├── Community links
└── News/blog
```

**Forum (forum.bigrangatech.com)**

```
├── General discussion
├── Hardware compatibility reports
├── Help and support
├── Feature discussions
└── Translations coordination
```

**Matrix (#spike-linux:matrix.org)**

```
├── Real-time chat
├── Quick questions
├── Development coordination
└── Off-topic community chat
```

**GitHub (github.com/bigrangatech/spike)**

```
├── Read-only mirror
├── Issues DISABLED (redirect to GitLab)
├── PRs DISABLED (redirect to GitLab)
└── Exists for visibility only
```

### Moderation

**Moderation principles:**

```
├── Be welcoming to newcomers (especially non-technical users)
├── Enforce the Code of Conduct (see CODE_OF_CONDUCT.md)
├── Spam and trolling: immediate removal
├── Personal attacks: warning, then removal
├── Off-topic posts: moved to appropriate section
├── Misinformation: corrected, not censored (unless malicious)
├── Commercial spam: removed without warning
└── Security vulnerabilities: directed to private disclosure (see SECURITY.md)
```

**Moderators:**

```
├── BDFL is the head moderator
├── Trusted Contributors may be appointed as moderators
├── Moderation decisions can be appealed to BDFL
└── BDFL's moderation decisions are final
```

### Support Channels

Support is community-driven (no official support team):

**Forum (forum.bigrangatech.com):**

```
├── "Help" category for user support questions
├── Community members answer questions
├── BDFL participates when possible
├── Common issues become wiki entries
└── Not a guaranteed response channel
```

**Matrix (#spike-linux:matrix.org):**

```
├── Quick questions answered by community
├── Best for "how do I do X" questions
├── Not for bug reports (use GitLab issues)
└── Not for complex troubleshooting (use forum)
```

**GitLab Issues:**

```
├── Bug reports (not general questions)
├── Feature requests
├── Documentation corrections
├── Require reproduction steps
└── Response time depends on BDFL availability
```

**Official support (paid/professional):**

```
├── Not offered
├── Not planned
└── Community support is the model
```

## Succession Planning

### Why Succession Planning Matters

The project is designed to outlast its founder. This is a core principle (see **PHILOSOPHY.md** — "Built To Outlast"). The documentation-first methodology exists so that anyone can understand the system and continue development. The governance model defines what happens if the BDFL becomes unavailable.

### Succession Scenarios

**Scenario 1: BDFL voluntarily steps down**

```
├── BDFL announces intention to step down (public)
├── Transition period: 3-6 months
├── BDFL appoints successor (or community elects replacement)
├── Successor gains GitLab Owner access
├── Successor inherits BDFL role and responsibilities
├── Successor bound by same BDFL limitations (cannot violate
│   structural constraints)
├── GOVERNANCE.md updated with new BDFL
└── Community informed
```

**Scenario 2: BDFL becomes temporarily unavailable**

```
├── No action needed for short absences (< 1 month)
├── For longer absence: Trusted Contributors maintain the project
│   ├── Bug fixes and security patches can be merged by Maintainers
│   ├── No new features without BDFL approval (deferred)
│   ├── Releases held (no new version without BDFL sign-off)
│   └── Community informed of BDFL's absence status (if permitted)
├── On BDFL return: normal operation resumes
└── If BDFL's return is uncertain: proceed to Scenario 3
```

**Scenario 3: BDFL becomes permanently unavailable**

```
├── Trigger: 6 months of no communication from BDFL
├── Trusted Contributors (or community) initiate succession
├── Process:
│   ├── Open community discussion (forum, GitLab issue)
│   ├── Nominate candidates for new BDFL
│   ├── Candidates must demonstrate:
│   │   ├── Understanding of Spike's mission and philosophy
│   │   ├── Commitment to the structural constraints
│   │   ├── Technical competence in the codebase
│   │   └── Community trust
│   ├── Community discussion period: 30 days
│   ├── Consensus sought (not pure vote — weighted by contribution
│   │   history and project understanding)
│   ├── New BDFL declared
│   ├── GitLab ownership transferred (via GitLab admin or BigRangaTech
│   │   organization if prior arrangement exists)
│   └── GOVERNANCE.md updated
├── The new BDFL inherits the same limitations as the original
└── The structural constraints (no telemetry, Golden Rules, GPLv2+)
    are permanent and cannot be changed by any BDFL
```

### Preparations Made In Advance

To prepare for succession, the following are maintained:

```
├── All decisions documented with rationale (DESIGN-DECISIONS.md)
├── All architecture documented (70+ docs)
├── All code open source (GitLab, publicly readable)
├── GitLab organization structure allows ownership transfer
├── Domain registration (bigrangatech.com) has contingency instructions
├── BDFL maintains a sealed document with:
│   ├── GitLab admin credentials location
│   ├── Domain registrar access
│   ├── Server access details
│   ├── Trademark registration details
│   └── Instructions for trusted contact in case of emergency
└── Trusted contact (non-contributor) has instructions to:
    ├── Announce BDFL's unavailability to the community
    ├── Facilitate access transfer to successor
    └── Not participate in project decisions (custodian only)
```

## Security Vulnerability Handling

### Disclosure Policy

**Security vulnerability handling:**

**1. Private disclosure:**

```
├── Report to security@bigrangatech.com
├── Do NOT open a public issue for security vulnerabilities
├── Acknowledgment within 48 hours
└── Initial assessment within 7 days
```

**2. Triage and verification:**

```
├── BDFL (or designate) verifies the vulnerability
├── Severity assessed (Critical / High / Medium / Low)
├── Affected versions identified
└── Fix timeline determined based on severity
```

**3. Fix development:**

```
├── Critical/High: Immediate priority, fix within days
├── Medium: Fix within 1-2 weeks
├── Low: Fix in next regular update cycle
├── Fix developed in private branch
└── Fix tested before disclosure
```

**4. Coordinated disclosure:**

```
├── Fix released first (security update)
├── Then public disclosure (GitLab advisory, CHANGELOG.md)
├── Credit to reporter (if they wish)
└── CVE requested if applicable
```

**5. Post-disclosure:**

```
├── Update SECURITY.md if needed
├── Add to CHANGELOG.md Security section
├── Verify automatic update deployment
└── Monitor for exploitation reports
```

### Security Response Team

Currently: BDFL only

As project grows:

```
├── BDFL may appoint a Security Officer (Trusted Contributor)
├── Security Officer handles initial triage
├── BDFL makes final decisions on severity and disclosure timing
└── Security Officer signs NDA? No — no NDA needed (open source)
```

**Security Officer responsibilities:**

```
├── Monitor security@bigrangatech.com
├── Initial triage of reported vulnerabilities
├── Coordinate with BDFL on fix development
├── Draft security advisories
└── Track CVE status
```

## Project Infrastructure

### Infrastructure Ownership

**Infrastructure owned/operated by BigRangaTech:**

```
├── Domain: bigrangatech.com (spike.bigrangatech.com subdomain)
├── DNS: Managed via registrar
├── GitLab CE: Self-hosted (git.bigrangatech.com)
├── Website: Self-hosted (spike.bigrangatech.com)
├── Forum: Self-hosted (forum.bigrangatech.com)
├── Matrix: Hosted on matrix.org (public room)
├── Email: security@bigrangatech.com (for security reports)
└── Build infrastructure: CI/CD runners (self-hosted or sponsored)
```

**Infrastructure costs:**

```
├── Server hosting: BDFL funded
├── Domain registration: BDFL funded
├── No ongoing cloud costs (self-hosted)
├── No paid services (all open source)
└── Sponsorship/donations: Not solicited (see FAQ.md)
```

### Infrastructure Changes

Infrastructure changes require BDFL approval:

```
├── Moving to a different hosting provider
├── Adding new services (e.g., translation platform)
├── Changing CI/CD pipeline (beyond routine updates)
├── Adding new domains or subdomains
├── Changing email/security infrastructure
└── Enabling or disabling any service
```

Infrastructure must not introduce privacy violations:

```
├── No analytics on website or forum
├── No tracking pixels or third-party scripts
├── No Cloudflare or CDN that injects JavaScript
├── Forum must not collect more data than necessary
├── Email must be private (no third-party scanning)
└── All infrastructure must comply with PRIVACY.md principles
```

## Conflict Resolution

### Internal Conflicts (Contributors)

**Conflict between contributors:**

```
1. Attempt resolution in the relevant issue/MR discussion
2. If unresolved: both parties request BDFL mediation
3. BDFL reviews the disagreement, makes a decision
4. Decision is documented in the issue/MR
5. BDFL's decision is final for technical matters
6. For conduct matters: see CODE_OF_CONDUCT.md enforcement process
```

**Conflict with the BDFL:**

```
1. Raise the concern publicly (issue) or privately (email)
2. BDFL must respond within 7 days
3. If the concern involves a structural constraint violation
   (e.g., BDFL proposes adding telemetry):
   ├── Community can invoke the structural constraint override
   │   ├── Requires open issue with 5+ contributor endorsements
   │   ├── Forces BDFL to document rationale in DESIGN-DECISIONS.md
   │   └── If rationale doesn't satisfy structural constraints:
   │       the change CANNOT be made (constraints are binding)
   └── This is the only check on BDFL authority
```

### Structural Constraint Override

The structural constraints (defined in "BDFL Limitations" above and in **AGENTS.md** Section 7) are the one area where the community has binding authority over the BDFL. If the BDFL proposes something that violates these constraints:

**Override process:**

```
├── 5 or more Contributors (people with at least one merged MR) open
│   a joint issue titled "Structural Constraint Override: [topic]"
├── Issue cites the specific constraint being violated
├── BDFL must respond within 14 days with either:
│   ├── Withdrawal of the proposed change (constraint upheld), OR
│   ├── Formal rationale in DESIGN-DECISIONS.md (attempting to justify)
├── Community reviews the rationale
├── If the rationale does not satisfy the constraint:
│   ├── The proposed change is blocked
│   └── It cannot be implemented regardless of BDFL desire
├── The only way to change a structural constraint is:
│   ├── A formal amendment to this document (GOVERNANCE.md)
│   ├── 30-day community discussion period
│   ├── Documented rationale in DESIGN-DECISIONS.md
│   └── BDFL + community consensus
└── Structural constraints are intentionally difficult to change
```

This ensures the project's core identity (no telemetry, Golden Rules, GPLv2+, old-hardware focus) survives any leadership change.

## Governance Changes

### How This Document Is Amended

**GOVERNANCE.md amendment process:**

```
├── Proposed amendment opened as a GitLab issue
├── Tagged "governance-change"
├── Minimum 14-day discussion period
├── BDFL makes decision
├── If approved: PR submitted with the change
├── PR merged by BDFL
├── Amendment announced on forum and website
└── CHANGELOG.md updated (noted as governance change)
```

**For amendments to structural constraints:**

```
├── 30-day discussion period (longer than normal amendments)
├── Requires community consensus (not just BDFL approval)
├── Must be documented in DESIGN-DECISIONS.md
└── Must not weaken any existing constraint without replacing
    it with an equivalent or stronger guarantee
```

## What This Document Does Not Cover

- Code of conduct and community behavior standards: See **CODE_OF_CONDUCT.md** 

- Contribution workflow and technical guidelines: See **CONTRIBUTING.md** 

- Design rationale for governance decisions: See **DESIGN-DECISIONS.md** 

- Project roadmap and release timeline: See **ROADMAP.md** 

- End-of-life policy for releases: See **END-OF-LIFE-POLICY.md** (to be written) 

- Security model and hardening details: See **SECURITY.md** 

- Privacy policy: See **PRIVACY.md** 

- Project philosophy and mission: See **PHILOSOPHY.md** 

- Architecture and technical decisions: See **ARCHITECTURE.md** and **DESIGN-DECISIONS.md** 

- License text: See **LICENSE** (GPLv2+) and **LICENSE-CC-BY-SA-4.0** (docs/artwork) 

- Changelog and release history: See **CHANGELOG.md** 



🐕 BigRangaTech

