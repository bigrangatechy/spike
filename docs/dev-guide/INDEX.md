# Spike Dev Guide — Index

Living index for `docs/dev-guide/`. Status matches `docs/INDEX.md` (update both when a page moves from stub → draft).

| Status | Doc | Notes |
| :-: | :-: | :-: |
| 📝 | `01-getting-started.md` | Clone → package → ISO loop |
| 📝 | `02-repo-structure.md` | Monorepo map (`src/`, scripts, iso-build) |
| 📝 | `03-build-environment.md` | Host deps |
| 📝 | `04-building-spike.md` | live-build / `build-iso.sh` |
| 📝 | `05-building-components.md` | Per-package `.deb` scripts |
| 📝 | `06-spike-shell-architecture.md` | Shell overview + Spike Tools launcher |
| 📝 | `07-installer-internals.md` | Custom Qt installer + SpikeBackup hooks |
| 📝 | `08-rescue-tool-internals.md` | Rescue recover/restore + helper |
| 📝 | `09-spike-config-internals.md` | Config engine sketch |
| 🔲 | `10-branding-and-theming.md` | Fill as branding ships |
| 🔲 | `11-testing.md` | Fill with CI / hardware matrix |
| 📝 | `12-debugging.md` | USB capture + rescue REPORT |
| 🔲 | `13-git-workflow.md` | Point at CONTRIBUTING until filled |
| 🔲 | `14-contribution-phases.md` | Point at ROADMAP / GOVERNANCE |
| 🔲 | `15-coding-conventions.md` | Use `agent-ops/CONVENTIONS.md` for now |
| 🔲 | `16-translation-workflow.md` | After TRANSLATIONS.md |
| 🔲 | `17-release-process.md` | After signing/publish exists |
| 🔲 | `18-accessibility-development.md` | After a11y prefs land |
| 📝 | `19-appendix-reference.md` | Quick package / ops pointers |

When you implement a component, update its internals page **in the same change** (or the next session) so this guide stays usable.
