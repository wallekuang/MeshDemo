/******************************************************************************
* @file    appli_vendor.c
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Application interface for Vendor Mesh Models 
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
#include "bluenrg_mesh.h"
#include "appli_mesh.h"
#include "vendor.h"
#include "appli_vendor.h"
#include "common.h"
#include "appli_light.h"
#include "models_if.h"
#include "mesh_cfg.h"
#include <string.h>

/** @addtogroup ST_BLE_Mesh
*  @{
*/

/** @addtogroup Application_Mesh_Models
*  @{
*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

MOBLEUINT8 ResponseBuffer[VENDOR_DATA_BYTE];
MOBLEUINT16 BuffLength;
extern MOBLEUINT8 Appli_LedState;
extern uint16_t DUTY;
MOBLEUINT32 TestHitCounter = 0;
extern Appli_LightPwmValue_t Appli_LightPwmValue;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  Process the Vendor Device Info Command
* @param  data: Pointer to the data received from peer_addr
* @param  length: Length of the data
* @retval MOBLE_RESULT status of result
*/          
MOBLE_RESULT Appli_Vendor_DeviceInfo(MOBLEUINT8 const *data, MOBLEUINT32 length)
{
        MOBLEUINT8 tempBuffer[10];
        MOBLE_RESULT status = MOBLE_RESULT_SUCCESS;  
        
        MOBLEUINT8 subCmd = data[0];
        char *libVersion;
        char *subLibVersion;
        MOBLEUINT8 inc = 0;
        /*First Byte is sending the Sub Command*/      
        ResponseBuffer[0] = subCmd;
        TRACE_M(TF_VENDOR_COMMAND,"#02-%02hx! \n\r",data[0]);
        
        switch(subCmd)
        {
        case IC_TYPE:
          {
            #ifdef BLUENRG1_DEVICE           
            ResponseBuffer[1] = BLUENRG1_BRD;              
            #endif
            
            #ifdef BLUENRG2_DEVICE              
            ResponseBuffer[1] = BLUENRG2_BRD;              
            #endif
            
            #ifdef BLUENRG_MS              
            ResponseBuffer[1] = BLUENRG_MS_BRD;               
            #endif
            
            BuffLength = 2;
            
            break;
          }
        case LIB_VER:
          {
            libVersion = BluenrgMesh_GetLibraryVersion();
            while(*libVersion != '\0')
            {
              tempBuffer[inc] = *libVersion;
              if(tempBuffer[inc] != 0x2E)
              {
                tempBuffer[inc] = BluenrgMesh_ModelsASCII_To_Char(tempBuffer[inc]);
                TRACE_M(TF_VENDOR,"Lib version is %x\n\r" ,(unsigned char)tempBuffer[inc]);
              }
              else
              {
                TRACE_M(TF_VENDOR,"Lib version is %c\n\r" ,(unsigned char)tempBuffer[inc]);
              }             
              libVersion++; 
             
              inc++;
            }             
            ResponseBuffer[1]= tempBuffer[0];
            ResponseBuffer[2]= tempBuffer[1];
            ResponseBuffer[3]= tempBuffer[3];
            ResponseBuffer[4]= tempBuffer[4];
            ResponseBuffer[5]= tempBuffer[6];
            ResponseBuffer[6]= tempBuffer[7];
            ResponseBuffer[7]= tempBuffer[8];
            BuffLength = 8;      
            break;
          }
        case LIB_SUB_VER:
          {
            subLibVersion = BluenrgMesh_GetLibrarySubVersion();
            while(*subLibVersion != '\0')
            {
              tempBuffer[inc] = * subLibVersion;
              if((tempBuffer[inc] != 0x2E) && (tempBuffer[inc] != 0x52))
              {               
                tempBuffer[inc] = BluenrgMesh_ModelsASCII_To_Char(tempBuffer[inc]);
                TRACE_M(TF_VENDOR,"Sub Lib version is %x\n\r" ,(unsigned char)tempBuffer[inc]);
              }
              else
              {
                TRACE_M(TF_VENDOR,"Sub Lib version is %c\n\r" ,(unsigned char)tempBuffer[inc]);
              } 
              subLibVersion++;
              inc++;
            }           
            ResponseBuffer[1]= tempBuffer[0];
            ResponseBuffer[2]= tempBuffer[1];
            ResponseBuffer[3]= tempBuffer[3];
            ResponseBuffer[4]= tempBuffer[5];
            ResponseBuffer[5]= tempBuffer[7];
            ResponseBuffer[6]= tempBuffer[9];
            
            BuffLength = 7;
            
            break;
          }
        case APPLICATION_VER:
          {
            /*Insert Command to check Application Version*/
            break;
          }
          
        default:
          {
            status = MOBLE_RESULT_FALSE;
            break;
          }
          
        }
        
        return status;
}


/**
* @brief  Process the Vendor Test Command 
* @param  data: Pointer to the data received from peer_addr
* @param  length: Length of the data
* @retval MOBLE_RESULT status of result
*/          
MOBLE_RESULT Appli_Vendor_Test(MOBLEUINT8 const *data, MOBLEUINT32 length)
{
       MOBLE_RESULT status = MOBLE_RESULT_SUCCESS;  
       MOBLEUINT8 subCmd = data[0];
       /*First Byte is sending the Sub Command*/      
       ResponseBuffer[0]=subCmd;
       TRACE_M(TF_VENDOR_COMMAND,"#01-%02hx! \n\r",data[0]);
       switch(subCmd)
       {             
           case APPLI_TEST_ECHO: 
             {    
               if(length > sizeof(ResponseBuffer))
               {
                 length = sizeof(ResponseBuffer);
                 TRACE_M(TF_VENDOR,"Length received greater than size of response buffer \r\n");
               }
               memcpy (&(ResponseBuffer[1]),&(data[1]),(length-1));
               BuffLength = length;
               break;
             }
           case APPLI_TEST_RANDOMIZATION_RANGE:  
             {
               /*Insert Test related Commands here*/
               break;
             }
           case APPLI_TEST_COUNTER:
             {               
                if((DUTY <= PWM_TIME_PERIOD) && (DUTY > 1))
                {                 
                  Appli_LightPwmValue.IntensityValue = LED_OFF_VALUE;
                  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
                }
                else
                {                 
                  Appli_LightPwmValue.IntensityValue = PWM_TIME_PERIOD;
                  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
                }
                  TRACE_M(TF_VENDOR,"Test Counter is running \r\n");
                  ResponseBuffer[0] = subCmd;
                  ResponseBuffer[1] = Appli_LedState ;
                  BuffLength = 2; 
               /*Insert Test related Commands here*/
               break;
             }
           case APPLI_TEST_INC_COUNTER: 
             {     
                
                if((DUTY <= PWM_TIME_PERIOD) && (DUTY > 1))
                {
                  Appli_LightPwmValue.IntensityValue = LED_OFF_VALUE;
                  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
                }
                else
                {
                  Appli_LightPwmValue.IntensityValue = PWM_TIME_PERIOD;
                  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
                }                                                    
              
                  TestHitCounter++;              
                  TRACE_M(TF_VENDOR,"Command received Count %.2x \r\n",TestHitCounter);
                  ResponseBuffer[0] = subCmd;
                  ResponseBuffer[1] = Appli_LedState ;
                  BuffLength = 2;
               /*Insert Test related Commands here*/
               break;
             }
           case APPLI_MODEL_PUBLISH_SELECT:
             {
               for (MOBLEUINT8 idx=0; idx<length; idx++)
               {
                 TRACE_I(TF_VENDOR,"data[%d]= %d",idx,data[idx]);  
                 TRACE_I(TF_VENDOR,"\n\r");
               } 
               break;
             }
            
             
           case APPLI_OTA_ENABLE:
             {
               
                /*Edited for OTA, Macro is available in preprocessor*/   
                #ifdef ST_USE_OTA_SERVICE_MANAGER_APPLICATION
                EnterInto_OTA = MOBLE_TRUE;
                /*Yellow LED for OTA Indication*/
                SdkEvalLedOn(LED1);
                ResponseBuffer[0] = subCmd;
                ResponseBuffer[1] = status;
                BuffLength = 2;
                #endif
                break;
             }
             
           case APPLI_OTA_ENTER:
             {
               #ifdef ST_USE_OTA_SERVICE_MANAGER_APPLICATION
               if(EnterInto_OTA == MOBLE_TRUE)
               {
                 #ifdef ST_USE_OTA_SERVICE_MANAGER_APPLICATION
                 EnterInto_OTA = MOBLE_FALSE;
                 /*Turn Off the Proxy Indication LED*/
                 SdkEvalLedOff(LED2);
                 OTA_Jump_To_Service_Manager_Application();
                 #endif
               }
               #endif
               break;
             }             
           default:
             {
                status = MOBLE_RESULT_FALSE;
                break;
             }                  
        }
       
       return status;
}


/**
* @brief  Process the Vendor LED Control Command
* @param  data: Pointer to the data received from peer_addr
* @param  length: Length of the data
* @param  elementNumber : element selected for a node        
* @retval MOBLE_RESULT status of result
*/ 
MOBLE_RESULT Appli_Vendor_LEDControl( MOBLEUINT8 const *data, MOBLEUINT32 length,
                                       MOBLEUINT8 elementNumber , MOBLE_ADDRESS dst_peer)
{
      MOBLE_RESULT status = MOBLE_RESULT_SUCCESS;
      MOBLEUINT8 subCommand; 
      subCommand = data[0];
      MOBLEUINT16 duty;
      MOBLEUINT16 intensityValue;
      TRACE_M(TF_VENDOR_COMMAND,"#03-%02hx!\n\r",data[0]);
      switch(subCommand)
      {
       /* 
       Message Received     B0     B1    B2      B3    B4    B5    B6     B7 
                   B0 - Sub-Cmd LED
                   B1-B7 - Data Bytes       
       */
      case APPLI_CMD_LED_BULB:
        {
          if(elementNumber == FIRST_ELEMENT)
          {
            Appli_LedState = *(data+1); /* Toggle the state of the Blue LED */
          }
          else if(elementNumber == SECOND_ELEMENT)
          {         
             /* user application code */
          }
          else if(elementNumber == THIRD_ELEMENT)
          {
            /* user application code */
          }
          break;
        }
        
        /* Toggle Command */  
      case APPLI_CMD_TOGGLE:
        {
          if(elementNumber == FIRST_ELEMENT)
          {
            if(Appli_LedState == 1)
            {
              Appli_LightPwmValue.IntensityValue = LED_OFF_VALUE;
                                    
#ifndef CUSTOM_BOARD_PWM_SELECTION            
            Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);   /* PWM_ID = PWM4, mapped on PWM4_PIN (GPIO_14 in mapping) */
#else
            Light_UpdateLedValue(RESET_STATE , Appli_LightPwmValue);   /* PWM_ID = PWM4, mapped on PWM4_PIN (GPIO_14 in mapping) */
#endif            
              Appli_LedState = 0;
            }
            else
            {
              Appli_LightPwmValue.IntensityValue = PWM_TIME_PERIOD;
              Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
              Appli_LedState = 1;
            }
              
          }
          else if(elementNumber == SECOND_ELEMENT)
          {
             /* user application code */
          }
          else if(elementNumber == THIRD_ELEMENT)
          {
            /* user application code */
          }
          break;
        }
        /* On Command */  
      case APPLI_CMD_ON:
        {
          if(elementNumber == FIRST_ELEMENT)
          {            
            Appli_LightPwmValue.IntensityValue = PWM_TIME_PERIOD;
            Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);   /* PWM_ID = PWM4, mapped on PWM4_PIN (GPIO_14 in mapping) */
            Appli_LedState = 1;
          }
          else if(elementNumber == SECOND_ELEMENT)
          {
            Appli_LightPwmValue.IntensityValue = PWM_TIME_PERIOD;
            Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue); /* PWM_ID = PWM3, mapped on PWM3_PIN (GPIO_2 in mapping) */
            Appli_LedState = 1;
          }
          else if(elementNumber == THIRD_ELEMENT)
          {
            /* user application code */
          }
          break;
        }
        /* Off Command */  
      case APPLI_CMD_OFF:
        {
          if(elementNumber == FIRST_ELEMENT)
          {                  
            Appli_LightPwmValue.IntensityValue = LED_OFF_VALUE;
            
#ifndef CUSTOM_BOARD_PWM_SELECTION            
            Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);   /* PWM_ID = PWM4, mapped on PWM4_PIN (GPIO_14 in mapping) */
#else
            Light_UpdateLedValue(RESET_STATE , Appli_LightPwmValue);   /* PWM_ID = PWM4, mapped on PWM4_PIN (GPIO_14 in mapping) */
#endif            
            Appli_LedState = 0;
          }
          else if(elementNumber == SECOND_ELEMENT)
          {        
            Appli_LightPwmValue.IntensityValue = LED_OFF_VALUE;
            Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue); /* PWM_ID = PWM3, mapped on PWM3_PIN (GPIO_2 in mapping) */
            Appli_LedState = 0;
          }
          else if(elementNumber == THIRD_ELEMENT)
          {
            /* user application code */
          }
          
          break;
        }
        /* intensity command */
      case APPLI_CMD_LED_INTENSITY:
        {
          if(elementNumber == FIRST_ELEMENT)
          {
             intensityValue = data[2] << 8;
             intensityValue |= data[1];
             
             duty = PwmValueMapping(intensityValue , 0x7FFF ,0);                         
             Appli_LightPwmValue.IntensityValue = duty;
             Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);             
          }
          else if(elementNumber == SECOND_ELEMENT)
          {          
             /* user application code */
          }
          else if(elementNumber == THIRD_ELEMENT)
          {
            /* user application code */
          }
          break;
        }
        /* Default case - Not valid command */
      default:
        {
          status = MOBLE_RESULT_FALSE;
          break;
        }
      }
         /*Buffer will be sent for Reliable Response*/
         /*First Byte is Sub Command and 2nd Byte is LED Status*/
           ResponseBuffer[0] = subCommand;
         if(subCommand == APPLI_CMD_LED_INTENSITY)
         {
            ResponseBuffer[1] = intensityValue >> 8 ;
            ResponseBuffer[2] = intensityValue ;
            BuffLength = 3;
         }
         else
         {
             ResponseBuffer[1] = Appli_LedState ;
             BuffLength = 2; 
         }
       
      return status;
}

/**
* @brief  Process the Vendor Data write Command 
* @param  data: Pointer to the data received from peer_addr
* @param  length: Length of the data
* @retval MOBLE_RESULT status of result
*/          
MOBLE_RESULT Appli_Vendor_Data_write(MOBLEUINT8 const *data, MOBLEUINT32 length)
{
       MOBLE_RESULT status = MOBLE_RESULT_SUCCESS;  
       MOBLEUINT8 subCmd = data[0];
       /*First Byte is sending the Sub Command*/      
       ResponseBuffer[0]=subCmd;
       TRACE_M(TF_VENDOR_COMMAND,"#0E-%02hx! \n\r",data[0]);
       switch(subCmd)
       {     
         case APPLI_STRING_WRITE:
           {            
             memcpy(&ResponseBuffer,data,length);
             BuffLength = length;
             break;
           }
         default:
          {
            status = MOBLE_RESULT_FALSE;
            break;
          }
       }
      return status;        
}
                  
/**
* @brief  Appli_GetTestValue: This function is callback for Application
*          when Vensor application test command received then status message is to be provided
* @param  responseValue: Pointer to the status message
* @retval void
*/ 
void Appli_GetTestValue (MOBLEUINT8 *responseValue)
{
  *responseValue = TestHitCounter;
  *(responseValue+1)  = TestHitCounter >> 8;
  *(responseValue+2)  = TestHitCounter >> 16;
  *(responseValue+3)  = TestHitCounter >> 24;
  TestHitCounter = 0;
}
/**
 * @}
 */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

