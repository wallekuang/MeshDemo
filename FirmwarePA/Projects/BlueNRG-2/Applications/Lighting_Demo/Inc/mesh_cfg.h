/**
******************************************************************************
* @file    mesh_cfg.h
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Header file for mesh_usr_cfg.c 
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
* Initial BlueNRG-Mesh is built over Motorola’s Mesh over Bluetooth Low Energy 
* (MoBLE) technology. The present solution is developed and maintained for both 
* Mesh library and Applications solely by STMicroelectronics.
*
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MESH_CFG_H
#define __MESH_CFG_H

/* Includes ------------------------------------------------------------------*/
#include "mesh_cfg_usr.h"

/* Exported macro ------------------------------------------------------------*/
/* Flash parameters */
#define PAGE_SIZE                          2048
#define RESET_MANAGER_FLASH_BASE_ADDRESS   (0x10040000)

void TraceHeader(const char* func_name, int mode);
#define TraceB0(func_name, ...)
#define TraceB1(func_name, ...) do {TraceHeader(func_name, 0); printf( __VA_ARGS__);} while(0)
#define TraceBX(flags, func_name, ...) TraceB ##flags(func_name, ##__VA_ARGS__)
#define TRACE_M(flag, ...)   TraceBX(flag, __func__, ##__VA_ARGS__)

/*Enable this Macro to enable the main function prints.
* This trace does not print the function name and clock.
*/
#define TraceC0(...)
#define TraceC1(...) do { printf( __VA_ARGS__);} while(0)
#define TraceCX(flags, ...) TraceC ##flags( ##__VA_ARGS__)
#define TRACE_I(flag, ...)   TraceCX(flag, ##__VA_ARGS__)


#ifdef EXTERNAL_MAC_ADDR_MGMT
  #define EXTERNAL_MAC_TYPE (uint8_t)(EXTERNAL_MAC_IS_PUBLIC_ADDR<<7)
#else
  #define EXTERNAL_MAC_TYPE (uint8_t)0
#endif

#if  (!(GENERATE_STATIC_RANDOM_MAC)) &&  (!(EXTERNAL_MAC_ADDR_MGMT)) && (!(INTERNAL_UNIQUE_NUMBER_MAC))
#error "Please select atleast one MAC address Option"
#endif

#ifdef ENABLE_UNPROVISIONING_BY_POWER_ONOFF_CYCLE
#define ENABLE_SAVE_MODEL_STATE_NVM
#endif

#ifdef ENABLE_SAVE_MODEL_STATE_NVM
#define SAVE_MODEL_STATE_NVM 1
#else
#define SAVE_MODEL_STATE_NVM 0
#endif

#define TRANSMIT_RECEIVE_PARAMS          \
{                                        \
    TR_GAP_BETWEEN_TRANSMISSION          \
}

#ifdef ENABLE_PB_ADV
#define PB_ADV_SUPPORTED 1
#else
#define PB_ADV_SUPPORTED 0
#endif

#ifdef ENABLE_PB_GATT
#define PB_GATT_SUPPORTED 1
#else
#define PB_GATT_SUPPORTED 0
#endif

#ifdef ENABLE_PUB_KEY_TYPE_OOB
#define PUB_KEY_TYPE_OOB        TRUE
#else
#define PUB_KEY_TYPE_OOB        FALSE
#endif


#if  (PB_ADV_SUPPORTED == 0 && PB_GATT_SUPPORTED == 0)
#error "Atleast one of PB-ADV and PB-GATT should be defined"
#endif

#define BLUENRG_MESH_PRVN_BEARER_INFO (PB_ADV_SUPPORTED << 0 | PB_GATT_SUPPORTED << 1)

#ifdef ENABLE_RELAY_FEATURE
#define RELAY_FEATURE 1
#else
#define RELAY_FEATURE 0
#endif

#ifdef ENABLE_PROXY_FEATURE
#define PROXY_FEATURE 1
#else
#define PROXY_FEATURE 0
#endif

#ifdef ENABLE_FRIEND_FEATURE
#define FRIEND_FEATURE 1
#else
#define FRIEND_FEATURE 0
#endif

#ifdef ENABLE_LOW_POWER_FEATURE
#define LOW_POWER_FEATURE 1
#else
#define LOW_POWER_FEATURE 0
#endif

#ifdef ENABLE_PROVISIONER_FEATURE
#define PROVISIONER_FEATURE 1
#else
#define PROVISIONER_FEATURE 0
#endif

#if (LOW_POWER_FEATURE && RELAY_FEATURE)
#error "Low power node can't be relay node"
#elif (LOW_POWER_FEATURE && PROXY_FEATURE)
#error "Low power node can't be proxy node"
#elif (LOW_POWER_FEATURE && FRIEND_FEATURE)
#error "Low power node can't be friend node"
#endif

#define BLUENRG_MESH_FEATURES (RELAY_FEATURE << 0 | PROXY_FEATURE << 1 | FRIEND_FEATURE << 2 | LOW_POWER_FEATURE << 3)

#define FRIEND_NODE_PARAMS               \
{                                        \
  FN_NO_OF_LPNS                          \
}

#define LOW_POWER_NODE_PARAMS            \
{                                        \
  LPN_RSSI_FACTOR_LEVEL,                 \
  LPN_RECIVE_WINDOW_FACTOR_LEVEL,        \
  LPN_MINIMUM_QUEUE_SIZE_LOG,            \
  LPN_RECEIVE_DELAY,                     \
  LPN_POLL_TIMEOUT,                      \
  LPN_FRIEND_REQUEST_FREQUENCY,          \
  LPN_FRIEND_POLL_FREQUENCY,             \
  LPN_RECEIVE_WINDOW_SIZE,               \
  LPN_SUBSCR_LIST_SIZE,                  \
  LPN_MINIMUM_RSSI,                      \
  LPN_NO_OF_RETRIES                      \
}

#define UNPROV_NODE_INFO_PARAMS          \
{                                        \
    PUB_KEY_TYPE_OOB,                    \
    PubKeyBuff,                          \
    PrivKeyBuff,                         \
    STATIC_OOB_SIZE,                     \
    StaticOobBuff,                       \
    OUTPUT_OOB_SIZE,                     \
    OUTPUT_OOB_ACTION,                   \
    Appli_BleOutputOOBAuthCb,            \
    INPUT_OOB_SIZE,                      \
    INPUT_OOB_ACTION,                    \
    Appli_BleInputOOBAuthCb              \
}

#ifdef ENABLE_NEIGHBOR_TABLE
#define NEIGHBOR_TABLE_SUPPORTED 1
#else
#define NEIGHBOR_TABLE_SUPPORTED 0
#endif

#if  (NEIGHBOR_TABLE_SUPPORTED == 1)
#if (NEIGHBOR_COUNT == 0)
#error "Number of neighbors should be nonzero"
#endif /* NO_OF_NEIGHBORS */
#if (NEIGHBOR_ALIVE_TIME == 0)
#error "Number of neighbor alive time should be nonzero"
#endif /* NEIGHBOR_ALIVE_TIME */
#if ((NEIGHBOR_UNPRVND_DEV_BEACON_NTU == 0) && (NEIGHBOR_MSG_TTLX_NTU == 0) && (NEIGHBOR_SECURE_NET_BEACON_NTU == 0))
#error "Atleast one of Network Table Update trigger should be enabled"
#endif
#if (NEIGHBOR_UNPRVND_DEV_BEACON_NTU > 1)
#error "Invalid valid of NEIGHBOR_UNPRVND_DEV_BEACON_NTU. Allowed value: 0 and 1"
#endif /* NEIGHBOR_UNPRVND_DEV_BEACON_NTU */
#if (NEIGHBOR_SECURE_NET_BEACON_NTU > 1)
#error "Invalid valid of NEIGHBOR_SECURE_NET_BEACON_NTU. Allowed value: 0 and 1"
#endif /* NEIGHBOR_SECURE_NET_BEACON_NTU */
#if (NEIGHBOR_MSG_TTLX_NTU > 2)
#error "Invalid valid of NEIGHBOR_MSG_TTLX_NTU. Allowed value: 0, 1 and 2"
#endif /* NEIGHBOR_MSG_TTLX_NTU */
#endif /* NEIGHBOR_TABLE_SUPPORTED */

#define NEIGHBOR_TABLE_PARAMS            \
{                                        \
    NEIGHBOR_COUNT,                      \
    NEIGHBOR_ALIVE_TIME,                 \
    NEIGHBOR_UNPRVND_DEV_BEACON_NTU,     \
    NEIGHBOR_SECURE_NET_BEACON_NTU,      \
    NEIGHBOR_MSG_TTLX_NTU                \
}

#if FRIEND_FEATURE
#define FRIEND_BUFF_DYNAMIC_MEMORY_SIZE    FN_NO_OF_LPNS*816+4
#elif LOW_POWER_FEATURE
#define FRIEND_BUFF_DYNAMIC_MEMORY_SIZE    112U
#else
#define FRIEND_BUFF_DYNAMIC_MEMORY_SIZE    4U
#endif

#define DYNAMIC_MEMORY_SIZE                3072U 

#define SEGMENTATION_COUNT                 (MAX_APPLICATION_PACKET_SIZE / 12) + 2 
#define SAR_BUFFER_SIZE                    ((uint8_t)SEGMENTATION_COUNT) * 40
/* Added Interrupt handler for Uart */
#define SdkEvalComIOUartIrqHandler         UART_Handler  

#if NEIGHBOR_TABLE_SUPPORTED
#define NEIGHBOR_TABLE_DYNAMIC_MEMORY_SIZE 32U*NEIGHBOR_COUNT
#else
#define NEIGHBOR_TABLE_DYNAMIC_MEMORY_SIZE 0
#endif

#define SdkEvalComIOUartIrqHandler         UART_Handler  /* Added Interrupt handler for Uart */
/******************** Serial Interface Handling Control **********************/

/* Exported variables  ------------------------------------------------------- */
extern const DynBufferParam_t DynBufferParam;
extern const tr_params_t TrParams;
extern const lpn_params_t LpnParams;
extern const fn_params_t FnParams;
extern const prvn_params_t PrvnParams;
extern const neighbor_table_init_params_t NeighborTableParams;

/* Exported Functions Prototypes ---------------------------------------------*/
MOBLEUINT8 ApplicationSetNodeSigModelList(void);

#endif /* __MESH_CFG_H */
