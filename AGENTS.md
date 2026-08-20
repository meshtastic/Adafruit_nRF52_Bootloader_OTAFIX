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
build. `renovate.json` surfaces bump PRs on a daily schedule, but nothing
bumps them automatically — these are boot-critical, and a version jump needs
real hardware testing before merge, not a bot auto-merge.

`lib/nrfx` is deliberately pinned to **v3.14.0**, not the latest commit
Renovate will keep proposing. nrfx 4.0 restructured the entire repo layout
(`mdk/` → `bsp/stable/mdk/`, among others) — a jump to it would need this
repo's Makefile `IPATH`s and every board's linker script reworked, not just
a digest bump. v3.14.0 is the last tag on the pre-4.0 layout, so it's the
practical ceiling for a same-day bump; going past it is a real project, not
a Renovate merge. If a future Renovate PR targets nrfx ≥4.0, that's this
gotcha firing — don't merge it without doing that rework.

`lib/tinyusb`'s `nrf5x` USB port (`dcd_nrf5x.c`) calls nrfx's chip-specific
errata functions (e.g. `nrf52_errata_199()`) — bumping tinyusb alone,
without nrfx at a version new enough to define them, fails to compile. The
two submodules move together, not independently, which is why Renovate's
separate per-submodule PRs (#11, #12 as originally filed) each failed CI on
their own; see the `linker/nrf_common.ld` gotcha below for the other half of
what that joint bump needed.

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

- **ARM GCC version matters.** CI pins exactly `12.3.Rel1`; 13.3.Rel1 also
  compiles clean and is the closest verified-working version if you can't
  get the exact CI-pinned one. GCC 15 used to fail with
  `-Werror=array-bounds` in
  `lib/sdk11/components/libraries/bootloader_dfu/bootloader_settings.c`
  (a false positive on a fixed MBR-address read); Adafruit's fix
  (`6b24be5`, a localised `#pragma GCC diagnostic`) is backported, but
  nobody has rebuilt here with GCC 15 since — if you do, update this note.
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
- **`linker/nrf_common.ld` needs `__data_start`/`__sdata_*`/`__tdata_*`/`__fast_*`
  symbols since nrfx 3.x.** `lib/nrfx/mdk/gcc_startup_<mcu>.S`'s data-copy loop
  changed from this repo's `__data_start__`-style (double underscore) names to
  plain `__data_start`, plus three new RAM-loaded regions (`.sdata`/`.tdata`/
  `.fast`) nothing in this codebase actually uses. **Do not** pull in nrfx's own
  updated `mdk/nrf_common.ld` to get these — it also redefines `.bss`/`.noinit`
  placement, which would silently fight this repo's board `.ld` files (e.g.
  `linker/nrf52840.ld`'s fixed-address `NOINIT` region used for BLE bond
  exchange across a DFU reset). The fix already applied is a handful of alias
  assignments plus zero-length filler for the three unused regions — extend
  that pattern, don't replace the file.
- **Board gaps are tracked as issues, not guessed at.** `Meshtastic-Android`
  flags `NANO_G2_ULTRA` and `NOMADSTAR_METEOR_PRO` as needing bootloader
  upgrade support with no board here yet (issues #4, #5) — bringing up a
  new board needs real hardware to get `UF2_BOARD_ID`/VID-PID/pin defs
  right; don't fabricate a `board.h` without one.
- **`CURRENT.UF2` dump-and-restore used to hang the device — fixed in #20,
  don't reintroduce it.** Root cause: `CURRENT.UF2` was sized off the max
  possible app region (`TRUE_USER_FLASH_SIZE`) instead of the real
  installed app, AND `msc_uf2.c`'s UF2-app-flash completion path never
  recorded the real app size into `bootloader_settings.bank_0_size` (stayed
  0 from a `memset`, only the DFU-serial protocol populated it). Together
  that meant restoring a `CURRENT.UF2` dump byte-for-byte could still hang
  the device on boot. Both fixed together in #20 (`ghostfat.c`'s
  `current_flash_size()` + `msc_uf2.c`'s `update_status.app_size`) —
  verified on real RAK4631 hardware, the same dump-and-restore sequence
  that hung now completes in ~2 seconds. If you change either of those two
  files, check this still holds.
