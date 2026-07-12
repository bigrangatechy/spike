## Pre-Format Review Summary: CODE\_OF\_CONDUCT.md

**Status:** Received ✅

**Document Size:** ~1,200+ lines

**Issues Identified:** 7

| **\#** | **Issue** | **Severity** | **Action** |
| :-: | :-: | :-: | :-: |
| 1 | Headers are plain text — no `\#` markers | High | Add Markdown headers |
| 2 | Tree structures lack code fencing | High | Wrap all `├──`/`└──` in \`\`\` blocks |
| 3 | ASCII enforcement flowchart needs code fencing | High | Wrap in \`\`\` block |
| 4 | ASCII enforcement policy box and timeline box need fencing | High | Wrap in \`\`\` blocks |
| 5 | "To be written" tags need updating | Moderate | CONTRIBUTING.md is done; LICENSE still pending |
| 6 | Cross-reference paths need verification | Moderate | Verify all doc names exist |
| 7 | Blank line standardization | Minor | Ensure around all blocks |


# Spike Code of Conduct

## Purpose

This document defines the standards of behavior for everyone participating in the Spike Linux community — contributors, forum users, bug reporters, documentation writers, and anyone else interacting with the project.

A code of conduct is not bureaucracy. It is a promise. The promise is: if you come to Spike's community in good faith, you will be treated with respect regardless of who you are or how much you know. If someone violates that promise, the community will act — quickly and decisively.

Spike exists to help non-technical users. Many people arriving at Spike's forum have never used Linux before. Some have never participated in an open-source community. They are nervous, frustrated with their old laptop, and possibly embarrassed that they don't know things that seem obvious to experienced users.

Every forum has the same problem. A beginner asks a basic question. Someone responds with condescension. Another person piles on. The beginner leaves and never comes back. The forum becomes a club for people who already know things, and the people who need help most are driven away.

This code of conduct exists to stop that. Spike's enforcement is strict by design. We give one warning. After that, you are removed. This is not because we are unforgiving — it is because every toxic member we tolerate costs us ten good members who simply leave quietly.

## Who This Applies To

This code of conduct applies to:

```
`├── All contributors (code, documentation, translations, testing)`

`├── All forum participants (posting, commenting, reading)`

`├── All bug tracker participants (reporting, commenting, triaging)`

`├── All chat participants (Matrix, IRC, if/when available)`

`├── All website visitors who interact with community features`

`├── All maintainers and moderators (held to the same standard,`

`│   and a higher one — they represent the project)`

`└── The BDFL (BigRangaTech) — no one is exempt`
```

This code of conduct applies in:

```
`├── The GitLab repository (issues, merge requests, comments, wiki)`

`├── The GitHub mirror (issues, comments)`

`├── The forum (forum.bigrangatech.com)`

`├── Community chat rooms (Matrix, IRC, if/when available)`

`├── Official Spike communication channels (email to maintainers,`

`│   if published)`

`├── Any space representing Spike Linux officially`

`└── Private communications about community matters that affect`

`    the project`
```

This code of conduct does NOT apply to:

```
`├── Private conversations unrelated to Spike`

`├── Discussions on other platforms (Reddit, Twitter, etc.) unless`

`│   they are in an official Spike community space there`

`├── Individual personal accounts (even if owned by contributors)`

`└── Behavior outside the Spike community that is not directed at`

`    Spike community members or the project itself`
```

However: if someone's behavior outside the community demonstrates a pattern of harassment or harm toward groups represented in the Spike community, that may inform moderation decisions within the community. Context matters.

## Our Standards

### Positive Behavior

Behaviors that strengthen the community:

```
`├── Welcoming newcomers: treating first-time participants as valued`

`│   members, not outsiders who need to prove themselves`

`├── Patience: recognizing that people have different skill levels,`

`│   native languages, and amounts of free time`

`├── Clarity: communicating in plain language, avoiding unnecessary`

`│   jargon, explaining technical terms when using them`

`├── Empathy: considering the other person's perspective before`

`│   responding, especially in disagreement`

`├── Constructive feedback: focusing on the problem, not the person;`

`│   criticizing ideas, not individuals`

`├── Graceful acceptance of feedback: when corrected, acknowledging`

`│   it rather than becoming defensive`

`├── Asking good questions: providing context, being specific,`

`│   searching before asking`

`├── Answering generously: sharing knowledge freely, writing`

`│   answers that help future readers not just the immediate asker`

`├── Respecting differing viewpoints: acknowledging that reasonable`

`│   people can disagree on technical choices`

`├── Respecting time: not demanding immediate responses, not`

`│   guilt-tripping volunteers for being slow`

`├── Using inclusive language: avoiding terms that exclude or`

`│   marginalize (see "Inclusive Language" below)`

`├── Helping de-escalate conflict: if a discussion is heating up,`

`│   encouraging cooler heads`

`├── Reporting violations: letting moderators handle issues rather`

`│   than retaliating`

`└── Leading by example: demonstrating the behavior you want to see`
```

### Unacceptable Behavior

Behaviors that will not be tolerated:

**Harassment:**

```
`├── Personal attacks (insults about someone's intelligence,`

`│   appearance, background, or character)`

`├── Sustained disruption of discussion (trolling, derailing threads,`

`│   posting repeatedly to prevent conversation)`

`├── Sexual attention or innuendo of any kind`

`├── Unwanted physical attention descriptions or advances (online)`

`├── Stalking: following someone across platforms, repeated unwanted`

`│   contact after being told to stop`

`├── Photographs or images that are sexual, violent, or demeaning`

`└── Intimidation: implied or stated threats`
```

**Discrimination:**

```
`├── Derogatory comments about gender, gender identity, sexual`

`│   orientation, disability, race, ethnicity, nationality, religion,`

`│   age, body size, socioeconomic status, or technical skill level`

`├── Stereotyping based on any of the above categories`

`├── Exclusionary jokes or "humor" that targets a group`

`└── Differential treatment based on identity rather than behavior`
```

**Intimidation and coercion:**

```
`├── Threatening negative consequences for disagreement`

`├── Leveraging reputation or status to silence others`

`├── Demanding credentials or "proof of competence" before engaging`

`│   with someone's contribution`

`├── "Gatekeeping": deciding who is and isn't "qualified" to participate`

`├── Deliberately intimidating new contributors`

`└── Retaliation against anyone who reports a violation`
```

**Disruption:**

```
`├── Spamming (repeated posting of irrelevant content)`

`├── Thread hijacking (redirecting discussions to unrelated topics)`

`├── Excessive self-promotion (especially commercial products)`

`├── Posting off-topic political or religious content designed to provoke`

`├── Repeatedly posting after being asked to stop`

`└── Creating multiple accounts to evade moderation`
```

**Doxxing:**

```
`├── Sharing someone's private information (real name, address, phone,`

`│   workplace) without consent`

`├── Threatening to share private information`

`├── Investigating someone's identity to use against them`

`└── This is an immediate permanent ban, no warning (see Enforcement)`
```

**Plagiarism:**

```
`├── Claiming someone else's work as your own`

`├── Submitting code or documentation without attribution`

`├── Removing author attribution from existing work`

`└── This is treated as a code of conduct violation, not just a`

`    technical issue (see CONTRIBUTING.md for attribution requirements)`
```

### Inclusive Language

Language matters. Words signal who belongs and who doesn't.

Spike's community uses:

```
`├── "Folks," "everyone," "people" (not "guys" for mixed groups)`

`├── "Newcomer" or "new contributor" (not "noob," "newbie")`

`├── "Non-technical user" (not "clueless," "idiot," "dummy")`

`├── "Feedback" or "correction" (not "criticism" when constructive)`

`├── Gender-neutral pronouns (they/them) when gender is unknown`

`├── People's stated pronouns (if shared)`

`└── Clear, jargon-free language in community-facing communication`
```

Avoid:

```
`├── Gendered terms when gender-neutral alternatives work`

`├── "Crazy," "insane," "retarded" (ableist language)`

`├── "Master/slave" in documentation (use primary/secondary or`

`│   controller/worker — this also aligns with technical accuracy)`

`├── "Blacklist/whitelist" (use blocklist/allowlist)`

`├── Sexualized or violent metaphors ("kill the process" is fine`

`│   in technical context, "rape the data" is not)`

`├── Diminishing language ("cute attempt," "adorable question")`

`└── "RTFM," "Just Google it," "Let me LMGTFY" (these are dismissive`

`    and count as unacceptable behavior — see below)`
```

Technical terms:

```
`├── Some terms have deep history in computing and may not have`

`│   obvious replacements (e.g., "kill signal," "daemon")`

`├── Spike uses inclusive alternatives where practical`

`├── Technical necessity is not an excuse for unnecessary exclusion`

`└── When in doubt: choose the inclusive option`
```

English as a second language:

```
`├── Many community members are not native English speakers`

`├── Do not mock grammar, spelling, or phrasing`

`├── If meaning is unclear: ask for clarification politely`

`├── Do not assume someone's technical competence from their English`

`└── Avoid idioms that non-native speakers may not understand`
```

### Beginner Protection

This section exists because every technical forum eventually fails its most vulnerable members: absolute beginners.

A beginner's first interaction with the community shapes whether they stay or leave forever. A single dismissive response can confirm every fear they had about Linux being "too hard" or "not for people like me."

Spike's community does not tolerate behavior that drives beginners away. The following are violations of this code of conduct:

```
`├── "RTFM," "Just Google it," "LMGTFY" — dismissive responses`

`│   to any question, regardless of how basic`

`├── Condescension disguised as help ("Oh, sweetie," "It's`

`│   simple, really," "You just...")`

`├── Implying a beginner is in the wrong place ("Maybe Linux`

`│   isn't for you")`

`├── Mocking someone's lack of knowledge in any form`

`├── Answering a different (harder) question to show off`

`│   technical knowledge`

`├── Responding with jargon when plain language was possible`

`├── Sighing (in text) or expressing exasperation at a question`

`├── "We get this question every week" or similar complaints`

`│   about repetition — if it's asked often, the docs need work,`

`│   not the user`

`├── Downvoting or burying beginner questions without responding`

`├── Gatekeeping: deciding who is "qualified" to participate`

`├── Treating a beginner's mistake as evidence they shouldn't`

`│   be here`

`└── Any behavior that a reasonable person would describe as`

`    "making a beginner feel stupid"`
```

These apply to ALL participants, including experienced contributors and moderators. Expertise does not grant immunity from courtesy.

The standard is not "was the answer technically correct." The standard is "would a nervous first-time Linux user feel welcome and helped by this response?"

If the answer is no, it's a violation.

## Enforcement

### Enforcement Philosophy

Spike uses a strict, simple enforcement model:

```
`┌──────────────────────────────────────────────────────────────┐`

`│                                                              │`

`│  ONE WARNING. THEN YOU'RE OUT.                              │`

`│                                                              │`

`│  First offense:  Formal warning (logged)                     │`

`│  Second offense: Ban (suspended, 7 days to appeal)           │`

`│  No appeal:     Permanent ban (assumed guilty after 7 days)  │`

`│  Appeal denied:  Permanent ban                               │`

`│  Appeal granted: 30-day probation (zero tolerance)           │`

`│  Probation violation: Permanent ban (7 days to appeal)       │`

`│                                                              │`

`│  Exception: Severe violations (doxxing, threats, illegal     │`

`│  content) bypass warnings entirely — immediate permanent     │`

`│  ban (see "Severe Violations" below).                        │`

`│                                                              │`

`└──────────────────────────────────────────────────────────────┘`
```

Why strict enforcement:

Most communities use graduated systems — three strikes, four levels, multiple warnings. These systems sound fair but have a fatal flaw: they protect repeat offenders at the expense of everyone else. While the community waits for someone to accumulate their third, fourth, fifth strike, dozens of newcomers have been driven away by that person's behavior.

Spike chooses a different model. One warning gives someone the benefit of the doubt — maybe they had a bad day, maybe they didn't realize how they came across. A second offense means they chose to ignore the warning. At that point, the community's wellbeing outweighs the individual's participation.

This model works because:

```
`├── Warnings are private and respectful (not public shaming)`

`├── Appeals exist for genuine mistakes`

`├── Probation gives a path back for those who deserve it`

`├── But tolerance for repeated harmful behavior is zero`

`├── And the people who suffer most from lenient enforcement`

`│   (beginners, newcomers, marginalized members) are protected`

`└── The community grows healthier, not larger`
```

### Enforcement Flow

```
`┌──────────────────────────────────────────────────────────────┐`

`│                                                              │`

`│                    ENFORCEMENT FLOWCHART                      │`

`│                                                              │`

`│  ┌─────────────┐                                             │`

`│  │ Violation    │                                             │`

`│  │ reported     │                                             │`

`│  └──────┬──────┘                                             │`

`│         │                                                     │`

`│         ▼                                                     │`

`│  ┌─────────────────────────┐                                │`

`│  │ Is this a SEVERE         │─── Yes ──→ Immediate permanent │`

`│  │ violation?               │            ban (see Severe     │`

`│  │ (doxxing, threats,      │            Violations)          │`

`│  │  illegal content)        │                                │`

`│  └──────┬──────────────────┘                                │`

`│         │ No                                                 │`

`│         ▼                                                     │`

`│  ┌─────────────────────────┐                                │`

`│  │ Has this person been     │─── No ────┐                   │`

`│  │ warned before?           │           │                   │`

`│  └──────┬──────────────────┘           ▼                   │`

`│         │ Yes                   ┌──────────────────┐        │`

`│         │                       │ FIRST OFFENSE     │        │`

`│         ▼                       │ → Formal warning  │        │`

`│  ┌──────────────────┐           │ → Logged          │        │`

`│  │ SECOND OFFENSE   │           │ → Private message │        │`

`│  │ → Account banned │           │ → No restriction  │        │`

`│  │ → 7 days to       │           └──────────────────┘        │`

`│  │   appeal          │                                       │`

`│  └──────┬───────────┘                                       │`

`│         │                                                     │`

`│    ┌────┴────┐                                               │`

`│    │         │                                               │`

`│    ▼         ▼                                               │`

`│  Appeals   Does NOT                                         │`

`│  within     appeal within                                    │`

`│  7 days     7 days                                           │`

`│    │              │                                         │`

`│    ▼              ▼                                         │`

`│  ┌──────────┐  ┌──────────────┐                             │`

`│  │ Appeal   │  │ PERMANENT     │                            │`

`│  │ reviewed │  │ BAN           │                            │`

`│  └────┬─────┘  │ (assumed      │                            │`

`│       │        │  guilty)      │                            │`

`│       ▼        └──────────────┘                             │`

`│  ┌────────────┐                                              │`

`│  │ Granted?   │                                              │`

`│  └──┬────┬────┘                                              │`

`│     │    │                                                    │`

`│    Yes   No                                                   │`

`│     │    │                                                    │`

`│     ▼    ▼                                                   │`

`│  ┌──────────┐ ┌──────────┐                                  │`

`│  │ 30-day   │ │PERMANENT │                                   │`

`│  │ PROBATION│ │ BAN      │                                   │`

`│  │          │ └──────────┘                                   │`

`│  │ Zero     │                                                 │`

`│  │ tolerance│                                                 │`

`│  │ for any  │                                                 │`

`│  │ offense  │                                                 │`

`│  └────┬─────┘                                                 │`

`│       │                                                       │`

`│       │ Violation during probation                           │`

`│       ▼                                                       │`

`│  ┌──────────────────┐                                        │`

`│  │ PERMANENT BAN    │                                        │`

`│  │ (7 days to       │                                        │`

`│  │  appeal)         │                                        │`

`│  └──────────────────┘                                        │`

`│                                                              │`

`└──────────────────────────────────────────────────────────────┘`
```

Key points:

```
`├── A "warning" is the ONLY second chance`

`├── The ban is not negotiable — it happens on second offense`

`├── The appeal is the ONLY path back`

`├── Not appealing = accepting the ban`

`├── Winning an appeal = probation, not a clean slate`

`├── Probation means ZERO tolerance — any offense, however minor`

`└── After probation ends cleanly: full member, no restrictions`
```

### First Offense — Warning

What constitutes a first offense:

```
`├── Any violation listed in "Unacceptable Behavior" above`

`├── Any instance of the behaviors listed in "Beginner Protection"`

`├── Any single action that warrants intervention`
```

Warning process:

```
`├── Moderator sends a private message (not a public call-out)`

`├── Message includes:`

`│   ├── What the person did (specific description)`

`│   ├── Which rule was violated`

`│   ├── Why the behavior is harmful (especially to beginners)`

`│   ├── That this is their one and only warning`

`│   ├── That a second offense will result in a ban`

`│   ├── That they will have 7 days to appeal if banned`

`│   └── An invitation to ask questions if anything is unclear`

`├── Warning is logged internally:`

`│   ├── Date, time, nature of violation`

`│   ├── Moderator who issued the warning`

`│   ├── Link to the original content (if applicable)`

`│   └── Status: WARNING ISSUED`

`├── Offending content may be:`

`│   ├── Edited (with a note indicating moderator action)`

`│   ├── Removed (with a note: "Removed by moderator — CoC violation")`

`│   └── Left in place (if the behavior was in the response, not the content)`

`├── The person is NOT restricted from posting (yet)`

`└── The warning expires after 12 months of clean behavior`

`    └── If no further violations in 12 months: record cleared,`

`        person returns to "no prior offenses" status`
```

What a warning is NOT:

```
`├── A public shaming (no "call-out" posts)`

`├── A debate (moderator decision is final, but questions are OK)`

`├── A punishment (it's an opportunity to correct course)`

`├── A permanent mark on someone's reputation`

`└── Negotiable (the warning stands; only future behavior matters)`
```

### Second Offense — Ban

What triggers the ban:

```
`├── Any violation of the code of conduct by someone who has an`

`│   active warning on file`

`├── The second offense does NOT need to be the same type of`

`│   violation as the first`

`│   ├── Example: warning for condescension, then banned for`

`│   │   thread hijacking — both count`

`│   └── The rule is "second offense" not "second of the same type"`

`├── The offense can be minor — a warning means you are on notice`

`│   and ANY further violation triggers the ban`

`└── Moderators do not have discretion to "give another chance"`

`    └── The enforcement model is the enforcement model`
```

Ban process:

```
`├── Account is suspended (cannot post, comment, or interact)`

`├── The person receives a notification:`

`│   ├── "Your account has been suspended due to a second violation`

`│   │   of the Spike Code of Conduct."`

`│   ├── Description of the violation`

`│   ├── Reference to their prior warning`

`│   ├── "You have 7 days to appeal this decision"`

`│   ├── Instructions on how to appeal (email, see "Appeals")`

`│   └── "If you do not appeal within 7 days, this suspension`

`│       becomes a permanent ban."`

`├── Offending content is removed`

`├── The ban is logged:`

`│   ├── Both violations (warning + second offense)`

`│   ├── Moderator who issued the ban`

`│   └── Status: BANNED — AWAITING APPEAL`

`├── No public announcement (bans are not a spectacle)`

`└── The person's existing content (posts, comments, code) is`

`    retained (not deleted) unless it is itself violating content`
```

The 7-day clock:

```
`├── Starts from the moment the ban notification is sent`

`├── Ends at the same time of day, 7 days later`

`├── If the 7th day falls on a weekend or holiday: the deadline`

`│   extends to the next business day (Australian timezone)`

`├── The person can appeal on day 1 or day 7 — timing doesn't`

`│   affect the outcome`

`└── After 7 days with no appeal: the ban becomes permanent`

`    automatically (no further review needed)`
```

### No Appeal — Permanent Ban

If the person does not appeal within 7 days:

```
`├── The suspension becomes a permanent ban`

`├── Status changes to: PERMANENT BAN — NO APPEAL`

`├── The account remains suspended permanently`

`├── No further communication is sent (the 7-day notification`

`│   made the consequences clear)`

`├── The person's existing content is retained (not deleted)`

`│   unless it is violating content`

`├── Creating new accounts to evade the ban is itself a violation`

`│   └── Evading accounts are permanently banned on sight`

`└── The internal log records: permanent ban, did not appeal,`

`    assumed guilty`
```

Rationale:

```
`├── The person was told they had 7 days to appeal`

`├── Not appealing = not contesting = accepting the decision`

`├── The community cannot hold space indefinitely for someone`

`│   who won't engage with the process`

`└── This is not harsh — it is the natural consequence of`

`    inaction after clear communication`
```

### Appeal — Denied

If the person appeals and the appeal is denied:

```
`├── The suspension becomes a permanent ban`

`├── Status changes to: PERMANENT BAN — APPEAL DENIED`

`├── The person is notified:`

`│   ├── "Your appeal has been reviewed and denied."`

`│   ├── Brief explanation of why (factual, not argumentative)`

`│   └── "This decision is final."`

`├── The person's existing content is retained`

`├── Account creation to evade: same as above (permanent ban on sight)`

`└── The internal log records: permanent ban, appealed, denied`
```

Appeal denial factors:

```
`├── The violation was clear and unambiguous`

`├── The person had a prior warning for similar or other behavior`

`├── The appeal did not present new information`

`├── The appeal acknowledged the behavior but argued it was justified`

`│   (it wasn't — the code of conduct is the standard, not personal`

`│   opinion)`

`├── The person's overall pattern shows disregard for community norms`

`└── The behavior would have driven away other community members`
```

There is no second appeal for the same incident.

### Appeal — Granted

If the person appeals and the appeal is granted:

```
`├── The ban is lifted`

`├── Status changes to: PROBATION — 30 DAYS`

`├── The person is notified:`

`│   ├── "Your appeal has been granted. You may return to the`

`│   │   community."`

`│   ├── "You are on probation for 30 days."`

`│   ├── "During probation, ANY violation of the code of conduct,`

`│   │   however minor, will result in a permanent ban."`

`│   ├── "You will have 7 days to appeal that permanent ban."`

`│   └── "After 30 days with no violations, your probation ends`

`│       and you are a full member again."`

`├── The 30-day probation starts from the moment the appeal`

`│   is granted`

`├── A probation indicator may appear internally (moderator-visible`

`│   only, not visible to other community members)`

`└── The person's existing content and account are fully restored`
```

Appeal granted factors:

```
`├── The original ban was based on behavior that, in context, was`

`│   less severe than initially assessed`

`├── New information emerged that changed the interpretation`

`│   of the incident`

`├── The person demonstrated genuine understanding of why their`

`│   behavior was problematic`

`├── The person showed willingness to change`

`├── The prior warning was very old (\>6 months) suggesting`

`│   the behavior was not habitual`

`├── The behavior, while inappropriate, was clearly out of character`

`│   based on community history`

`└── A reasonable moderator believes the person can rejoin without`

`    harming the community`
```

Granting an appeal does NOT mean the ban was wrong:

```
`├── It means the person deserves a second chance`

`├── The original violation still stands on record`

`├── The probation period reflects that trust is being rebuilt`

`└── If trust is broken again: it's permanent`
```

### Probation Period

30-day probation rules:

```
`├── ANY violation of the code of conduct, however minor, triggers`

`│   a permanent ban`

`│   ├── No warning (they already had their warning)`

`│   ├── No second chance (they already had their appeal)`

`│   └── No discretion for moderators (probation = zero tolerance)`

`├── The permanent ban triggered during probation has the same`

`│   7-day appeal window`

`│   ├── If they appeal and win: they do NOT get another probation`

`│   │   period — they are permanently banned (a third chance`

`│   │   does not exist)`

`│   └── If they appeal and lose: permanent ban confirmed`

`├── After 30 days with zero violations:`

`│   ├── Probation ends`

`│   ├── Status: ACTIVE MEMBER (clean record going forward)`

`│   ├── The internal log retains the history (for pattern`

`│   │   tracking)`

`│   └── The person is a full member with no restrictions`

`├── What "any violation" means during probation:`

`│   ├── Even Level 1 behavior (mild impatience, non-inclusive`

`│   │   language) triggers the permanent ban`

`│   ├── The standard is higher during probation because the`

`│   │   person has been told twice already`

`│   └── Moderators do not have flexibility here`

`└── The 30-day countdown:`

`    ├── Pauses if the person is offline for extended periods`

`    │   (7+ days): clock resumes on next login`

`    ├── Otherwise runs continuously`

`    └── Both the person and moderators can see the countdown`

`        (internally)`
```

Note on probation transparency:

```
`├── The probation status is NOT visible to other community members`

`├── The person appears as a normal member to everyone except`

`│   moderators`

`├── This prevents stigma and "probation pile-on" where other`

`│   members scrutinize someone they know is on probation`

`└── If the person tells others they're on probation: that's their`

`    choice (not a violation)`
```

### Severe Violations

Some violations are severe enough to bypass the warning system entirely. These result in an immediate permanent ban.

Severe violations:

```
`├── Doxxing (sharing someone's private information)`

`├── Threats of violence (against individuals or groups)`

`├── Stalking (across platforms, targeted harassment)`

`├── Predatory behavior toward minors`

`├── Coordinated harassment campaigns`

`├── Illegal content (under Australian law — see "Legal Compliance")`

`├── Sexualized content targeting a specific person`

`└── Creating accounts to evade an existing ban`
```

Process for severe violations:

```
`├── Immediate account suspension`

`├── Content removed`

`├── No warning (these are not "first offense" material)`

`├── The person IS given a 7-day appeal window`

`│   ├── Because even severe violations can be misinterpreted`

`│   │   (false positive, misunderstanding context, mistaken identity)`

`│   ├── If appeal is granted: 30-day probation (same as above)`

`│   └── If appeal is denied or not filed: permanent ban`

`├── Reported to platform (GitLab, GitHub, forum host) if TOS violated`

`├── Reported to law enforcement if illegal (Australian authorities)`

`└── No appeal after the 7-day window (same as standard enforcement)`
```

Why severe violations skip warnings:

```
`├── Doxxing can cause real-world harm instantly`

`├── Threats cause fear and trauma regardless of intent`

`├── A warning for doxxing implies "try once more" — that's unacceptable`

`├── The victim's safety outweighs the offender's second chance`

`└── These behaviors are universally understood to be unacceptable`
```

### Enforcement Responsibilities

Community leaders (moderators, maintainers) are responsible for:

```
`├── Clarifying standards of acceptable behavior`

`├── Applying the enforcement model consistently`

`├── Removing or editing unacceptable content (with explanation)`

`├── Locking threads that become unproductive`

`├── Issuing warnings for first offenses`

`├── Issuing bans for second offenses`

`├── Reviewing appeals within the 7-day window`

`├── Managing probation periods`

`├── Documenting all enforcement actions (internal log)`

`├── Protecting community members from retaliation`

`├── Reviewing appeals fairly and in good faith`

`└── Modeling the behavior expected of the community`
```

Moderators are NOT expected to:

```
`├── Be online 24/7 (they are volunteers)`

`├── Respond instantly to every report`

`├── Be perfect (mistakes happen; appeals exist)`

`├── Engage in debates about enforcement decisions in public threads`

`├── Explain every decision in exhaustive detail (a brief reason`

`│   is provided; lengthy debate is not productive)`

`├── Give extra chances beyond the defined enforcement model`

`│   └── If a moderator wants to be lenient: they should not be a moderator`

`└── Tolerate abuse directed at them for enforcing the code`
```

Moderator appointments:

```
`├── Appointed by BigRangaTech (BDFL)`

`├── Selected from active, respected community members`

`├── Demonstrate: fairness, patience, good judgment, commitment to`

`│   Spike's values`

`├── Not based on: technical skill alone (a great coder who is`

`│   dismissive to newcomers is not a good moderator)`

`├── Removal: by BDFL decision (if a moderator is not fulfilling`

`│   their responsibilities or violates the code themselves)`

`└── Number of moderators scales with community size`
```

## Reporting

### How To Report

If you experience or witness a code of conduct violation:

Report to: `conduct@bigrangatech.com`

```
`├── This email goes to all moderators`

`├── Do NOT report code of conduct violations in public forum threads`

`│   ├── Public reports can escalate conflict`

`│   ├── They expose the reporter to retaliation`

`│   └── Moderators need to review privately`

`├── For forum-specific issues: use the "Flag" button on the post`

`│   └── This alerts moderators and is appropriate for content`

`│       that needs immediate removal`

`├── For GitLab issues: flag the comment or contact a maintainer`

`│   directly (maintainer list in CONTRIBUTING.md)`

`└── For chat: contact a moderator directly (private message)`
```

What to include in your report:

```
`├── Your name (or pseudonym — anonymous reports are accepted but`

`│   harder to investigate)`

`├── Contact method (so moderators can follow up)`

`├── What happened (specific description)`

`├── Where it happened (forum, GitLab, chat, other)`

`├── When it happened (date and approximate time)`

`├── Who was involved (usernames, links to posts/comments)`

`├── Evidence (screenshots, quotes, links)`

`├── Witnesses (if known — others who saw it)`

`├── Impact (how it affected you or others)`

`└── What you'd like to see happen (suggestion, not requirement —`

`    moderators decide the response)`
```

Report handling:

```
`├── Reports are confidential`

`│   ├── Only moderators and the BDFL see reports`

`│   ├── Reporter's identity is not shared with the reported person`

`│   ├── Reports are not discussed publicly`

`│   └── Internal logs are accessible only to moderators`

`├── Reports are taken seriously`

`│   ├── Every report is reviewed`

`│   ├── No report is dismissed without investigation`

`│   └── Even "minor" reports are logged for pattern tracking`

`├── False reports`

`│   ├── Making a false report (knowingly inaccurate) is itself`

`│   │   a code of conduct violation`

`│   ├── However: genuine reports that are mistaken are NOT`

`│   │   violations (good faith errors are understood)`

`│   └── Repeated false reports will be addressed`

`└── Anonymous reports`

`    ├── Accepted and investigated`

`    ├── Harder to investigate without follow-up capability`

`    ├── Action may be limited if evidence is insufficient`

`    └── Still logged and reviewed`
```

### What Happens After You Report

```
`┌──────────────────────────────────────────────────────────────┐`

`│                                                              │`

`│  Timeline:                                                   │`

`│                                                              │`

`│  Within 72 hours:  Report acknowledged by a moderator        │`

`│  Within 7 days:    Report reviewed by at least 2 moderators │`

`│                    Action decided (if warranted)             │`

`│  After action:     Reporter notified of outcome (general)   │`

`│  If no action:      Reporter notified with explanation       │`

`│                                                              │`

`│  For severe violations (doxxing, threats, illegal content):   │`

`│  Immediate:        Content removed, account suspended        │`

`│  Within 24 hours:  Moderators review and confirm             │`

`│                                                              │`

`└──────────────────────────────────────────────────────────────┘`
```

You will be informed:

```
`├── That your report was received`

`├── That it was reviewed`

`├── Whether action was taken (general nature, not specifics)`

`├── If no action was taken: why not`

`└── That the matter is closed`
```

You will NOT be informed:

```
`├── Exact punishment details (a person's privacy matters too)`

`├── Internal moderator discussions`

`├── Other reports about the same person (past or present)`

`├── Private communications with the reported person`

`└── The person's reaction or response to the action`
```

If you feel your report was not handled properly:

```
`├── You can appeal to the BDFL directly: conduct@bigrangatech.com`

`│   (mark subject as "APPEAL — HANDLING REVIEW")`

`├── The BDFL reviews the handling (not just the original decision)`

`├── This is about whether process was followed, not whether`

`│   you agree with the outcome`

`└── BDFL's review is final`
```

### Appeals

Anyone who receives a ban (second offense or severe violation) can appeal within 7 days.

How to appeal:

```
`├── Email: conduct@bigrangatech.com`

`├── Subject: "APPEAL — \[your username\]"`

`├── Include:`

`│   ├── Your username`

`│   ├── The action taken against you (ban, what you were told)`

`│   ├── Why you believe it was unjust`

`│   ├── Any new information or context`

`│   └── What outcome you're seeking`

`├── Submit within 7 days of receiving the ban notification`

`│   ├── After 7 days: ban becomes permanent, no appeal accepted`

`│   └── The 7-day window was clearly communicated in the ban`

`│       notification — ignorance of the deadline is not grounds`

`│       for late appeal`

`└── Be respectful in your appeal — an aggressive or abusive`

`    appeal will be denied and may eliminate any chance of`

`    favorable review`
```

Appeal review:

```
`├── Reviewed by BDFL (or a designated moderator not involved in`

`│   the original decision)`

`├── Factors considered:`

`│   ├── Was the code of conduct applied correctly?`

`│   ├── Was the process followed?`

`│   ├── Is there new information that changes the situation?`

`│   ├── Has the person demonstrated understanding of why their`

`│   │   behavior was problematic?`

`│   ├── Is there a pattern or was this an isolated incident?`

`│   ├── Will this person be a positive contributor going forward?`

`│   └── Will lifting the ban harm the community (especially`

`│       beginners)?`

`├── Outcomes:`

`│   ├── Granted → 30-day probation (see "Appeal — Granted")`

`│   ├── Denied → Permanent ban`

`│   └── (There is no "reduced" — the model is binary: back on`

`│       probation, or permanently banned)`

`├── Appeal decisions are final`

`└── No further appeals on the same incident`
```

Appeal timeline:

```
`├── Acknowledged within 72 hours`

`├── Decision within the 7-day window (or before, if possible)`

`├── If the decision takes longer than 7 days:`

`│   ├── The ban is held in "suspended" state until the decision`

`│   │   (not converted to permanent)`

`│   ├── The person is informed of the delay`

`│   └── The 7-day window for the person was met — the delay is`

`│       on the moderator side, not the person's`

`└── If the person does not file an appeal within 7 days and`

`    later claims they "didn't see" the notification:`

`    ├── This is not grounds for late appeal`

`    ├── The notification system delivery is sufficient`

`    └── It is the person's responsibility to check their email`

`        associated with their account`
```

## Community Guidelines In Detail

### Technical Disagreements

Disagreement about technical decisions is healthy and expected. Spike makes many debatable choices (Ubuntu base, Flatpak-only, Qt6, BDFL governance). Reasonable people can disagree.

Rules for technical disagreement:

```
`├── Attack the idea, not the person`

`│   ├── "I think Flatpak's sandboxing model has limitations because..."`

`│   └── NOT "Only an idiot would choose Flatpak"`

`├── Provide reasoning, not just opinion`

`│   ├── "Snap has better CLI integration for my use case because..."`

`│   └── NOT "Snap is better"`

`├── Acknowledge trade-offs`

`│   ├── Every technical choice has downsides — pretending otherwise`

`│   │   is not productive`

`│   └── "I understand the simplicity argument, but..."`

`├── Respect that some decisions are made`

`│   ├── Structural constraints (Golden Rules, zero telemetry,`

`│   │   old-hardware focus) are not negotiable`

`│   ├── These exist by design, not oversight`

`│   ├── If you fundamentally disagree with the project's direction,`

`│   │   Spike may not be the right project for you`

`│   └── Forking is always an option (GPLv2+ permits it)`

`├── Know when to stop`

`│   ├── If a decision has been made and explained: accept it`

`│   ├── Continuing to relitigate settled decisions is disruptive`

`│   └── If you have NEW information: present it once, then accept`

`│       the outcome`

`└── Remember: the BDFL has final say on design decisions`

`    (see GOVERNANCE.md)`
```

### Conflict Between Community Members

When two community members disagree (not about code, but personally):

```
`├── Take it private: if a thread is becoming personal, move to`

`│   private messages`

`├── Mediation: if both parties want help resolving it, a moderator`

`│   can mediate (privately)`

`├── Disengage: if you can't resolve it, disengage. You are not`

`│   obligated to interact with everyone`

`├── Don't recruit: don't try to get others to "take your side"`

`│   in a personal dispute`

`├── Don't carry grudges across threads: what happened in one`

`│   discussion stays in that discussion`

`└── If a pattern of targeted hostility develops: report it`

`    (that's harassment, not disagreement)`
```

Between contributors:

```
`├── Code review disagreements are about code quality, not personal worth`

`├── If a reviewer rejects your MR: ask for specifics, don't get defensive`

`├── If you're reviewing: be specific and constructive`

`│   ├── "This function could be simplified by extracting X" (helpful)`

`│   └── "This is overcomplicated" (not helpful)`

`├── Multiple reviewers can disagree: the maintainer decides`

`└── A rejected MR is not a rejection of you as a person`
```

### Legal Compliance

Spike's operations are based in Australia and subject to Australian law.

Illegal content (under Australian law):

```
`├── Content promoting or inciting violence`

`├── Child abuse material (CSAM) — including advocacy thereof`

`├── Content using a carriage service to menace, harass, or`

`│   offend (Criminal Code Act 1995 (Cth), s474.17)`

`├── Hate speech meeting thresholds under the Racial`

`│   Discrimination Act 1975 (Cth) (s18C)`

`├── Incitement to violence under state criminal codes`

`├── Copyright infringement (Copyright Act 1968 (Cth))`

`├── Defamatory content meeting Australian defamation law thresholds`

`│   (Defamation Act 2005, as amended)`

`├── Content prohibited by the Online Safety Act 2021 (Cth)`

`│   (eSafety Commissioner jurisdiction)`

`└── Other illegal content per applicable Australian law`
```

What this means in practice:

```
`├── Illegal content is removed immediately when discovered`

`├── Accounts posting illegal content are permanently banned`

`│   (severe violation — no warning)`

`├── Reports of illegal content are handled with highest priority`

`├── The eSafety Commissioner's directives are respected if issued`

`├── Law enforcement requests are handled per Australian legal process`

`├── Spike does NOT actively monitor all content (not feasible)`

`├── Spike DOES act on all reports`

`└── Moderators are not legal professionals — when in doubt, they`

`    err on the side of removing potentially illegal content and`

`    consulting the BDFL`
```

Copyright complaints:

```
`├── If you believe your copyrighted work has been used without`

`│   permission in the Spike project:`

`│   ├── Contact: conduct@bigrangatech.com (or legal@bigrangatech.com)`

`│   ├── Provide: description of work, proof of ownership, location`

`│   │   in Spike project`

`│   ├── Spike will review and respond within 14 days`

`│   └── If valid: infringing content removed, contributor notified`

`├── Spike takes intellectual property seriously`

`├── All contributions require DCO (affirming you have the right to`

`│   contribute the code — see CONTRIBUTING.md)`

`└── Repeated copyright violations result in permanent ban`
```

## Scope Of This Document

This code of conduct is adapted from the Contributor Covenant (version 2.1) with modifications specific to Spike Linux.

Adaptations include:

```
`├── Strict enforcement model (one warning, then ban)`

`├── Extended beginner protection (reflecting Spike's target audience)`

`├── Detailed enforcement flow with appeals and probation`

`├── Australian legal compliance section`

`├── Technical disagreement guidelines`

`├── Inclusive language guidance specific to Spike's terminology`

`└── Community-specific structure (forum, GitLab, chat)`
```

This document may be updated:

```
`├── As the community grows and evolves`

`├── In response to incidents that reveal gaps`

`├── By the BDFL, with community input`

`├── Changes are announced on the forum (Announcements category)`

`├── Material changes (not editorial) require 14-day comment period`

`│   before taking effect`

`└── The BDFL has final authority on all code of conduct matters`
```

Version history:

```
`├── v1.0 — Initial code of conduct (pre-alpha)`

`└── (future versions documented here)`
```

### Attribution

This code of conduct adapts the Contributor Covenant 2.1 ([https://www.contributor-covenant.org/version/2/1/code\_of\_conduct/](https://www.contributor-covenant.org/version/2/1/code_of_conduct/)) with substantial modifications for the Spike Linux community.

Contributor Covenant is licensed under CC BY 4.0. This adaptation is licensed under CC BY-SA 4.0.

Spike Linux code of conduct: GPLv2+ (same as the project).

## What This Document Does Not Cover

- Contribution process (DCO, merge requests, code review): See `CONTRIBUTING.md` 

- Governance and decision-making authority: See `GOVERNANCE.md` 

- Bug reporting process (technical, not behavioral): See `SUPPORT.md` 

- Forum structure and categories: See `SUPPORT.md` 

- Privacy policy (data collection, account data): See `PRIVACY.md` 

- Security vulnerability disclosure (separate from CoC): See `SECURITY.md` 

- Trademark usage policy: See `GOVERNANCE.md` 

- License details: See `LICENSE` (to be written) 

- Project philosophy and values: See `PHILOSOPHY.md` 

- Roadmap and project status: See `ROADMAP.md` 

🐕 BigRangaTech


