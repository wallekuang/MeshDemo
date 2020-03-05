/**
******************************************************************************
* @file    main.c
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   main file of the application
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
#include "appli_mesh.h"
#include "appli_light.h"
#ifdef ENABLE_PROVISIONER_FEATURE
#include "appli_config_client.h"
#include "appli_nvm.h"
#endif
#include "models_if.h"
#include "mesh_cfg.h"
#include "PWM_config.h"
#include "PWM_handlers.h"
#include "LPS25HB.h"
#include "BlueNRG_x_device.h"
#include "miscutil.h"


/** @addtogroup ST_BLE_Mesh
 *  @{
 */

/** @addtogroup Application
*  @{
*/
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

const MOBLE_USER_BLE_CB_MAP user_ble_cb =
{  
  Appli_BleStackInitCb,
  Appli_BleSetTxPowerCb,
  Appli_BleGattConnectionCompleteCb,
  Appli_BleGattDisconnectionCompleteCb,
  Appli_BleUnprovisionedIdentifyCb,
  Appli_BleSetUUIDCb,
  Appli_BleSetProductInfoCB,
  Appli_BleSetNumberOfElementsCb,
  Appli_BleDisableFilterCb
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* This structure contains Mesh library Initialisation info data */
const Mesh_Initialization_t BLEMeshlib_Init_params = {
  bdaddr,
  &TrParams,
  &FnParams,
  &LpnParams,
  &NeighborTableParams,
  BLUENRG_MESH_FEATURES,
  BLUENRG_MESH_PRVN_BEARER_INFO,
  &PrvnParams,
  &DynBufferParam
};

/* Private function prototypes -----------------------------------------------*/
void Appli_ConfigurationInfoDump(void);

/* Private functions ---------------------------------------------------------*/

/**
* @brief This function is the Main routine.
* @param  None
* @retval None
*/
int main(void)
{
   MOBLEUINT8 uuid[16];
   MOBLEUINT8 PrvnDevKeyFlag = 0;
 
   
  /* Device Initialization */
  InitDevice();
  
  /* Initialization of PWM value to 1 */
  PWM_Init();
  
  /* Get the information if code is crashed */
  Get_CrashHandlerInfo();
  
  /* Check for valid Board Address */
  if (!Appli_CheckBdMacAddr())
  {  
    TRACE_I(TF_INIT,"Bad BD_MAC ADDR!\r\n");
    /* LED Blinks if BDAddr is not appropriate */
    while (1)
    {
      Appli_LedBlink();
    }
  }
  
  /* Set BLE configuration function callbacks */
  BluenrgMesh_BleHardwareInitCallBack(&user_ble_cb);  
  
  /* Initializes BlueNRG-Mesh Library */
  if (MOBLE_FAILED (BluenrgMesh_Init(&BLEMeshlib_Init_params) ))
  {   
    TRACE_I(TF_INIT,"Could not initialize BlueNRG-Mesh library!\r\n");   
    /* LED continuously blinks if library fails to initialize */
    while (1)
    {
      Appli_LedBlink();
    }
  }

/* Check if Node is a Provisioner */
#if PROVISIONER_FEATURE
  MOBLEUINT8 prvsnrDevKey[16];
  /* Initializes Mesh network parameters */
  BluenrgMesh_CreateNetwork(prvsnrDevKey);
  
  /* Following functions help to Configure the Provisioner to default settings*/
  ApplicationSetNodeSigModelList();
  AppliConfigClient_SelfPublicationSetDefault();
  AppliConfigClient_SelfSubscriptionSetDefault();
  Appli_ConfigClient_SelfDefaultAppKeyBind();

  TRACE_I(TF_PROVISION,"Provisioner node \r\n");
  TRACE_I(TF_PROVISION,"Provisioner Dev Key:");
   for(MOBLEUINT8 i=0;i<16;i++)
  {
    TRACE_I(TF_INIT,"[%02x] ",prvsnrDevKey[i]);
  }
  TRACE_I(TF_INIT,"\r\n");  
  
#else
  /* Checks if the node is already provisioned or not */
  if (BluenrgMesh_IsUnprovisioned() == MOBLE_TRUE)
  {
    BluenrgMesh_InitUnprovisionedNode(); /* Initalizes Unprovisioned node */
    
    TRACE_I(TF_INIT,"Unprovisioned device \r\n");
    
#if PB_ADV_SUPPORTED
    BluenrgMesh_SetUnprovisionedDevBeaconInterval(100);
#endif    
  }
  else
  {
    BluenrgMesh_InitProvisionedNode();  /* Initalizes Provisioned node */      
    TRACE_I(TF_INIT,"Provisioned node \r\n");
    TRACE_I(TF_INIT,"Provisioned Node Address: [%04x] \n\r", BluenrgMesh_GetAddress());       
  }
#endif  
  /* Initializes the Application */
  /* This function also checks for Power OnOff Cycles     
     Define the following Macro "ENABLE_UNPROVISIONING_BY_POWER_ONOFF_CYCLE" 
     to check the Power-OnOff Cycles
    5 Continous cycles of OnOff with Ontime <2 sec will cause unprovisioning
  */
  Appli_Init(&PrvnDevKeyFlag);
#if PROVISIONER_FEATURE  
  AppliNnm_saveProvisionerDevKey(&prvsnrDevKey[0] ,sizeof(prvsnrDevKey) 
                                                      ,&PrvnDevKeyFlag);  
#endif  
  /* Check to manually unprovision the board */
  /* On STEVAL-IDB008V2/ V1/ 7V1, 7V2 / 9V1 : GPIO_Pin_13 is used to check the 
     Button pressed state at Power-On */
  Appli_CheckForUnprovision();
  
  /* Set attention timer callback */
  BluenrgMesh_SetAttentionTimerCallback(Appli_BleAttentionTimerCb);

#if TF_MISC
  Appli_ConfigurationInfoDump();
#endif  
  
  /* Prints the MAC Address of the board */ 
  TRACE_I(TF_INIT,"BlueNRG-Mesh Lighting Demo v%s\n\r", BLUENRG_MESH_APPLICATION_VERSION); 
  TRACE_I(TF_INIT,"BlueNRG-Mesh Library v%s\n\r", BluenrgMesh_GetLibraryVersion()); 

  /* Set uuid for the board*/
  Appli_BleSetUUIDCb(uuid);

  for(MOBLEUINT8 i=0;i<16;i++)
  {
    TRACE_I(TF_INIT,"[%02x] ",uuid[i]);
  }
  TRACE_I(TF_INIT,"\r\n");
  
  /* Models intialization */  
  BluenrgMesh_ModelsInit();
  
  /* Turn on Yellow LED */
#if (LOW_POWER_FEATURE == 1)
  SdkEvalLedOn(LED1);
#endif
   
  /* Main infinite loop */
  while(1)
  {
    BluenrgMesh_Process();
    BluenrgMesh_ModelsProcess(); /* Models Processing */
    Appli_Process();   
  }
}


void Appli_ConfigurationInfoDump(void)
{
  TRACE_I(TF_MISC,"\r\n********************\r\n");
  
#ifdef ENABLE_PB_ADV
  TRACE_I(TF_MISC,"PB-ADV Enabled \r\n");
#endif   

#ifdef ENABLE_PB_GATT  
  TRACE_I(TF_MISC,"PB-GATT Enabled \r\n");
#endif  

#ifdef ENABLE_RELAY_FEATURE
  TRACE_I(TF_MISC,"Feature: Relay Enabled \r\n");
#endif 
  
#ifdef ENABLE_PROXY_FEATURE
  TRACE_I(TF_MISC,"Feature: Proxy Enabled \r\n");
#endif
  
#ifdef ENABLE_FRIEND_FEATURE
  TRACE_I(TF_MISC,"Feature: Friend Enabled \r\n");
#endif  
  
#ifdef ENABLE_PUB_KEY_TYPE_OOB
  TRACE_I(TF_MISC,"Public Key OOB Enabled \r\n");
#endif
  
#ifdef ENABLE_AUTH_TYPE_STATIC_OOB
  TRACE_I(TF_MISC,"Static OOB Enabled \r\n");
#endif
  
#ifdef ENABLE_AUTH_TYPE_OUTPUT_OOB
  TRACE_I(TF_MISC,"Output OOB Enabled \r\n");
#endif
  
#ifdef ENABLE_AUTH_TYPE_INPUT_OOB
  TRACE_I(TF_MISC,"Input OOB Enabled \r\n");
#endif

#ifdef ENABLE_UNPROVISIONING_BY_POWER_ONOFF_CYCLE
  TRACE_I(TF_MISC,"To Unprovision: Do Power On-Off/Reset %d time \r\n", POWER_ON_OFF_CYCLES_FOR_UNPROVISIONING);
#endif
      
#ifdef ENABLE_SAVE_MODEL_STATE_NVM
  TRACE_I(TF_MISC,"Models data will be saved in Flash \r\n");
#endif
  
/* Only one Macro will be enable at one time */
#ifdef SAVE_MODEL_STATE_FOR_ALL_MESSAGES         
  TRACE_I(TF_MISC,"Models data for all messages wll be saved \r\n");
#endif
  
#ifdef SAVE_MODEL_STATE_POWER_FAILURE_DETECTION
  TRACE_I(TF_MISC,"Models data will be saved on Power failure \r\n");
#endif
  
#if SAVE_EMBD_PROVISION_DATA     
  TRACE_I(TF_MISC,"Embedded Provisioner data saving enabled \r\n");
#endif
  
#ifdef APPLICATION_NUMBER_OF_ELEMENTS     
  TRACE_I(TF_MISC,"Number of Elements enabled: %d \r\n", APPLICATION_NUMBER_OF_ELEMENTS);  
#endif
  
#ifdef ENABLE_NEIGHBOR_TABLE
  TRACE_I(TF_MISC,"Neighbour Table is enabled \r\n");
#endif
  
#ifdef ENABLE_GENERIC_MODEL_SERVER_ONOFF
  TRACE_I(TF_MISC,"Generic On Off Server Model enabled \r\n");
#endif
 
#ifdef ENABLE_GENERIC_MODEL_SERVER_LEVEL
  TRACE_I(TF_MISC,"Generic Level Server Model enabled \r\n");
#endif

#ifdef ENABLE_GENERIC_MODEL_SERVER_DEFAULT_TRANSITION_TIME
  TRACE_I(TF_MISC,"Generic Default Transition Server Model enabled \r\n");
#endif

#ifdef ENABLE_GENERIC_MODEL_SERVER_POWER_ONOFF
  TRACE_I(TF_MISC,"Generic Power On Off Server Model enabled \r\n");
#endif

#ifdef ENABLE_GENERIC_MODEL_SERVER_POWER_LEVEL
  TRACE_I(TF_MISC,"Generic Power Level Server Model enabled \r\n");
#endif

#ifdef ENABLE_GENERIC_MODEL_SERVER_BATTERY
  TRACE_I(TF_MISC,"Generic Battery Server Model enabled \r\n");
#endif

#ifdef ENABLE_GENERIC_MODEL_SERVER_LOCATION
  TRACE_I(TF_MISC,"Generic Location Server Model enabled \r\n");
#endif

#ifdef ENABLE_GENERIC_MODEL_SERVER_ADMIN_PROPERTY
  TRACE_I(TF_MISC,"Generic Admin Property Server Model enabled \r\n");
#endif

#ifdef ENABLE_GENERIC_MODEL_SERVER_MANUFACTURER_PROPERTY
  TRACE_I(TF_MISC,"Generic Manufacturer Property Server Model enabled \r\n");
#endif

#ifdef ENABLE_GENERIC_MODEL_SERVER_USER_PROPERTY
  TRACE_I(TF_MISC,"Generic User Property Server Model enabled \r\n");
#endif

#ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS
  TRACE_I(TF_MISC,"Light Lightness Server Model enabled \r\n");       
#endif 

#ifdef ENABLE_LIGHT_MODEL_SERVER_CTL
  TRACE_I(TF_MISC,"Light CTL Server Model enabled \r\n");
#endif 

#ifdef ENABLE_LIGHT_MODEL_SERVER_CTL_TEMPERATURE
  TRACE_I(TF_MISC,"Light CTL Temperature Server Model enabled \r\n");
#endif 

#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL
  TRACE_I(TF_MISC,"Light HSL Server Model enabled \r\n");
#endif 

#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL_HUE
  TRACE_I(TF_MISC,"Light HSL Hue Model enabled \r\n");
#endif 

#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL_SATURATION
  TRACE_I(TF_MISC,"Light HSL Saturation Server Model enabled \r\n");
#endif 

#ifdef ENABLE_LIGHT_MODEL_SERVER_XYL
  TRACE_I(TF_MISC,"Light XYL Server Model enabled \r\n");
#endif 

#ifdef ENABLE_LIGHT_MODEL_SERVER_LC
  TRACE_I(TF_MISC,"Light LC Server Model enabled \r\n");
#endif 

#ifdef ENABLE_SENSOR_MODEL_SERVER
  TRACE_I(TF_MISC,"Sensor Server Model enabled \r\n");   
#endif

#ifdef ENABLE_TIME_MODEL_SERVER 
  TRACE_I(TF_MISC,"Time Server Model enabled \r\n");            
#endif
          
#ifdef ENABLE_SCENE_MODEL_SERVER 
  TRACE_I(TF_MISC,"Scene Model enabled \r\n")           
#endif
  TRACE_I(TF_MISC,"********************\r\n\r\n");  

}

#ifdef USE_FULL_ASSERT /* USE_FULL_ASSERT */
/**
* @brief This function is the assert_failed function.
* @param file
* @param line
* @note  Reports the name of the source file and the source line number
*        where the assert_param error has occurred.
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {
    SetLed(1);
    Clock_Wait(100);
    SetLed(0);
    Clock_Wait(100);
  }
}

#endif /* USE_FULL_ASSERT */
/**
 * @}
 */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
