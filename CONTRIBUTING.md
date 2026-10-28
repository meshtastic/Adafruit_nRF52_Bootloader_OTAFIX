# Contributing

Thanks for considering a contribution. This is boot-critical firmware — a
broken bootloader can leave a device needing a hardware programmer to
recover — so a few things here are stricter than a typical repo.

## Before you start

- **Read [`AGENTS.md`](./AGENTS.md) first.** It covers the board abstraction,
  the build, the vendored submodules, and a list of known gotchas. This file
  covers process; that one covers the code.
- **New board support needs real hardware.** `UF2_BOARD_ID`, VID/PID, and
  pin definitions cannot be guessed from a datasheet alone — see
  [Adding a new board](#adding-a-new-board) below.
- **This repo is [MIT-licensed](./LICENSE)** (originally Adafruit
  Industries). By contributing, you agree your changes are under the same
  license.

## Building locally

You need `arm-none-eabi-gcc` and Python 3 installed.

```sh
git submodule update --init --recursive   # lib/nrfx, lib/tinyusb, lib/uf2 — required, not vendored inline

python3 -m venv .venv && source .venv/bin/activate
pip install adafruit-nrfutil uritemplate requests intelhex setuptools

# ARM GCC 12.3.Rel1 is what CI pins (.github/workflows/githubci.yml).
# 13.3.Rel1 also compiles clean; anything newer is unverified here. See
# AGENTS.md's Gotchas section before reaching for a newer toolchain.

make BOARD=wiscore_rak4631_board all
make BOARD=wiscore_rak4631_board copy-artifact   # writes _bin/<board>/
```

Board names are the directory names under `src/boards/`. `tools/build_all.py`
builds every board and prints a pass/fail + size table — run it before
opening a PR; it's the same check CI's board matrix does per-PR, just local.

There is no lint or test suite — for a bootloader, "does it compile for
every board" (CI's job) and real hardware testing are the correctness
signals that exist.

## Adding a new board

The [README's board list](./README.md#boards-supported) says to raise an
issue for a board you'd like supported — that's still the right first step,
and [#4](https://github.com/meshtastic/nRF52_Bootloader/issues/4)
and
[#5](https://github.com/meshtastic/nRF52_Bootloader/issues/5)
are open examples. To actually submit the board yourself:

1. You need the physical hardware to test on — a bootloader that compiles
   but was never flashed is not a contribution, it's a liability.
2. Add `src/boards/<your-board>/board.h` and `board.mk`, following an
   existing board (e.g. `src/boards/wiscore_rak4631_board`) as a template.
   `board.mk` carries the BLE OTA advertising name
   (`CFLAGS += -DDEVICE_NAME='"XXXX_DFU"'`) and, for a board whose firmware
   runs on S140 7.3.0, `SD_VERSION = 7.3.0` (the default is 6.1.1).
3. `UF2_BOARD_ID` must follow the format in the
   [UF2 spec](https://github.com/microsoft/uf2#files-exposed-by-bootloaders).
   If your board has an allocated VID/PID, link to it in the PR. The
   bootloader's self-update check keys on VID/PID alone, so a board that
   reuses another board's pair will accept that board's `update-` UF2 and
   the reverse; say so in the PR if that is the case.
4. Optional `board.h` features, each off unless defined:
   - A DFU screen: `DISPLAY_PIN_SCK` plus the other `DISPLAY_PIN_*` pins and
     `DISPLAY_CONTROLLER_ST7789` or `_ST7735` for an SPI TFT, or
     `DISPLAY_PIN_SDA`/`DISPLAY_PIN_SCL` and `DISPLAY_CONTROLLER_SSD1306` or
     `_SH1106` for a 128x64 I2C OLED (`DISPLAY_I2C_ADDR` names the panel's
     address, 0x3C by default; the bootloader probes it and its sibling).
     Never both buses: `boards.h` rejects it. `DISPLAY_TITLE` is the board name on the first line and
     `BANNER_TEXT` the last line. A board whose OLED is optional still opts
     in; a missing panel only means no picture.
   - `BUTTON_DFU_HOLD` and `BUTTON_DFU_HOLD_MS` for a board with one button
     and no usable RESET button: holding that button through boot enters
     UF2 mode, and the momentary `BUTTON_DFU`/`BUTTON_FRESET` checks are
     skipped. Pair it with `UF2_EXIT_ON_EJECT` so a host eject leaves DFU,
     since there is no reset button to leave by. `t1000_e` and
     `mesh_tracker_x1` are the examples.
   - Without `BUTTON_DFU_HOLD`, a `BUTTON_DFU` held through reset enters
     BLE OTA mode, never UF2 mode. `BUTTON_PULL` must match the board's
     wiring or the button reads as never (or always) pressed.
5. Confirm `make BOARD=<your-board> all` succeeds and the resulting UF2
   actually boots the board correctly, not just compiles.
6. Add the board to the [README's board table](./README.md#boards-supported)
   and [BLE advertising names table](./README.md#ble-advertising-names), to
   the board dropdown in `.github/ISSUE_TEMPLATE/bug_report.yml`, and to the
   unreleased section of `changelog.md`.
7. The PR template has a checklist for exactly this; fill it in.

A note on required checks: branch protection on `master` lists one
`build (<board>)` check per board by name, so a PR that adds a board cannot
merge until a maintainer adds its check to that list. CI itself picks the
new directory up on its own. Mention it in the PR so it is not forgotten.

## Pull requests

- CI (`.github/workflows/githubci.yml`) builds every board in the matrix on
  every PR. Branch protection on `master` requires all of those checks to
  pass before merge.
- Recent commit history is
  [Conventional Commits](https://www.conventionalcommits.org/)-style
  (`docs:`, `ci:`, `chore:`, etc.); older history (pre-fork) is looser —
  match the newer style going forward.
- Keep PRs scoped to one change. A board addition, a docs fix, and a CI
  tweak are three PRs, not one.

## Code of Conduct and security

This project follows the
[Meshtastic Code of Conduct](./CODE_OF_CONDUCT.md). Report security
vulnerabilities privately per [`SECURITY.md`](./SECURITY.md) — not as a
public issue.
