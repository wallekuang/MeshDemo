/**
******************************************************************************
* @file    appli_nvm.c
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Application interface for NVM = Non Volatile memory interface
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
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "hal_common.h"
#include "appli_nvm.h"
#include "mesh_cfg.h"
#include "pal_nvm.h"
#include "appli_mesh.h"
#include "common.h"
#include "mesh_cfg_usr.h"

/** @addtogroup ST_BLE_Mesh
*  @{
*/

/** @addtogroup Application_Mesh_Models
*  @{
*/
/* Private define ------------------------------------------------------------*/

extern const MOBLEUINT8* _bdaddr[];
extern const MOBLEUINT8* _prvsnr_data[];
extern const void* bnrgmNvmBase;
extern MOBLEUINT8 PowerOnOff_flag;
MOBLEUINT8 PrvnFlag = 0;
extern MOBLEUINT16 nodeAddressOffset;


/* Reserved for Bluenrg-Mesh library */
#define BLUENRGMESH_NVM_BASE               ((unsigned int)bnrgmNvmBase)
#define BLUENRGMESH_NVM_BACKUP_BASE        (BLUENRGMESH_NVM_BASE + PAGE_SIZE)

/*
*  Page of size 2k, BlueNRG-1 and BlueNRG-2, reserved for application is divided into 8 subpages of each 256 bytes
*  First subpage is reserved (for e.g. External MAC is present in this area)
*  Rest 7 subpages are used on rolling basis for application states. 
*    First byte of each subpage indicates if page is valid or not
*/
#define APP_NVM_BASE                      ((unsigned int)_bdaddr)
#define APP_NVM_RESERVED_SIZE             256U
#define APP_NVM_SUBPAGE_SIZE              256U
#define APP_NVM_MAX_SUBPAGE               7U
#define APP_NVM_SUBPAGE_OFFSET(i)         (unsigned int)(APP_NVM_BASE+APP_NVM_RESERVED_SIZE+APP_NVM_SUBPAGE_SIZE*i)

/* offsets defined wrt start of subpage */
#define APP_NVM_VALID_FLAG_OFFSET         0U
#define APP_NVM_VALID_FLAG_SIZE           4U
#define APP_NVM_RESET_COUNT_OFFSET        (unsigned int)APP_NVM_VALID_FLAG_SIZE
#define APP_NVM_RESET_COUNT_SIZE          12U /* 12 bytes reserved for write cycle optimization */
#define APP_NVM_GENERIC_MODEL_OFFSET      (unsigned int)(APP_NVM_VALID_FLAG_SIZE+APP_NVM_RESET_COUNT_SIZE)
#define APP_NVM_LIGHT_MODEL_OFFSET        (unsigned int)(APP_NVM_VALID_FLAG_SIZE+APP_NVM_RESET_COUNT_SIZE+APP_NVM_GENERIC_MODEL_SIZE)

/* offset defined for the embedded provisioner node */
#define PRVN_NVM_PAGE_SIZE          2048U
#define PRVN_NVM_BASE_OFFSET        (unsigned int)_prvsnr_data    // 0X1007C800;
#define PRVN_NVM_SUBPAGE_SIZE       16U
#define PRVN_NVM_MAX_SUBPAGE        (unsigned int)(PRVN_NVM_PAGE_SIZE/PRVN_NVM_SUBPAGE_SIZE)
#define PRVN_NVM_SUBPAGE_OFFSET(i)  (unsigned int)(PRVN_NVM_BASE_OFFSET+PRVN_NVM_SUBPAGE_SIZE*i)

#define FIRST_PRVND_NODE_ADDRSS         2U
#define LAST_PRVND_NODE_ADDRSS    (unsigned int)(PRVN_NVM_MAX_SUBPAGE+FIRST_PRVND_NODE_ADDRSS)
/* Private variables ---------------------------------------------------------*/

typedef struct
{
  MOBLEUINT8  modelData[APP_NVM_GENERIC_MODEL_SIZE+APP_NVM_LIGHT_MODEL_SIZE];
  MOBLEBOOL erasePageReq;
  MOBLEBOOL writeReq;
} APPLI_NVM_REQS;

ALIGN(4) 
APPLI_NVM_REQS  AppliNvm_Reqs;

typedef struct
{
  MOBLEUINT8  prvnData[PRVN_NVM_SUBPAGE_SIZE];
  MOBLEBOOL erasePageReq;
  MOBLEBOOL writeReq;
} PRVN_NVM_REQS;

ALIGN(4) 
PRVN_NVM_REQS  PrvnNvm_Reqs;

/**
* @brief  This function erases a specified flash page
* @param  Page Number
* @retval MOBLE_RESULT_INVALIDARG if PageNumber overflow
*         MOBLE_RESULT_FALSE if flash operation is very close to next radio event
*         MOBLE_RESULT_FAIL if flash command error is set
*         MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliNvm_FlashErase(uint16_t PageNumber)
{
  MOBLE_RESULT result;
  
  if (PageNumber < N_PAGES)
  {
    BluenrgMesh_StopAdvScan();
    ATOMIC_SECTION_BEGIN();
    if (BluenrgMesh_IsFlashReadyToErase())
    {
      FLASH_ErasePage(PageNumber);
      
      if (FLASH_GetFlagStatus(Flash_CMDERR) == SET)
      {
        result = MOBLE_RESULT_FAIL;
      }
      else
      {
        result = MOBLE_RESULT_SUCCESS;
      }
    }
    else /* Radio not in a position to sync with flash operation */
    {
      result = MOBLE_RESULT_FALSE;
    }
    ATOMIC_SECTION_END();
  }
  else /* Invalid page no */
  {
    result = MOBLE_RESULT_INVALIDARG;
  }
  
  return result;
}

/** 
*  @brief  Program Flash using FLASH_WRITE or FLASH_BURSTWRITE
*          User to make sure that programming word burst should not happen across multiple pages
*  @param  Address (word aligned)
*  @param  Data to be programmed
*  @param  size of data to be programmed
* @retval  MOBLE_RESULT_INVALIDARG if parameters invalid
*          MOBLE_RESULT_FALSE if flash operation is very close to next radio event
*          MOBLE_RESULT_FAIL if flash command error is set
*          MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliNvm_FlashProgram(uint32_t Address, uint8_t Data[], uint16_t size)
{	
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  uint32_t* src = (uint32_t*)Data;
  
  if (Address < FLASH_START || Address > FLASH_END || Address & 3 
      || size & 3 || size > N_BYTES_PAGE)
  {
    result = MOBLE_RESULT_INVALIDARG;
  }
  else
  {
    BluenrgMesh_StopAdvScan();
    ATOMIC_SECTION_BEGIN();
    if (BluenrgMesh_IsFlashReadyToErase())
    {
      for (uint16_t count=0; count<size && FLASH_GetFlagStatus(Flash_CMDERR) == RESET; )
      {
        if (size-count >= 4*N_BYTES_WORD)
        {
          FLASH_ProgramWordBurst(Address+count, &src[count >> 2]);
          count += 4*N_BYTES_WORD;
        }
        else
        {
          FLASH_ProgramWord(Address+count, src[count >> 2]);
          count += N_BYTES_WORD;
        }
      }
      
      if (FLASH_GetFlagStatus(Flash_CMDERR) == SET)
      {
        result = MOBLE_RESULT_FAIL;
      }
      else
      {
        result = MOBLE_RESULT_SUCCESS;
      }
    }
    else
    {
      result = MOBLE_RESULT_FALSE;
    }
    ATOMIC_SECTION_END();
  }
  
  return result;
}

/**
*  @brief  Mark first valid subpage as invalid. 
*  @param  void
*  @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliNvm_MarkSubpageInvalid(void)
{
  MOBLEUINT32 valid = 0;
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  MOBLEINT8 currSubPageIdx;
  
  /* find valid subpage */
  result = AppliNvm_FindFirstEmptyPage(&currSubPageIdx,APP_NVM_SUBPAGE_SIZE,APP_NVM_MAX_SUBPAGE,
                                       (APP_NVM_BASE+APP_NVM_RESERVED_SIZE));//AppliNvm_FindFirstValidSubPage(&currSubPageIdx);
  
  
  if (result == MOBLE_RESULT_OUTOFMEMORY)
  {
    AppliNvm_Reqs.erasePageReq = MOBLE_TRUE;
  }
  else
  {    
    /* mark valid subpage as invalid */
    result = AppliNvm_FlashProgram(APP_NVM_SUBPAGE_OFFSET(currSubPageIdx)+APP_NVM_VALID_FLAG_OFFSET,
                                   (uint8_t*)&valid, 4);
    
    if (MOBLE_FAILED(result))
    {
      result = MOBLE_RESULT_FAIL;
    }
    else if (result == MOBLE_RESULT_FALSE)
    {
      /* radio busy, try again later */
    }
    else
    {
      /* do nothing */
    }
  }
  
  return result;
}

/**
* @brief  This function writes Reset count value in flash
*         if device has been turned ON 5 times consecutively with each ON duration < 2s, reset to factory settings
*         LED blinks 2 times to indicate device startup without reset
*         LED blinks 5 times to indicate reset at device startup
* @param  void
* @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliNvm_FactorySettingReset(void)
{
  MOBLEINT8 currSubPageIdx;
  MOBLE_RESULT result;
  MOBLEUINT32 resetCount;
  
  result = AppliNvm_FindFirstEmptyPage(&currSubPageIdx,APP_NVM_SUBPAGE_SIZE,APP_NVM_MAX_SUBPAGE,
                                       (APP_NVM_BASE+APP_NVM_RESERVED_SIZE));//AppliNvm_FindFirstValidSubPage(&currSubPageIdx);
  
  if(result == MOBLE_RESULT_OUTOFMEMORY)
  {
    AppliNvm_Reqs.erasePageReq = MOBLE_TRUE;
    result = MOBLE_RESULT_FAIL;
  }
  else
  {
    /* read saved reset counter from subpage */
    memcpy((void*)&resetCount, (void*)(APP_NVM_SUBPAGE_OFFSET(currSubPageIdx)+APP_NVM_RESET_COUNT_OFFSET), 4);
    resetCount = resetCount << 1; /* Shifting for making Bits 0 from LSB */
    
    /* Check for "POWER_ON_OFF_CYCLES_FOR_UNPROVISIONING" times, that many bits shall be zero */
    if (resetCount > MASK_BITS_FOR_POWER_ON_OFF_CYCLES)
    {
      /* MASK_BITS_FOR_POWER_ON_OFF_CYCLES = 0xFFFFFFE0 */
      
      /* update reset count */
      AppliNvm_FlashProgram(APP_NVM_SUBPAGE_OFFSET(currSubPageIdx)+APP_NVM_RESET_COUNT_OFFSET,
                            (uint8_t*)&resetCount, 4);
      
      if (!memcmp((void*)&resetCount, (void*)(APP_NVM_SUBPAGE_OFFSET(currSubPageIdx)+APP_NVM_RESET_COUNT_OFFSET), 4))
      {
        /* updated value of reset count in flash */
      }
      
      /* Blink twice to indicate device startup 
      1 second to blink once and 2 seconds to blink twice */
      for (MOBLEUINT8 i=0; i< ON_TIME_IN_SECONDS_FOR_POWER_CYCLING; i++)
      {
        Appli_LedBlink();
      }
      
      /* Load model data copy from nvm */
      memcpy((void*)AppliNvm_Reqs.modelData, 
             (void*)(APP_NVM_SUBPAGE_OFFSET(currSubPageIdx)+APP_NVM_GENERIC_MODEL_OFFSET), 
             APP_NVM_GENERIC_MODEL_SIZE+APP_NVM_LIGHT_MODEL_SIZE);
      
      result = AppliNvm_MarkSubpageInvalid();
      
      if (MOBLE_FAILED(result))
      {
        result = MOBLE_RESULT_FAIL;
      }
    }
    else /* Device is forced to factory reset, 5 LSBs are zero */
    {
      /* Unprovision node */
      BluenrgMesh_Unprovision();
      
      /* Clear lib data, primary and backup nvm used by BlueNRG-Mesh lib */
      FLASH_ErasePage((uint16_t)((BLUENRGMESH_NVM_BASE - RESET_MANAGER_FLASH_BASE_ADDRESS) / PAGE_SIZE));
      FLASH_ErasePage((uint16_t)((BLUENRGMESH_NVM_BACKUP_BASE - RESET_MANAGER_FLASH_BASE_ADDRESS) / PAGE_SIZE));
      
      AppliNvm_ClearModelState();
      
      result = AppliNvm_MarkSubpageInvalid();
      
      if (MOBLE_FAILED(result))
      {
        result = MOBLE_RESULT_FAIL;
      }
      
      BluenrgMesh_SetUnprovisionedDevBeaconInterval(100);
      
      /* Blink 5 times to indicate factory setting reset */
      for (MOBLEUINT8 i=0; i< BLINK_TIMES_FOR_FACTORY_RESET ; i++)
      {
        Appli_LedBlink();
      }
      
      TRACE_M(TF_MISC, "Reset to factory settings \r\n");
    }
    
    result = MOBLE_RESULT_SUCCESS;
  }
  
  return result;
}

/**
* @brief  Save model states in nvm
* @param  model state buff
* @param  model state buff size
* @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliNvm_SaveModelState(uint8_t* state, uint8_t size)
{
  MOBLE_RESULT result = MOBLE_RESULT_FAIL; /* if save model state not defined, return MOBLE_RESULT_FAIL */
  
#if (SAVE_MODEL_STATE_NVM == 1)
  
  if (size > APP_NVM_MODEL_SIZE)
  {
    /* incorrect size */
    result = MOBLE_RESULT_FAIL;
  }
  else
  {
    memcpy(AppliNvm_Reqs.modelData, state, size);
    
    AppliNvm_Reqs.writeReq = MOBLE_TRUE;
    AppliNvm_Reqs.erasePageReq = MOBLE_FALSE;
    result = AppliNvm_MarkSubpageInvalid();
    
    if (MOBLE_FAILED(result))
    {
      result = MOBLE_RESULT_FAIL;
    }
    else
    {
      result = MOBLE_RESULT_SUCCESS;
    }
  }
#endif /* SAVE_MODEL_STATE_NVM */  
  return result;
}

/**
* @brief  Clear model states in nvm
* @param  none
* @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliNvm_ClearModelState(void)
{
  MOBLE_RESULT result = MOBLE_RESULT_FAIL; /* if save model state not defined, return MOBLE_RESULT_FAIL */
  
#if (SAVE_MODEL_STATE_NVM == 1)
  MOBLEUINT8 clearBuff[APP_NVM_GENERIC_MODEL_SIZE+APP_NVM_LIGHT_MODEL_SIZE] = {0};
  
  memcpy(AppliNvm_Reqs.modelData, clearBuff, APP_NVM_GENERIC_MODEL_SIZE+APP_NVM_LIGHT_MODEL_SIZE);
  AppliNvm_Reqs.writeReq = MOBLE_TRUE;
  
  result = MOBLE_RESULT_SUCCESS;
#endif /* SAVE_MODEL_STATE_NVM */  
  return result;
}


/**
* @brief  Load Model states from nvm
* @param  model state buff
* @param  model state buff size
* @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliNvm_LoadModelState(uint8_t state[], uint8_t* size)
{  
#if (SAVE_MODEL_STATE_NVM == 1)  
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  
  memcpy((void*)state, (void*)(AppliNvm_Reqs.modelData), APP_NVM_MODEL_SIZE);
  *size = APP_NVM_MODEL_SIZE;
  
  return result;  
#else /* SAVE_MODEL_STATE_NVM */  
  *size = 0;
  return MOBLE_RESULT_SUCCESS;
#endif /* SAVE_MODEL_STATE_NVM */
}


/**
* @brief  Process NVM erase and write requests
* @param  void
* @retval void
*/
void AppliNvm_Process(void)
{
  MOBLE_RESULT result;
  MOBLEINT8 subPageIdx;
  uint8_t reserveAreaCopy[APP_NVM_RESERVED_SIZE];
  
  /* Erase if required */
  if (AppliNvm_Reqs.erasePageReq == MOBLE_TRUE)
  {
    /* save reserve flash area */
    memcpy((void*)reserveAreaCopy, (void*)APP_NVM_BASE, 16);
    
    result = AppliNvm_FlashErase((uint16_t)((APP_NVM_BASE - RESET_MANAGER_FLASH_BASE_ADDRESS) / PAGE_SIZE));
    
    if (result == MOBLE_RESULT_SUCCESS)
    {
      AppliNvm_Reqs.erasePageReq = MOBLE_FALSE;
      AppliNvm_Reqs.writeReq = MOBLE_TRUE;
      /* restore reserve area */
      FLASH_ProgramWordBurst(APP_NVM_BASE, (uint32_t*)reserveAreaCopy);
    }
  }
  
  if (AppliNvm_Reqs.erasePageReq == MOBLE_FALSE
      && AppliNvm_Reqs.writeReq == MOBLE_TRUE)
  {
    result = AppliNvm_FindFirstEmptyPage(&subPageIdx,APP_NVM_SUBPAGE_SIZE,APP_NVM_MAX_SUBPAGE,
                                         (APP_NVM_BASE+APP_NVM_RESERVED_SIZE));//AppliNvm_FindFirstValidSubPage(&subPageIdx);
    if(result == MOBLE_RESULT_OUTOFMEMORY)
    {
      AppliNvm_Reqs.erasePageReq = MOBLE_TRUE;
      result = MOBLE_RESULT_FAIL;
    }
    
    if (result == MOBLE_RESULT_SUCCESS)
    {  
       result = AppliNvm_FlashProgram(APP_NVM_SUBPAGE_OFFSET(subPageIdx)+APP_NVM_GENERIC_MODEL_OFFSET, 
                                     AppliNvm_Reqs.modelData,
                                     APP_NVM_GENERIC_MODEL_SIZE+APP_NVM_LIGHT_MODEL_SIZE);
    
      if (result == MOBLE_RESULT_SUCCESS)
      {
        AppliNvm_Reqs.writeReq = MOBLE_FALSE;
      }
    }
    else
    {
      /* do nothing */
    }
    
  }
}

/**
* @brief  Fuction used to set the flag which is responsible for storing the 
states in flash.
* @param  void
* @retval void
*/
void AppliNvm_SaveMessageParam (void)
{
#ifdef SAVE_MODEL_STATE_FOR_ALL_MESSAGES
  PowerOnOff_flag = FLAG_SET;
#endif 
} 


/********************Embedded Provisioner *******************************/

/**
*  @brief  Find first valid subpage available. 
*          If no valid subpage found, erase page appli nvm to reset it
*  @param  pointer to variable to update first valid subpage index
*  @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliNvm_FindFirstEmptyPage(MOBLEINT8* subPageIndex,MOBLEUINT32 SubPageSize,
                                         MOBLEUINT32 totalSubPage, MOBLEUINT32 nvmBaseOffset)
{
  MOBLEUINT32 valid;
  MOBLEINT8 subPageIdx = -1;
  MOBLEUINT32 *subPageOffset;
  MOBLEUINT32 addrssValidPage;
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  
  for (MOBLEUINT8 count=0; count<totalSubPage; count++)
  {
    addrssValidPage = nvmBaseOffset + SubPageSize*count;
    subPageOffset = &addrssValidPage;
    memcpy((void*)&valid, (void*)(*subPageOffset), 4);
    
    if (valid == 0xFFFFFFFF)
    {
      subPageIdx = count;   
      break;
    }
  }
  
  if (subPageIdx < 0)
  {
    TRACE_M(TF_PROVISION,"Erase operation for data saving \r\n");
    result = MOBLE_RESULT_OUTOFMEMORY;
    //AppliNvm_Reqs.erasePageReq = MOBLE_TRUE;
  }
  else
  {
    result = MOBLE_RESULT_SUCCESS;
  }
  
  *subPageIndex = subPageIdx;
  
  return result;
}

/**
* @brief  Load Provisioner data from nvm
* @param  model state buff
* @param  model state buff size
* @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliPrvnNvm_LoadData(uint8_t state[], uint8_t* size)
{  
#if (SAVE_EMBD_PROVISION_DATA == 1)  
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  
  memcpy((void*)state, (void*)(PrvnNvm_Reqs.prvnData), PRVN_NVM_SUBPAGE_SIZE);
  *size = PRVN_NVM_SUBPAGE_SIZE;
  
  return result;  
#else /* SAVE_EMBD_PROVISION_DATA */  
  *size = 0;
  return MOBLE_RESULT_SUCCESS;
#endif /* SAVE_EMBD_PROVISION_DATA */
}

/**
* @brief  Save Embedded provisioner data in nvm
* @param  data: data buffer to be saved
* @param  size: data buffer size
* @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliPrvnNvm_SaveData(uint8_t* data, uint8_t size)
{
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS; /* if save model state not defined, return MOBLE_RESULT_FAIL */
  
  if (size > PRVN_NVM_SUBPAGE_SIZE)
  {
    /* incorrect size */
    result = MOBLE_RESULT_FAIL;
  }
  else
  {
    memcpy(PrvnNvm_Reqs.prvnData, data, size);
    PrvnNvm_Reqs.writeReq = MOBLE_TRUE;
    
    if (MOBLE_FAILED(result))
    {
      result = MOBLE_RESULT_FAIL;
    }
    else
    {
      result = MOBLE_RESULT_SUCCESS;
    }
  }
  
  return result;
}

/**
* @brief  Function to get the particular node device key with node address friom the 
*         provisioner flash.
* @param  MOBLE_ADDRESS: node address
* @retval subPageAddrss: pointer to the sub page address
*/
MOBLEUINT32* AppliPrvnNvm_GetNodeDevKey(MOBLE_ADDRESS addrss)
{
  MOBLEUINT16 subPageIndx;
  MOBLEUINT32 *subPageAddrss;
  
  subPageIndx = addrss - FIRST_PRVND_NODE_ADDRSS;
  
  if(subPageIndx >= PRVN_NVM_MAX_SUBPAGE)
  {
    TRACE_M(TF_PROVISION,"Invalid Address \r\n");
    return 0;   
  }
  else
  {
    subPageAddrss = (MOBLEUINT32 *)(PRVN_NVM_SUBPAGE_OFFSET(subPageIndx));
    TRACE_M(TF_PROVISION,"Address Of SubPage =  %0.8x \r\n", subPageAddrss);
  }
  return subPageAddrss;
}

/**
* @brief  backup by Copy all the data from the page in RAM.
* @param  model state buff
* @param  model state buff size
* @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliPrvnNvm_CopyData(uint8_t state[], uint8_t* size)
{
  
#if SAVE_EMBD_PROVISION_DATA    
  MOBLEUINT8 deviceKeysSaved[2048]; 
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  
  /* find valid subpage */
  for (MOBLEUINT8 count=0; count<8; count++)
  {    
    memcpy((void*)deviceKeysSaved, (void*)(PRVN_NVM_SUBPAGE_OFFSET(count)), PRVN_NVM_SUBPAGE_SIZE);
  }
  
  *size = PRVN_NVM_SUBPAGE_SIZE;
  
  return result;  
#else /* SAVE_EMBD_PROVISION_DATA */  
  *size = 0;
  return MOBLE_RESULT_SUCCESS;
#endif /* SAVE_EMBD_PROVISION_DATA */
  
}

/**
* @brief  Process Provisioner NVM erase and write requests
* @param  void
* @retval void
*/
void AppliPrvnNvm_Process(void)
{
  MOBLE_RESULT result;
  MOBLEINT8 subPageIdx;
  
  if ( (PrvnNvm_Reqs.erasePageReq == MOBLE_FALSE)
      && (PrvnNvm_Reqs.writeReq == MOBLE_TRUE) )
  {
    result = AppliNvm_FindFirstEmptyPage(&subPageIdx,PRVN_NVM_SUBPAGE_SIZE,
                                         (PRVN_NVM_MAX_SUBPAGE-1),PRVN_NVM_BASE_OFFSET);//AppliPrvnNvm_FindFirstValidSubPage(&subPageIdx);
    if(result == MOBLE_RESULT_OUTOFMEMORY)
    {
      PrvnNvm_Reqs.erasePageReq = MOBLE_TRUE;
      result = MOBLE_RESULT_FAIL;
    }
    else 
    {
      TRACE_M(TF_PROVISION,"Saving in SubPage[%.8x] = \r\n", PRVN_NVM_SUBPAGE_OFFSET(subPageIdx));
     
        result = AppliNvm_FlashProgram(PRVN_NVM_SUBPAGE_OFFSET(subPageIdx), 
                                       PrvnNvm_Reqs.prvnData,
                                       PRVN_NVM_SUBPAGE_SIZE);    
      
      if (result == MOBLE_RESULT_SUCCESS)
      {
        PrvnNvm_Reqs.writeReq = MOBLE_FALSE;
      }
    }
  }
}

/**
* @brief  This function is to save the provisioner device key in the flash.          
* @param  data:Pointer to the data passed.
* @param  size:size of the data
* @param  prvnFlag:Pointer to the flag passed.
* @retval void
*/
void AppliNnm_saveProvisionerDevKey(MOBLEUINT8 *data , MOBLEUINT8 size , MOBLEUINT8 *prvnFlag)
{
  if(*prvnFlag == 1)
  {
    AppliPrvnNvm_SaveData(&data[0] ,size);
    prvnFlag = 0;
  }
}
/**
* @brief  This function load the Provisioner nvm data to the buffer after reset.          
* @param  void
* @retval MOBLE_RESULT_SUCCESS on success
*/
MOBLE_RESULT AppliPrvnNvm_FactorySettingReset(MOBLEUINT8 *flag)
{
  MOBLEINT8 currSubPageIdx;
  MOBLE_RESULT result;
  
  result = AppliNvm_FindFirstEmptyPage(&currSubPageIdx,PRVN_NVM_SUBPAGE_SIZE,
                                       (PRVN_NVM_MAX_SUBPAGE-1),PRVN_NVM_BASE_OFFSET);//AppliPrvnNvm_FindFirstValidSubPage(&currSubPageIdx);
  
  if(result == MOBLE_RESULT_OUTOFMEMORY)
  {
    result = MOBLE_RESULT_FAIL;
    //PrvnNvm_Reqs.erasePageReq = MOBLE_TRUE;
  }
  else
  {   
    /* After reset of provisioner , retreiving the next node address to be provisioned */
#ifdef ENABLE_PROVISIONER_FEATURE
    if(currSubPageIdx == 0)
    {
      *flag = 1;
    }
    if(currSubPageIdx > 1)
    {
      nodeAddressOffset = currSubPageIdx;         
    }
    
#endif           
    
    
    if(currSubPageIdx > 0)
    {
      currSubPageIdx = currSubPageIdx -1;
    }
    
    /* Load model data copy from nvm */       
    memcpy((void*)PrvnNvm_Reqs.prvnData, 
           (void*)(PRVN_NVM_SUBPAGE_OFFSET(currSubPageIdx)),PRVN_NVM_SUBPAGE_SIZE);    
    
    TRACE_I(TF_PROVISION,"NEXT NVM ADDRESS %.8x \r\n",PRVN_NVM_SUBPAGE_OFFSET(currSubPageIdx+PRVN_NVM_SUBPAGE_SIZE));
    
  }
  
  return result;
}

