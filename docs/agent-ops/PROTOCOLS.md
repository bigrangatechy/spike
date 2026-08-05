# Spike — PROTOCOLS.md

## Standard Operating Procedures & Workflows

This document defines step-by-step protocols for common development, maintenance, and operational tasks. Agents and human contributors must follow these protocols to ensure consistency, safety, and traceability across all Spike activities.

> **Note:** These are procedural guides, not specifications. The authoritative specifications remain in the top-level docs (e.g., `CONFIGURATION.md`, `MEMORY.md`, etc.). Use this file as a checklist/workflow reference.


## 1. Documentation-First Protocol

**Purpose:** Ensure all changes are discussed, documented, and approved before implementation.

### 1.1 Before Writing Any Code

1. **Check existing documentation**

```
Search: DESIGN-DECISIONS.md, ARCHITECTURE.md, relevant spec file
```

2. **Determine if the decision already exists**

   - ✅ If documented → Implement as specified 

   - ❓ If undocumented → Proceed to step 3 

3. **Identify the decision type**

| **Type** | **Authority** | **Protocol** |
| :-: | :-: | :-: |
| Spec correction (typo, clarity) | Contributor | Fix, log in DECISIONS.md |
| Missing spec entry | Contributor | Draft spec section, BDFL review |
| Architectural change | BDFL only | Propose in issues, wait for approval |
| Golden Rules change | BDFL only | Requires explicit approval, DECISIONS.md entry |

4. **Create a proposal (if new decision needed)**

   - Open issue in GitHub/GitLab 

   - Title format: `proposal: <brief description>` 

   - Include: rationale, alternatives considered, impact assessment 

   - Tag with `decision-needed` 

5. **Wait for convergence**

   - Discussion until consensus or BDFL ruling 

   - No code written during discussion phase 

   - Document decisions in `DECISIONS.md` (when created) 

6. **Write/update specification**

   - Update relevant spec document(s) 

   - Add entry to `CHANGELOG.md` if applicable 

   - Get BDFL approval for spec completion 

7. **Begin implementation**

   - Only proceed after step 6 is complete 

   - Follow coding conventions (`CONVENTIONS.md`) 

   - Test thoroughly 

### 1.2 Handling Ambiguous Situations

```
┌─────────────────────────────────────────┐
│  Situation unclear? No spec covers it? │
└──────────────────┬──────────────────────┘
                   ▼
         ┌──────────────────┐
         │ Check DESIGN-    │
         │ DECISIONS.md for │
         │ related rationale│
         └────────┬─────────┘
                  ▼
    ┌───────────────────────────┐
    │ Still unclear?            │
    │ Check AGENTS.md cross-ref │
    └───────────┬───────────────┘
                ▼
      ┌─────────────────────┐
      │ Create clarification│
      │ issue or PR comment │
      └───────────┬─────────┘
                  ▼
    ┌───────────────────────────┐
    │ Wait for BDFL/contributor │
    │ guidance before proceeding│
    └───────────────────────────┘
```

### 1.3 Emergency Deviation (Rare)

**When allowed:** Only for critical security fixes or system-breaking bugs where waiting would cause harm.

**Protocol:**

1. Document deviation reason in commit message 

2. Apply hotfix 

3. Within 24 hours: propose formal spec update retroactively 

4. If rejected: revert changes, implement as specified 

```
Commit message footer:
EMERGENCY DEVIATION: Hotfix applied without prior spec due to [reason]
Retroactive proposal filed in issue #[NN]
Revert deadline: 2026-XX-XX (if proposal rejected)
```


## 2. Hardware Detection Protocol

**Purpose:** Ensure consistent, reproducible hardware detection across install and runtime.

### 2.1 CPU Detection Steps

```
# Step 1: Read lscpu output
cpu_info=$(lscpu 2>/dev/null) || return 1
# Step 2: Extract key fields
cpu_model=$(echo "$cpu_info" | grep "Model name:" | cut -d: -f2 | xargs)
cores=$(echo "$cpu_info" | grep "^CPU(s):" | cut -d: -f2 | xargs)
bogomips=$(echo "$cpu_info" | grep "BogoMIPS:" | cut -d: -f2 | xargs | cut -d'.' -f1)
# Step 3: Calculate bogomips per core
bogomips_per_core=$((bogomips / cores))
# Step 4: Classify
if [[ $bogomips_per_core -lt 2000 ]]; then
    classification="low-end"
elif [[ $bogomips_per_core -ge 2200 ]] && [[ $cores -ge 2 ]]; then
    classification="capable"
else
    classification="modern"
fi
# Step 5: Special case detection
if [[ "$cpu_model" =~ "Celeron" ]] || [[ "$cpu_model" =~ "Pentium" ]] || [[ "$cpu_model" =~ "Atom" ]]; then
    celeron_class="yes"
fi
# Step 6: Store in state store
spike-config --state-set cpu model "$cpu_model"
spike-config --state-set cpu cores "$cores"
spike-config --state-set cpu bogomips_per_core "$bogomips_per_core"
spike-config --state-set cpu classification "$classification"
```

### 2.2 Storage Detection Steps

```
# Step 1: List block devices
devices=$(lsblk -d -o NAME,TYPE,SIZE,ROTA --json)
# Step 2: Find primary storage (non-removable, largest)
primary_dev=$(echo "$devices" | jq -r '.blockdevices[] | select(.type=="disk" and .rota!=null) | {name, size, rota} | sort_by(.size) | last | .name')
# Step 3: Check storage type
rotational=$(lsblk -d -o NAME,ROTA -n "$primary_dev" | awk '{print $2}')
if [[ "$rotational" == "0" ]]; then
    storage_type="SSD"
elif [[ "$rotational" == "1" ]]; then
    storage_type="HDD"
else
    storage_type="unknown"
fi
# Step 4: Check for eMMC (NOT SUPPORTED)
if lsblk -d -o NAME,MODEL -n | grep -qi "eMMC"; then
    echo "ERROR: eMMC storage not supported (wear-out risk)" >&2
    exit 1
fi
# Step 5: Check minimum capacity
capacity_gb=$((capacity_bytes / 1024 / 1024 / 1024))
if [[ $capacity_gb -lt 128 ]]; then
    echo "WARNING: Less than 128GB recommended" >&2
fi
# Step 6: Determine mount flags
if [[ "$storage_type" == "SSD" ]]; then
    mount_flags="defaults,noatime"
elif [[ "$storage_type" == "HDD" ]]; then
    mount_flags="defaults,noatime,commit=60"
fi
# Step 7: Store in state store
spike-config --state-set storage type "$storage_type"
spike-config --state-set storage mount_flags "$mount_flags"
```

### 2.3 GPU Detection Steps

```
# Step 1: Detect GPU vendor
vendor=$(lspci | grep -i "vga\\|3d" | cut -d: -f3 | cut -d'(' -f1 | xargs)
# Step 2: Match driver
if [[ "$vendor" =~ "Intel" ]]; then
    driver="i915"
    vaapi_driver="intel-media-va-driver-non-free"
    libva_name="iHD"
elif [[ "$vendor" =~ "AMD" ]]; then
    driver="amdgpu"
    vaapi_driver="mesa-va-drivers"
    libva_name="radeonsi"
elif [[ "$vendor" =~ "NVIDIA" ]]; then
    driver="nvidia"
    vaapi_driver="vdpau-va-driver"
    libva_name="N/A"  *# Uses VDPAU, not VA-API
else
    driver="unknown"
fi
# Step 3: Check AV1 support (Intel N4020 = no AV1)
if [[ "$vendor" =~ "Intel" ]] && [[ "$cpu_model" =~ "N4020" ]]; then
    av1_support="no"
    firefox_av1_setting="false"
else
    av1_support="yes"
    firefox_av1_setting="true"
fi
# Step 4: Store in state store
spike-config --state-set gpu vendor "$vendor"
spike-config --state-set gpu driver "$driver"
spike-config --state-set gpu vaapi_driver "$vaapi_driver"
spike-config --state-set gpu av1_support "$av1_support"
```

### 2.4 Network Detection Steps

```
# Step 1: Detect network interfaces
interfaces=$(ip link show | grep -E "^[0-9]+:" | awk -F': ' '{print $2}' | grep -v "lo")
# Step 2: Check Wi-Fi
wifi_interfaces=""
for iface in $interfaces; do
    if ip link show "$iface" | grep -q "wl"; then
        wifi_interfaces="$wifi_interfaces $iface"
    fi
done
# Step 3: Check Ethernet
ethernet_interfaces=""
for iface in $interfaces; do
    if ip link show "$iface" | grep -q "en"; then
        ethernet_interfaces="$ethernet_interfaces $iface"
    fi
done
# Step 4: Check wireless firmware availability
firmware_loaded=yes
for fw in iwlwifi ath9k ath10k rtl8723de bcmwl; do
    if [[ ! -f "/lib/firmware/${fw}*" ]]; then
        firmware_loaded=no
        break
    fi
done
# Step 5: Store in state store
spike-config --state-set network wifi_interfaces "$wifi_interfaces"
spike-config --state-set network ethernet_interfaces "$ethernet_interfaces"
spike-config --state-set network firmware_loaded "$firmware_loaded"
```

### 2.5 Bluetooth Detection (Conditional)

```
# Only run if Bluetooth hardware detected
if systemctl cat bluetooth.service >/dev/null 2>&1; then
    if hciconfig -a >/dev/null 2>&1; then
        bt_detected="yes"

        *# Check codec support based on variant
        variant=$(spike-config --state-get system variant)
        if [[ "$variant" == "plus" ]]; then
            bt_codecs="SBC,AAC,LDAC,aptX"
        else
            bt_codecs="SBC"
        fi

        spike-config --state-set bluetooth detected "$bt_detected"
        spike-config --state-set bluetooth codecs "$bt_codecs"
    else
        bt_detected="no"
        spike-config --state-set bluetooth detected "$bt_detected"
    fi
else
    bt_detected="no"
    spike-config --state-set bluetooth detected "$bt_detected"
fi
```


## 3. Configuration Generation Protocol

**Purpose:** Ensure atomic, safe, validated configuration file generation.

### 3.1 Standard Generation Flow

```
┌─────────────────────┐
│ spike-config --     │
│ generate <module>   │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 1. Load state       │
│    store (JSON)     │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 2. Read template    │
│    (.tpl file)      │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 3. Substitute       │
│    {{variables}}    │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 4. Validate output  │
│    (syntax, empty   │
│    critical fields) │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 5. Write to .tmp    │
│    file             │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 6. fsync(.tmp)      │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 7. mv .tmp → final  │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 8. fsync(dir)       │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 9. Log to           │
│    changelog.json   │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│ 10. Reload service  │
│     (if configured) │
└─────────────────────┘
```

### 3.2 Atomic Write Implementation

```
# Python pseudo-code for atomic config write
import os
import json
from pathlib import Path
def atomic_write(filepath: Path, content: str) -> None:
    """Write file atomically using temp + rename pattern."""

    tmp_path = Path(f"{filepath}.tmp.{os.getpid()}")

    try:
        *# Step 1: Write to temp file
        tmp_path.write_text(content)

        *# Step 2: Flush to disk
        with open(tmp_path, 'a') as f:
            f.flush()
            os.fsync(f.fileno())

        *# Step 3: Rename (atomic on POSIX)
        os.rename(tmp_path, filepath)

        *# Step 4: Sync directory
        dir_fd = os.open(os.path.dirname(filepath), os.O_RDONLY | os.O_DIRECTORY)
        try:
            os.fsync(dir_fd)
        finally:
            os.close(dir_fd)

    except Exception as exc:
        *# Cleanup temp file on failure
        if tmp_path.exists():
            tmp_path.unlink()
        raise ConfigWriteError(f"Atomic write failed: {exc}") from exc
```

### 3.3 Validation Checklist

Before writing any generated config:

| **Check** | **Action** | **Fail Condition** |
| :-: | :-: | :-: |
| Template syntax | `grep -c '{{'` | Leftover `{{` un-substituted |
| Critical fields | Check required keys exist | Empty/missing mandatory values |
| Syntax validity | Run config-specific checker | Invalid GRUB syntax, JSON parse fail, etc. |
| Security scan | Check for secrets/credentials | Hardcoded passwords, tokens, keys |
| Variant consistency | Verify variant-specific values | Standard variant with Plus settings |

```
# Example validation for grub config
if grep -q '{{' /etc/default/grub; then
    echo "ERROR: Unsubstituted template variables in GRUB config" >&2
    exit 1
fi
# Example validation for JSON state store
if ! python3 -c "import json; json.load(open('$STATE_STORE'))"; then
    echo "ERROR: Invalid JSON in state store" >&2
    exit 1
fi
```

### 3.4 Rollback Procedure

```
# Step 1: Identify rollback target
entry_id=$(spike-config --changelog | jq -r '.[0].entry_id')
# Step 2: Execute rollback
spike-config --rollback "$entry_id"
# Step 3: Verify rollback succeeded
if [[ $? -ne 0 ]]; then
    echo "Rollback failed, attempting manual recovery" >&2
    *# Trigger state store regeneration
    spike-config --detect
    spike-config --generate-all
fi
# Step 4: Notify user
notify-send "Configuration rolled back" "Entry $entry_id restored"
```


## 4. Installer Protocol

**Purpose:** Safe, automated installation with data protection guarantees.

### 4.1 Pre-Install Checks

```
# Step 1: Verify live environment
if [[ "$(cat /proc/cmdline)" != *"live-media"* ]]; then
    echo "ERROR: Not running from live media" >&2
    exit 1
fi
# Step 2: Check storage capacity
target_disk="$1"
capacity_gb=$(get_disk_size_gb "$target_disk")
if [[ $capacity_gb -lt 128 ]]; then
    echo "ERROR: Minimum 128GB required (found ${capacity_gb}GB)" >&2
    exit 1
fi
# Step 3: Scan for existing OS/data
existing_os=$(detect_existing_os "$target_disk")
user_data=$(scan_user_data "$target_disk")
# Step 4: Present options to user
if [[ -n "$user_data" ]]; then
    echo "WARNING: User data detected on disk"
    echo "Options:"
    echo "  1. Backup data to USB and proceed"
    echo "  2. Cancel and backup manually"
    read -p "Selection: " choice

    if [[ "$choice" != "1" ]]; then
        exit 0  *# Cancel installation
    fi

    *# Trigger backup wizard
    run_backup_wizard "$user_data"
fi
```

### 4.2 Partitioning Protocol

```
# WARNING: This wipes ALL data on target disk
# Must be explicit, confirmed by user
target_disk="$1"
variant="$2"  *# standard or plus
# Step 1: Wipe partition table
wipefs -a "$target_disk"
partprobe "$target_disk"
# Step 2: Create partitions
case "$firmware_type" in
    UEFI)
        parted "$target_disk" mklabel gpt
        parted "$target_disk" mkpart ESP fat32 1MiB 513MiB
        parted "$target_disk" set 1 boot on
        mkfs.vfat -F32 "${target_disk}1"
        ;;
    BIOS)
        parted "$target_disk" mklabel msdos
        parted "$target_disk" mkpart primary 1MiB 1025MiB
        mkfs.ext4 "${target_disk}1"
        ;;
esac
# Step 3: Create root partition
case "$firmware_type" in
    UEFI)
        parted "$target_disk" mkpart root ext4 513MiB 100%
        mkfs.ext4 "${target_disk}2"
        root_part="${target_disk}2"
        ;;
    BIOS)
        parted "$target_disk" mkpart root ext4 1025MiB 100%
        mkfs.ext4 "${target_disk}2"
        root_part="${target_disk}2"
        ;;
esac
# Step 4: Create swapfile (deferred to post-install)
# Note: Using swapfile instead of swap partition for flexibility
# Step 5: Mount filesystems
mkdir -p /mnt/spike
mount "$root_part" /mnt/spike
mkdir -p /mnt/spike/boot/efi
mount "${target_disk}1" /mnt/spike/boot/efi  *# UEFI only
# Step 6: Record partition mapping for post-install
cat > /mnt/spike/etc/spike/partitions.conf << EOF
boot_device=${target_disk}1
root_device=$root_part
swap_size=8GB
filesystem=ext4
mount_flags=$(get_mount_flags_for_storage_type)
EOF
```

### 4.3 Base System Installation

```
# Step 1: Copy base system (from ISO, faster than debootstrap)
rsync -avHAX --progress /spike-base/ /mnt/spike/
# Step 2: Install kernel
cp /boot/vmlinuz /mnt/spike/boot/
cp /boot/initrd.img /mnt/spike/boot/
# Step 3: Generate fstab
cat > /mnt/spike/etc/fstab << EOF
${target_disk}1  /boot/efi  vfat  defaults,noatime  0  2
$root_part       /          ext4  ${mount_flags}    0  1
/swapfile        none       swap  sw                0  0
EOF
# Step 4: Chroot and configure
mount --bind /dev /mnt/spike/dev
mount --bind /proc /mnt/spike/proc
mount --bind /sys /mnt/spipe/sys
chroot /mnt/spike /bin/bash << 'CHROOT_SCRIPT'
    # Set hostname
    echo "$hostname" > /etc/hostname

    # Create user account
    useradd -m -G sudo -s /bin/bash "$username"
    echo "$username:$password" | chpasswd

    # Lock root account
    passwd -l root

    # Configure timezone
    ln -sf "/usr/share/zoneinfo/$timezone" /etc/localtime

    # Install Flatpak
    apt-get update
    apt-get install -y flatpak

    # Add Flathub remote
    flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

    # Pre-seed runtimes (based on variant)
    if [[ "$variant" == "standard" ]]; then
        flatpak install -y org.kde.Platform//5.15 org.gnome.Platform//45
    else
        flatpak install -y org.kde.Platform//5.15 org.gnome.Platform//45 org.freedesktop.Platform//24.08
    fi

    # Install KDE standalone apps
    apt-get install -y dolphin konsole discover ark spectacle kate kcalc

    # Install Firefox Flatpak
    flatpak install -y org.mozilla.firefox

    # Strip telemetry components
    apt-get purge -y ubuntu-report apport whoopsie popularity-contest \\
                       landscape-client ubuntu-advantage-tools \\
                       motd-news-client cloud-init snapd

    # Configure ZRAM (based on CPU detection)
    configure_zram()

    # Configure power management
    configure_power()

    # Configure automatic updates
    configure_updates()

    # Generate spike-config state store
    spike-config --detect
    spike-config --generate-all
CHROOT_SCRIPT
# Step 5: Unmount chroot
umount /mnt/spike/dev
umount /mnt/spike/proc
umount /mnt/spike/sys
# Step 6: Install GRUB
case "$firmware_type" in
    UEFI)
        grub-install --target=x86_64-efi --efi-directory=/mnt/spike/boot/efi --bootloader-id=Spike
        ;;
    BIOS)
        grub-install --target=i386-pc "$target_disk"
        ;;
esac
# Step 7: Generate GRUB config
chroot /mnt/spike update-grub
# Step 8: Install Plymouth theme
install_plymouth_theme "$variant"
# Step 9: Unmount filesystems
umount /mnt/spike/boot/efi
umount /mnt/spike
```

### 4.4 Post-Install First Boot

```
# First-boot script (runs via systemd FirstBoot.target)
# Step 1: Welcome wizard
if [[ ! -f /home/"$username"/.config/spike/first_run_completed ]]; then
    /usr/bin/spike-first-run-wizard
fi
# Step 2: Hardware detection confirmation
hardware_report=$(generate_hardware_report)
echo "$hardware_report" > /home/"$username"/Documents/Spike_Hardware_Report.txt
notify-send "Welcome to Spike" "Hardware report saved to Documents/"
# Step 3: Check for firmware updates
if apt-get check-firmware-update; then
    notify-send "Firmware Update Available" "Visit Software Center to update"
fi
# Step 4: Mark first boot complete
touch /home/"$username"/.config/spike/first_run_completed
chown "$username":"$username" /home/"$username"/.config/spike/first_run_completed
```


## 5. Disaster Recovery Protocol

**Purpose:** Systematic response to boot failures and system corruption.

### 5.1 Layer 1 — Boot Failure Counter

```
# Script: /usr/lib/spike/boot-count-handler.sh
COUNT_FILE="/var/lib/spike/boot/count"
# Step 1: Read current count
current_count=$(cat "$COUNT_FILE" 2>/dev/null || echo 0)
# Step 2: Increment
((current_count++))
echo "$current_count" > "$COUNT_FILE"
# Step 3: Check threshold
if [[ $current_count -ge 3 ]]; then
    *# Trigger GRUB automatic recovery mode
    echo "3 consecutive boot failures detected" >> /var/log/spike/boot-failures.log

    *# Create marker file for GRUB
    touch /run/spike/force_recovery_mode

    *# Reset count on successful boot (done in Layer 2)
else
    echo "$current_count" > "$COUNT_FILE"
fi
# Step 4: If successful boot completes, reset counter
# (triggered by FirstBoot.target after 5 minutes of uptime)
if uptime -s | awk '{print $1}' == "$(date +%Y-%m-%d)"; then
    echo "0" > "$COUNT_FILE"
fi
```

### 5.2 Layer 2 — Recovery Mode Procedure

```
User boots into GRUB → Selects "Spike (recovery mode)"
┌─────────────────────────────────────────┐
│ Recovery Menu:                          │
│ ┌─────────────────────────────────────┐ │
│ │ 1. dpkg — Repair broken packages    │ │
│ │ 2. fsck — Check filesystem          │ │
│ │ 3. network — Enable networking      │ │
│ │ 4. root — Drop to root shell        │ │
│ │ 5. clean — Free disk space          │ │
│ │ 6. menu — Return to main menu       │ │
│ │ 7. resume — Boot normally           │ │
│ └─────────────────────────────────────┘ │
└─────────────────────────────────────────┘
```

**Root shell commands:**

```
# Reset boot counter
echo "0" > /var/lib/spike/boot/count
# Rollback configuration
spike-config --rollback $(spike-config --changelog | jq -r '.[0].entry_id')
# Rebuild initramfs
update-initramfs -u -k all
# Check filesystem
fsck -y /dev/nvme0n1p2
# Reset permissions
chmod 755 /home
chown -R username:username /home/username
# Exit and resume
exit
```

### 5.3 Layer 3 — Live ISO Rescue

```
Boot from Spike USB → Select "Rescue my data"
┌─────────────────────────────────────────┐
│ Rescue Wizard:                          │
│                                         │
│ 1. Scanning for data...                 │
│    ✓ Found: /dev/nvme0n1p2 (Spike)     │
│    ✓ Found: /dev/sdb1 (Windows)        │
│                                         │
│ 2. Selecting data to recover...         │
│    ☑ Documents                         │
│    ☑ Pictures                          │
│    ☑ Videos                            │
│    ☑ Music                             │
│    ☑ Downloads                         │
│    ☐ Applications (system configs)     │
│                                         │
│ 3. Selecting destination...             │
│    Target: /media/user/USB_BACKUP      │
│    Space required: 4.2 GB              │
│    Space available: 28.7 GB            │
│                                         │
│ [Begin Recovery]                        │
└─────────────────────────────────────────┘
```

**Rescue script:**

```
# Step 1: Mount source read-only (prevent further damage)
mount -o ro,noload /dev/nvme0n1p2 /mnt/source
# Step 2: Scan for user data directories
data_dirs=(
    "/mnt/home/username/Documents"
    "/mnt/home/username/Pictures"
    "/mnt/home/username/Videos"
    "/mnt/home/username/Music"
    "/mnt/home/username/Downloads"
    "/mnt/home/username/Desktop"
)
# Step 3: Copy with verification
for dir in "${data_dirs[@]}"; do
    if [[ -d "$dir" ]]; then
        rsync -avH --checksum "$dir/" "/media/user/USB_BACKUP/$(basename $dir)/"

        *# Verify with checksum
        sha256sum "$dir" > "/media/user/USB_BACKUP/checksums.txt"
    fi
done
# Step 4: Report results
total_copied=$(du -sh /media/user/USB_BACKUP | cut -f1)
echo "Recovery complete: ${total_copied} recovered"
notify-send "Recovery Complete" "${total_copied} data recovered to USB"
```

### 5.4 Layer 4 — Reinstall with Restore

```
Installer detects existing Spike installation
┌─────────────────────────────────────────┐
│ Existing Installation Detected          │
│                                         │
│ Found Spike on /dev/nvme0n1p2           │
│                                         │
│ Options:                                │
│ ○ Fresh install (delete everything)    │
│ ● Fresh install and restore my data    │
│ ○ Cancel                                │
│                                         │
│ If you choose restore:                  │
│ - Personal files will be preserved     │
│ - System configs will be fresh         │
│ - App configs will be reset            │
│                                         │
│ [Continue]                              │
└─────────────────────────────────────────┘
```

**Restore procedure (post-install):**

```
# Step 1: Check for SpikeBackup folder
backup_source="/media/user/USB_BACKUP"
if [[ ! -d "$backup_source" ]]; then
    echo "No backup found, skipping restore"
    exit 0
fi
# Step 2: Restore personal files
restore_dirs=(Documents Pictures Videos Music Downloads Desktop)
for dir in "${restore_dirs[@]}"; do
    if [[ -d "$backup_source/$dir" ]]; then
        rsync -av "$backup_source/$dir/" "/home/username/$dir/"
    fi
done
# Step 3: Fix ownership/permissions
chown -R username:username /home/username/Documents
chown -R username:username /home/username/Pictures
# ... repeat for all dirs
# Step 4: Verify restoration
restored_total=$(du -sh /home/username/Documents /home/username/Pictures ...)
notify-send "Installation Complete" "Restored ${restored_total} of personal data"
```


## 6. Testing Protocol

**Purpose:** Ensure all code changes are validated before merging.

### 6.1 Pre-Merge Checklist

```
┌─────────────────────────────────────────┐
│ Pre-Merge Verification:                 │
│                                         │
│ □ Unit tests pass (pytest/ctest)       │
│ □ Integration tests pass               │
│ □ Lint checks pass (shellcheck, pylint)│
│ □ Code coverage ≥80%                   │
│ □ No new warnings in compiler output   │
│ □ Manual test on target hardware       │
│ □ Documentation updated                │
│ □ CHANGELOG.md entry added             │
│ □ DCO signed off                       │
│                                         │
│ [Submit Merge Request]                  │
└─────────────────────────────────────────┘
```

### 6.2 CI/CD Pipeline Stages

```
# .gitlab-ci.yml stages
stages:
  - lint
  - test
  - build
  - package
lint:
  stage: lint
  script:
    - shellcheck *.sh
    - pylint spike/**/*.py
    - clang-format --dry-run src/**/*.cpp
test:
  stage: test
  script:
    - pytest tests/python --cov=spike --cov-report=xml
    - ctest --output-on-failure tests/cpp
    - bash tests/shell/run_all_tests.sh
  artifacts:
    reports:
      coverage_report: coverage.xml
build:
  stage: build
  script:
    - cmake -B build -DCMAKE_BUILD_TYPE=Release
    - cmake --build build
  artifacts:
    paths:
      - build/
package:
  stage: package
  script:
    - cpack -G DEB
  artifacts:
    paths:
      - spike-*.deb
```

### 6.3 Hardware Test Matrix

| **Test** | **Celeron N4020** | **AMD A4** | **ThinkPad P50** | **Frequency** |
| :-: | :-: | :-: | :-: | :-: |
| Boot time | ✅ | ✅ | ✅ | Every release |
| Idle memory (Standard) | ✅ | N/A | ✅ | Every release |
| Idle memory (Plus) | ✅ | N/A | ✅ | Every release |
| VA-API video playback | ✅ | ⚠️ | ✅ | Every release |
| Power profiles | ✅ | ✅ | ✅ | Every release |
| ZRAM compression | ✅ | ❌ (skipped) | ✅ | Every release |
| Hibernate/resume | ✅ | ✅ | ✅ | Weekly |
| Install (fresh) | ✅ | ✅ | ✅ | Every release |
| Install (with restore) | ✅ | ✅ | ✅ | Every release |
| Recovery layers 1-4 | ✅ | ✅ | ✅ | Every release |

### 6.4 Performance Baseline Testing

```
# Script: benchmarks/run_all.sh
# Boot time
start_time=$(date +%s%N)
reboot
# Wait for login screen
end_time=$(date +%s%N)
boot_time_ns=$((end_time - start_time))
boot_time_ms=$((boot_time_ns / 1000000))
echo "Boot time: ${boot_time_ms}ms" >> /var/log/spike/benchmarks.log
# Idle memory
idle_memory=$(free -m | awk '/^Mem:/ {print $3}')
echo "Idle memory (Standard): ${idle_memory}MB" >> /var/log/spike/benchmarks.log
# App launch times
firefox_launch=$(time -p firefox --headless 2>&1 | grep real | awk '{print $2}')
echo "Firefox launch: ${firefox_launch}s" >> /var/log/spike/benchmarks.log
dolphin_launch=$(time -p dolphin --version 2>&1 | grep real | awk '{print $2}')
echo "Dolphin launch: ${dolphin_launch}s" >> /var/log/spike/benchmarks.log
```


## 7. Security Review Protocol

**Purpose:** Systematic security assessment before deployment.

### 7.1 Pre-Deployment Checklist

| **Category** | **Item** | **Status** |
| :-: | :-: | :-: |
| Secrets | No hardcoded passwords/tokens | □ |
| Secrets | No API keys in source | □ |
| Input | All inputs validated | □ |
| Input | No command injection vectors | □ |
| Permissions | Files created with correct perms | □ |
| Permissions | No world-writable configs | □ |
| Network | No unexpected outbound connections | □ |
| Network | Firewall rules reviewed | □ |
| Updates | Dependencies checked for CVEs | □ |
| Updates | No deprecated/insecure libs | □ |
| Logging | No sensitive data in logs | □ |
| Logging | Logs don't leak user data | □ |

### 7.2 Dependency Vulnerability Scan

```
# Step 1: Scan Python dependencies
pip-audit requirements.txt
# Step 2: Scan system packages
apt-list-bugs --severity=high
# Step 3: Scan Flatpak runtimes
flatpak remote-info --verbose flathub | grep -i vulnerability
# Step 4: Generate SBOM (Software Bill of Materials)
cyclonedx-python -r requirements.txt -o sbom.json
```

### 7.3 Penetration Testing (Internal)

```
# Network attack surface analysis
sudo nmap -sV -sC localhost
# Port scan (should show only expected services)
sudo nmap -p- localhost
# Expected: None (no listening services by default)
# Service enumeration
systemctl list-unit-files --state=enabled
# Verify: Only expected services enabled
# Firewall audit
sudo ufw status verbose
# Verify: Incoming DENY, outgoing ALLOW, mDNS exception
```


## 8. Contribution Protocol

**Purpose:** Streamlined workflow for external contributors.

### 8.1 Phased Contribution Model

```
┌─────────────────────────────────────────┐
│ Pre-Alpha                               │
│ ├─ Repo: Read-only                      │
│ ├─ Contributions: Issues only           │
│ └─ Code: Not accepted                   │
└─────────────────────────────────────────┘
                  ▼
┌─────────────────────────────────────────┐
│ Alpha                                   │
│ ├─ Repo: Public                         │
│ ├─ Contributions: Bugs, translations,   │
│ │   hardware submissions                │
│ ├─ Patches: Case-by-case with DCO       │
│ └─ Review: BDFL/maintainer approval     │
└─────────────────────────────────────────┘
                  ▼
┌─────────────────────────────────────────┐
│ Beta                                    │
│ ├─ Repo: Public                         │
│ ├─ Contributions: Full (code, docs,     │
│ │   translations, features)             │
│ ├─ Review: Code review required         │
│ └─ Protection: Branch rules enforced    │
└─────────────────────────────────────────┘
```

### 8.2 Pull Request Protocol

1. **Fork repository** 

2. **Create feature branch** 

```
git checkout -b feat/<short-description>
```

3. **Make changes following conventions** 

4. **Write tests** 

5. **Update documentation** 

6. **Submit PR** 

```
Title: <type>(<scope>): <subject>
Body:
- What changes were made
- Why they were made
- How to test
- Related issues (Fixes: #NN)
```

7. **Wait for review** 

8. **Address feedback** 

9. **Merge (squash if approved)** 

### 8.3 Issue Reporting Protocol

```
Title: [Component] Short description
Description:
- Steps to reproduce
- Expected behavior
- Actual behavior
- System info (hardware, variant, version)
Attachments:
- Screenshots (if UI issue)
- Logs (from /var/log/spike/)
- Hardware report (from Documents/)
Labels: bug/enhancement/question/documentation
Priority: low/medium/high/critical
```


## 9. Emergency Response Protocol

**Purpose:** Rapid response to critical issues.

### 9.1 Severity Levels

| **Level** | **Definition** | **Response Time** | **Examples** |
| :-: | :-: | :-: | :-: |
| **Critical** | System-breaking, data loss risk | Immediate | Boot failure, data corruption, security exploit |
| **High** | Major functionality broken | ≤24 hours | Feature regression, security warning |
| **Medium** | Minor functionality affected | ≤72 hours | UI glitch, non-blocking bug |
| **Low** | Cosmetic, enhancement request | Next release cycle | Typo, feature suggestion |

### 9.2 Critical Incident Response

```
┌─────────────────────────────────────────┐
│ 1. Identify severity                    │
│    └─ Is it critical? (boot/data/sec)  │
└──────────────────┬──────────────────────┘
                   ▼ Yes
         ┌──────────────────┐
         │ 2. Communicate   │
         │    (issue, email │
         │    all maintainers) │
         └────────┬─────────┘
                  ▼
        ┌──────────────────┐
        │ 3. Contain       │
        │    (hotfix,      │
        │    rollback)     │
        └────────┬─────────┘
                 ▼
       ┌──────────────────┐
       │ 4. Diagnose      │
       │    (root cause)  │
       └────────┬─────────┘
                ▼
      ┌──────────────────┐
      │ 5. Fix           │
      │    (properly,    │
      │    with tests)   │
      └────────┬─────────┘
               ▼
     ┌──────────────────┐
     │ 6. Document      │
     │    (post-mortem, │
     │    DECISIONS.md) │
     └──────────────────┘
```

### 9.3 Security Vulnerability Handling

**Discovered vulnerability:**

1. **Report privately** (do not disclose publicly)

   - Email: [security@bigrangatech.com](mailto:security@bigrangatech.com) (encrypted) 

   - OR GitHub Security Advisory 

2. **Assess severity**

   - CVSS scoring 

   - Impact on target hardware 

3. **Develop patch**

   - Follow emergency protocol 

   - Test thoroughly 

4. **Disclose responsibly**

   - Coordinate with affected parties 

   - Public disclosure after patch available 

5. **Document**

   - Security advisory in CHANGELOG 

   - Lessons learned in MISTAKES.md 


🐕 BigRangaTech


