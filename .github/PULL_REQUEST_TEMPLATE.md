## Checklist

*By completing this PR sufficiently, you help us to review this Pull Request quicker and also help improve the quality of Release Notes.*
*See [`CONTRIBUTING.md`](../CONTRIBUTING.md) for the full dev setup and PR process.*

- [ ] PR title specifically describes the change
- [ ] `tools/build_all.py` (or CI's board matrix) passes
- [ ] If adding a new board, see [Adding a new board](../CONTRIBUTING.md#adding-a-new-board):
  - [ ] Tested on real hardware, not just compiled
  - [ ] Link to allocated VID/PID provided, if applicable
  - [ ] `UF2_BOARD_ID` in `board.h` follows the [UF2 spec](https://github.com/microsoft/uf2#files-exposed-by-bootloaders) format

*Checklist items that don't apply to your PR can be deleted.*

-----------

## Description of Change

Please describe your proposed Pull Request and it's impact.
