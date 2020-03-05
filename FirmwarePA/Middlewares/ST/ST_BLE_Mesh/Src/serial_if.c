/**
******************************************************************************
* @file    serial_if.c
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Serial Interface file 
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
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
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "hal_common.h"
#include "mesh_cfg.h"
#include "serial_if.h"
#if ENABLE_SERIAL_CONTROL
#include "serial_ctrl.h"
#endif
#if ENABLE_UT
#include "serial_ut.h"
#endif
#if ENABLE_APPLI_TEST
#include "appli_test.h"
#endif
#if ENABLE_SERIAL_PRVN
#include "serial_prvn.h"
#endif
/** @addtogroup BlueNRG_Mesh
*  @{
*/

/** @addtogroup Application
*  @{
*/

/* Private define ------------------------------------------------------------*/
#define RECEIVE_STRING_SIZE     48

/* Private macro -------------------------------------------------------------*/
typedef enum 
{
    STATE_IDLE,
    STATE_INPUT_ENTERED,
}tSerialState;

/* Private variables ---------------------------------------------------------*/
static char Rcvd_String[RECEIVE_STRING_SIZE];
tSerialState SerialCurrentState = STATE_IDLE;

/* Private function prototypes -----------------------------------------------*/
static int Serial_GetString(MOBLEUINT8*, MOBLEUINT8);

/* Private functions ---------------------------------------------------------*/
/**
* @brief  This funcrion is used to parse the string given by the user(If 
*         implemented in application, 
*         linker would replace weak linking in library )
* @param  rcvdStringBuff: buffer to store input string 
* @param  rcvdStringSize: length of the input string 
* @retval void
*/ 
#if (!ENABLE_SERIAL_CONTROL)
__weak void SerialCtrl_Process(char *rcvdStringBuff, uint16_t rcvdStringSize)
{}
__weak void SerialCtrlVendorRead_Process(char *rcvdStringBuff, uint16_t rcvdStringSize)
{}
__weak void SerialCtrlVendorWrite_Process(char *rcvdStringBuff, uint16_t rcvdStringSize)
{}
#endif
/**
* @brief  Upper Tester control commands (If implemented in application, 
*         linker would replace weak linking in library )
* @param  rcvdStringBuff: Pointer to received string   
* @retval void
*/
#if (!ENABLE_UT)
__weak void SerialUt_Process(char *rcvdStringBuff, uint16_t rcvdStringSize)
{
}
#endif 

#if (!ENABLE_APPLI_TEST)
__weak void SerialResponse_Process(char *rcvdStringBuff, uint16_t rcvdStringSize)
{
}
#endif

#if (!ENABLE_SERIAL_PRVN)
__weak void SerialPrvn_Process(char *rcvdStringBuff, uint16_t rcvdStringSize)
{
}
#endif
/**
* @brief  Processes data coming from serial port   
* @param  void  
* @retval void
*/
void Serial_InterfaceProcess(void)
{
    int stringSize = 0;
    
    stringSize = Serial_GetString((MOBLEUINT8*)Rcvd_String, sizeof(Rcvd_String) - 1);
    /* Check if no input has come from user */
    if (!stringSize)
    {
        return;
    }
    else
    {
        Rcvd_String[stringSize] = 0; /* Make last char NULL for string comp */

        /* Check if correct string has been entered or not */
#ifdef ENABLE_SERIAL_CONTROL
        if (!strncmp(Rcvd_String, "ATCL", 4))
        {            
            SerialCtrl_Process(Rcvd_String, stringSize);
        }
        else if (!strncmp(Rcvd_String, "ATVR", 4))
        {            
            SerialCtrlVendorRead_Process(Rcvd_String, stringSize);
        }
        else if (!strncmp(Rcvd_String, "ATVW", 4))
        {            
            SerialCtrlVendorWrite_Process(Rcvd_String, stringSize);
        }
#endif
#if ENABLE_UT
        else if(!strncmp(Rcvd_String, "ATUT", 4))
        {
            SerialUt_Process(Rcvd_String, stringSize);  
        }
#endif
#if ENABLE_APPLI_TEST
        else if(!strncmp(Rcvd_String, "ATAP", 4))
        {
            SerialResponse_Process(Rcvd_String, stringSize);  
        }
#endif
#ifdef ENABLE_AUTH_TYPE_INPUT_OOB        
        else if(!strncmp(Rcvd_String, "ATIN", 4))
        {
            Appli_BleSerialInputOOBValue(Rcvd_String, stringSize);  
        }   
#endif
#if ENABLE_SERIAL_PRVN        
        else if(!strncmp(Rcvd_String, "ATEP", 4))
        {
          SerialPrvn_Process(Rcvd_String, stringSize);
        }
#endif        
        else
        {
            TRACE_I(TF_SERIAL_CTRL,"Not Entered valid test parameters\r\n");  
            SerialCurrentState = STATE_IDLE;
        }
        while(stringSize){
          Rcvd_String[--stringSize] = 0;
        }
    }
}

/**
* @brief  Gets the input from user from Serial port
* @param  text: String to take input 
* @param  size: Size of string
* @retval int: Running Status of the test case
*/
static int Serial_GetString(MOBLEUINT8* text, MOBLEUINT8 size)
{
    static int index = 0;
    int stringSize = 0;
    
#ifndef __IAR_SYSTEMS_ICC__
        clearerr(stdin);
#endif
    int ch = getchar();
    /* Note: Please use Full Library configuration in project options to use the full 
         configuration of the C/C++ runtime library for printf and scanf functionality */
    /* Check for error in get function */
    if (ch == EOF)
    {                       
        clearerr(stdin);
    }
    /* check for backspace press */
    else if ((ch == 0x7F) | (ch == 0x08))
    {
        if (index) --index;
        TRACE_I(TF_SERIAL_CTRL,"\b");
    }
    /* Check for the enter key*/
    else if ((ch == 0x0D) || (ch == 0xFFFFFF0D))
    {
        /* check for first time enter to display help message */
        if (!index)
        {
            stringSize = 1;
        }
        else
        {
            stringSize = index;
        }
        index = 0;
        TRACE_I(TF_SERIAL_CTRL,"\n\r");
        return stringSize;    
    }
    else
    {
      TRACE_I(TF_SERIAL_CTRL,"%c", ch);
        if (index < size)
        {        
            /* cehck if lowercase and convert it to upper case */
          if ((ch >= 'a') && (ch <= 'z'))
        {
            ch = ch + 'A' - 'a';
        }
        text[index++] = (char)ch;
        }
    }
    return stringSize;
}

/**
* @brief  Convert ASCII character to hexadecimal number
* @param  addr: input ASCI character
* @retval MOBLEUINT8
*/ 

 MOBLEUINT8 Serial_CharToHexConvert(char addr)
{
  MOBLEUINT8 retVal=0;
  if (addr >= '0' && addr <= '9')
        retVal = addr - '0';
  else if (addr >= 'a' && addr <= 'f')
        retVal = addr+10-'a';
  else if (addr >= 'A' && addr <= 'F')
        retVal = addr+10-'A';
  else if (addr == ' ')
        retVal = addr+10-' ';
  else
       return 0xFF;
    
  return retVal;
}

/**
* @brief  Callback function to print data serially  
* @param  *message: Pointer of data string  
* @retval void
*/
void BluenrgMesh_PrintStringCb(const char *message)
{
    TRACE_I(TF_SERIAL_CTRL,"%s\n\r", (char*)message);
}
/**
* @brief  Callback function to print data array on screen LSB first 
* @param  *data : Pointer to the data to be printed 
* @param  size : Length of data to be printed
* @retval void
*/
void BluenrgMesh_PrintDataCb(MOBLEUINT8* data, MOBLEUINT16 size)
{
    for (int count=0; count<size; ++count)
    {
        TRACE_I(TF_SERIAL_CTRL,"%02X", data[count]);
    }
    TRACE_I(TF_SERIAL_CTRL,"\n\r");
}
/**
* @}
*/

/**
* @}
*/
/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
