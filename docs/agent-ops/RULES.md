# Spike — Core Workflow Rules

## Purpose

These are the **non-negotiable workflow rules** that govern how AI agents (and humans) develop Spike Linux. Violating these rules breaks the documentation-first methodology that makes this project maintainable long-term.

> **Golden Rule:** If you're unsure whether something violates these rules, ASK before proceeding. One question is cheaper than one revert.


## 1. Documentation-First Mandate (Priority #1)

**Before writing ANY implementation code**, verify a specification exists for the feature you're building.

### Verification Steps

1. **Check `INDEX.md`** — Confirm the target spec document exists (e.g., `MEMORY.md` for memory module) 

2. **Check the relevant section** — Find the specific section describing the feature (e.g., `MEMORY.md §3.2 ZRAM setup`) 

3. **Verify completeness** — Ensure the section has concrete values, not placeholders like `[TBD]` or `[TODO]` 

4. **If anything fails:** STOP and follow the **Missing Spec Protocol** below 

### What "Documentation" Means

| **Type** | **Where It Lives** | **Status Required** |
| :-: | :-: | :-: |
| **Spec document** | Root directory (`MEMORY.md`, `CONFIGURATION.md`, etc.) | ✅ Complete (approved) or 📝 Drafted (pending review) |
| **Spec section** | Within the spec document | Must contain concrete values, not placeholders |
| **Operation rule** | This folder (`agent-ops/RULES.md`) | Always active (these rules themselves) |


## 2. Missing Spec Protocol

### Scenario A: Spec Document Does Not Exist

> *Request: "Implement Bluetooth audio codec negotiation"*  
*Reality: No doc exists for this feature yet*

**Steps:**

1. **STOP immediately.** Do not write code. 

2. **Check `INDEX.md`** — Verify if the doc is planned but not yet created 

3. **Ask human:** 

> "No spec exists for [FEATURE]. I'll need to create [DOC-NAME.md]. Should I proceed with drafting?"

4. **Upon approval:** 

   - Create the new `.md` file using the **Spec Doc Template** in `TEMPLATES.md` 

   - Cross-reference from `INDEX.md` (add to topic index) 

   - Add to dependency map in `INDEX.md` if applicable 

   - Mark all sections as **[DRAFT]** with timestamp 

   - Update `STATE.md` as "📝 Drafted, pending review" 

5. **WAIT for human approval** — Do not proceed until approved 

6. **Update `INDEX.md` status:** 📝 Drafted → ✅ Complete (after approval) 

7. **THEN implement** per the newly approved spec 

### Scenario B: Spec Exists But Section Is Incomplete

> *Request: "Implement ZRAM setup"*  
*Reality: `MEMORY.md` exists but §3.2 says "ZRAM to be specified"*

**Steps:**

1. **Identify which section** is missing/incomplete (e.g., `MEMORY.md §3.2`) 

2. **Ask human:** 

> "`MEMORY.md` exists but §3.2 (ZRAM setup) is incomplete. Shall I draft that section for approval?"

3. **Draft the missing section** with concrete values (not placeholders) 

4. **Mark as [DRAFT]** with timestamp 

5. **Wait for human approval** 

6. **Once approved:** Implement per the approved section 

7. **Remove [DRAFT]** tag after implementation 

### Scenario C: Spec Has Placeholders

> *Request: "Implement atomic config writes"*  
*Reality: `CONFIGURATION.md` says "write pattern: [INSERT METHOD]"*

**Steps:**

1. **Identify the placeholder** and the correct value (e.g., from `PATTERNS.md` or `CONSTRAINTS.md`) 

2. **Ask human:** 

> "`CONFIGURATION.md` §6.1 has placeholder '[INSERT METHOD]'. Based on `PATTERNS.md`, the pattern is tmp→fsync→rename→fsync(dir). Should I update the spec?"

3. **Upon approval:** Update the spec to replace the placeholder 

4. **Mark change as [IMPLEMENTATION NOTE]** with date 

5. **THEN implement** per the corrected spec 


## 3. Approval Gates

**Never implement without explicit approval** when any of these conditions apply:

| **Situation** | **Action** |
| :-: | :-: |
| New spec document being created | Ask for approval before drafting |
| Missing section in existing spec | Ask for approval before drafting |
| Placeholder in spec needs filling | Ask for approval before updating |
| Implementation discovery contradicts spec | Ask for approval before changing spec |
| Two specs contradict each other | STOP and ask for resolution |
| Constraint violation detected | STOP and notify immediately |

**Approval Format:**

Human must respond with explicit language like:

- ✅ "Approved" 

- ✅ "Go ahead" 

- ✅ "Yes, draft it" 

- ✅ "Proceed" 

Silence ≠ approval. Ambiguity ≠ approval. "Sounds good" is okay only if it clearly refers to the specific draft.


## 4. State Tracking Mandate

**At the start of EVERY task:**

- Read `agent-ops/STATE.md` to see current progress 

- Check `SESSION_LOG.md` for what happened last session 

- Check `DECISIONS.md` for recent decisions affecting your work 

- Confirm with human: "Continuing [MODULE] from last session. Last state was [STATE]. Proceed?" 

**At the end of EVERY task:**

- Update `STATE.md` with current progress 

- Append to `SESSION_LOG.md` with summary 

- Update `INDEX.md` if any docs were modified 

- Commit all changes (code + spec + state files together) 

- Notify human: "Task complete per [SPEC.md]. Ready for review." 


## 5. No Silent Deviation Rule

**Never silently deviate from the spec.** Even minor deviations require documentation.

### What Constitutes Deviation?

| **Deviation Type** | **Example** | **Required Action** |
| :-: | :-: | :-: |
| **Value change** | Spec says "priority 100", you implement "priority 95" | Update spec, log in `SESSION_LOG.md` |
| **Behavior change** | Spec says "always enabled", you add a conditional | Update spec, flag for review |
| **Omission** | Spec section exists, you skip it entirely | STOP, notify human |
| **Addition** | You add a feature not in the spec | STOP, ask for approval |
| **Timing change** | Spec says "run at boot", you run "every 6 hours" | Update spec, log change |

**Exception:** Minor wording/phrasing in comments does not constitute deviation. Code logic changes do.


## 6. Small Changes Rule

**There is NO such thing as "too small to document."**

| **Change Size** | **Documentation Required?** |
| :-: | :-: |
| One-line code fix | Yes — log in `SESSION_LOG.md` |
| Comment update | Yes — if it changes semantics |
| Variable rename | Yes — update spec if it affects interface |
| Typo fix in spec | Yes — mark as correction with date |
| Whitespace/formatting | No — unless it changes behavior |

**Commit Message Requirement:** Every commit must cite the spec doc updated.

```
[memory] Implement ZRAM setup per MEMORY.md §3.2
[config] Add atomic write pattern per CONFIGURATION.md §6.1
[docs] Update MEMORY.md with implementation discovery §3.2
```


## 7. Optimization Ban

**Do not optimize beyond spec.** If you see a potential improvement:

1. **DO NOT implement it automatically** 

2. **Suggest it as a comment** in the code: 

```
# POTENTIAL OPTIMIZATION: zstd compression is slower than lz4 on low-end CPUs.
# MEMORY.md specifies zstd for better ratios. Consider benchmarking before changing.
```

3. **Notify human in `SESSION_LOG.md`:** 

> "Discovery: lz4 might be faster than zstd on Celeron N4020. MEMORY.md specifies zstd. Proposed optimization logged for review."

**Human Decision Required Before Implementation:** Never assume "better = allowed."


## 8. Spec Authority Principle

**If code and spec disagree, the spec is authoritative until human decides otherwise.**

### Resolution Process

1. **Detect disagreement:** "Code says X, spec says Y" 

2. **STOP implementing** until resolved 

3. **Flag to human:** 

> "Discrepancy found: [FILE] implements [X], but [SPEC.md] §[N] specifies [Y]."

4. **Wait for human decision:** 

   - Option A: Update code to match spec 

   - Option B: Update spec to match code (with rationale) 

   - Option C: Create compromise and update both 

5. **Implement per human decision** 

6. **Log resolution in `DECISIONS.md`** 


## 9. Dependency Chain Rule

**Never start a module that has unverified dependencies.**

### How To Check

1. **Read `INDEX.md` dependency map** — Find your module's dependencies 

2. **Check `STATE.md`** — Verify all dependencies are marked "✅ Complete" 

3. **If any dependency is "🟡 In Progress" or "🔲 Not Started":** 

   - Ask human: "Can't start [MODULE] yet. [DEPENDENCY] is still [STATUS]. Proceed?" 

4. **Upon explicit override:** You may proceed, but note risk in `SESSION_LOG.md` 

### Example

```
Module: power
Depends on: hardware (detection), variant (selection)
State:
├── hardware: ✅ Complete
└── variant: 🟡 In Progress
Action: STOP. Power module cannot start until variant is complete.
```


## 10. Session Start Protocol

**Before starting any coding session:**

1. **Read `STATE.md`** — Current progress, blockers 

2. **Read `SESSION_LOG.md`** — What happened last session 

3. **Read `DECISIONS.md`** — Recent decisions affecting current work 

4. **Confirm with human:** "Continuing [MODULE] from [DATE]. Last state: [STATUS]. Ready to proceed?" 

**Never begin coding without confirming current context.**


## 11. Session End Protocol

**After completing any task:**

1. **Run `SELF-CHECK.md`** — Pre-completion checklist 

2. **Update `STATE.md`:** 

   - Mark module as "✅ Complete" or "🟡 In Progress" 

   - Log completion date 

3. **Append to `SESSION_LOG.md`:** 

   - Focus area 

   - Tasks attempted + results 

   - Spec changes made 

   - Code changes made 

   - Discoveries 

   - Open questions 

4. **Update `INDEX.md`** if any specs were modified 

5. **Commit all changes together:** 

```
git commit -m "[module] Implement per [SPEC.md] §[N]"
git push origin main
```

6. **Notify human:** "Task complete. State updated. Ready for review." 


## 12. Conflict Escalation

When in doubt, escalate rather than guess.

| **Uncertainty Level** | **Action** |
| :-: | :-: |
| "I'm not sure which spec to read" | Check `INDEX.md` topic index, then ask if unclear |
| "Two specs contradict each other" | STOP, escalate to human immediately |
| "Spec is ambiguous" | Draft interpretation, ask for approval |
| "Constraint is unclear" | Check `CONSTRAINTS.md`, then ask if still unclear |
| "I think there's a mistake in the spec" | Flag respectfully, ask for confirmation |
| "I don't have enough context to proceed" | STOP, request additional context from human |


## 13. Emergency Override

**Only the human (BDFL) can override these rules.**

If an emergency requires breaking a rule:

1. **Document the exception** in `SESSION_LOG.md` (timestamp, reason, scope) 

2. **Get explicit human authorization** before proceeding 

3. **Plan a fix-up commit** to restore rule compliance ASAP 

4. **Log in `DECISIONS.md`** as "Emergency override" with resolution path 

**No agent can authorize rule overrides on their own.**


## Quick Reference

| **Question** | **File to Consult** |
| :-: | :-: |
| "What's the workflow?" | This file (`RULES.md`) |
| "What can I never do?" | `CONSTRAINTS.md` |
| "How do I do X?" | `PROTOCOLS.md` |
| "How do I write Y code?" | `PATTERNS.md` |
| "Is this done?" | `SELF-CHECK.md` |
| "What's happening now?" | `STATE.md` |
| "What did we decide?" | `DECISIONS.md` |


🐕 BigRangaTech


