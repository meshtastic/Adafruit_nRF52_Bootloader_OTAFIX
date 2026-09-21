#!/usr/bin/env python3
"""Emit the Meshtastic factory-erase UF2.

A single UF2 block with family ID CFG_UF2_MESHTASTIC_ERASE_ID (0x4D455348, "MESH"),
read from src/usb/uf2/uf2cfg.h. The bootloader flashes
nothing from it: it erases its App Data reservation (the application's
LittleFS) and reboots into UF2 mode -- see src/usb/uf2/ghostfat.c and
src/usb/msc_uf2.c. The file is board-agnostic, so one copy serves every board.
Older bootloaders do not know the family and ignore the block.

    tools/make_factory_erase_uf2.py [OUTPUT]   (default: tools/meshtastic_factory_erase.uf2)
"""
import re
import struct
import sys
from pathlib import Path

UF2_MAGIC_START0 = 0x0A324655
UF2_MAGIC_START1 = 0x9E5D5157
UF2_MAGIC_END = 0x0AB16F30
UF2_FLAG_FAMILYID = 0x00002000
PAYLOAD_SIZE = 256      # the bootloader rejects any other payloadSize

REPO = Path(__file__).resolve().parent.parent
UF2CFG = REPO / "src" / "usb" / "uf2" / "uf2cfg.h"


def family_id() -> int:
    """CFG_UF2_MESHTASTIC_ERASE_ID, read from the header the bootloader compiles, so
    the two cannot drift apart."""
    m = re.search(r"^#define\s+CFG_UF2_MESHTASTIC_ERASE_ID\s+(0x[0-9A-Fa-f]+)", UF2CFG.read_text(), re.M)
    if not m:
        sys.exit(f"CFG_UF2_MESHTASTIC_ERASE_ID not found in {UF2CFG}")
    return int(m.group(1), 16)


FAMILY_ID = family_id()


def block() -> bytes:
    header = struct.pack("<IIIIIIII", UF2_MAGIC_START0, UF2_MAGIC_START1,
                         UF2_FLAG_FAMILYID, 0, PAYLOAD_SIZE, 0, 1, FAMILY_ID)
    return header + bytes(476) + struct.pack("<I", UF2_MAGIC_END)


def main() -> None:
    out = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).with_name("meshtastic_factory_erase.uf2")
    data = block()
    assert len(data) == 512
    out.write_bytes(data)
    print(f"wrote {out} ({len(data)} bytes, family 0x{FAMILY_ID:08X})")


if __name__ == "__main__":
    main()
