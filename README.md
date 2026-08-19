# Meshtastic OTAFIX Bootloader

[![Build](https://github.com/meshtastic/Adafruit_nRF52_Bootloader_OTAFIX/actions/workflows/githubci.yml/badge.svg)](https://github.com/meshtastic/Adafruit_nRF52_Bootloader_OTAFIX/actions/workflows/githubci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)

Adafruit nRF52 bootloader with enhanced OTA DFU, forked for [Meshtastic](https://meshtastic.org) from [oltaco's OTAFIX bootloader](https://github.com/oltaco/Adafruit_nRF52_Bootloader_OTAFIX). This is the bootloader several nRF52-based Meshtastic devices ship with, and the one the [Meshtastic Android app](https://github.com/meshtastic/Meshtastic-Android) can upgrade in-app.

Current release: **OTAFIX 2.2** — see [changelog.md](changelog.md) for version history.

## Contents

- [How this fits together](#how-this-fits-together)
- [Boards supported](#boards-supported)
- [BLE advertising names](#ble-advertising-names)
- [Installation](#installation)
- [Bootloader upgrade from the Meshtastic Android app](#bootloader-upgrade-from-the-meshtastic-android-app)
- [Troubleshooting](#troubleshooting)
- [Recommended OTA DFU settings](#recommended-ota-dfu-settings)
- [Notes on Xiao NRF52840 BLE](#notes-on-xiao-nrf52840-ble)
- [Notes on RAK4631 bootloader](#notes-on-rak4631-bootloader)
- [Contributing](#contributing)
- [Getting help](#getting-help)
- [License](#license)

---

## How this fits together

This repo builds **only the bootloader** — the small program that runs
before anything else on the device. It does not contain the Meshtastic
application (the LoRa mesh, BLE, and display code); that's built entirely
separately in [`meshtastic/firmware`](https://github.com/meshtastic/firmware).
This bootloader's job is just to start that application, or, when asked,
replace it (or itself) with a new version.

On every power-up or reset, a tiny fixed piece of code at the very start of
flash — Nordic's **MBR** — hands off to this bootloader. From there, the
bootloader either:

- boots straight into the installed Meshtastic application, or
- if no valid application is installed, or the user has asked for it, waits
  for a new one.

A new application image — or occasionally the bootloader itself — can be
installed three different ways:

| Method | Transport | Typical use |
|---|---|---|
| **UF2 drag-and-drop** | USB, appears as a drive | Manual flashing — see [Installation](#installation) |
| **Serial DFU** | USB, appears as a serial port | Recovery, and flashing a full bootloader+SoftDevice package with `adafruit-nrfutil` — see [Installation](#installation) |
| **BLE OTA DFU** | Bluetooth, no cable needed | The Meshtastic Android app's firmware/bootloader update, and any Nordic DFU app — see [below](#bootloader-upgrade-from-the-meshtastic-android-app) and [recommended settings](#recommended-ota-dfu-settings) |

BLE OTA DFU is the only *wireless* path, which is what the "OTA" in
"OTAFIX" refers to — and it's also the bootloader's default fallback: since
**OTAFIX 2.0**, if no valid application is present, the device waits in BLE
OTA mode automatically rather than risk getting stuck (see
[Troubleshooting](#troubleshooting)).

The application also relies on Nordic's **SoftDevice** — a closed-source,
precompiled Bluetooth stack (vendored here as a hex blob under
`lib/softdevice/`) that sits in flash alongside it. The bootloader and the
application both call into it for BLE, so bootloader and SoftDevice
versions are usually flashed together as a matched pair (that's why the
[Installation](#installation) instructions below mention flashing "a full
bootloader and SoftDevice zip package").

---

## Boards supported
- Elecrow ThinkNode M1
- Elecrow ThinkNode M3
- Elecrow ThinkNode M6
- Heltec T096
- Heltec T1
- Heltec T114 / HT-nRF5262
- LilyGO T-Echo
- Minewsemi MX25LE01
- Nologo ProMicro NRF52840 (aka SuperMini NRF52840)
- RAK 3401
- RAK 4631 ([See note](#notes-on-rak4631-bootloader)) — also covers NomadStar Meteor Pro, see the same note
- RAK WisMesh Tag
- Seeed Studio SenseCAP Card Tracker T1000-E
- Seeed SenseCAP Solar Node P1
- Seeed Studio Wio Tracker L1
- Seeed Studio XIAO nRF52840 BLE ([See note](#notes-on-xiao-nrf52840-ble))
- Seeed Studio XIAO nRF52840 BLE SENSE

If there is another nRF52840-based Meshtastic board you would like to see supported, please [raise a GitHub issue](https://github.com/meshtastic/Adafruit_nRF52_Bootloader_OTAFIX/issues/new/choose) — or see [Adding a new board](./CONTRIBUTING.md#adding-a-new-board) in `CONTRIBUTING.md` if you want to submit it yourself.

## BLE advertising names

When in OTA DFU mode, devices advertise using a board-specific name rather than the generic `AdaDFU`.

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
| ProMicro NRF52840            | `PROM_DFU`               |
| RAK 4631                     | `4631_DFU`               |
| RAK 3401                     | `3401_DFU`               |
| RAK WisMesh Tag              | `RTAG_DFU`               |
| Seeed SenseCAP Solar Node P1 | `SCAP_DFU`               |
| Seeed T1000e                 | `T1KE_DFU`               |
| Seeed WioTracker L1          | `WTL1_DFU`               |
| XIAO NRF52 BLE / SENSE       | `XIAO_DFU`               |

---

## Installation

The recommended way to install the bootloader is using the UF2 file.  
Download the UF2 file for your board (they can be found in the [releases](https://github.com/meshtastic/Adafruit_nRF52_Bootloader_OTAFIX/releases) with filenames beginning with `update-`), enter UF2 mode (usually by double pressing the reset button within 0.5s) and copy the UF2 file across.

If an incorrect bootloader has been flashed to the device, a full bootloader and SoftDevice zip package will need to be flashed using ``adafruit-nrfutil``.

---

## Bootloader upgrade from the Meshtastic Android app

The [Meshtastic Android app](https://github.com/meshtastic/Meshtastic-Android) can flash this bootloader directly — no manual UF2 drag-and-drop needed.

With the radio connected over **USB/serial** (not Bluetooth), open the connected radio's configuration, go to **Advanced → Firmware Update**, and where an upgraded bootloader is published for your board you'll see an **Upgrade bootloader** option alongside **Erase and reinstall**. The app reads `INFO_UF2.TXT` from the device's update drive first to confirm the board and Bluetooth stack before writing anything, and will ask you to select the update drive twice — once for the bootloader image, once for the firmware. See the app's [Firmware Updates guide](https://github.com/meshtastic/Meshtastic-Android/blob/main/docs/en/user/firmware.md) for the full flow.

---

## Troubleshooting

### Device does not appear as a USB drive or serial port

If the device does not show up on your computer after flashing the bootloader or performing an OTA update, it may be **waiting in OTA DFU mode**.

In **OTAFIX 2.0** and above, OTA DFU is the default state when no valid application is present.  
In this mode:
- No UF2 drive is exposed
- No serial port is available
- The device is waiting for an OTA firmware update over BLE

**What to do:**
- Perform an OTA update using a supported DFU app, **or**
- Explicitly request UF2/serial mode using **double-reset**.

This behaviour is intentional and prevents devices from getting stuck in UF2 mode after failed OTA updates.

---

### OTA update fails with `Error: Operation Failed`

If an OTA update consistently fails early with `Error: Operation Failed`, this is often caused by BLE stack incompatibilities when **Request High MTU** is enabled.

**What to try:**
- Experiment with different PRN settings — try 12, 8, 1, or off altogether.
- Disable **Request High MTU** in the DFU app.

While high MTU significantly improves performance on supported devices, it is not required for a successful OTA update.

---

## Recommended OTA DFU settings

To perform the OTA update, use **nRF Device Firmware Update**  
([Android](https://play.google.com/store/apps/details?id=no.nordicsemi.android.dfu&hl=en&gl=US) / [iOS](https://apps.apple.com/sa/app/device-firmware-update/id1624454660))  
or **nRF Connect**  
([Android](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en&gl=US) / [iOS](https://apps.apple.com/gb/app/nrf-connect-for-mobile/id1054362403)).

**nRF Device Firmware Update** is the recommended app of the two.

For **OTAFIX 2.0** and later, the following settings are recommended (these may change — feel free to experiment and report findings via a GitHub issue):

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
- Some Android devices and BLE stacks do not behave well with **Request high MTU** enabled.  
  If the transfer fails early with `ERROR: Operation Failed`, retry with **Request high MTU turned OFF**.
- For maximum speed, Packet Receipt Notification can be disabled, and the number of packets increased.  
  Android is generally more tolerant of higher values; on iOS and other small-packet hosts, values above ~60 are not recommended.

</td>
</tr>
</table>

[Recommended settings for versions prior to 2.0 can be found here](docs/oldsettings.md).

**IMPORTANT:**  
On <u>older versions</u> of the bootloader, performing an OTA update while the device was connected to a computer USB host would complete successfully but **would not automatically boot into the new application firmware**, requiring a manual reset.  
This issue is fixed in **OTAFIX 2.0**.

---

## Notes on Xiao NRF52840 BLE

Many of these boards are shipped with the Sense version of the bootloader installed. If your board has the Sense version installed you must use the Sense version when updating via UF2.

You can look at the INFO_UF2.TXT file on the UF2 drive to check what version is currently installed.

To check:
1. Enter UF2 DFU mode (double-press reset) 
2. Open the `INFO_UF2.TXT` file on the mounted drive  

If the file shows: "Board-ID: nRF52840-SeeedXiaoSense-v1" then the ***SENSE*** variant must be used if updating via UF2 file.

## Notes on RAK4631 bootloader

This version of the RAK4631 bootloader is based on a much newer version (0.9.2) of the Adafruit nRF52 bootloader than what RAK Wireless uses on their official bootloader (0.6.2-11). It has been tested with no problems found; whether RAK's own patches to the Adafruit bootloader introduce any behavioral difference has not been investigated. A variant of the official RAK bootloader with these patches included instead is available [here](https://github.com/oltaco/WisCore_RAK4631_Bootloader/releases).

**NomadStar Meteor Pro** is RAK4631-based hardware with an added RGBW LED and e-ink display bolted on via a WisBlock IO module. Diffing its [firmware variant](https://github.com/meshtastic/firmware/tree/master/variants/nrf52840/rak4631_nomadstar_meteor_pro) against plain RAK4631's shows only peripheral-level differences (the RGB LED driver chip, sensor I2C addresses, a buzzer pin, battery-sense calibration) — nothing this bootloader's MCU/flash/USB identity touches. It should be able to use this same RAK4631 UF2 unchanged, but **this has not been confirmed on real hardware** — if you have one, please try flashing it and report back on [issue #5](https://github.com/meshtastic/Adafruit_nRF52_Bootloader_OTAFIX/issues/5).

---

## Contributing

Want to build from source, add a board, or submit a fix? See
[`CONTRIBUTING.md`](./CONTRIBUTING.md) for the development setup and PR
process, and [`AGENTS.md`](./AGENTS.md) for how the codebase is put
together.

## Getting help

- **Questions or troubleshooting:** [Meshtastic Discussions](https://github.com/orgs/meshtastic/discussions)
- **Bug reports and feature requests:** [open an issue](https://github.com/meshtastic/Adafruit_nRF52_Bootloader_OTAFIX/issues/new/choose)
- **Security vulnerabilities:** see [`SECURITY.md`](./SECURITY.md) — please do not open a public issue
- **General Meshtastic docs:** [meshtastic.org](https://meshtastic.org/)

## License

[MIT](./LICENSE), originally Copyright (c) 2016 Adafruit Industries.
