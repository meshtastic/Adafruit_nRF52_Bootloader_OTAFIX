# Contributing

Thanks for considering a contribution. This is boot-critical firmware — a
broken bootloader can leave a device needing a hardware programmer to
recover — so a few things here are stricter than a typical repo.

## Before you start

- **Read [`AGENTS.md`](./AGENTS.md) first.** It covers the board abstraction,
  the build system split (`make` vs. the incomplete `CMakeLists.txt`), the
  vendored submodules, and a list of known gotchas. This file covers
  process; that one covers the code.
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
# Newer versions (13.x verified working, 15.x does not) can hit a
# -Werror=array-bounds false positive in bootloader_settings.c — see
# AGENTS.md's Gotchas section before reaching for a newer toolchain.

make BOARD=wiscore_rak4631_board all
make BOARD=wiscore_rak4631_board copy-artifact   # writes _bin/<board>/
```

Board names are the directory names under `src/boards/`. `tools/build_all.py`
builds every board and prints a pass/fail + size table — run it before
opening a PR; it's the same check CI's board matrix does per-PR, just local.

**Use `make`, not `cmake`.** `CMakeLists.txt` only has a `board.cmake` for 4
of the 18 boards; `cmake -DBOARD=<anything else>` fails outright. Nobody
uses the CMake path in practice.

There is no lint or test suite — for a bootloader, "does it compile for
every board" (CI's job) and real hardware testing are the correctness
signals that exist.

## Adding a new board

The [README's board list](./README.md#boards-supported) says to raise an
issue for a board you'd like supported — that's still the right first step,
and [#4](https://github.com/meshtastic/Adafruit_nRF52_Bootloader_OTAFIX/issues/4)
and
[#5](https://github.com/meshtastic/Adafruit_nRF52_Bootloader_OTAFIX/issues/5)
are open examples. To actually submit the board yourself:

1. You need the physical hardware to test on — a bootloader that compiles
   but was never flashed is not a contribution, it's a liability.
2. Add `src/boards/<your-board>/board.h` and `board.mk`, following an
   existing board (e.g. `src/boards/wiscore_rak4631_board`) as a template.
3. `UF2_BOARD_ID` must follow the format in the
   [UF2 spec](https://github.com/microsoft/uf2#files-exposed-by-bootloaders).
   If your board has an allocated VID/PID, link to it in the PR.
4. Confirm `make BOARD=<your-board> all` succeeds and the resulting UF2
   actually boots the board correctly — not just compiles.
5. The PR template has a checklist for exactly this; fill it in.

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
