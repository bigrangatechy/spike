# Spike Branding

## Purpose

This document defines the complete visual identity of Spike Linux. Every color, font, logo variant, icon, wallpaper, animation, and tone of voice is specified here. If it is visible to the user, it is defined in this document.

Branding is not decoration. Branding is trust. A consistent visual identity tells the user "this is Spike, and Spike is taking care of you." Inconsistent branding — mismatched colors, varying fonts, ad-hoc icons — signals instability. On an operating system for non-technical users, perceived instability erodes confidence in the entire system.

Every contributor, every theme file, every asset, and every pixel of UI must conform to this document. The BDFL is the final arbiter of branding decisions.

## Brand Identity

### What Spike Is

Spike is:

```
├── A Linux distribution for old laptops
├── Named after a dog — loyal, dependable, approachable
├── Built for people, not enthusiasts
├── Free and open source (GPLv2+)
├── Made by BigRangaTech
└── Tagline (boot): "Let's make tech repairable again"
```

Spike is NOT:

```
├── Corporate or sterile
├── Playful or cartoonish (despite the dog mascot)
├── Minimalist for the sake of trendiness
├── Retro or skeuomorphic
├── Aggressively technical (no terminal aesthetics)
└── A clone of any existing OS brand
```

### Brand Personality

Personality traits (design decisions evaluated against these):

```
├── Dependable: Nothing flashy that compromises reliability
├── Warm: Not cold or clinical — this is a companion, not a tool
├── Calm: No aggressive colors, no jarring transitions, no alarmist UI
├── Clear: Every visual element communicates its purpose immediately
├── Honest: No fake depth, no illusory affordances, no decorative chrome
└── Unpretentious: Doesn't try to look expensive — looks purposeful
```

### Tone of Voice

Tone applies to all user-facing text:

```
├── UI labels, settings, notifications, dialogs
├── Documentation (user guide, FAQ, troubleshooting)
├── Website copy
├── Release announcements
└── Community communications
```

Rules:

```
├── Plain language — no jargon, no acronyms without explanation
├── Active voice — "Spike installed updates" not "Updates were installed"
├── Concise — say what needs to be said, no more
├── Friendly, not casual — "Hello" not "Hey there!" / "Greetings, user."
├── No exclamation marks in system messages (calm, not excited)
├── No humor in error messages (errors are stressful; humor feels dismissive)
├── No blaming the user — "Password incorrect" not "You entered the wrong password"
├── Specific, not vague — "3 files selected" not "Several files selected"
└── Consistent terminology across all surfaces:
    ├── "Shut Down" (not "Power Off", "Turn Off", "Shutdown")
    ├── "Restart" (not "Reboot", "Restart Computer")
    ├── "Settings" (not "Preferences", "Configuration", "Control Panel")
    ├── "Install" (not "Download and install", "Get")
    ├── "Updates" (not "Upgrades", "Patches")
    └── "Files" (not "Documents" when referring to the file manager generally)
```

## Color Palette

### Primary Colors

| **Color Name** | **Hex** | **RGB** | **Usage** |
| :-: | :-: | :-: | :-: |
| **SPIKE PURPLE** | #6d4aff | 109, 74, 255 | Primary brand color, buttons, selections, highlights |
| **SPIKE CYAN** | #00bcd4 | 0, 188, 212 | Accent color, borders, secondary highlights, links |

### Surface Colors

| **Color Name** | **Hex** | **RGB** | **Usage** |
| :-: | :-: | :-: | :-: |
| **DARK BASE** | #1a1a2e | 26, 26, 46 | Backgrounds (GRUB, SDDM, Plymouth, wallpapers) |
| **PANEL SURFACE** | #222236 | 34, 34, 54 | Desktop panel background, applet popouts |
| **CARD SURFACE** | #2a2a4a | 42, 42, 74 | Dialog backgrounds, settings cards, notification cards |
| **ELEVATED SURFACE** | #33335a | 51, 51, 90 | Hovered items, active tabs, dropdown menus |

### Text Colors

| **Color Name** | **Hex** | **RGB** | **Usage** |
| :-: | :-: | :-: | :-: |
| **PRIMARY TEXT** | #e0e0e0 | 224, 224, 224 | Body text, labels, titles |
| **SECONDARY TEXT** | #a0a0b8 | 160, 160, 184 | Descriptions, hints, placeholder text, subtitles |
| **DISABLED TEXT** | #606078 | 96, 96, 120 | Disabled controls, locked settings, greyed items |

### Semantic Colors

| **Color Name** | **Hex** | **RGB** | **Usage** |
| :-: | :-: | :-: | :-: |
| **SUCCESS / OK** | #4caf50 | 76, 175, 80 | "Up to date", "Connected", "Installed", green status |
| **WARNING** | #ff9800 | 255, 152, 0 | "Maintenance phase", "Battery low", caution banners |
| **ERROR / CRITICAL** | #f44336 | 244, 67, 54 | "Update failed", "Disk full", "Error", red status |
| **INFO** | #2196f3 | 33, 150, 243 | Informational banners, tips, neutral notifications |

### Color Usage Rules

1. **Spike Purple is the ONLY color for primary actions**

   - Primary buttons: purple background, white text 

   - Selected items: purple highlight 

   - Focus indicators: purple border/ring 

   - Never use purple for error/warning/success states 

2. **Spike Cyan is the ONLY accent color**

   - Thin borders, underlines, decorative accents 

   - Links in text (cyan, underlined) 

   - Secondary buttons: cyan outline 

   - Never use cyan for primary actions or status indicators 

3. **Semantic colors are used ONLY for their semantic meaning**

   - Green = success/ok — never decorative 

   - Orange = warning — never decorative 

   - Red = error/critical — never decorative 

   - Blue = info — never decorative 

   - These colors never appear in branding, logos, or themes 

4. **Surface colors form a strict hierarchy**

   - Dark Base (darkest) → backgrounds 

   - Panel Surface → panel, popouts 

   - Card Surface → dialogs, cards, notifications 

   - Elevated Surface (lightest dark) → hover, active, menus 

5. **Text on surfaces**

   - Primary text on Dark Base/Panel/Card/Elevated: #e0e0e0 

   - Secondary text on same surfaces: #a0a0b8 

   - Text on Spike Purple buttons: white (#ffffff) 

   - Text on Spike Cyan accents: white (#ffffff) or Dark Base (#1a1a2e) 

   - Text on semantic status: white (#ffffff) 

### High-Contrast Override

When High Contrast accessibility mode is enabled, the entire color palette is overridden:

```
High-contrast palette:
├── Background: #000000 (pure black)
├── Foreground: #ffffff (pure white)
├── Selection: #ffff00 (pure yellow) on black
├── Borders: #ffffff, 3px minimum thickness
├── Links: #00ffff (bright cyan), underlined
├── Error: #ff0000 (pure red)
├── Warning: #ffff00 (pure yellow)
├── Success: #00ff00 (pure green)
└── No gradients, no opacity, no translucency
```

See ACCESSIBILITY.md for high-contrast implementation details.

## Typography

### Primary Font

| **Property** | **Value** |
| :-: | :-: |
| **Font family** | Noto Sans |
| **Foundry** | Google (open source, SIL Open Font License) |
| **Usage** | All UI text, documentation, website, installer, settings, panel, launcher, notifications, dialogs |

**Weights used:**

```
├── Regular (400): Body text, labels, descriptions
├── Medium (500): Section headers, tab labels, applet names
├── Bold (700): Titles, button text, important labels
└── (No light, extralight, or black weights)
```

**Why Noto Sans:**

```
├── Covers all 11 supported languages (single font family)
├── Designed for screen readability (not print)
├── Open source (no licensing cost)
├── Available in Qt6 font system
├── Renders well at small sizes (important for panel text)
└── Professional appearance (not decorative, not quirky)
```

### Monospace Font

| **Property** | **Value** |
| :-: | :-: |
| **Font family** | Noto Sans Mono |
| **Usage** | Terminal (Konsole), code blocks in documentation, system logs, diagnostic output |

**Weights used:**

```
├── Regular (400): Terminal output, log text
└── Bold (700): Terminal bold (commands, keywords)
```

### Font Sizes

| **Size Label** | **px** | **Usage** |
| :-: | :-: | :-: |
| Caption | 11px | Tooltips, secondary metadata, timestamps |
| Body | 13px | Default text, labels, descriptions, notifications |
| Body Large | 14px | Settings descriptions, dialog body text |
| Subtitle | 15px | Section headers in settings, card titles |
| Title | 18px | Dialog titles, page headers in settings |
| Title Large | 22px | Window titles, installer step titles |
| Display | 28px | Welcome screen, large notifications, splash text |

**Font scaling (accessibility):**

```
├── Normal: 1.0x (sizes above)
├── Large: 1.25x (all sizes multiplied by 1.25)
├── Extra Large: 1.5x
├── Huge: 2.0x
└── See ACCESSIBILITY.md for large text implementation
```

### Font Rules

1. **Noto Sans is the ONLY sans-serif font in Spike**

   - No alternative UI fonts shipped 

   - No font selection in Settings (consistency over choice) 

   - Applications may bundle their own fonts (Firefox, LibreOffice) but system UI uses Noto Sans exclusively 

2. **No decorative fonts anywhere in system UI**

   - No handwriting fonts, no display fonts, no script fonts 

   - The Spike logo is a graphic, not text (see Logo section) 

3. **Font rendering**

   - Anti-aliasing: subpixel RGB (default) 

   - Hinting: slight (best balance of sharpness and smoothness) 

   - DPI: 96 (default, scaled for large text accessibility) 

   - Configured by spike-config at install time 

4. **Line height**

   - Body text: 1.4x font size (comfortable reading) 

   - Titles: 1.2x font size (tighter, more compact) 

   - UI labels: 1.0x font size (single line, no wrapping expected) 

## Logo

### The Spike Dog

Spike's mascot is a dog named Spike. The logo is a stylized illustration of this dog.

**Character description:**

```
├── Breed: Mixed breed (not a specific purebred)
├── Build: Stocky, sturdy, medium-sized
├── Expression: Calm, alert, friendly (not aggressive, not goofy)
├── Ears: Pointed, semi-erect (alert but relaxed)
├── Eyes: Wide, dark, friendly
├── Mouth: Closed or slight smile (never open/barking)
├── Posture: Sitting upright, facing forward (head-on)
├── Tail: Curled or resting (not wagging in logo — static)
└── Collar: Thin collar with a small circular tag
```

**Art style:**

```
├── Flat 2D vector illustration (no 3D, no realistic rendering)
├── Clean shapes, minimal detail (recognizable at small sizes)
├── Solid fills (no gradients on the dog itself)
├── Purple (#6d4aff) as primary fur color
├── Cyan (#00bcd4) accents (collar tag, inner ears)
├── White or light gray for eyes and chest marking
├── Dark Base (#1a1a2e) for outline strokes (thin, 2px)
└── Designed to work on both dark and light backgrounds
    (dark background version: solid colors as described)
    (light background version: dark outline added for contrast)
```

### Logo Variants

**Variant 1: FULL LOGO (dog + wordmark)**

```
├── Dog illustration on the left
├── "Spike" text on the right (Noto Sans Bold)
├── Dog and text vertically centered relative to each other
└── Used on: website header, installer splash, documentation header, release announcements
```

**Variant 2: EMBLEM (dog only, no text)**

```
├── Dog illustration only, no wordmark
└── Used on: GRUB menu (top center), Plymouth splash (center), SDDM login screen (center), app icon, favicon, panel launcher button, ISO file icon
```

**Variant 3: WORDMARK (text only, no dog)**

```
├── "Spike" text only (Noto Sans Bold)
└── Used on: boot text (where graphics unavailable), CLI tools, documentation footers, package names
```

**Variant 4: MONOCHROME (single color)**

```
├── Dog silhouette in a single color
└── Used on: etched/engraved surfaces, single-color print, GRUB text mode (if needed)
```

### Logo Sizing And Clear Space

**Clear space (minimum margin around logo):**

```
├── The height of the dog's ear defines the clear space unit
├── No other element may enter this margin
└── Clear space applies to ALL variants
```

**Minimum sizes:**

| **Variant** | **Digital** | **Print** |
| :-: | :-: | :-: |
| Full logo | 120px wide | 3cm wide |
| Emblem | 24px | 0.8cm |
| Wordmark | 48px wide | 1.5cm |
| Monochrome | 16px | 0.5cm |

### Logo File Inventory

```
/usr/share/spike/branding/logo/
├── spike-logo-full.svg              → Full logo (dog + wordmark), dark bg
├── spike-logo-full-light.svg        → Full logo, light bg variant
├── spike-emblem.svg                 → Emblem (dog only), dark bg
├── spike-emblem-light.svg           → Emblem, light bg variant
├── spike-wordmark.svg               → Wordmark (text only), dark bg
├── spike-wordmark-light.svg         → Wordmark, light bg variant
├── spike-monochrome-white.svg       → Monochrome, white
├── spike-monochrome-black.svg       → Monochrome, black
├── spike-emblem-16.png              → Emblem, 16px raster
├── spike-emblem-24.png              → Emblem, 24px raster
├── spike-emblem-32.png              → Emblem, 32px raster
├── spike-emblem-48.png              → Emblem, 48px raster
├── spike-emblem-64.png              → Emblem, 64px raster
├── spike-emblem-128.png             → Emblem, 128px raster
├── spike-emblem-256.png             → Emblem, 256px raster
└── spike-emblem-512.png             → Emblem, 512px raster
```

### BigRangaTech Logo

BigRangaTech is the project's parent entity. Its logo is separate from Spike's.

**BigRangaTech logo:**

```
├── Wordmark only: "bigRangaTech" (camelCase, Noto Sans Medium)
├── Color: #e0e0e0 on dark surfaces, #1a1a2e on light surfaces
├── No icon/emblem (text-only brand)
└── Used on: documentation footer (🐕 BigRangaTech), website footer, about page, copyright notices
```

**Placement:**

```
├── Documentation: bottom of every document (🐕 BigRangaTech)
├── Website: footer (small, unobtrusive)
├── Settings → About: "Made by BigRangaTech" (link to website)
├── Installer: final screen ("Thank you for choosing Spike")
└── Never appears on: login screen, panel, desktop, launcher
    (Spike is the brand the user sees, not BigRangaTech)
```

## Motifs And Patterns

### Circuit Trace Pattern

The circuit trace pattern is Spike's signature visual motif. It appears on dark backgrounds and gives Spike a distinctive, technical-but-approachable identity.

**Circuit trace pattern:**

```
├── Thin lines (1-2px) in cyan (#00bcd4) at low opacity (15-25%)
├── Lines form circuit-board-like patterns:
│   ├── Straight horizontal and vertical segments
│   ├── 90-degree corners (no diagonals)
│   ├── Small circular "nodes" at intersections (2-3px diameter)
│   └── Occasional small rectangular "pads" (4x4px)
├── Distribution: sparse, not dense (background texture, not foreground)
├── Glow: very subtle (blur 1px, opacity 20%)
└── Never animated (static pattern only)
```

**Where it appears:**

```
├── GRUB background (most visible — user sees it when GRUB menu shows)
├── SDDM login background
├── Plymouth background (behind logo)
├── Installer background
├── Default wallpaper (as a layer)
├── Website header/footer background
└── Documentation PDF cover page
```

**Where it does NOT appear:**

```
├── Desktop panel (solid Panel Surface color)
├── Application windows (solid Card Surface)
├── Settings pages (solid Card Surface)
├── Notification cards (solid Card Surface)
└── Any surface where text readability is critical
```

**Design rationale:**

```
├── Circuit traces = technology, precision, engineering
├── Low opacity = subtle, not distracting
├── Cyan color = brand accent, visible on dark base
├── Static = calm, not busy
└── The pattern says "this was designed" without screaming "look at me"
```

### Geometric Language

**Corner radius:**

| **Element** | **Radius** |
| :-: | :-: |
| Panels and popouts | 8px |
| Cards and dialogs | 12px |
| Buttons | 6px |
| Applet icons | 4px (slight rounding) |
| Input fields | 6px |
| Notification cards | 8px |
| Thumbnails (file previews) | 4px |
| Windows (KWin decorations) | 0px (sharp corners for windows) |

**Borders:**

| **Border Type** | **Style** |
| :-: | :-: |
| Standard | 1px, Elevated Surface color (#33335a) |
| Focus | 2px, Spike Purple (#6d4aff) |
| Error | 2px, Error Red (#f44336) |
| Accent dividers | 1px, cyan (#00bcd4) at 30% opacity |

No drop shadows on UI elements (flat design language). Exception: floating panels and popouts have a subtle dark shadow (0px offset, 4px blur, #000000 at 40% opacity) to create depth from the desktop background.

## Components

### Buttons

**Primary button:**

| **Property** | **Value** |
| :-: | :-: |
| Background | Spike Purple (#6d4aff) |
| Text | White (#ffffff), Noto Sans Bold |
| Padding | 8px 16px (vertical horizontal) |
| Border radius | 6px |
| Border | none |
| Hover | lighten purple by 10% (#7d5aff) |
| Active (pressed) | darken purple by 10% (#5d3aff) |
| Disabled | gray (#606078), text #a0a0b8 |
| Example | "Install", "Apply", "Log In" |

**Secondary button:**

| **Property** | **Value** |
| :-: | :-: |
| Background | transparent |
| Text | Primary Text (#e0e0e0), Noto Sans Regular |
| Padding | 8px 16px |
| Border radius | 6px |
| Border | 1px, cyan (#00bcd4) at 50% opacity |
| Hover | Elevated Surface background (#33335a) |
| Active (pressed) | Card Surface background (#2a2a4a) |
| Disabled | gray border, text #606078 |
| Example | "Cancel", "Later", "Not Now" |

**Text button (tertiary):**

| **Property** | **Value** |
| :-: | :-: |
| Background | transparent |
| Text | Spike Cyan (#00bcd4), Noto Sans Regular, underlined |
| Padding | 4px 8px |
| Border | none |
| Hover | text lightens to #4dd0e1 |
| Active | text darkens to #0097a7 |
| Disabled | text #606078 |
| Example | "Learn more", "View details", "Help" |

**Danger button:**

| **Property** | **Value** |
| :-: | :-: |
| Background | Error Red (#f44336) |
| Text | White (#ffffff), Noto Sans Bold |
| Padding | 8px 16px |
| Border radius | 6px |
| Hover | lighten red by 10% (#ff5a4d) |
| Active (pressed) | darken red by 10% (#d32f2f) |
| Disabled | gray (#606078), text #a0a0b8 |
| Example | "Delete", "Erase", "Format" |

**Icon button:**

| **Property** | **Value** |
| :-: | :-: |
| Background | transparent |
| Icon | 20px, Primary Text color (#e0e0e0) |
| Padding | 8px |
| Border radius | 4px |
| Hover | Elevated Surface background |
| Active | Card Surface background |
| Disabled | icon at 40% opacity |
| Example | volume mute, brightness, close window |

### Input Fields

**Text input:**

| **Property** | **Value** |
| :-: | :-: |
| Background | Dark Base (#1a1a2e) |
| Text | Primary Text (#e0e0e0), Noto Sans Regular |
| Placeholder text | Secondary Text (#a0a0b8) |
| Padding | 8px 12px |
| Border | 1px, Elevated Surface (#33335a) |
| Border radius | 6px |
| Focus | border becomes 2px Spike Purple (#6d4aff) |
| Error | border becomes 2px Error Red (#f44336), error text below |
| Disabled | background #222236, text #606078 |
| Height | 36px (single line), auto-expand (multiline) |

**Password input:**

```
├── Same as text input, plus:
├── Eye icon button on the right (toggle visibility)
│   ├── Eye open: visibility on (password shown)
│   └── Eye slash: visibility off (password masked)
├── Masked character: • (bullet, not asterisk *)
└── Caps Lock indicator: small warning below field if Caps Lock on
```

**Toggle switch:**

| **Property** | **Value** |
| :-: | :-: |
| Track | 44px wide, 24px tall, border radius 12px (pill shape) |
| Knob | 20px circle, border radius 10px |
| Off | track #33335a, knob #e0e0e0 (knob on left) |
| On | track Spike Purple (#6d4aff), knob white (knob on right) |
| Transition | 150ms ease (200ms on Plus, instant on Standard) |
| Disabled | track and knob at 40% opacity |

**Slider:**

| **Property** | **Value** |
| :-: | :-: |
| Track | 4px tall, border radius 2px |
| Track (unfilled) | Elevated Surface (#33335a) |
| Track (filled) | Spike Purple (#6d4aff) |
| Handle | 16px circle, white, border 2px Spike Purple |
| Handle hover | 18px circle |
| Handle active | 20px circle |
| Tick marks (if present) | 2px tall, Secondary Text color |

### Dropdowns And Menus

**Dropdown select:**

```
├── Closed: same as text input, with chevron-down icon on right
├── Open: list of options in a Card Surface popup
│   ├── Option (unselected): Primary Text on Card Surface
│   ├── Option (hovered): Elevated Surface background
│   ├── Option (selected): Spike Purple background, white text
│   └── Border radius: 8px (popup container)
└── Max height: 300px (scrolls if more options)
```

**Context menu:**

| **Property** | **Value** |
| :-: | :-: |
| Background | Card Surface (#2a2a4a) |
| Item text | Primary Text (#e0e0e0) |
| Item icon (if present) | 16px, Primary Text color |
| Hovered item | Elevated Surface (#33335a) |
| Disabled item | #606078 |
| Separator | 1px, Elevated Surface, full width |
| Padding | 6px 12px per item |
| Border radius | 8px (container) |
| Shadow | 0px offset, 4px blur, #000000 at 40% opacity |

### Notification Cards

**Notification card (popup):**

| **Property** | **Value** |
| :-: | :-: |
| Background | Card Surface (#2a2a4a) |
| Border | 1px, Elevated Surface (#33335a) |
| Border radius | 8px |
| Padding | 12px 16px |
| Shadow | 0px offset, 6px blur, #000000 at 50% opacity |
| Width | 360px (fixed) |
| App icon | 24px, top-left |
| App name | 11px Caption, Secondary Text |
| Title | 14px Body Large, Bold, Primary Text |
| Body | 13px Body, Regular, Secondary Text |
| Timestamp | 11px Caption, Secondary Text, right-aligned |
| Action buttons (if any) | Secondary button style, right-aligned |
| Close button | Icon button (X), top-right |

**Urgency indicators:**

```
├── Normal: no special treatment
├── High: left border 3px, Warning Orange
└── Critical: left border 3px, Error Red
```

Animation (Plus only): slide-in from right, 200ms ease

### Panel Applets

**Applet button (in panel):**

```
├── Background: transparent
├── Icon: 18px, Primary Text (#e0e0e0)
├── Padding: 6px
├── Hover: Elevated Surface background, border radius 4px
├── Active (menu open): Card Surface background, border radius 4px
├── Badge (notification count): 8px circle, Error Red, white text
│   └── Positioned: top-right of applet icon, -2px offset
└── Tooltip: 11px Caption, white text, Dark Base background, 8px padding
```

**Applet popout (expanded):**

| **Property** | **Value** |
| :-: | :-: |
| Background | Panel Surface (#222236) |
| Border | 1px, Elevated Surface (#33335a) |
| Border radius | 8px (top corners only if from bottom panel) |
| Shadow | 0px offset, 6px blur, #000000 at 50% opacity |
| Width | varies (applet-specific, 240-360px typical) |
| Header | 13px Body, Bold, Primary Text, 12px padding |
| Content | 12px padding, varying content |
| Footer (if present) | Secondary button style, right-aligned |

## Boot Visual Assets

### GRUB Theme

```
GRUB theme: /usr/share/spike/grub/
├── theme.txt                    → Theme definition
├── background.png               → Dark Base + circuit trace pattern
├── spike-emblem.png             → Dog emblem, centered top (96px)
├── tagline.png                  → "Let's make tech repairable again" under emblem
└── font.pf2                     → Noto Sans compiled for GRUB
```

**(no variant difference — same theme for Standard and Plus)**

**Theme specification:**

```
├── Background: #1a1a2e with circuit trace pattern (baked into PNG)
├── Title area:
│   ├── Spike emblem centered, 96px, top ~8% of screen
│   └── Tagline under emblem: "Let's make tech repairable again"
│       ├── Color: #a0a0b8
│       └── Asset: tagline.png (baked text; no GRUB font required)
├── Menu area:
│   ├── Position: centered, 60% width, below emblem
│   ├── Background: semi-transparent Panel Surface (#222236 at 80%)
│   ├── Border: 1px cyan (#00bcd4) at 30% opacity
│   ├── Border radius: 8px
│   └── Padding: 16px
├── Menu items:
│   ├── Font: Noto Sans Regular, 14px
│   ├── Unselected: #e0e0e0
│   ├── Selected: #ffffff on Spike Purple (#6d4aff) background
│   └── Item height: 28px
├── Footer:
│   ├── Help text: "Use ↑↓ to select, Enter to boot. Press 'e' to edit."
│   ├── Font: Noto Sans Regular, 11px
│   ├── Color: #a0a0b8
│   └── Position: bottom center, 5% from bottom
└── No animations (GRUB doesn't support them)
```

### Plymouth Themes

Two Plymouth themes, variant-dependent:

**Spike Standard — spike-minimal:**

```
├── Theme name: spike-minimal
├── Background: Dark Base (#1a1a2e) solid color
│   └── No circuit trace pattern (Plymouth rendering limitations)
├── Spike emblem: centered, static, 256px (logo.png)
├── Tagline under emblem: "Let's make tech repairable again"
│   ├── Color: #a0a0b8
│   └── Asset: tagline.png
├── Progress indicator: row of 5 dots beneath tagline
│   ├── Dot size: 8px circle
│   ├── Dot spacing: 12px between centers
│   ├── Inactive dot: Elevated Surface (#33335a)
│   ├── Active dot: Spike Purple (#6d4aff)
│   └── Animation: dots fill sequentially left to right, loop
├── No glow effects
├── Renderer: software (framebuffer) or DRM (simple)
└── Designed to look identical on any GPU
```

**Spike Plus — spike-full:**

```
├── Theme name: spike-full
├── Background: Dark Base (#1a1a2e) solid color
├── Spike emblem: centered, 256px (logo.png)
│   └── Glow effect: soft purple halo around emblem (pulsing gently) — optional / future
├── Tagline under emblem: "Let's make tech repairable again"
│   ├── Color: #a0a0b8
│   └── Asset: tagline.png
├── Progress indicator: horizontal progress bar beneath tagline
│   ├── Bar: 200px wide, 4px tall
│   ├── Track: Elevated Surface (#33335a)
│   ├── Fill: Spike Purple (#6d4aff) (gradient Spike Purple → Spike Cyan when feasible)
│   └── Animation: smooth fill, tied to boot progress
├── Renderer: script plugin (DRM / framebuffer)
├── Falls back to spike-minimal rendering if needed
└── Designed for GPUs with working DRM/KMS
```

### SDDM Theme

```
SDDM theme: /usr/share/sddm/themes/spike/
├── theme.conf                    → Theme configuration
├── Main.qml                      → QML layout
├── background.png                → Dark Base + circuit trace pattern
├── spike-logo.png                → Full logo (dog + wordmark), 180px wide
├── spike-emblem.png              → Dog emblem (for small uses)
└── fonts/                        → Noto Sans (bundled)
```

**(no variant difference — same theme)**

**Theme specification:**

```
├── Background: #1a1a2e with circuit trace pattern (baked into PNG)
├── Logo:
│   ├── Full logo (dog + "Spike" wordmark), centered horizontally
│   ├── Position: upper third of screen
│   └── Size: 180px wide
├── Login card:
│   ├── Position: center of screen (vertically, below logo)
│   ├── Background: Card Surface (#2a2a4a) at 90% opacity
│   ├── Border: 1px, Elevated Surface (#33335a)
│   ├── Border radius: 12px
│   ├── Padding: 32px
│   ├── Width: 320px
│   └── Shadow: 0px offset, 8px blur, #000000 at 50% opacity
├── Username field:
│   ├── Style: standard text input (see Components)
│   ├── Placeholder: "Username"
│   └── Submit: Enter key advances to password
├── Password field:
│   ├── Style: standard password input (see Components)
│   ├── Placeholder: "Password"
│   ├── Eye icon: toggle visibility
│   └── Caps Lock warning: below field
├── Login button:
│   ├── Style: primary button (see Components)
│   ├── Text: "Log In"
│   ├── Full width of card
│   └── Enter key triggers login
├── Error message:
│   ├── Position: below login button
│   ├── Style: 13px Body, Error Red (#f44336)
│   └── Text: "Incorrect password. Try again."
├── Accessibility controls:
│   ├── Position: bottom-left of screen
│   ├── Icons: screen reader, on-screen keyboard, high contrast, large text
│   ├── Size: 32px icons, 8px spacing
│   └── Style: icon buttons (see Components)
├── Power buttons:
│   ├── Position: bottom-right of screen
│   ├── Icons: restart, shut down
│   ├── Size: 24px icons
│   └── Style: icon buttons with tooltips
├── Session selector: hidden (only spike.session available)
├── Clock: not shown on login (clean, focused)
└── No animations (login screen should be instant)
```

### Qt Stylesheet

The Qt stylesheet (QSS) is the master style definition for all Qt6 applications in Spike:

| **Property** | **Value** |
| :-: | :-: |
| **Stylesheet location** | `/usr/share/spike/themes/spike.qss` |

**What it styles:**

```
├── QPushButton (buttons)
├── QLineEdit (text input)
├── QComboBox (dropdowns)
├── QCheckBox (checkboxes)
├── QSlider (sliders)
├── QScrollBar (scrollbars)
├── QMenu (context menus)
├── QToolTip (tooltips)
├── QGroupBox (group boxes in settings)
├── QTabWidget (tab bars)
├── QProgressBar (progress bars)
├── QSpinBox / QDoubleSpinBox
├── QTreeView / QListView / QTableView (item views)
├── QHeaderView (table headers)
├── QDockWidget (dockable panels)
├── QStatusBar (status bars)
├── QToolBar (toolbars)
└── QToolButton (toolbar buttons)
```

**What it does NOT style (managed separately):**

```
├── KWin window decorations (KWin config, not QSS)
├── SDDM theme (QML, not QSS)
├── Plymouth theme (script, not QSS)
├── GRUB theme (GRUB theme.txt, not QSS)
└── Firefox UI (Firefox manages its own styling)
```

**Variant differences:**

```
├── Spike Standard:
│   ├── No transitions/animations in QSS
│   ├── No blur effects on backgrounds
│   ├── No translucency on panels (solid surfaces)
│   └── Simpler hover states (color change, no animation)
└── Spike Plus:
    ├── CSS transitions on hover (150ms ease)
    ├── Translucent surfaces where appropriate
    ├── Blur behind translucent panels (KWin effect)
    └── Animated hover states (fade, not slide)
```

**High-contrast override:**

```
├── /usr/share/spike/themes/high-contrast.qss
├── Replaces spike.qss entirely when high contrast is enabled
├── See ACCESSIBILITY.md for color specification
└── Applied by spike-shell theme engine
```

### KWin Window Decorations

**Window decoration style:**

| **Property** | **Value** |
| :-: | :-: |
| Border | 1px, cyan (#00bcd4) at 30% opacity |
| Border radius | 0px (sharp corners) |
| Title bar height | 32px (Standard) / 36px (Plus) |
| Title bar background | Panel Surface (#222236) |
| Title text | 13px Body, Primary Text (#e0e0e0), centered |

**Buttons (right side):**

```
├── Minimize: − (minus icon)
├── Maximize: ▢ (square icon)
└── Close: ✕ (X icon)
```

- Button size: 24px 

- Button hover: Elevated Surface background 

- Close button hover: Error Red (#f44336) background, white icon 

- Button padding: 6px 

- No title bar icon (window icon not shown) 

**Active vs inactive:**

```
├── Active window: title bar Panel Surface (#222236), border visible
└── Inactive window: title bar Dark Base (#1a1a2e), border faded (15% opacity)
```

## Wallpapers

### Default Wallpapers

Spike ships with a small set of default wallpapers:

**Wallpaper 1: "Circuit Field" (default)**

| **Property** | **Value** |
| :-: | :-: |
| Background | Dark Base (#1a1a2e) |
| Pattern | Circuit trace pattern (more dense than background motif) |
| Glow | Subtle purple (#6d4aff) glow in lower-right corner, Subtle cyan (#00bcd4) glow in upper-left corner |
| Emblem | No Spike emblem (wallpaper is ambient, not branding) |
| Resolution | 3840×2160 (downscales to all target resolutions) |
| File | `/usr/share/spike/wallpapers/circuit-field.png` |

**Wallpaper 2: "Solid Dark"**

| **Property** | **Value** |
| :-: | :-: |
| Background | Pure Dark Base (#1a1a2e) |
| Pattern | No pattern, no gradient, no glow |
| Use | For users who want maximum minimalism |
| Resolution | 3840×2160 |
| File | `/usr/share/spike/wallpapers/solid-dark.png` |

**Wallpaper 3: "Spike Gradient"**

| **Property** | **Value** |
| :-: | :-: |
| Background | Vertical gradient: Dark Base (#1a1a2e) at top → slightly lighter (#2a2a4a) at bottom |
| Pattern | Circuit trace pattern at very low opacity (5%) |
| Appearance | Subtle, nearly solid |
| Resolution | 3840×2160 |
| File | `/usr/share/spike/wallpapers/spike-gradient.png` |

**Wallpaper 4: "Good Boy"**

| **Property** | **Value** |
| :-: | :-: |
| Background | Dark Base |
| Emblem | Spike dog emblem in lower-right corner, 20% opacity |
| Pattern | Circuit trace pattern (sparse) |
| Note | The only wallpaper featuring the Spike dog |
| Resolution | 3840×2160 |
| File | `/usr/share/spike/wallpapers/good-boy.png` |

### Custom Wallpapers

Users can set custom wallpapers:

```
├── Settings → Appearance → Wallpaper → "Browse..."
├── Supported formats: PNG, JPEG, WebP
├── Recommended resolution: native display resolution or higher
├── Minimum resolution: 1366×768 (below this, image quality degrades)
├── Custom wallpapers are copied to: ~/Pictures/Wallpapers/
├── Original file is preserved (Spike copies, doesn't move)
└── Scaling options:
    ├── Fill (default): fills screen, may crop
    ├── Fit: fits within screen, may letterbox
    ├── Stretch: fills screen, may distort
    └── Center: original size, centered (may tile if small)
```

## Icons

### Icon Theme

Spike uses a custom icon theme: "spike-icons"

**Based on:** Breeze (KDE) with modifications

**Why Breeze-based:**

```
├── Breeze is the KDE default — matches bundled KDE applications
├── Breeze is complete (covers all standard freedesktop.org icons)
├── Breeze is maintained by KDE (upstream updates)
└── Spike overrides key icons (apps, panel, branding) with custom designs
```

**Icon theme location:** `/usr/share/spike/icons/spike-icons/`

```
├── index.theme                 → Theme definition (inherits Breeze)
├── apps/                       → Application icons (custom)
├── panel/                      → Panel/applet icons (custom)
├── places/                     → Folder icons (custom for main folders)
├── actions/                    → Action icons (Breeze, inherited)
├── devices/                    → Device icons (Breeze, inherited)
├── mimetypes/                  → File type icons (Breeze, inherited)
├── status/                     → Status icons (Breeze, inherited)
├── categories/                 → Category icons (Breeze, inherited)
└── emblems/                    → Emblem icons (Breeze, inherited)
```

### Custom Icon Design

Icons designed by Spike (not inherited from Breeze):

**Application icons:**

```
├── Style: flat 2D, single-color glyph on rounded square
├── Background: Card Surface (#2a2a4a)
├── Glyph color: Primary Text (#e0e0e0) or Spike Purple (#6d4aff)
├── Border: 1px, Elevated Surface (subtle)
├── Border radius: 4px (corners)
├── Size: 48x48 (standard), 22x22 (small), 16x16 (tiny)
└── Examples:
    ├── Firefox: fox glyph (inherits Firefox's icon, not restyled)
    ├── Discover: shopping bag glyph (inherits Breeze)
    ├── Dolphin: dolphin glyph (inherits Breeze)
    ├── Konsole: terminal glyph (inherits Breeze)
    ├── Settings: gear glyph (custom — Spike-style gear)
    └── Files: folder glyph (custom — matches Spike folder design)
```

**Panel/applet icons:**

```
├── Style: monochrome glyph, no background
├── Color: Primary Text (#e0e0e0)
├── Size: 18x18 (in panel), 24x24 (in applet popout)
├── Active state: Spike Purple (#6d4aff)
├── Disabled state: #606078
└── Examples:
    ├── Network: WiFi arc waves
    ├── Volume: speaker with wave
    ├── Battery: battery outline with fill
    ├── Brightness: sun
    ├── Bluetooth: Bluetooth rune
    ├── Notifications: bell
    ├── Clock: (handled by text, no icon)
    ├── Session menu: power symbol
    ├── Removable devices: USB symbol
    ├── Update notifier: downward arrow with badge
    └── Night light: moon
```

**Folder icons:**

| **Folder** | **Accent Color** |
| :-: | :-: |
| Home | Spike Purple accent |
| Documents | white accent (neutral) |
| Downloads | cyan accent |
| Pictures | Warning Orange accent |
| Videos | Error Red accent |
| Music | Info Blue accent |

Color-coded folders help non-technical users identify locations. Standard Breeze folders used for non-main folders.

### Cursor Theme

| **Property** | **Value** |
| :-: | :-: |
| **Cursor theme** | Breeze (default) |

**Modified cursor sizes:**

```
├── Normal: 24px
├── Large: 32px (accessibility)
├── Extra Large: 48px (accessibility)
└── Huge: 64px (accessibility)
```

See ACCESSIBILITY.md for cursor size configuration.

**High-contrast cursor:**

```
├── Black and white two-tone cursor
├── Visible on any background
└── Used when high-contrast mode is enabled
```

Custom cursor design (future consideration): A Spike-branded cursor is possible but low priority. Breeze cursor is functional and recognizable. Only customize if a clear benefit is identified.

## Sound Design

### System Sounds

Spike includes a minimal set of system sounds:

```
├── Designed to be calm, unobtrusive, and non-startling
├── Format: Ogg Vorbis (small size, good quality)
├── Sample rate: 44100Hz (matches PipeWire default)
├── Duration: all sounds under 1 second
└── Location: /usr/share/sounds/spike/
```

**Sound inventory:**

| **Sound File** | **Description** |
| :-: | :-: |
| `notification.ogg` | Soft two-tone chime (descending, purple-feeling), 0.3s, moderate volume |
| `notification-critical.ogg` | Sharper two-tone (ascending, urgent), 0.4s, slightly louder |
| `volume-change.ogg` | Tiny tick (0.05s), plays only when adjusting via slider |
| `trash-empty.ogg` | Soft crinkle/paper sound (0.3s), satisfying but not loud |
| `startup.ogg` | Soft ascending chime (0.5s), plays when desktop ready, can be disabled |
| `shutdown.ogg` | Soft descending chime (0.5s), plays when shutdown initiated, can be disabled |
| `error.ogg` | Soft low buzz (0.2s), not harsh or alarming, can be disabled |

**Sounds NOT included:**

```
├── No login sound (boot sound replaces it)
├── No logout sound
├── No click sounds (too noisy)
├── No beep on keypress (annoying)
├── No music (no ambient/background audio)
└── No voice prompts or spoken messages
```

**Design principles:**

```
├── Every sound must be identifiable but ignorable
├── Sounds complement visual feedback, never replace it
├── All sounds are short (under 1 second)
├── No sound is louder than the user's set volume
├── Every sound can be individually disabled
└── "Silent" is always an option (turn off all sounds)
```

## Animation Guidelines

### Animation Philosophy

Animations serve two purposes:

1. **Feedback:** confirm an action was registered (button press, toggle) 

2. **Continuity:** show where things came from and went (window open, menu expand) 

**Animations must NOT:**

```
├── Delay user actions (no animation that blocks input)
├── Draw attention to themselves (no flashy or elaborate effects)
├── Be longer than necessary (shortest duration that communicates the action)
└── Play when "Reduce Motion" is enabled (see ACCESSIBILITY.md)
```

### Animation Specifications

**Variant differences:**

| **Action** | **Spike Standard** | **Spike Plus** |
| :-: | :-: | :-: |
| Button hover | instant color change | 150ms color transition (ease-out) |
| Button press | instant color change | 100ms color transition (ease-in) |
| Toggle switch | instant position change | 200ms slide (ease-in-out) |
| Window open | instant | 150ms fade + scale from 95% to 100% (ease-out) |
| Window close | instant | 150ms fade + scale to 95% (ease-in) |
| Window minimize | instant | 200ms scale to 50% + fade (ease-in) |
| Menu expand | instant | 150ms fade + slight scale from 95% (ease-out) |
| Notification popup | instant | 200ms slide-in from right (ease-out) |
| Panel show/hide | instant | 200ms slide (ease-in-out) |
| Launcher open | instant | 150ms fade + scale from 97% (ease-out) |
| Workspace switch | instant | 200ms slide (ease-in-out) |
| Slider drag | follows pointer | follows pointer (no smoothing) |

**Timing rules:**

```
├── Micro-interactions (hover, press): 100-150ms
├── Appear/disappear (window, menu, notification): 150-200ms
├── Movement (slide, workspace switch): 200ms
├── Never exceed 250ms for any animation
├── Easing: ease-out for appearances, ease-in for disappearances, ease-in-out for movements
└── Frame rate target: 60fps (see PERFORMANCE-BASELINES.md)
```

## Spacing And Layout

### Spacing Scale

Spike uses a 4px base grid for all spacing:

| **Token** | **Size** | **Usage** |
| :-: | :-: | :-: |
| xs | 4px | tight: between icon and label in compact UI |
| sm | 8px | compact: button padding vertical, list item spacing |
| md | 12px | default: card padding, applet content padding |
| lg | 16px | comfortable: button padding horizontal, section spacing |
| xl | 24px | section separation in settings, dialog padding |
| xxl | 32px | major section breaks, installer step padding |
| xxxl | 48px | page margins, large dialog spacing |

**Rules:**

```
├── All spacing values are multiples of 4px
├── No arbitrary spacing (use the nearest token)
├── Consistent spacing within a component
└── Larger spacing = more importance (section breaks use xl/xxl)
```

### Panel Dimensions

**Bottom panel:**

| **Property** | **Standard** | **Plus** | **Accessibility** |
| :-: | :-: | :-: | :-: |
| Height | 32px | 36px | - |
| Height (1.25x text) | 40px | 40px | - |
| Height (1.5x text) | 48px | 48px | - |
| Height (2.0x text) | 64px | 64px | - |
| Width | 100% of screen | 100% of screen | - |
| Position | bottom (fixed) | bottom (fixed) | - |
| Content margin | 4px from left/right edges | 4px from left/right edges | - |
| Applet spacing | 4px between adjacent applets | 4px between adjacent applets | - |

**Launcher:**

| **Property** | **Value** |
| :-: | :-: |
| Width | 400px (default) |
| Height | 500px (default, or screen height - 80px, whichever is smaller) |
| Position | centered horizontally, bottom-aligned above panel |
| Search bar | full width, 48px tall, 12px padding |
| App grid | 4 columns, 72px per cell (48px icon + label) |
| App list | 1 column, 32px per row |
| Toggle | grid view / list view (user preference, persisted) |

## Asset File Inventory

### Complete Asset Tree

```
/usr/share/spike/
├── branding/
│   └── logo/
│       ├── spike-logo-full.svg
│       ├── spike-logo-full-light.svg
│       ├── spike-emblem.svg
│       ├── spike-emblem-light.svg
│       ├── spike-wordmark.svg
│       ├── spike-wordmark-light.svg
│       ├── spike-monochrome-white.svg
│       ├── spike-monochrome-black.svg
│       ├── spike-emblem-{16,24,32,48,64,128,256,512}.png
│       └── README (asset license and attribution)
├── grub/
│   ├── theme.txt
│   ├── background.png
│   ├── spike-emblem.png
│   ├── tagline.png
│   └── font.pf2
├── plymouth/
│   └── themes/
│       ├── spike-minimal/
│       │   ├── spike-minimal.plymouth
│       │   ├── spike-minimal.script
│       │   ├── logo.png
│       │   ├── tagline.png
│       │   ├── dot-off.png
│       │   └── dot-on.png
│       └── spike-full/
│           ├── spike-full.plymouth
│           ├── spike-full.script
│           ├── logo.png
│           ├── tagline.png
│           ├── progress-track.png
│           └── progress-fill.png
├── sddm/
│   └── themes/spike/
│       ├── theme.conf
│       ├── Main.qml
│       ├── background.png
│       ├── spike-logo.png
│       └── fonts/NotoSans-Regular.ttf, NotoSans-Bold.ttf
├── themes/
│   ├── spike.qss
│   ├── high-contrast.qss
│   └── (variant-specific overrides in state.json)
├── icons/
│   └── spike-icons/
│       ├── index.theme
│       ├── apps/ (custom)
│       ├── panel/ (custom)
│       ├── places/ (custom for main folders)
│       └── (inherits Breeze for all other categories)
├── wallpapers/
│   ├── circuit-field.png
│   ├── solid-dark.png
│   ├── spike-gradient.png
│   └── good-boy.png
├── sounds/
│   ├── notification.ogg
│   ├── notification-critical.ogg
│   ├── volume-change.ogg
│   ├── trash-empty.ogg
│   ├── startup.ogg
│   ├── shutdown.ogg
│   └── error.ogg
├── cursors/
│   └── (Breeze cursor theme, symlinked — no custom cursor yet)
└── patterns/
    ├── circuit-trace.svg (master pattern, used for backgrounds)
    └── circuit-trace-dark.png (pre-rendered for GRUB/SDDM backgrounds)
```

### Asset Licensing

All Spike branding assets are:

```
├── Licensed under CC BY-SA 4.0 (Creative Commons Attribution-ShareAlike 4.0)
├── Copyright BigRangaTech
├── Source files (SVG) provided alongside rendered files (PNG)
├── Modifications permitted (with attribution and share-alike)
└── Commercial use permitted (with attribution)
```

Exception: the "Spike" name and dog mascot are trademarks of BigRangaTech:

```
├── The artwork is CC BY-SA (you can remix the art)
├── The trademark is not licensed (you can't call your project "Spike")
└── See GOVERNANCE.md for trademark policy
```

**Third-party assets:**

```
├── Noto Sans font: SIL Open Font License 1.1 (Google)
├── Breeze icon theme: LGPL-3.0 (KDE)
├── Breeze cursor theme: LGPL-3.0 (KDE)
├── espeak-ng voices: various open licenses
└── All third-party licenses documented in LICENSE file
```

### Branding Checklist For New Components

When adding any new visual element to Spike, verify:

| **Category** | **Checklist** |
| :-: | :-: |
| **Colors** | ☐ Uses only colors from the Spike palette  
☐ Primary actions use Spike Purple  
☐ Accents use Spike Cyan  
☐ Status uses semantic colors only  
☐ Surfaces follow the hierarchy (Base < Panel < Card) |
| **Typography** | ☐ Uses Noto Sans (Regular, Medium, or Bold)  
☐ Font size matches a defined size token  
☐ No decorative or custom fonts |
| **Geometry** | ☐ Corner radii match component spec  
☐ Spacing uses 4px grid multiples  
☐ Borders follow width and color rules |
| **Icons** | ☐ Uses Spike icon theme or Breeze inherited  
☐ Panel icons are monochrome  
☐ Application icons follow flat 2D style |
| **Animation** | ☐ Standard: no animation (instant)  
☐ Plus: within timing guidelines (100-200ms)  
☐ Respects "Reduce Motion" accessibility setting |
| **Text** | ☐ Follows tone of voice rules  
☐ Terminology matches established vocabulary  
☐ No exclamation marks in system messages  
☐ No jargon in user-facing text |
| **Accessibility** | ☐ Works in high-contrast mode  
☐ Works with screen reader (AT-SPI exposed)  
☐ Works with large text scaling (1.5x)  
☐ Works with reduced motion  
☐ Color is not the sole indicator of meaning |
| **Variant Compliance** | ☐ Looks acceptable on Spike Standard (no effects)  
☐ Looks polished on Spike Plus (with effects)  
☐ Variant differences documented (if any) |
| **Performance** | ☐ No additional memory cost above baseline  
☐ No animation exceeds 250ms  
☐ No GPU-dependent effect without software fallback |

## What This Document Does Not Cover

- Component behavior and interaction logic: See DESKTOP.md 

- Variant system and detection logic: See VARIANT-DIFFERENCES.md 

- Accessibility feature implementation: See ACCESSIBILITY.md 

- Performance targets for rendering: See PERFORMANCE-BASELINES.md 

- Boot process and Plymouth handoff: See BOOT-PROCESS.md 

- SDDM configuration and login flow: See BOOT-PROCESS.md 

- Multimeida architecture: See MULTIMEDIA.md 

- Governance and trademark details: See GOVERNANCE.md 


🐕 BigRangaTech


