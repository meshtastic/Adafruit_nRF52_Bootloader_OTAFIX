# Meshtastic nRF52 Bootloader

[![Build](https://github.com/meshtastic/nRF52_Bootloader/actions/workflows/githubci.yml/badge.svg)](https://github.com/meshtastic/nRF52_Bootloader/actions/workflows/githubci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)

Adafruit nRF52 bootloader with enhanced OTA DFU, forked for [Meshtastic](https://meshtastic.org) from [oltaco's bootloader](https://github.com/oltaco/Adafruit_nRF52_Bootloader_OTAFIX) and [Adafruit's](https://github.com/adafruit/Adafruit_nRF52_Bootloader). Formerly the OTAFIX bootloader. Several nRF52 Meshtastic devices ship with it, and the [Meshtastic Android app](https://github.com/meshtastic/Meshtastic-Android) can upgrade it in-app.

Current release: **OTAFIX 2.5** (tag `0.9.2-OTAFIX2.5`). `master` carries the unreleased **2.6.0**; later releases are tagged `x.y.z`. "2.6.0 and later" below means `master` and every release from 2.6.0. See [changelog.md](changelog.md).

## How this fits together

This repo builds only the bootloader, the small program that runs before anything else on the device. The Meshtastic application is built separately in [`meshtastic/firmware`](https://github.com/meshtastic/firmware). On every power-up or reset, Nordic's **MBR** hands off to the bootloader, which either boots the installed application or, when there is none or the user asks, waits for a new one.

An application image, or the bootloader itself, installs three ways:

- **UF2 drag-and-drop** over USB, where the device appears as a drive. See [Installation](#installation).
- **Serial DFU** over USB, for recovery and for flashing a full bootloader+SoftDevice package with `adafruit-nrfutil`.
- **BLE OTA DFU**, with no cable: the Meshtastic Android app's update and any Nordic DFU app. See [recommended settings](#recommended-ota-dfu-settings).

The UF2 drive also accepts the [factory-erase file](#factory-erase), which wipes the application's saved data and nothing else.

The application uses Nordic's **SoftDevice**, a precompiled Bluetooth stack vendored under `lib/softdevice/`. Each board builds against one version, S140 6.1.1 or 7.3.0, listed below. A UF2 built for the other SoftDevice is rejected (2.6.0 and later); `INFO_UF2.TXT` shows which one a device has.

---

## Boards supported

| Board | `BOARD=` name | SoftDevice | DFU screen |
| --- | --- | --- | --- |
| Elecrow ThinkNode M1 | `thinknode_m1` | S140 6.1.1 | |
| Elecrow ThinkNode M3 | `thinknode_m3` | S140 6.1.1 | |
| Elecrow ThinkNode M6 | `thinknode_m6` | S140 6.1.1 | |
| Heltec T096 | `heltec_t096` | S140 6.1.1 | SPI TFT |
| Heltec T1 | `heltec_t1` | S140 6.1.1 | SPI TFT |
| Heltec T114 / HT-nRF5262 | `heltec_t114` | S140 6.1.1 | SPI TFT |
| LilyGO T-Echo | `lilygo_techo` | S140 6.1.1 | |
| Minewsemi MX25LE01 | `minewsemi_mx25le01` | S140 7.3.0 | |
| MuziWorks Base Uno / Base Duo / Superbase | `muzi_base` | S140 6.1.1 | |
| Meshtastic Generic, S140 6.1.1 | `meshtastic_6.1.1` | S140 6.1.1 | |
| Meshtastic Generic, S140 7.3.0 | `meshtastic_7.3.0` | S140 7.3.0 | |
| Nologo ProMicro NRF52840 (aka SuperMini NRF52840) | `promicro_nrf52840` | S140 6.1.1 | I2C OLED |
| RAK 3401 | `wiscore_rak3401` | S140 6.1.1 | I2C OLED |
| RAK 4631 ([See note](#notes-on-rak4631-bootloader)) | `wiscore_rak4631_board` | S140 6.1.1 | I2C OLED |
| RAK WisMesh Tag | `wismesh_tag` | S140 6.1.1 | |
| Seeed Studio SenseCAP Card Tracker T1000-E ([See note](#notes-on-single-button-boards-t1000-e-and-meshtracker-x1)) | `t1000_e` | S140 7.3.0 | |
| Seeed Studio SenseCAP MeshTracker X1 ([See note](#notes-on-single-button-boards-t1000-e-and-meshtracker-x1)) | `mesh_tracker_x1` | S140 7.3.0 | |
| Seeed SenseCAP Solar Node P1 | `sensecap_solar_p1` | S140 7.3.0 | |
| Seeed Studio Wio Tracker L1 | `wio_tracker_l1` | S140 7.3.0 | I2C OLED |
| Seeed Studio Wio Tracker L1 Pro 1W | `wio_tracker_l1_pro_1w` | S140 7.3.0 | I2C OLED |
| Seeed Studio XIAO nRF52840 BLE ([See note](#notes-on-xiao-nrf52840-ble)) | `xiao_nrf52840_ble` | S140 7.3.0 | I2C OLED |
| Seeed Studio XIAO nRF52840 BLE SENSE | `xiao_nrf52840_ble_sense` | S140 7.3.0 | I2C OLED |

The `BOARD=` name is the directory under `src/boards/` and the prefix of the release asset names (`update-<name>_bootloader-<version>_nosd.uf2`, `<name>_bootloader-<version>_s140_<softdevice>.zip`). See [DFU screens](#dfu-screens) for the last column.

**Meshtastic Generic** (2.6.0 and later) is for DIY nRF52840 boards with no board-specific bootloader. Pick the variant whose SoftDevice your firmware expects. It drives no LED and its only button is reset (P0.18), so enter UF2 mode by double-pressing reset or from the application. The Board-IDs are `nRF52840-Meshtastic-Generic-S140v6` and `nRF52840-Meshtastic-Generic-S140v7`. Neither build has been run on hardware by the maintainers.

To request another board, [raise an issue](https://github.com/meshtastic/nRF52_Bootloader/issues/new/choose) or see [Adding a new board](./CONTRIBUTING.md#adding-a-new-board).

## BLE advertising names

In OTA DFU mode, devices advertise a board-specific name instead of the generic `AdaDFU`.

| Board                        | OTA DFU advertising name |
| ---------------------------- | ------------------------ |
| Elecrow ThinkNode M1         | `TNM1_DFU`               |
| Elecrow ThinkNode M3         | `TNM3_DFU`               |
| Elecrow ThinkNode M6         | `TNM6_DFU`               |
| Heltec T096                  | `T096_DFU`               |
| Heltec T1                    | `T1_DFU`                 |
| Heltec T114                  | `T114_DFU`               |
| LILYGO T-Echo                | `LGTE_DFU`               |
| Minewsemi MX25LE01           | `MX25_DFU`               |
| MuziWorks Base / Superbase   | `MUZI_DFU`               |
| Meshtastic Generic           | `MESH_DFU`               |
| ProMicro NRF52840            | `PROM_DFU`               |
| RAK 4631                     | `4631_DFU`               |
| RAK 3401                     | `3401_DFU`               |
| RAK WisMesh Tag              | `RTAG_DFU`               |
| Seeed MeshTracker X1         | `MTX1_DFU`               |
| Seeed SenseCAP Solar Node P1 | `SCAP_DFU`               |
| Seeed T1000e                 | `T1KE_DFU`               |
| Seeed WioTracker L1          | `WTL1_DFU`               |
| Seeed WioTracker L1 Pro 1W   | `WTLP_DFU`               |
| XIAO NRF52 BLE / SENSE       | `XIAO_DFU`               |

The device advertises at the application's BLE address plus one and exposes its bootloader version through the Device Information Service (firmware revision, `0x2A26`).

---

## Installation

The recommended way to install the bootloader is the UF2 file. Download the `update-` UF2 for your board from the [releases](https://github.com/meshtastic/nRF52_Bootloader/releases), enter UF2 mode and copy it across. The board installs it and reboots into the application; see [Updating the bootloader](#updating-the-bootloader).

If an incorrect bootloader was flashed, flash a full bootloader and SoftDevice zip package over serial DFU:

```sh
adafruit-nrfutil dfu serial --package <board>_bootloader-<version>_s140_<softdevice>.zip --port <serial port> -b 115200
```

The package replaces the SoftDevice, which marks the installed application invalid. On a USB host the device comes up as a UF2 drive showing NO FIRMWARE (2.6.0 and later); copy the firmware `.uf2` across next.

### Entering UF2 mode

- **Double-press reset** within half a second. The drive stays until you copy a file, eject, or reset. On a board with a valid application it also closes after 6 minutes with no activity (2.6.0 and later).
- **Hold the button through power-on** on the single-button boards, see the [single-button note](#notes-on-single-button-boards-t1000-e-and-meshtracker-x1).
- **From the application:** `meshtastic --enter-dfu` over USB, or the Meshtastic Android app's firmware-update screen on a USB connection. The board needs a USB host within 3 seconds, otherwise it boots the application again.
- **No valid application:** the device goes to the drive on its own whenever a USB host is attached (2.6.0 and later).

Holding the DFU button while the board resets enters BLE OTA mode, not UF2 mode, on every board except the single-button boards.

### The UF2 drive

The drive is named after the board (`RAK4631`, `XIAO-BOOT`, `T1000-E`, and so on) and holds three files:

- `INFO_UF2.TXT`: the bootloader version, board model and `Board-ID:`, the installed `SoftDevice:` version, a `Factory-Erase:` line if the bootloader accepts the [factory-erase file](#factory-erase), and a `Last-Fault:` line if the bootloader recorded a fault (2.6.0 and later), see [Troubleshooting](#troubleshooting).
- `CURRENT.UF2`: a dump of the installed application, see [Backing up](#backing-up-the-installed-firmware).
- `INDEX.HTM`: a redirect to the board vendor's page (the firmware repo on the generic boards).

Copy one `.uf2` file at a time. What happens next depends on the file:

- **A firmware image** is written as it arrives, with a progress bar on boards with a screen. When the file is complete the drive detaches and the board boots the new application. The old application is marked invalid when the first block is written, so an interrupted copy leaves no application and the drive comes back for a retry (2.6.0 and later).
- **A firmware image for the other SoftDevice** is rejected (2.6.0 and later): nothing is written, the drive stays up, and a board with a screen shows WRONG FILE. The check assumes the host writes front to back, as Linux does; macOS and Windows copies have not been exercised. Images that carry their own SoftDevice, and `CURRENT.UF2` restores, pass.
- **A bootloader update** (`update-*.uf2`): see [Updating the bootloader](#updating-the-bootloader).
- **The factory-erase file**: see [Factory erase](#factory-erase).
- **A UF2 of an unknown family**, or a bootloader update for a different USB ID, is rejected with WRONG FILE on a board with a screen. A rejected bootloader update ends the session: the drive detaches and the board boots the application if it has one. Files that are not UF2 are ignored. The bootloader cannot tell one board's firmware from another's on the same SoftDevice, so check the file name.

Routing when the device has **no valid application** (2.6.0 and later):

- On a USB host it comes up as the UF2 drive showing NO FIRMWARE.
- On battery (no VBUS) it goes straight to the BLE OTA wait.
- On a charger or power bank (VBUS, no host enumerating within 3 seconds) it goes to the BLE OTA wait.
- Pulling the cable while the drive is up sends it to the BLE OTA wait.
- Plugging a cable into a device in the BLE OTA wait, with no phone connected, brings the drive up. A cable already present when the wait began, or a connected phone, leaves the wait alone.
- A rejected or aborted copy, or an eject on a [single-button board](#notes-on-single-button-boards-t1000-e-and-meshtracker-x1), returns it to the drive.

A device **with** a valid application left in DFU mode boots it after 6 minutes with no activity (2.6.0 and later). A BLE connection or DFU transfer, a read or write of the drive, a USB host enumerating, or serial DFU traffic restarts the 6 minutes; an idle BLE link does not.

### Updating the bootloader

Copying `update-<board>_bootloader-<version>_nosd.uf2` onto the drive stages the new bootloader, checks it, and installs it over the running one; the board then reboots into the application. Boards with a screen show BOOTLOADER while it is staged.

The bootloader accepts an update whose USB vendor and product ID match its own. Several boards share an ID, so it installs the other board's file without complaint:

| USB ID | Boards that share it |
| --- | --- |
| `239A:0029` | RAK4631, RAK3401, T-Echo, WisMesh Tag, MX25LE01, Meshtastic Generic (both) |
| `239A:0071` | Heltec T096, T1, T114 |
| `239A:00DA` | ThinkNode M1, M3, M6 |
| `2886:0044` | SenseCAP Solar Node P1, XIAO nRF52840 BLE |
| `2886:0057` | T1000-E, MeshTracker X1 |
| `2886:1667` | Wio Tracker L1, L1 Pro 1W |

Check the file name against your board before copying. `Board-ID:` in `INFO_UF2.TXT` tells you which bootloader is installed.

An installed application larger than 744 KB on S140 6.1.1 or 740 KB on 7.3.0 would be overwritten by the staged bootloader, so it is marked invalid first and the drive shows NO FIRMWARE (2.6.0 and later); copy the firmware across again. Current Meshtastic firmware is smaller than this.

### Backing up the installed firmware

`CURRENT.UF2` is a byte-for-byte dump of the application in flash. Copy it off to keep a backup, and back onto the drive to reinstall it (verified on a RAK4631: restore boots and the next dump matches). If the application's size is unknown (after a debug-probe flash, for example), the dump covers the whole application region and still restores and boots.

### Factory erase

To wipe the device's saved data (config, node DB, keys, BLE bonds), enter UF2 mode and copy [`meshtastic_factory_erase.uf2`](./tools/meshtastic_factory_erase.uf2) (also on every [release](https://github.com/meshtastic/nRF52_Bootloader/releases)) onto the drive. The bootloader erases the application's data region, then reboots into UF2 mode. The application stays in place, so if you unplug instead, it boots factory-fresh. Copy this file on its own: the drive detaches about half a second after the copy (or when you eject) and returns a second or two later. Boards with a screen show ERASE.

The file is the same for every board: one UF2 block with family ID `0x4D455348` and no payload. OTAFIX 2.4 and later accept it, shown by a `Factory-Erase:` line in `INFO_UF2.TXT`. Regenerate it with `tools/make_factory_erase_uf2.py` (or `make factory-erase-uf2`).

---

## DFU screens

Boards marked in the [board table](#boards-supported) show a screen in DFU mode: the SPI TFT on the Heltec T096, T1 and T114, and (2.6.0 and later) a 128x64 I2C OLED at 0x3C or 0x3D on the others. The OLED controller (SSD1306 or SH1106) is detected; a board with no panel shows nothing and otherwise behaves the same. On the RAK4631 and RAK3401 the bootloader powers the switched 3V3_S rail that feeds the WisBlock slots while in DFU, and turns it off if no panel answers.

The OLED shows the board name, the mode (`USB UF2` or `BLE OTA`), the bootloader version, and a banner line that becomes a progress bar during a transfer. The TFT shows the same first and last lines with a drag-and-drop picture or `BLE OTA` between them.

| | |
| --- | --- |
| ![RAK4631 OLED in UF2 mode](docs/dfu_screen_oled_uf2.png) | ![RAK4631 OLED in BLE OTA mode](docs/dfu_screen_oled_ble.png) |

The first line says what the bootloader is doing (2.6.0 and later):

| Title | Meaning |
| --- | --- |
| `NO FIRMWARE` | DFU started because there is no valid application. Copy a firmware `.uf2` across, or run a BLE OTA update. |
| `WRONG FILE` | The last UF2 block was rejected: unknown family, a bootloader update for another USB ID, or a firmware image for the other SoftDevice. Nothing was written by that block. |
| `BOOTLOADER` | An `update-*.uf2` bootloader update is being staged. |
| `ERASE` | The factory-erase file was received. |

---

## Bootloader upgrade from the Meshtastic Android app

With the radio connected over **USB/serial** (not Bluetooth), open the [Meshtastic Android app](https://github.com/meshtastic/Meshtastic-Android)'s radio configuration, go to **Advanced → Firmware Update**, and where an upgraded bootloader is published for your board choose **Upgrade bootloader**. The app reads `INFO_UF2.TXT` to confirm the board and Bluetooth stack first, and asks you to select the update drive twice: once for the bootloader, once for the firmware. See its [Firmware Updates guide](https://github.com/meshtastic/Meshtastic-Android/blob/main/docs/en/user/firmware.md).

---

## Troubleshooting

### Device does not appear as a USB drive or serial port

The device may be **waiting in OTA DFU mode**: no UF2 drive or serial port, advertising as its [board-specific name](#ble-advertising-names). See [The UF2 drive](#the-uf2-drive) for when a device with no valid application lands here.

What to do:
- Run an OTA update with a supported DFU app, **or**
- Double-reset (or hold the button on a [single-button board](#notes-on-single-button-boards-t1000-e-and-meshtracker-x1)) to get UF2/serial mode, **or**
- Plug in a USB cable with no phone connected and the device comes up as the drive (2.6.0 and later). If the cable was already in when the wait began, replug it.

If the bootloader recorded a fault, `INFO_UF2.TXT` shows a `Last-Fault:` line with the fault id, program counter and info word (2.6.0 and later). It appears only if the next boot reaches the drive before an application boots. Include it in a bug report.

---

### OTA update fails with `Error: Operation Failed`

An OTA update that consistently fails early with `Error: Operation Failed` is often a BLE stack incompatibility with **Request High MTU** enabled.

What to try:
- Try different PRN settings: 12, 8, 1, or off.
- Disable **Request High MTU** in the DFU app. It improves speed but is not required.

---

### OTA update stops partway, or finishes and the device stays in the bootloader

If the phone loses the BLE link mid-transfer, the bootloader goes back to advertising and the phone can reconnect and start the transfer over without a reset (2.6.0 and later).

If an update reports success but the device keeps advertising as `<BOARD>_DFU`, or shows NO FIRMWARE on the drive, one known cause is a flash timeout while the bootloader saves the result. The write is retried every 100 ms for up to 2 seconds; if it still fails, `INFO_UF2.TXT` shows `Last-Fault: id=0x4001 pc=0x0 info=0xd` (2.6.0 and later). The transfer was fine, so running the update again completes it.

---

### The screen says WRONG FILE

The last UF2 block was not for this board: an unknown family, a bootloader update for a board with a different USB ID, or a firmware image for the other SoftDevice. Nothing was written by that block. A firmware image for another board on the same SoftDevice is not caught, so check the file name. Copy the right file; the drive is still up. See [The UF2 drive](#the-uf2-drive) and [Updating the bootloader](#updating-the-bootloader).

---

## Recommended OTA DFU settings

Use **nRF Device Firmware Update**
([Android](https://play.google.com/store/apps/details?id=no.nordicsemi.android.dfu&hl=en&gl=US) / [iOS](https://apps.apple.com/sa/app/device-firmware-update/id1624454660)),
the recommended app, or **nRF Connect**
([Android](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en&gl=US) / [iOS](https://apps.apple.com/gb/app/nrf-connect-for-mobile/id1054362403)).

Recommended settings (these may change; feel free to experiment and report findings via a GitHub issue):

<table>
<tr>
<td valign="top">

**Packet Receipt Notification (PRN):** ON  
**Number of packets:** 30  
**Reboot time:** 0ms  
**Scan timeout:** 2000ms  
**Request high MTU:** ON for Android (see notes below) / not available on iOS  
**Disable resume:** ON  
**Prepare object delay:** 0ms  
**Force scanning:** ON  
**Keep bond:** OFF  
**External MCU DFU:** OFF  

**Notes:**
- If the transfer fails early with `ERROR: Operation Failed`, retry with **Request high MTU** OFF.
- For maximum speed, disable PRN and raise the packet count. On iOS and other small-packet hosts, values above ~60 are not recommended.

</td>
</tr>
</table>

---

## Notes on Xiao NRF52840 BLE

Many of these boards ship with the Sense version of the bootloader installed. If yours does, use the Sense version when updating via UF2.

To check, enter UF2 mode (double-press reset) and open `INFO_UF2.TXT` on the drive. If it shows `Board-ID: nRF52840-SeeedXiaoSense-v1`, use the **Sense** variant.

The two variants have different USB product IDs (`2886:0044` and `2886:0045`), so the bootloader rejects the other variant's `update-` file.

## Notes on RAK4631 bootloader

This bootloader is based on Adafruit 0.9.2; RAK Wireless's official one is based on 0.6.2-11. Testing found no problems; whether RAK's own patches make a behavioral difference has not been investigated. A variant of the official RAK bootloader with these patches is [available](https://github.com/oltaco/WisCore_RAK4631_Bootloader/releases).

## Notes on single-button boards: T1000-E and MeshTracker X1

The SenseCAP Card Tracker T1000-E and MeshTracker X1 have a single user button, and RESET is wired to the MCU reset pin rather than a second button, so **double-press reset does not work**. From **OTAFIX 2.5** both enter UF2 mode by hold: with USB connected, hold the button while the device boots and keep holding for about 3 seconds. The drive mounts a few seconds later. Releasing the button before then boots the application as normal.

A short press does not enter DFU; the hold has to run through boot. A hold while the application is running belongs to the application (on the T1000-E a long press powers off). To boot with the button held, power on with the button down, or reboot from a client while holding it.

To leave DFU mode, eject the drive (`eject`, `udisksctl power-off`, or Finder/Explorer "Safely Remove"): the bootloader boots the application. Copying a `.uf2` across also reboots the board. With no valid application, an eject brings the drive back (2.6.0 and later). Unplugging does not leave DFU; on battery the board stays in the bootloader until the 6-minute inactivity timeout boots a valid application (2.6.0 and later).

Avoid the 1200 bps serial touch on these boards: it selects serial-only DFU, which exposes no drive and has no eject. On 2.6.0 and later the 6-minute inactivity timer applies to it too, so a board with a valid application boots on its own; that path has not been exercised on hardware.

To get the button path on a board with an older bootloader, enter UF2 mode from the application (`meshtastic --enter-dfu` or the Meshtastic Android app) and install `update-t1000_e_bootloader-*.uf2` (T1000-E) or `update-mesh_tracker_x1_bootloader-*.uf2` (X1) from the [releases](https://github.com/meshtastic/nRF52_Bootloader/releases). The boards share a USB ID, so the bootloader accepts the wrong board's file.

---

## Contributing

Want to build from source, add a board, or submit a fix? See [`CONTRIBUTING.md`](./CONTRIBUTING.md) for the development setup and PR process, and [`AGENTS.md`](./AGENTS.md) for how the codebase is put together.

## Getting help

- **Questions or troubleshooting:** [Meshtastic Discussions](https://github.com/orgs/meshtastic/discussions)
- **Bug reports and feature requests:** [open an issue](https://github.com/meshtastic/nRF52_Bootloader/issues/new/choose)
- **Security vulnerabilities:** see [`SECURITY.md`](./SECURITY.md), please do not open a public issue
- **General Meshtastic docs:** [meshtastic.org](https://meshtastic.org/)

## License

[MIT](./LICENSE), originally Copyright (c) 2016 Adafruit Industries.
