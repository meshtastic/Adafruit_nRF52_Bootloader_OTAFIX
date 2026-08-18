# Meshtastic OTAFIX Bootloader

Adafruit nRF52 bootloader with enhanced OTA DFU, forked for [Meshtastic](https://meshtastic.org) from [oltaco's OTAFIX bootloader](https://github.com/oltaco/Adafruit_nRF52_Bootloader_OTAFIX). This is the bootloader several nRF52-based Meshtastic devices ship with, and the one the [Meshtastic Android app](https://github.com/meshtastic/Meshtastic-Android) can upgrade in-app (see [Bootloader upgrade from the Meshtastic Android app](#bootloader-upgrade-from-the-meshtastic-android-app) below).

Current release: **OTAFIX 2.2** — see [changelog.md](changelog.md) for version history.

---

## Boards supported
- Elecrow ThinkNode M1
- Elecrow ThinkNode M3
- Elecrow ThinkNode M6
- Heltec Automation Mesh Node T114 / HT-nRF5262
- LilyGO T-Echo
- Minewsemi MX25LE01
- Nologo ProMicro NRF52840 (aka SuperMini NRF52840)
- RAK 4631 ([See note](#notes-on-rak4631-bootloader))
- RAK WisMesh Tag
- Seeed Studio SenseCAP Card Tracker T1000-E
- Seeed SenseCAP Solar Node P1
- Seeed Studio Wio Tracker L1
- Seeed Studio XIAO nRF52840 BLE ([See note](#notes-on-xiao-nrf52840-ble))
- Seeed Studio XIAO nRF52840 BLE SENSE

If there is another nRF52840-based Meshtastic board you would like to see supported, please raise a GitHub issue.

---

## Installation

The recommended way to install the bootloader is using the UF2 file.  
Download the UF2 file for your board (they can be found in the releases with filenames beginning with `update-`), enter UF2 mode (usually by double pressing the reset button within 0.5s) and copy the UF2 file across.

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
