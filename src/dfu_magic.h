#ifndef DFU_MAGIC_H_
#define DFU_MAGIC_H_

#include "bootloader_types.h"

/* Magic that written to NRF_POWER->GPREGRET by application when it wish to go into DFU
 * - DFU_MAGIC_OTA_APPJUM        : used by BLEDfu service, SD is already inited
 * - DFU_MAGIC_OTA_RESET         : entered by soft reset, SD is not inited yet
 * - DFU_MAGIC_SERIAL_ONLY_RESET : with CDC interface only
 * - DFU_MAGIC_UF2_RESET         : with CDC and MSC interfaces
 * - DFU_MAGIC_SKIP              : skip DFU entirely including double reset delay,
 *                                 Can be used with systemoff or quick reset to app
 *
 * Note: for DFU_MAGIC_OTA_APPJUM Softdevice must not initialized.
 * since it is already in application. In all other case of OTA SD must be initialized
 */
#define DFU_MAGIC_OTA_APPJUM            BOOTLOADER_DFU_START  // 0xB1
#define DFU_MAGIC_OTA_RESET             0xA8
#define DFU_MAGIC_SERIAL_ONLY_RESET     0x4e
#define DFU_MAGIC_UF2_RESET             0x57
#define DFU_MAGIC_SKIP                  0x6d

#endif /* DFU_MAGIC_H_ */
