# AGENTS.md

Canonical guidance for AI coding agents and maintainers working in this repo.
(`CLAUDE.md` is a pointer to this file.)

## First read

1. [README.md's "How this fits together"](./README.md#how-this-fits-together)
   — the boot chain (MBR → this bootloader → SoftDevice → application), why
   this repo is only the bootloader and not the Meshtastic application
   itself, and the three ways a new image gets installed (UF2, serial DFU,
   BLE OTA DFU). Read this before touching `main.c` or `usb/` — the
   Architecture section below assumes it.
2. The rest of `README.md` — boards supported, installation, the Meshtastic
   Android in-app upgrade flow, troubleshooting.
3. `CONTRIBUTING.md` — dev setup, PR process, adding a new board.
4. `changelog.md` — OTAFIX version history (2.1/2.2 at the top; everything
   below predates the OTAFIX fork).
5. The design invariants and gotchas below — do not violate them.

## What this is

Meshtastic's fork (org-owned since 2026-08) of
[oltaco's OTAFIX bootloader](https://github.com/oltaco/Adafruit_nRF52_Bootloader_OTAFIX),
itself a fork of `adafruit/Adafruit_nRF52_Bootloader` with OTA DFU
enhancements. It's the bootloader several nRF52-based Meshtastic devices
ship with, and the one the
[Meshtastic Android app](https://github.com/meshtastic/Meshtastic-Android)
can upgrade in-app over USB/serial (`feature/firmware` there).

Meshtastic's separate `Adafruit_nRF52_Bootloader` fork is a **different
lineage without** the OTAFIX patches — don't confuse the two.

This is C, built with a bare Makefile (the supported path) or `CMakeLists.txt`
(**not** fully wired — see the CMake gotcha below). Not PlatformIO.

## Build

You need `arm-none-eabi-gcc` (CI pins **12.3.Rel1** exactly — see the
toolchain gotcha below) and Python 3.

```sh
git submodule update --init --recursive   # lib/nrfx, lib/tinyusb, lib/uf2 — required, not vendored inline

python3 -m venv .venv && source .venv/bin/activate   # or: uv venv .venv && source .venv/bin/activate
pip install adafruit-nrfutil uritemplate requests intelhex setuptools

make BOARD=wiscore_rak4631_board all              # board = a directory name under src/boards/
make BOARD=wiscore_rak4631_board copy-artifact    # -> _bin/<board>/*.zip, *.hex, update-*.uf2
make BOARD=wiscore_rak4631_board clean

tools/build_all.py   # builds every board in src/boards/, prints pass/fail + flash/RAM size table
```

No lint, no test suite — this is a bootloader; the only correctness signal is
"does it compile for every board" (CI's job) and real hardware testing.

## Architecture

### Board abstraction: `src/boards/<board>/`

Each board directory can carry:
- `board.h` — pin/peripheral defines (`DISPLAY_PIN_SCK` gates the OLED code
  in `src/screen.c` and `src/images.c`; `BLEDIS_MANUFACTURER`/`BLEDIS_MODEL`
  are the real vendor name, e.g. RAKWireless — don't rebrand these).
  Defining `BANNER_TEXT` overrides the on-screen DFU banner default in
  `src/screen.c`.
- `board.mk` — Makefile board config (`MCU_SUB_VARIANT`, per-board
  `CFLAGS` like the BLE `DEVICE_NAME`).
- `board.cmake` — **only exists for 2 of 14 boards** (`heltec_t114`,
  `thinknode_m1`). `cmake -DBOARD=<anything else>` fails outright. Nobody
  uses the CMake path in practice (CI and `tools/build_all.py` both use
  `make`); don't assume feature parity between the two build systems.
- `pinconfig.c` — present on some boards, board-specific pin table.

`Makefile` and `CMakeLists.txt` both dynamically discover the board's files
under `src/boards/$(BOARD)/` — there is no separate hardcoded board list to
keep in sync in either build system (unlike the CI matrix, see below).

### Core bootloader (`src/`)

`main.c` is the entry point (MBR/SoftDevice handoff, DFU state machine
dispatch). `screen.c`/`images.c` render the OLED UF2/BLE-OTA screens, active
only when the board defines `DISPLAY_PIN_SCK` — today that's `heltec_t114`
alone. `dfu_ble_svc.c`/`dfu_init.c` are the BLE DFU service; `flash_nrf5x.c`
wraps flash writes. `usb/` is the USB MSC (UF2 drive) + CDC stack on top of
the vendored `lib/tinyusb`. `cmsis/` is ARM CMSIS headers.

### Vendored submodules (`lib/`)

`lib/nrfx`, `lib/tinyusb`, `lib/uf2` are git submodules, **not** vendored
copies — `git submodule update --init --recursive` is required before any
build. They are also **years out of date** (checked 2026-08-18: nrfx pinned
2019, tinyusb 2021, uf2 2020, all several years behind upstream HEAD).
`renovate.json` now surfaces bump PRs on a daily schedule instead of leaving
that invisible, but nothing bumps them automatically — these are
boot-critical, and a multi-year jump needs real hardware testing before
merge, not a bot auto-merge.

`lib/softdevice/` vendors Nordic's SoftDevice binaries directly (not a
submodule) — `SD_NAME`/`SD_VERSION` in `Makefile` select which one.

### CI (`.github/workflows/githubci.yml`)

A `set-matrix` job lists `src/boards/*/` and fans out a `build` job per
board (currently 14), on every PR and on `release: created`. Release events
additionally upload `.zip`/`.hex`/`update-*.uf2` per board as release
assets; PR runs just validate the compile and get 1-day artifact retention
(release runs keep 90).

Branch protection on `master` requires all 15 checks (`set-matrix` + 14
`build (<board>)` contexts) by literal name. **Adding, removing, or renaming
a board changes those context names** — branch protection does not update
itself; a human has to edit it too.

## Gotchas

- **ARM GCC version matters.** CI pins exactly `12.3.Rel1`. Much newer
  toolchains (verified: 15.2.Rel1) fail with `-Werror=array-bounds` in
  `lib/sdk11/components/libraries/bootloader_dfu/bootloader_settings.c`
  (a false positive from newer GCC's stricter analysis of a fixed
  MBR-address read) — verified by actually building with it. 13.3.Rel1
  compiles clean and is the closest verified-working version if you can't
  get the exact CI-pinned one.
- **MeshCore/Ripple content has been deliberately removed** from the docs
  (README, changelog) — this is Meshtastic's own branded fork now, not a
  place to re-add other companion-firmware documentation. If you're
  porting docs changes from upstream `oltaco`, check for this before
  copying anything verbatim.
- **This repo's `src/boards/*` folder names are internal-only.** Nothing
  cross-repo reads them — `Meshtastic-Android`'s
  `device_bootloader_ota_quirks.json` matches boards by the canonical
  `HardwareModel` protobuf enum (`hwModel`/`hwModelSlug`) and by firmware's
  PlatformIO target names, never by this repo's directory names. Renaming a
  board directory here for "consistency" achieves nothing functionally and
  risks breaking `UF2_BOARD_ID`/build-artifact filenames for no benefit.
- **Board gaps are tracked as issues, not guessed at.** `Meshtastic-Android`
  flags `NANO_G2_ULTRA` and `NOMADSTAR_METEOR_PRO` as needing bootloader
  upgrade support with no board here yet (issues #4, #5) — bringing up a
  new board needs real hardware to get `UF2_BOARD_ID`/VID-PID/pin defs
  right; don't fabricate a `board.h` without one.
