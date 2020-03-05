/**
******************************************************************************
* @file    appli_generic_client.c
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

/* Includes ------------------------------------------------------------------*/
#include "hal_common.h"
#include "types.h"
#include "appli_generic.h"
#include "appli_light.h"
#include "common.h"
#include "mesh_cfg_usr.h"
#include "appli_nvm.h"
#include "appli_mesh.h"
#include "generic_client.h"
#include "appli_generic_client.h"

/** @addtogroup ST_BLE_Mesh
*  @{
*/

/** @addtogroup Application_Mesh_Models
*  @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

MOBLEUINT8 Led_Value;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  Appli_Generic_OnOff_Set: This function is callback for Application
*          when Generic OnOff message is called
* @param  void
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_GenericClient_OnOff_Set(void) 
{ 
  MOBLE_ADDRESS elementAddr = 0; 
  MOBLEUINT8 pGeneric_OnOffParam[2];
  
  Led_Value ^= APPLI_LED_ON;
  pGeneric_OnOffParam[0] = Led_Value;
  GenericClient_OnOff_Set_Unack(elementAddr,
                          (_Generic_OnOffParam*) pGeneric_OnOffParam, 
                          sizeof(pGeneric_OnOffParam) ); 
  
  return MOBLE_RESULT_SUCCESS;
} 

#ifdef ENABLE_GENERIC_MODEL_CLIENT_LEVEL
/**
* @brief  Appli_Generic_Level_Set: This function is callback for Application
*          when Generic Level message is called
* @param  void: 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_GenericClient_Level_Set_Unack(void) 
{ 
  MOBLE_ADDRESS elementAddr = 0; 
  MOBLEUINT8 pGeneric_LevelParam[3];
  
  Appli_IntensityControlPublishing(pGeneric_LevelParam);
  GenericClient_Level_Set_Unack(elementAddr,
                          (_Generic_LevelParam*) pGeneric_LevelParam, 
                          sizeof(pGeneric_LevelParam));
  
  return MOBLE_RESULT_SUCCESS;
}

#endif

/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

