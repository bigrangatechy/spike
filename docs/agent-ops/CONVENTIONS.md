# Spike — CONVENTIONS.md

## Coding Conventions & Style Guide

This document defines the coding standards, naming conventions, commit message formats, and documentation requirements for all Spike source code. Both human contributors and AI agents must follow these conventions to maintain consistency across the codebase.

> **Note:** AI agents should consult this file before writing any code. Violations will be caught in CI/CD and must be corrected before merging.


## 1. General Principles

### 1.1 Consistency Over Personal Preference

- Follow the existing style of each language/file 

- Do not impose personal preferences on the codebase 

- When in doubt, match the surrounding code's style 

- If a file already has an established style, maintain that style even if it differs from these conventions 

### 1.2 Clarity Over Cleverness

- Write readable, self-documenting code 

- Avoid overly clever one-liners or obscure tricks 

- Prefer explicit over implicit behavior 

- Comments should explain *why*, not *what* 

### 1.3 Minimalism

- Only include what is necessary 

- No premature optimization 

- Remove dead code immediately 

- Keep functions/modules focused on single responsibilities 

### 1.4 Safety First

- Validate all inputs 

- Handle errors gracefully 

- Fail safely and log meaningfully 

- Never assume external dependencies are available 


## 2. File Structure

### 2.1 Header Comments

Every source file must begin with a header comment block:

**Bash (.sh):**

```
`\#!/bin/bash`

*`\#`*

*`\# Filename:    script-name.sh`*

*`\# Purpose:     Brief description of what this script does`*

*`\# Author:      Spike Team \<team@bigrangatech.org\>`*

*`\# License:     GPLv2+`*

*`\# Created:     YYYY-MM-DD`*

*`\# Modified:    YYYY-MM-DD (optional)`*

*`\#`*

*`\# Dependencies: bash, coreutils, optional deps`*

*`\# Usage:       ./script-name.sh \[options\]`*
```

**Python (.py):**

```
*`\#!/usr/bin/env python3`*

`"""`

`Module:      module\_name.py`

`Purpose:     Brief description of what this module does`

`Author:      Spike Team \<team@bigrangatech.org\>`

`License:     GPLv2+`

`Created:     YYYY-MM-DD`

`Modified:    YYYY-MM-DD (optional)`


`Dependencies:`

`    - Python 3.10+`

`    - Optional: package-name (\>= 2.0)`


`Usage:`

`    python module\_name.py \[arguments\]`


`Classes:`

`    ClassName: Description`

`Functions:`

`    function\_name(): Description`

`"""`


`import os`

`import sys`


*`\# ... rest of code`*
```

**C++ (.cpp, .hpp):**

```
*`/\*`*

` *\* Filename:    filename.cpp/hpp`*

` *\* Purpose:     Brief description of what this file does`*

` *\* Author:      Spike Team \<team@bigrangatech.org\>`*

` *\* License:     GPLv2+`*

` *\* Created:     YYYY-MM-DD`*

` *\* Modified:    YYYY-MM-DD (optional)`*

` *\*`*

` *\* Dependencies: Qt6, KDE Frameworks (if applicable)`*

` *\* Usage:       Compiled as part of spike-project`*

` *\*/`*


`\#ifndef FILENAME\_HPP`

`\#define FILENAME\_HPP`


*`// Includes`*

`\#include \<QSomething\>`


`namespace Spike \{`

`    *// Classes, structs, enums`*

`\}`


`\#endif *// FILENAME\_HPP`*
```

**QML (.qml):**

```
*`/\*`*

` *\* Filename:    ComponentName.qml`*

` *\* Purpose:     Brief description of what this component does`*

` *\* Author:      Spike Team \<team@bigrangatech.org\>`*

` *\* License:     GPLv2+`*

` *\* Created:     YYYY-MM-DD`*

` *\* Modified:    YYYY-MM-DD (optional)`*

` *\*`*

` *\* Dependencies: Qt6 Quick Controls 2, Spike QML types`*

` *\* Parent:      (if composite component)`*

` *\* Properties:`*

` *\*   - propertyType: propertyName — Description`*

` *\*/`*


`import QtQuick 2.15`

`import QtQuick.Controls 2.15`

`import org.kde.spike 1.0`


`Item \{`

`    *// Component implementation`*

`\}`
```

### 2.2 File Naming

| **Type** | **Convention** | **Example** |
| :-: | :-: | :-: |
| Shell scripts | kebab-case, `.sh` | `hardware-detect.sh` |
| Python modules | snake\_case, `.py` | `config\_parser.py` |
| Python executables | snake\_case, `.py` | `spike\_config.py` |
| C++ headers | PascalCase or snake\_case, `.hpp` | `HardwareDetector.hpp` or `hardware\_detector.hpp` |
| C++ sources | Matches header, `.cpp` | `HardwareDetector.cpp` |
| QML components | PascalCase, `.qml` | `SettingsPanel.qml` |
| JSON data | snake\_case, `.json` | `state\_store.json` |
| Templates | snake\_case, `.tpl` | `network-manager.conf.tpl` |

### 2.3 Directory Layout

```
`spike-project/`

`├── src/                          \# Source code`

`│   ├── bash/                     \# Shell scripts`

`│   │   ├── hardware-detect.sh`

`│   │   └── setup-zram.sh`

`│   ├── python/                   \# Python modules/executables`

`│   │   ├── \_\_init\_\_.py`

`│   │   ├── config/`

`│   │   │   ├── \_\_init\_\_.py`

`│   │   │   ├── parser.py`

`│   │   │   └── validator.py`

`│   │   └── spike-config          \# Main executable`

`│   ├── cpp/                      \# C++ source`

`│   │   ├── detector/`

`│   │   │   ├── cpu\_detector.hpp`

`│   │   │   ├── cpu\_detector.cpp`

`│   │   │   ├── storage\_detector.hpp`

`│   │   │   └── storage\_detector.cpp`

`│   │   └── shell/`

`│   │       ├── spike-shell.hpp`

`│   │       └── spike-shell.cpp`

`│   └── qml/                      \# QML components`

`│       ├── main.qml`

`│       ├── components/`

`│       │   ├── Panel.qml`

`│       │   ├── Launcher.qml`

`│       │   └── TrayIcon.qml`

`│       └── styles/`

`│           ├── default.qml`

`│           └── minimal.qml`

`├── lib/                          \# Libraries/shared code`

`├── tests/                        \# Unit/integration tests`

`├── docs/                         \# Generated documentation`

`└── build/                        \# Build artifacts (gitignored)`
```


## 3. Bash Conventions

### 3.1 Shell Options

All scripts must begin with strict mode:

```
`\#!/bin/bash`

`set -euo pipefail`


*`\# Optional: trace commands in debug builds`*

*`\# set -x  \# Uncomment only during debugging`*
```

### 3.2 Variable Naming

| **Pattern** | **Use Case** | **Example** |
| :-: | :-: | :-: |
| `UPPER\_SNAKE\_CASE` | Constants, environment vars | `MAX\_RETRIES=3` |
| `lower\_snake\_case` | Local variables | `cpu\_model=$(detect\_cpu)` |
| `lower\_snake\_case()` | Functions | `check\_dependencies()` |

### 3.3 Quoting Rules

- **Always quote variable expansions:** `"$\{VAR\}"` 

- **Never use unquoted variables:** `$VAR` ❌ 

- **Arrays require special handling:** `"$\{ARRAY\[@\]\}"` 

- **Exception:** Here-documents and arithmetic contexts don't need quotes 

```
*`\# ✅ CORRECT`*

`readonly CONFIG\_DIR="/etc/spike"`

`local config\_file="$\{CONFIG\_DIR\}/settings.conf"`


`if \[\[ -f "$\{config\_file\}" \]\]; then`

`    echo "Config found at $\{config\_file\}"`

`fi`


*`\# ❌ WRONG`*

`CONFIG\_DIR=/etc/spike`

`config\_file=$CONFIG\_DIR/settings.conf`


`if \[ -f $config\_file \]; then`

`    echo Config found at $config\_file`

`fi`
```

### 3.4 Error Handling

```
*`\# Function must return meaningful exit codes`*

`check\_hardware() \{`

`    local cpu\_info`

`    cpu\_info=$(lscpu 2\>/dev/null) || return 1`

`    `

`    if \[\[ -z "$\{cpu\_info\}" \]\]; then`

`        echo "ERROR: Could not read CPU information" \>&2`

`        return 2`

`    fi`

`    `

`    return 0`

`\}`


*`\# Main execution with error trapping`*

`main() \{`

`    trap 'echo "Fatal error on line $LINENO"' ERR`

`    `

`    check\_hardware || \{`

`        echo "Failed to detect hardware. Aborting." \>&2`

`        exit 1`

`    \}`

`    `

`    *\# Continue with logic`*

`\}`


`main "$@"`
```

### 3.5 Comment Style

```
*`\# One-liner comments start with "\# " (hash + space)`*

*`\# Align comments in columns when describing multiple items`*

*`\#`*

*`\# Multi-line comment blocks explain complex logic, algorithms,`*

*`\# or decision rationale. Maximum line length: 72 characters.`*


*`\# --- SECTION HEADER (aligned dashes) ---`*

*`\# This separates logical blocks within long scripts`*


*`\# TODO(john): Fix race condition in ZRAM initialization  \# Optional: tag with author/date`*

*`\# FIXME: Workaround for kernel bug \#12345`*

*`\# NOTE: This assumes ext4 filesystem (see DISASTER-RECOVERY.md)`*
```


## 4. Python Conventions

### 4.1 PEP 8 Compliance

All Python code must follow PEP 8 with the following overrides:

| **Setting** | **Value** | **Reason** |
| :-: | :-: | :-: |
| Line length | 88 characters (Black default) | Balance between readability and screen width |
| Indentation | 4 spaces | Standard Python |
| Blank lines | 2 between top-level defs, 1 between methods | Clear visual separation |
| Imports | Grouped by type (stdlib, third-party, local), alphabetized | Predictable import order |

### 4.2 Import Order

```
*`\#!/usr/bin/env python3`*

`"""Module documentation here."""`


*`\# Standard library imports`*

`import json`

`import logging`

`import sys`

`from pathlib import Path`

`from typing import Dict, List, Optional`


*`\# Third-party imports`*

`import yaml`


*`\# Local application imports`*

`from spike.config.parser import ConfigParser`

`from spike.config.validator import ValidationError`

`from spike.detectors.cpu import CpuDetector`


*`\# Module constants (after imports)`*

`CONFIG\_PATH = Path("/etc/spike/config/state.json")`

`DEFAULT\_TIMEOUT = 30`


*`\# ... rest of module`*
```

### 4.3 Naming Conventions

| **Element** | **Convention** | **Example** |
| :-: | :-: | :-: |
| Classes | PascalCase | `HardwareDetector`, `ConfigParser` |
| Functions | snake\_case | `detect\_cpu()`, `load\_state()` |
| Methods | snake\_case | `self.detect\_storage()`, `self.save\_config()` |
| Instance vars | snake\_case | `self.cpu\_model`, `self.storage\_path` |
| Class constants | UPPER\_SNAKE\_CASE | `MAX\_RETRY\_COUNT = 3` |
| Private (internal) | Leading underscore | `\_parse\_config()`, `\_validate\_input()` |
| Private (class) | Double underscore | `\_\_private\_method()` |
| Modules/packages | snake\_case | `config\_parser.py`, `detector/` |
| Boolean flags | `is\_`, `has\_`, `can\_` prefix | `is\_valid`, `has\_permission`, `can\_upgrade` |

### 4.4 Type Hints

Use type hints everywhere. Use `typing` module for generics:

```
`from typing import Dict, List, Optional, Tuple, Union, Any`


*`\# Function signatures`*

`def parse\_config(config\_path: str) -\> Dict\[str, Any\]:`

`    """Parse configuration file and return state dictionary."""`

`    pass`


`def detect\_cpu() -\> Optional\[CpuInfo\]:`

`    """Return CPU info if detected, None otherwise."""`

`    pass`


`def validate\_settings(settings: Dict\[str, str\]) -\> Tuple\[bool, List\[str\]\]:`

`    """Return (success, list\_of\_errors)."""`

`    pass`


*`\# Class attributes`*

`class HardwareState:`

`    def \_\_init\_\_(self, cpu: Optional\[CpuInfo\], storage: StorageInfo) -\> None:`

`        self.cpu: Optional\[CpuInfo\] = cpu`

`        self.storage: StorageInfo = storage`


*`\# Generic types`*

`cache: Dict\[str, List\[str\]\] = \{\}`

`results: List\[Tuple\[str, bool\]\] = \[\]`
```

### 4.5 Docstrings

Use Google-style docstrings for modules, classes, and public functions:

```
`def load\_state\_store(path: Path) -\> Dict\[str, Any\]:`

`    """`

`    Load and deserialize the Spike configuration state store.`


`    Args:`

`        path: Path to the state.json file. Must exist and be readable.`


`    Returns:`

`        Dictionary containing the parsed state store.`


`    Raises:`

`        FileNotFoundError: If the state file does not exist.`

`        json.JSONDecodeError: If the file contains invalid JSON.`

`        PermissionError: If the file is not readable.`


`    Example:`

`        \>\>\> state = load\_state\_store(Path('/var/lib/spike/config/state.json'))`

`        \>\>\> print(state\['system'\]\['variant'\])`

`        'standard'`


`    See Also:`

`        save\_state\_store(): Serialize state to disk`

`        validate\_state(): Validate loaded state structure`

`    """`

`    pass`
```

For private/internal functions, use inline comments or minimal docstrings:

```
`def \_normalize\_cpu\_model(model\_str: str) -\> str:`

`    *\# Strip whitespace and normalize spacing in CPU model strings`*

`    return ' '.join(model\_str.strip().split())`
```

### 4.6 Exception Handling

```
*`\# Catch specific exceptions, never bare 'except:'`*

`try:`

`    state = load\_state\_store(CONFIG\_PATH)`

`except FileNotFoundError:`

`    logger.warning(f"State store not found at \{CONFIG\_PATH\}, using defaults")`

`    state = generate\_default\_state()`

`except json.JSONDecodeError as exc:`

`    logger.error(f"Invalid JSON in state store: \{exc\}")`

`    state = regenerate\_from\_defaults()`

`except PermissionError:`

`    logger.critical(f"Cannot read state store: permission denied")`

`    raise SystemExit(1) from exc`


*`\# Use 'raise ... from' for chained exceptions`*

`def update\_setting(module: str, key: str, value: str) -\> None:`

`    try:`

`        state = load\_state\_store(CONFIG\_PATH)`

`    except Exception as exc:`

`        raise ConfigError(f"Failed to load state store") from exc`

`    `

`    try:`

`        save\_state\_store(state, CONFIG\_PATH)`

`    except IOError as exc:`

`        raise ConfigError(f"Failed to save state store") from exc`
```

### 4.7 Logging

```
`import logging`


`logger = logging.getLogger(\_\_name\_\_)`


*`\# Log levels (use appropriately)`*

`logger.debug("Detailed diagnostic information")`

`logger.info("General operational information")`

`logger.warning("Warning conditions (non-fatal)")`

`logger.error("Error conditions (handled but logged)")`

`logger.critical("Critical conditions (may cause shutdown)")`


*`\# Include context in messages`*

`logger.warning(f"Storage type '\{storage\_type\}' not recognized, defaulting to SSD")`

`logger.error(f"Failed to configure module '\{module\}': \{error\_message\}")`


*`\# Structured logging for critical paths`*

`logger.info(`

`    "Configuration change applied",`

`    extra=\{`

`        'module': 'power',`

`        'setting': 'cpu\_governor',`

`        'old\_value': 'powersave',`

`        'new\_value': 'schedutil'`

`    \}`

`)`
```


## 5. C++ Conventions

### 5.1 Qt/KDE Style Guide

Spike C++ code follows Qt/KDE conventions:

```
*`// Include order:`*

*`// 1. Corresponding header (for .cpp files)`*

*`// 2. Qt/C++ standard headers`*

*`// 3. Third-party headers`*

*`// 4. Local/project headers`*


`\#include "cpu\_detector.hpp"`


`\#include \<QDebug\>`

`\#include \<QFile\>`

`\#include \<QStringList\>`


`\#include \<fstream\>`

`\#include \<regex\>`


`\#include "hardware\_common.hpp"`

`\#include "../utils/string\_utils.hpp"`


*`// Forward declarations where possible`*

`class QFile;`
```

### 5.2 Naming Conventions

| **Element** | **Convention** | **Example** |
| :-: | :-: | :-: |
| Classes | PascalCase | `CpuDetector`, `HardwareScanner` |
| Methods | camelCase | `detectCpu()`, `isValid()` |
| Variables | camelCase | `cpuModel`, `storagePath` |
| Constants | SCREAMING\_SNAKE\_CASE | `MAX\_RETRY\_COUNT` |
| Private members | m\_ prefix | `m\_cpuModel`, `m\_initialized` |
| Private methods | camelCase (same as public) | `parseOutput()`, `validateResult()` |
| Enums | PascalCase enum + PascalCase values | `CpuType::Capable`, `StorageType::SSD` |
| Signal/Slot | descriptive names | `hardwareDetected()`, `scanProgress(int)` |

```
`class CpuDetector : public QObject`

`\{`

`    Q\_OBJECT`


`public:`

`    explicit CpuDetector(QObject \*parent = nullptr);`

`    ~CpuDetector() override = default;`


`    CpuInfo detect();`

`    bool isValid(const QString &model) const;`


`signals:`

`    void scanStarted();`

`    void scanProgress(int percentage);`

`    void scanCompleted(const CpuInfo &info);`


`private slots:`

`    void parseLscpuOutput(const QString &output);`


`private:`

`    QString readCpuInfo();`

`    CpuClassification classifyBogomips(int bogomips);`


`    *// Member variables (m\_ prefix)`*

`    int m\_bogomips;`

`    QString m\_cpuModel;`

`    bool m\_initialized;`

`\};`
```

### 5.3 Memory Management

- Use smart pointers (`QSharedPointer`, `std::unique\_ptr`, `std::shared\_ptr`) 

- Prefer parent-child ownership hierarchy for Qt objects 

- Never use `new` without storing result in smart pointer or parent 

```
*`// ✅ CORRECT - Smart pointer`*

`auto detector = std::make\_unique\<CpuDetector\>();`


*`// ✅ CORRECT - Parent ownership`*

`QWidget \*panel = new Panel(this);  *// 'this' owns panel`*


*`// ❌ WRONG - Raw pointer leak`*

`void init() \{`

`    auto widget = new QWidget;  *// Who deletes this?`*

`\}`
```

### 5.4 String Handling

```
*`// Use QString for user-visible text, std::string for pure data processing`*

`class CpuDetector \{`

`public:`

`    QString detectModel() const;  *// User-visible`*

`    std::string getRawSerial() const;  *// Internal data`*


`private:`

`    QString m\_model;  *// User-visible`*

`\};`


*`// QString concatenation (preferred)`*

`QString message = QString("Found CPU: %1").arg(cpuModel);`


*`// Or modern C++ style`*

`QString message = u"Found CPU: "\_s + cpuModel;`
```

### 5.5 Error Handling

```
*`// Return status/error codes where appropriate`*

`struct DetectionResult \{`

`    bool success;`

`    QString errorMessage;`

`    CpuInfo info;`

`\};`


`DetectionResult CpuDetector::detect() \{`

`    if (!m\_initialized) \{`

`        return \{false, "Detector not initialized"\};`

`    \}`

`    *// ... detection logic`*

`    return \{true, QString(), resultInfo\};`

`\}`


*`// Use exceptions for truly exceptional cases`*

`void CpuDetector::initialize() \{`

`    if (!m\_configFile.exists()) \{`

`        throw ConfigException(QStringLiteral("Config file not found: %1").arg(m\_configFile.path()));`

`    \}`

`\}`


*`// Q\_RETURN\_VAL macro for early return`*

`int CpuDetector::calculateScore() const \{`

`    if (!m\_hasValidData)`

`        return -1;  *// Invalid score indicates error`*

`    *// ... calculation`*

`    return m\_score;`

`\}`
```

### 5.6 Comments

```
*`// Single-line comments: "// " (slash-slash-space)`*

*`// Max line length: 78 characters for comments`*


*`// Multi-line comments:`*

*`/\*`*

` *\* Block comments explain algorithms, complex logic, or rationale`*

` *\* Start with /\* (star-asterisk)`*

` *\* End with \*\\/ (asterisk-slash)`*

` *\*/`*


*`// Qt-specific: Use QDoc format for doxygen-style comments`*

*`/\*\*`*

` *\* @brief Detect CPU hardware information`*

` *\* @return CpuInfo struct containing model, cores, bogomips`*

` *\* @throws HardwareException if detection fails`*

` *\* `*

` *\* @sa detectStorage(), detectGpu()`*

` *\*/`*

`CpuInfo CpuDetector::detect();`


*`// Markers for future work:`*

*`// TODO: Add AVX support for newer CPUs (see roadmap)`*

*`// FIXME: Race condition in parallel detection mode`*

*`// HACK: Workaround for kernel bug \#12345 (remove when fixed)`*

*`// NOTE: This behavior differs from Ubuntu default (see DESIGN-DECISIONS.md)`*
```


## 6. QML Conventions

### 6.1 Component Structure

```
*`/\*`*

` *\* Filename:    SettingsPanel.qml`*

` *\* Purpose:     Container for all system settings controls`*

` *\*/`*


`import QtQuick 2.15`

`import QtQuick.Layouts 1.15`

`import QtQuick.Controls 2.15`

`import org.kde.spike 1.0 as Spike`


`Pane \{`

`    id: control`


`    *// Public properties`*

`    property alias powerProfile: powerCombo.currentIndex`

`    property var onProfileChanged: null`


`    *// Component layout`*

`    ColumnLayout \{`

`        anchors.fill: parent`

`        spacing: 20`


`        *// Section headers`*

`        Label \{`

`            text: qsTr("Power Management")`

`            font.pixelSize: 16`

`            font.bold: true`

`        \}`


`        *// Control group`*

`        GroupBox \{`

`            title: qsTr("CPU Governor")`

`            Layout.fillWidth: true`


`            ColumnLayout \{`

`                anchors.left: parent.left`

`                anchors.right: parent.right`

`                spacing: 10`


`                ComboBox \{`

`                    id: cpuCombo`

`                    model: \["powersave", "schedutil", "performance"\]`

`                    Layout.fillWidth: true`

`                \}`

`            \}`

`        \}`

`    \}`

`\}`
```

### 6.2 Naming Conventions

| **Element** | **Convention** | **Example** |
| :-: | :-: | :-: |
| Components | PascalCase | `SettingsPanel.qml`, `TrayIcon.qml` |
| IDs | lowercase\_camelCase (descriptive) | `cpuCombo`, `powerSlider`, `control` |
| Signals | camelCase, past tense or noun | `profileChanged()`, `value` |
| Properties | camelCase | `currentIndex`, `enabled`, `text` |
| Signals/Handlers | camelCase | `onValueChanged`, `clicked: \{ \}` |

```
*`// Good ID names (descriptive)`*

`ComboBox \{ id: powerCombo \}`

`Button \{ id: saveButton \}`


*`// Bad ID names (ambiguous)`*

`ComboBox \{ id: combo \}`

`Button \{ id: btn \}`


*`// Signals`*

`signal settingsApplied(var settings)`

`signal errorOccurred(string message)`


*`// Handler methods`*

`function validateInput() \{ */\* ... \*/* \}`

`function applyChanges() \{ */\* ... \*/* \}`
```

### 6.3 Property Bindings

```
*`// ✅ CORRECT - Declarative binding (reactive)`*

`Label \{`

`    text: powerProfile === 0 ? qsTr("Standard") : qsTr("Plus")`

`    visible: powerSlider.value \> 50`

`\}`


*`// ❌ WRONG - Imperative assignment (not reactive)`*

`property int powerProfile: 0`

`Component.onCompleted: \{`

`    label.text = powerProfile === 0 ? "Standard" : "Plus"  *// Won't update!`*

`\}`


*`// Use onXXX handlers for side effects`*

`powerSlider.onValueChanged: \{`

`    console.log("Power level changed to:", value)`

`    savePreference("power\_level", value)`

`\}`
```

### 6.4 Internationalization

```
*`// Always use qsTr() for user-visible strings`*

`Label \{`

`    text: qsTr("CPU Governor")`

`\}`


*`// For plural forms`*

`Text \{`

`    text: qsPluralN(`

`        "notification",`

`        "notifications",`

`        notificationCount`

`    )`

`\}`


*`// Contextual strings (when translation context is unclear)`*

`Button \{`

`    text: qsTrContext("Power profile", "Power management setting")`

`\}`


*`// For translators: add translator comments`*

*`/\*\*`*

` *\* @translators This text appears in the power management section`*

` *\* of the settings panel. It refers to the CPU governor mode.`*

` *\*/`*

`Label \{`

`    text: qsTr("CPU Governor")`

`\}`
```

### 6.5 Styling

```
*`// Use custom style properties for consistent theming`*

`import org.kde.spike.styles 1.0`


`SpikeStyle \{`

`    *// Reference style colors`*

`    color: SpikeStyle.colors.primary  *// Purple \#6d4aff`*

`    `

`    *// Apply to children`*

`    Rectangle \{`

`        color: parent.color`

`        border.color: SpikeStyle.colors.accent`

`        radius: SpikeStyle.radius.medium`

`    \}`

`\}`


*`// Or define in a central styles object`*

`QtObject \{`

`    id: theme`

`    `

`    readonly property color primary: "\#6d4aff"`

`    readonly property color accent: "\#00d4d4"`

`    readonly property int fontSize: 10`

`    readonly property int padding: 8`

`\}`
```


## 7. Commit Message Format

### 7.1 Structure

All commits must follow this format:

```
`\<type\>(\<scope\>): \<subject\>`


`\<body\>`


`\<footer\>`
```

**Example:**

```
`feat(config): add variant switching support`


`Add spike-config --state-set system variant command to enable`

`post-install variant switching without reinstallation.`


`Changes:`

`- New CLI command: spike-config --state-set system variant \<plus|standard\>`

`- spike-config --generate-all applies variant-specific settings`

`- Warning shown if variant doesn't match hardware recommendations`


`Related to: VARIANT-DIFFERENCES.md spec`

`Fixes: \#42`
```

### 7.2 Types

| **Type** | **Description** |
| :-: | :-: |
| `feat` | New feature |
| `fix` | Bug fix |
| `docs` | Documentation changes (non-spec) |
| `style` | Formatting, missing semicolons, etc. (no code change) |
| `refactor` | Code restructuring (no feature/bug-fix change) |
| `perf` | Performance improvements |
| `test` | Adding/updating tests |
| `chore` | Maintenance, build tools, etc. |
| `revert` | Reverting previous commits |

### 7.3 Scopes

Common scopes for Spike projects:

| **Scope** | **Meaning** |
| :-: | :-: |
| `config` | spike-config system |
| `shell` | Spike Shell / desktop |
| `installer` | Installation process |
| `rescue` | Disaster recovery tools |
| `detect` | Hardware detection |
| `memory` | ZRAM/swap management |
| `power` | Power profiles/governor |
| `multimedia` | PipeWire/VA-API/Firefox |
| `network` | NetworkManager/firmware |
| `security` | Security hardening |
| `privacy` | Privacy/telemetry removal |
| `boot` | GRUB/Plymouth/initramfs |
| `gui` | UI components |
| `cli` | Command-line interfaces |
| `core` | Core system/base |
| `(none)` | Global/unspecified scope |

### 7.4 Subject Line Rules

- **Imperative mood:** "Add feature" NOT "Added feature" 

- **First person omitted:** "Fix bug" NOT "I fix bug" 

- **No period at end** 

- **Max 50 characters** (soft limit) 

- **Lowercase after first word** (unless proper noun) 

- **Prefix with affected area:** "config: ", "shell: ", etc. 

```
`✅ Good subjects:`

`feat(memory): add ZRAM compression ratio logging`

`fix(installer): correct partition size calculation`

`docs(readme): clarify minimum hardware requirements`

`refactor(cli): extract hardware detection into separate module`


`❌ Bad subjects:`

`Fixed the config parsing bug`

`Adding new power management features`

`UPDATE: Changed something in shell`
```

### 7.5 Body Rules

- Wrap at 72 characters 

- Explain **what** and **why**, not **how** (code shows how) 

- Use bullet points for multiple changes 

- Reference related issues/docs 

```
`feat(power): implement adaptive power profiles`


`Currently, power profiles are static. This change introduces`

`adaptive behavior that adjusts settings based on:`


`- Battery percentage (critical threshold at 20%)`

`- Charging status (AC vs battery)`

`- Peripheral power states (Wi-Fi, BT, USB autosuspend)`


`Rationale:`

`- Extend battery life on portable systems`

`- Reduce unnecessary CPU throttling on AC power`

`- Align with Golden Rule \#9 (longevity focus)`


`Testing:`

`- Verified profile transitions at 50% and 20% thresholds`

`- Confirmed peripheral power states change correctly`

`- Monitored power consumption reduction on battery test machine`


`Related: POWER-MANAGEMENT.md`

`Addresses: \#78`
```

### 7.6 Footer Rules

- Reference issues with `Fixes: \#NN` or `Closes: \#NN` 

- Reference PRs with `See also: PR\#NN` 

- Breaklines between sections 

- Optionally include breaking change marker: 

```
`BREAKING CHANGE: spike-config CLI arguments changed`


`Old syntax: spike-config --set power governor schedutil`

`New syntax: spike-config --state-set power cpu\_governor schedutil`


`Migration script provided in docs/MIGRATION-GUIDE.md`


`Closes: \#91`
```

### 7.7 Signed-off-by (DCO)

All commits must include Developer Certificate of Origin signoff:

```
`feat(memory): optimize ZRAM allocation algorithm`


`Improved compression ratio by adjusting zstd parameters based`

`on available CPU cycles.`


`Signed-off-by: John Doe \<john@example.com\>`
```

AI agents should add signoffs when committing code they generated:

```
`feat(cli): implement variant switch command`


`Added spike-config --state-set system variant command.`


`Generated by: Lumo AI assistant`

`Signed-off-by: Lumo AI \<lumo@proton.me\>`

`Co-authored-by: BigRangaTech \<team@bigrangatech.org\>`
```


## 8. Documentation Conventions

### 8.1 Inline Comments

- Explain **why** the code exists, not **what** it does 

- Avoid obvious comments 

- Update comments when code changes 

```
*`// ✅ GOOD: Explains rationale`*

*`// ZRAM disabled for AMD A4 (\<2000 bogomips/core) due to`*

*`// excessive CPU overhead on compression/decompression`*

`if (bogomips\_per\_core \< 2000) \{`

`    disable\_zram();`

`\}`


*`// ❌ BAD: Obvious restatement`*

*`// Increment counter`*

`counter++;`


*`// ❌ BAD: Outdated/incorrect`*

*`// This function calculates the CPU governor based on RAM size`*

*`// (now based on bogomips, not RAM - see commit 3a2b1c4)`*
```

### 8.2 README Files

Every directory with substantial code should have a README.md:

```
`\# Directory Purpose`


`Brief description of what this directory contains.`


`\#\# Contents`


`| File | Purpose |`

`|------|---------|`

`| \`detector.cpp\` | CPU hardware detection logic |`

`| \`parser.cpp\` | Configuration file parser |`


`\#\# Entry Points`


`- \`main()\` in \`detector.cpp\` is the primary entry point`


`\#\# Dependencies`


`- Qt6 Core`

`- Standard C++17`


`\#\# Tests`


`Run unit tests:`

`\`\`\`bash`

`ctest --output-on-failure tests/detector`
```

## Related Documentation

- `src/cpp/README.md`: High-level architecture 

- `tests/cpp/README.md`: Testing guidelines 


`\#\#\# 8.3 Changelog Entries`


`When adding/removing/modifying features, update CHANGELOG.md:`


`\`\`\`markdown`

`\#\# Unreleased`


`\#\#\# Added`

`- feat(cli): spike-config --rollback \<entry-id\> to revert changes`

`- gui: Camera/mic kill switch in Settings → Privacy`


`\#\#\# Changed`

`- refactor(memory): Moved ZRAM detection into spike-config memory module`

`- BREAKING: CLI argument changed from --set to --state-set`


`\#\#\# Fixed`

`- fix(installer): Correct partition size calculation for \>1TB drives`

`- fix(gui): Panel icons not updating on theme change`


`\#\#\# Removed`

`- chore(deps): Removed dependency on deprecated library X`


`\#\# v0.2.0 (2026-XX-XX)`


`\#\#\# Added`

`- Initial release of spike-config with 8 modules`
```


## 9. Testing Conventions

### 9.1 Test File Naming

Match source file naming:

| **Source** | **Test File** |
| :-: | :-: |
| `cpu\_detector.cpp` | `test\_cpu\_detector.cpp` |
| `config\_parser.py` | `test\_config\_parser.py` |
| `hardware\_detect.sh` | `test\_hardware\_detect.sh` |

### 9.2 Test Structure

```
*`\# test\_config\_parser.py`*


`import pytest`

`from spike.config.parser import ConfigParser`

`from spike.config.validator import ValidationError`



`class TestConfigParser:`

`    """Tests for configuration file parser."""`


`    def test\_parse\_valid\_json(self, tmp\_path):`

`        """Verify valid JSON is parsed correctly."""`

`        config\_file = tmp\_path / "config.json"`

`        config\_file.write\_text('\{"version": 1, "variant": "standard"\}')`

`        `

`        parser = ConfigParser(str(config\_file))`

`        result = parser.parse()`

`        `

`        assert result\["version"\] == 1`

`        assert result\["variant"\] == "standard"`


`    def test\_parse\_invalid\_json(self, tmp\_path):`

`        """Verify invalid JSON raises exception."""`

`        config\_file = tmp\_path / "invalid.json"`

`        config\_file.write\_text('\{invalid json\}')`

`        `

`        parser = ConfigParser(str(config\_file))`

`        `

`        with pytest.raises(json.JSONDecodeError):`

`            parser.parse()`


`    def test\_missing\_file\_raises\_file\_not\_found(self, tmp\_path):`

`        """Verify missing file raises FileNotFoundError."""`

`        parser = ConfigParser(str(tmp\_path / "nonexistent.json"))`

`        `

`        with pytest.raises(FileNotFoundError):`

`            parser.parse()`
```

### 9.3 Coverage Requirements

- Minimum 80% line coverage for all modules 

- Critical paths (security, boot, installer) must have 95%+ coverage 

- All new code requires corresponding tests 


## 10. Code Review Checklist

Before submitting a merge request, verify:

### Functional

- Code works as intended 

- Edge cases handled 

- Error paths tested 

- No regressions introduced 

### Style

- Follows language conventions (this document) 

- Consistent with surrounding code 

- No trailing whitespace 

- Proper indentation 

### Documentation

- Header comment present 

- Docstrings for public functions/classes 

- Inline comments for non-obvious logic 

- CHANGELOG.md updated (if applicable) 

### Testing

- Tests added for new functionality 

- Tests pass locally 

- No test failures in CI 

- Coverage maintained/increased 

### Security

- No hardcoded secrets/credentials 

- Input validation present 

- Error messages don't leak sensitive data 

- Follows Golden Rule \#7 (data recoverability) 

### Sign-off

- DCO signed off 

- Commit messages follow format 

- References issues/PRs where applicable 


## 11. Quick Reference

### Bash

```
`set -euo pipefail`

`readonly VAR="constant"`

`local temp="local"`

`"$\{VAR\}"  *\# Always quote!`*

`\[\[ condition \]\] && true || false`
```

### Python

```
`from typing import Optional, List, Dict`

`def func(x: int) -\> Optional\[str\]: ...`

`"""Google-style docstring."""`

`try:`

`    pass`

`except SpecificException as e:`

`    raise OtherException("msg") from e`
```

### C++

```
`class ClassName : public Base \{`

`    Q\_OBJECT`

`public:`

`    explicit ClassName(QObject \*parent = nullptr);`

`    ~ClassName() override = default;`

`    QString publicMethod() const;`

`private:`

`    void privateMethod();`

`    QString m\_memberVar;`

`signals:`

`    void signalName(int value);`

`\};`
```

### QML

```
`Item \{`

`    id: control`

`    property alias text: label.text`

`    Label \{`

`        id: label`

`        text: qsTr("Hello")`

`    \}`

`    onClicked: \{`

`        *// Handler`*

`    \}`

`\}`
```


🐕 BigRangaTech

