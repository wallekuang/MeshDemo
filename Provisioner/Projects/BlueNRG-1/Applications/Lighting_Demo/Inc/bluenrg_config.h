/**
******************************************************************************
* @file    appli_mesh.h
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Header file for the user application file 
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Initial BlueNRG-Mesh is built over Motorola�s Mesh over Bluetooth Low Energy 
* (MoBLE) technology. The present solution is developed and maintained for both 
* Mesh library and Applications solely by STMicroelectronics.
*
******************************************************************************
*/

#ifndef _APPLI_CONFIG_H_
#define _APPLI_CONFIG_H_

#include "bluenrg1_stack.h"
#include "stack_user_cfg.h"
#ifdef BLUENRG2_DEVICE
#include "OTA_btl.h"
#endif
#include "mesh_cfg.h"

/* This file contains all the information needed to init the BlueNRG-1 stack. 
 * These constants and variables are used from the BlueNRG-1 stack to reserve RAM and FLASH 
 * according the application requests
 */

/* Default number of link */
#define MIN_NUM_LINK                   2

/* Default number of GAP and GATT services */
#define DEFAULT_NUM_GATT_SERVICES      2
/* Default number of GAP and GATT attributes */
#define DEFAULT_NUM_GATT_ATTRIBUTES    9

/* Enable/disable Data length extension Max supported ATT_MTU size based on OTA client & server Max ATT_MTU sizes capabilities */
#if (CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED == 1) && (OTA_EXTENDED_PACKET_LEN == 1) 
  #define OTA_MAX_ATT_MTU_SIZE         (OTA_ATT_MTU_SIZE) //(220) /* OTA Client & Server supported ATT_MTU */   
#else /* BlueNRG-1 device: no data length extension support */
  #define OTA_MAX_ATT_MTU_SIZE         (DEFAULT_ATT_MTU)          /* DEFAULT_ATT_MTU size = 23 bytes */ 
#endif  

#if defined (ST_OTA_LOWER_APPLICATION) || defined (ST_OTA_HIGHER_APPLICATION)
/* Number of services requests */
#define NUM_APP_GATT_SERVICES          (2 + 1) /* 2 services + 1 OTA service */

/* Number of attributes requests from the chat demo */
#define NUM_APP_GATT_ATTRIBUTES        (15 + 9) /* 15 attributes x BLE demo services characteristics + 9 for OTA Service characteristics */

/**
 * Set the number of 16-bytes units used on an OTA FW data packet for matching OTA client MAX ATT_MTU
 */
#define OTA_16_BYTES_BLOCKS_NUMBER     ((OTA_MAX_ATT_MTU_SIZE-4)/16)   /* 4 bytes for OTA sequence numbers + needs ack + checksum bytes */
   
/* OTA characteristics maximum lenght */
#define OTA_MAX_ATT_SIZE               (4 + OTA_16_BYTES_BLOCKS_NUMBER * 16) 

#else /* NO OTA Service is required */

/* Number of services requests */
#define NUM_APP_GATT_SERVICES          1

/* Number of attributes requests */
#define NUM_APP_GATT_ATTRIBUTES        6

/* OTA characteristics maximum lenght */
#define OTA_MAX_ATT_SIZE               (0) 
   
#endif

#define MAX_CHAR_LEN(a,b)              ((a) > (b) )? (a) : (b)

/* Application characteristics maximum lenght */
#define _MAX_ATT_SIZE	               (6)

/* Set supported max value for attribute size: it is the biggest attribute size enabled by the application. */
#define APP_MAX_ATT_SIZE	       MAX_CHAR_LEN(OTA_MAX_ATT_SIZE,  _MAX_ATT_SIZE)

/* Number of links needed
 */
#define NUM_LINKS                      (MIN_NUM_LINK)

/* Number of GATT attributes needed */
#define NUM_GATT_ATTRIBUTES            (DEFAULT_NUM_GATT_ATTRIBUTES + NUM_APP_GATT_ATTRIBUTES)

/* Number of GATT services needed */
#define NUM_GATT_SERVICES       (DEFAULT_NUM_GATT_SERVICES + NUM_APP_GATT_SERVICES)

/* Array size for the attribute value for OTA service */
#if defined (ST_OTA_LOWER_APPLICATION) || defined (ST_OTA_HIGHER_APPLICATION)
/* OTA service: 4 characteristics (1 notify property): 99 bytes + 
   Image Content characteristic length = 4  + (OTA_16_BYTES_BLOCKS_NUMBER * 16); 4 for sequence number, checksum and needs acks bytes */
#define OTA_ATT_VALUE_ARRAY_SIZE       (99 + (4 + (OTA_16_BYTES_BLOCKS_NUMBER * 16))) 
#else
#define OTA_ATT_VALUE_ARRAY_SIZE       (0)       /* No OTA service is used */
#endif

/* Array size for the attribute value */
#define ATT_VALUE_ARRAY_SIZE           256

/* Flash security database size */
#define FLASH_SEC_DB_SIZE              (0x400)

/* Flash server database size */
#define FLASH_SERVER_DB_SIZE           (0x400)

/* Set supported max value for ATT_MTU enabled by the application */
#if (CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED == 1) && (OTA_EXTENDED_PACKET_LEN == 1) 
  #define MAX_ATT_MTU                  (OTA_MAX_ATT_MTU_SIZE)
#else
  #define MAX_ATT_MTU                  (DEFAULT_ATT_MTU) 
#endif 

/* Set supported max value for attribute size: it is the biggest attribute size enabled by the application */
#define MAX_ATT_SIZE                   (APP_MAX_ATT_SIZE)  

/* Set the minumum number of prepare write requests needed for a long write procedure for a characteristic with len > 20bytes: 
 * 
 * It returns 0 for characteristics with len <= 20bytes
 * 
 * NOTE: If prepare write requests are used for a characteristic (reliable write on multiple characteristics), then 
 * this value should be set to the number of prepare write needed by the application.
 * 
 *  [New parameter added on BLE stack v2.x] 
*/
#define PREPARE_WRITE_LIST_SIZE        0

/* Additional number of memory blocks  to be added to the minimum  */
#define OPT_MBLOCKS		       6 /* 6:  for reaching the max throughput: ~220kbps (same as BLE stack 1.x) */

/* Set the number of memory block for packet allocation */
#define MBLOCKS_COUNT           (MBLOCKS_CALC(PREPARE_WRITE_LIST_SIZE, MAX_ATT_MTU, NUM_LINKS) + OPT_MBLOCKS)

/* RAM reserved to manage all the data stack according the number of links,
 * number of services, number of attributes and attribute value length
 */
NO_INIT(uint32_t dyn_alloc_a[TOTAL_BUFFER_SIZE(NUM_LINKS,NUM_GATT_ATTRIBUTES, \
                             NUM_GATT_SERVICES,ATT_VALUE_ARRAY_SIZE,MBLOCKS_COUNT, \
                             CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED)>>2]);


/* FLASH reserved to store all the security database information and
 * and the server database information
 */
//NO_INIT_SECTION(uint32_t stacklib_flash_data[TOTAL_FLASH_BUFFER_SIZE(FLASH_SEC_DB_SIZE, FLASH_SERVER_DB_SIZE)>>2], ".noinit.stacklib_flash_data");
NO_INIT_SECTION(const uint32_t stacklib_flash_data[TOTAL_FLASH_BUFFER_SIZE(FLASH_SEC_DB_SIZE, \
                                     FLASH_SERVER_DB_SIZE)>>2], ".noinit.stacklib_flash_data");

/* FLASH reserved to store: security root keys, static random address, public address */
NO_INIT_SECTION(const uint8_t stacklib_stored_device_id_data[56], ".noinit.stacklib_stored_device_id_data");

/* Maximum duration of the connection event */
#define MAX_CONN_EVENT_LENGTH          0xFFFFFFFF

/* Sleep clock accuracy */
#if (LS_SOURCE == LS_SOURCE_INTERNAL_RO)

/* Sleep clock accuracy in Slave mode */
#define SLAVE_SLEEP_CLOCK_ACCURACY     500

/* Sleep clock accuracy in Master mode */
#define MASTER_SLEEP_CLOCK_ACCURACY MASTER_SCA_500ppm

#else

/* Sleep clock accuracy in Slave mode */
#define SLAVE_SLEEP_CLOCK_ACCURACY     100

/* Sleep clock accuracy in Master mode */
#define MASTER_SLEEP_CLOCK_ACCURACY    MASTER_SCA_100ppm

#endif

/* Low Speed Oscillator source */
#if (LS_SOURCE == LS_SOURCE_INTERNAL_RO)
#define LOW_SPEED_SOURCE               1 // Internal RO
#else
#define LOW_SPEED_SOURCE               0 // External 32 KHz
#endif

/* High Speed start up time */
#define HS_STARTUP_TIME                328 // 800 us

/* Radio Config Hot Table */
extern uint8_t hot_table_radio_config[];

/* Low level hardware configuration data for the device */
#define CONFIG_TABLE            \
{                               \
  (uint32_t*)hot_table_radio_config,          \
  MAX_CONN_EVENT_LENGTH,        \
  SLAVE_SLEEP_CLOCK_ACCURACY,   \
  MASTER_SLEEP_CLOCK_ACCURACY,  \
  LOW_SPEED_SOURCE,             \
  HS_STARTUP_TIME               \
}

/* This structure contains memory and low level hardware configuration data for the device */
const BlueNRG_Stack_Initialization_t BlueNRG_Stack_Init_params = {
    (uint8_t*)stacklib_flash_data,
    FLASH_SEC_DB_SIZE,
    FLASH_SERVER_DB_SIZE,
    (uint8_t*)stacklib_stored_device_id_data,
    (uint8_t*)dyn_alloc_a,
    TOTAL_BUFFER_SIZE(NUM_LINKS,NUM_GATT_ATTRIBUTES,NUM_GATT_SERVICES,ATT_VALUE_ARRAY_SIZE,MBLOCKS_COUNT,CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED),
    NUM_GATT_ATTRIBUTES,
    NUM_GATT_SERVICES,
    ATT_VALUE_ARRAY_SIZE,
    NUM_LINKS,
    0, /* reserved for future use */
    PREPARE_WRITE_LIST_SIZE,
    MBLOCKS_COUNT,
    MAX_ATT_MTU,
    CONFIG_TABLE,
};

#endif // _APPLI_CONFIG_H_

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

