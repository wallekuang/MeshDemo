/**
******************************************************************************
* @file    appli_config_client.h
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Application interface for Generic Mesh Models  
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
#ifndef __APPLI_CONFIG_CLIENT_H
#define __APPLI_CONFIG_CLIENT_H

/* Includes ------------------------------------------------------------------*/
#include "types.h"
#include "generic.h"
#include "mesh_cfg.h"
#include "config_client.h"


/* Exported macro ------------------------------------------------------------*/
/* Exported variables  ------------------------------------------------------- */
/* Application Variable-------------------------------------------------------*/
/* Exported Functions Prototypes ---------------------------------------------*/
MOBLE_RESULT Appli_ConfigClient_Init(void);
MOBLE_RESULT Appli_ConfigClient_Process(void);
MOBLE_RESULT Appli_ConfigClient_ConfigureNode(void);
void Appli_CompositionDataStatusCb(MOBLE_RESULT); 
void Appli_AppKeyStatusCb(MOBLEUINT8 status);
void Appli_AppBindModelStatusCb(MOBLEUINT8 status);
void Appli_PublicationStatusCb(MOBLEUINT8 status);
void Appli_SubscriptionAddStatusCb(MOBLEUINT8 status);
  
void Appli_ConfigClientStartNodeConfiguration(MOBLEUINT8 prvState);
MOBLE_RESULT Appli_ConfigClient_GetCompositionData (void);
MOBLE_RESULT Appli_ConfigClient_DefaultAppKeyAdd (void);
MOBLE_RESULT Appli_ConfigClient_DefaultAppKeyBind (void);
MOBLE_RESULT AppliConfigClient_SubscriptionAddDefault (void);
MOBLE_RESULT AppliConfigClient_PublicationSetDefault (void);
MOBLE_RESULT AppliConfigClient_SelfPublicationSetDefault (void); 
MOBLE_RESULT AppliConfigClient_SelfSubscriptionSetDefault (void);
MOBLE_RESULT Appli_ConfigClient_SelfDefaultAppKeyBind (void);
Composition_Data_Page0_t* Appli_GetNodeCompositionBuff (void);

MOBLEUINT16 GetSIGModelToBindApp(MOBLEUINT8 elementIdx, MOBLEUINT8 idxSIG);
MOBLEUINT32 GetVendorModelToBindApp(MOBLEUINT8 elementIdx, MOBLEUINT8 idxSIG);
MOBLEUINT8 GetCountSIGModelToBindApp(MOBLEUINT8 elementIdx);
MOBLEUINT8 GetCountVendorModelToBindApp(MOBLEUINT8 elementIdx);

MOBLEUINT16 GetSIGModelToSubscribe(MOBLEUINT8 elementIdx, MOBLEUINT8 idxSIG);
MOBLEUINT32 GetVendorModelToSubscribe(MOBLEUINT8 elementIdx, MOBLEUINT8 idxSIG);
MOBLEUINT8 GetCountSIGModelToSubscribe(MOBLEUINT8);
MOBLEUINT8 GetCountVendorModelToSubscribe(MOBLEUINT8);

MOBLEUINT16 GetSIGModelToPublish(MOBLEUINT8 elementIdx, MOBLEUINT8 idxSIG);
MOBLEUINT32 GetVendorModelToPublish(MOBLEUINT8 elementIdx, MOBLEUINT8 idxSIG);
MOBLEUINT8 GetCountSIGModelToPublish(MOBLEUINT8 elementIdx);
MOBLEUINT8 GetCountVendorModelToPublish(MOBLEUINT8 elementIdx);


#endif /* __APPLI_CONFIG_CLIENT_H */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

