/**
******************************************************************************
* @file    appli_config_client.c
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Application interface for Config CLient Mesh Model
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
#include "config_client.h"
#include "appli_config_client.h"
#include "appli_mesh.h"

/** @addtogroup ST_BLE_Mesh
*  @{
*/

/** @addtogroup Application_Mesh_Models
*  @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define DEFAULT_GROUP_ADDR   0xC000
#define DEFAULT_PUBLISH_ADDR 0xC000
#define DEFAULT_NETKEY_INDEX 0x0000
#define DEFAULT_APPKEY_INDEX 0x0000
#define DEFAULT_CREDENTIAL_FLAG 0x00
#define DEFAULT_PUBLISH_TTL   0x08
#define DEFAULT_PUBLISH_PERIOD   0x00
#define DEFAULT_PUBLISH_RETRANSMIT_COUNT 0x00   
#define DEFAULT_PUBLISH_RETRANSMIT_INTERVAL_STEPS 0x00

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

eClientSendMsgState_t eClientSendMsgState; /* Keeps the state of Sent Message */
eServerRespRecdState_t eServerRespRecdState; /* Keeps the state of Received Message */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  This function is callback from library after the Provisioning is 
          completed by embedded Provisioner. 
* @param  prvState: Provisioning State of the Node. Expecting "1" when the
                   provisioning is completed
* @retval None
*/ 
void Appli_ConfigClientStartNodeConfiguration(MOBLEUINT8 prvState)
{
  if (prvState==1 )
  {
    Appli_ConfigClient_Init();
    eClientSendMsgState = ProvisioningDone_State;
    NodeInfo.nodePrimaryAddress = GetAddressToConfigure();
  }
}

/**
* @brief  This function is Init function for the state machine of the 
          Configuration Client. The Function shall be called everytime a new 
          node is provisioned 
* @param  None
* @retval None
*/ 
MOBLE_RESULT Appli_ConfigClient_Init(void) 
{
  eClientSendMsgState = ClientIdle_State; /* Init the value of state machine  */ 
  eServerRespRecdState = NodeIdle_State;  /* Init the value of state machine  */
  ConfigClient_ResetTrials();
  
  //eClientSendMsgState = SetPublication_State;  /* Alok - Testing */
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Appli_ConfigClient_Process: This function is Process function and 
          shall be called from while(1) loop 
* @param  None
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_ConfigClient_Process(void) 
{
    
  Appli_ConfigClient_ConfigureNode();
  
  return MOBLE_RESULT_SUCCESS;
}


/**
* @brief  This function is used to configure the Node after provisioning
* @param  None
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_ConfigClient_ConfigureNode(void) 
{
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  MOBLEUINT32 nowClockTime;
  
    /* 
    State                  response                   called function
    --------------------------------------------------------------------------------
    ClientIdle_State       X                          None
    ProvisioningDone_State X                          Start the configuration
    CompositionGet_State   ConfigRespInit_State       GetComposition
    CompositionGet_State   CompositionRecd_State      ChangeThe State to next 
    AppKeyAdd_State        X                          Issue AppKey
    AppKeyAdd_State        AppkeyAck_State            ChangeThe State to next 
    AppBindModel_State     X                          Issue AppKetBind
    AppBindModel_State     AppBindModelAck_State      ChangeThe State to next 
    AddSubscription_State  X                          Issue the Subscription 
    AddSubscription_State  SubscriptionAck_State      ChangeThe State to next 
    SetPublication_State   PublicationStatus_State    Issue the Subscription 
    SetPublication_State   PublicationStatus_State    ChangeThe State to next 

  */
  
  /* If the Node is already configured, return from here  */
  if (eClientSendMsgState == ConfigurationDone_State) 
    return result;
  
  if (eClientSendMsgState == ClientIdle_State) 
  {
    /* Waiting for the Provisioning to be done before to Start the 
       node configuration procedure */
      return result;
  }
  
  if (eServerRespRecdState == NodeNoResponse_State) 
  {
    /* No Response received from Node under Provisioning for some config 
       messages. So, no need to do the trials  */
      return MOBLE_RESULT_FAIL;
  }
  
  if (eClientSendMsgState == ProvisioningDone_State) 
  {
    /* Start the node configuration procedure */
    eClientSendMsgState = CompositionGet_State;
    ConfigClient_SaveMsgSendingTime();
  }
  
  else if (eClientSendMsgState == CompositionGet_State)
  {

     nowClockTime = Clock_Time();
     if( (nowClockTime - NodeInfo.Initial_time) < CONFIGURATION_START_DELAY)
     {
       return result;
     }
     /*------------- Add the delay before to start the configuration messages */
    
    if (eServerRespRecdState == CompositionRecdCompleted_State)
    {
      eClientSendMsgState = AppKeyAdd_State;  /* Change the state to Next */
      eServerRespRecdState = NodeIdle_State;
      
      /* Modify this for the number of Models to bind or configure */
      SetSIGModelCountToConfigure(1);  
      SetVendorModelCountToConfigure(1);
    }
    else 
    {
      /* Continue the GetComposition servicing */
      Appli_ConfigClient_GetCompositionData();
    }
  }

  else if (eClientSendMsgState == AppKeyAdd_State)
  {
    if (eServerRespRecdState == AppkeyAckCompleted_State)
    {
      eClientSendMsgState = AppBindModel_State;  /* Change the send state */
      eServerRespRecdState = NodeIdle_State;
    }
    else
    {
      /* Continue the AppKeyAdd servicing */
      Appli_ConfigClient_DefaultAppKeyAdd();
    }
  }  
    
  else if (eClientSendMsgState == AppBindModel_State)
  {
    if (eServerRespRecdState == AppBindModelAckCompleted_State)
    {
      eClientSendMsgState = AddSubscription_State;  /* Change the send state */
      eServerRespRecdState = NodeIdle_State;
    }
    else
    {
       /* Continue the AppKeyBIND servicing */
      Appli_ConfigClient_DefaultAppKeyBind();
    }
  }

  else if (eClientSendMsgState == AddSubscription_State)
  {
    if (eServerRespRecdState == SubscriptionAckCompleted_State)
    {
      eClientSendMsgState = SetPublication_State;  /* Change the send state */
      eServerRespRecdState = NodeIdle_State;
    }
    else 
    {
      /* Continue the Subscription add servicing */
      AppliConfigClient_SubscriptionAddDefault();
    }    
  }

  else if (eClientSendMsgState == SetPublication_State)
  {
    if (eServerRespRecdState == PublicationStatusCompleted_State)
    {
      eClientSendMsgState = ConfigurationDone_State;  /* Change the send state */
      eServerRespRecdState = NodeIdle_State;
      TRACE_M(TF_CONFIG_CLIENT,"**Node is configured** \r\n");  
    }
    else 
    {
      /* Continue the Publication add servicing */
      AppliConfigClient_PublicationSetDefault();
    }
  }
  
  return result;
}




/**
* @brief  This function is called to Get add the default AppKeys and net keys to a node under configuration
* @param  None
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_ConfigClient_GetCompositionData (void)
{
    MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
    
    switch(eServerRespRecdState)
    {
    case NodeIdle_State:

      ConfigClient_SaveMsgSendingTime();
      /* Start the Get Composition Message */
      ConfigClient_CompositionDataGet();

      /* Switch to InProgress_State */
      eServerRespRecdState = InProgress_State;
      break;

    case CompositionRecd_State:
      /* Switch the state to next state AddAppKey_State */
      ConfigClient_ResetTrials();
      eServerRespRecdState = CompositionRecdCompleted_State;
      break;
      
    case InProgress_State:
      /* Just wait and let the messages be completed 
         or look for timeout */
      ConfigClient_ChkRetrialState (&eServerRespRecdState);
      break;
      
    default:
      /* Error State */
      break;
    }
    
  return result;
}


/**
* @brief  Appli_ConfigClient_DefaultAppKeyAdd: This function is called to 
          add the default AppKeys and net keys to a node under configuration
* @param  None
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_ConfigClient_DefaultAppKeyAdd (void)
{
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
   
   MOBLEUINT8 *pAppKey;  GetNewProvNodeAppKey();
   MOBLEUINT16 netKeyIndex = DEFAULT_NETKEY_INDEX;
   MOBLEUINT16 appKeyIndex = DEFAULT_APPKEY_INDEX;

   pAppKey = GetNewProvNodeAppKey();
   
    switch(eServerRespRecdState)
    {
    case NodeIdle_State:
      ConfigClient_SaveMsgSendingTime();
      /* Start the Set Appkey message  */
      ConfigClient_AppKeyAdd (netKeyIndex, 
                                  appKeyIndex,
                                  pAppKey);
      /* Switch to InProgress_State */
      eServerRespRecdState = InProgress_State; 
      break;

    case AppkeyAck_State:
      ConfigClient_ResetTrials();
      eServerRespRecdState = AppkeyAckCompleted_State;
      break;
      
    case InProgress_State:
      /* Just wait and let the messages be completed 
         or look for timeout */
      ConfigClient_ChkRetrialState (&eServerRespRecdState);
      break;
      
    default:
      /* Error State */
      break;
    } 
    
  return result;
}

/**
* @brief  Appli_ConfigClient_DefaultAppKeyBind: This function is application used for 
          function to Bind the element(node) with AppKeyIndex and Models
* @param  pGeneric_OnOffParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_ConfigClient_DefaultAppKeyBind (void)
{

    /* 
    ElementAddress : 2B : Address of the element
    AppKeyIndex : 2B : Index of the AppKey
    ModelIdentifier : 2 or 4: SIG Model ID or Vendor Model ID
    */  
   MOBLEUINT32 modelIdentifier;
   MOBLEUINT16 appKeyIndex = DEFAULT_APPKEY_INDEX;
   static MOBLEUINT16 elementAddress;
   static MOBLEUINT8 elementIndex;
   static MOBLEUINT8  indexSIGmodels;
   static MOBLEUINT8  indexVendormodels;
   MOBLEUINT8  numSIGmodels;
   MOBLEUINT8  numVendorModels;
   MOBLEUINT8  numofElements;        
   MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  
    switch(eServerRespRecdState)
    {
    case NodeIdle_State:
      /* Start the AppBindModel_State message  */
        appKeyIndex = DEFAULT_APPKEY_INDEX;
        elementIndex = 0; /* Initialize it for the complete loop */
        indexSIGmodels = 0; /* Initialize it for the complete loop */
        indexVendormodels = 0;
        elementAddress = GetNodeElementAddress();
        elementAddress += elementIndex;
        modelIdentifier = GetSIGModelFromCompositionData(elementIndex,indexSIGmodels );
        /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;
        
        ConfigClient_SaveMsgSendingTime();
        ConfigClient_ModelAppBind (elementAddress, appKeyIndex, modelIdentifier);
      
      break;

    case AppBindModelAck_State:
       /* Need to check if all SIG Models are binded ? */

      ConfigClient_ResetTrials();
            
      numSIGmodels = GetNumberofSIGModels(elementIndex);
      numVendorModels = GetNumberofVendorModels(elementIndex);
      appKeyIndex = DEFAULT_APPKEY_INDEX;
      elementAddress = GetNodeElementAddress();
      elementAddress += elementIndex;
      
      if (indexSIGmodels < numSIGmodels )
      { /* Even when all SIG Models are serviced, we need to start for Vendor Models */
        indexSIGmodels++; 
        indexVendormodels =0;  /* Reset back, bcoz, we are still process the SIG Models */
      }
      else if (indexVendormodels < numVendorModels)
      {
        indexVendormodels++; /* When SIG Models and Vendor Models are processed
                                the loop condition will become true */
      }
      
      if (indexSIGmodels < numSIGmodels )
      {/* if index is still less, then we have scope of reading 1 more index */
       
        /* Get the Next Model and Bind it again till all SIG Models are binded */
        modelIdentifier = GetSIGModelFromCompositionData(elementIndex,indexSIGmodels );
        /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;
        ConfigClient_SaveMsgSendingTime();
        ConfigClient_ModelAppBind (elementAddress, appKeyIndex, modelIdentifier); 
 
      }
      else if (indexVendormodels < numVendorModels)
      {
        /*Now, do binding for Vendor Model */
        modelIdentifier = GetVendorModelFromCompositionData(elementIndex,indexVendormodels );
        
        /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;
        ConfigClient_SaveMsgSendingTime();
        ConfigClient_ModelAppBind (elementAddress, appKeyIndex, modelIdentifier); 

      }
      else
      {
        /* Now, the element index is handled, change the element index */
        elementIndex++;
        numofElements = ConfigClient_GetNodeElements();  
        if (elementIndex >=  numofElements)
        {/* we are comparing Index whose counting started from 0, becomes equal, 
            then exit the loop */
          
           eServerRespRecdState = AppBindModelAckCompleted_State; 
        }
        else if (elementIndex < numofElements)
        { /* When the Element Index is still less than the total number of 
             elements in the Node: So, Restart the cycle */
          
          eServerRespRecdState = NodeIdle_State; 
          indexSIGmodels =0; /* Reset the variable again for the next element */
          indexVendormodels = 0;
        }
      }
      break;
      
    case InProgress_State:
      /* Just wait and let the messages be completed 
         or look for timeout */
      ConfigClient_ChkRetrialState (&eServerRespRecdState);
      break;
      
    default:
      /* Error State */
      break;
    }
    
  return result;
}


/**
* @brief  AppliConfigClient_SubscriptionAddDefault: This function is application
          used for adding subscription to the element(node) for default settings
* @param  pGeneric_OnOffParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT AppliConfigClient_SubscriptionAddDefault (void) 
{
   static MOBLEUINT32 modelIdentifier;
   static MOBLEUINT16 elementAddress;
   static MOBLEUINT8 elementIndex;
   static MOBLEUINT8  indexSIGmodels;
   static MOBLEUINT8  indexVendormodels;
   MOBLEUINT8  numSIGmodels;
   MOBLEUINT8  numVendorModels;
   MOBLEUINT8  numofElements;        
   MOBLEUINT16 address = DEFAULT_GROUP_ADDR;
   MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
   
    switch(eServerRespRecdState)
    {
    case NodeIdle_State:
      /* Start the SubscriptionAdd message  */

        elementIndex = 0; /* Initialize it for the complete loop */
        indexSIGmodels = 0; /* Initialize it for the complete loop */
        indexVendormodels = 0;
        elementAddress = GetNodeElementAddress();
        elementAddress += elementIndex;
        modelIdentifier = GetSIGModelFromCompositionData(elementIndex,indexSIGmodels );
         /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;        
        ConfigClient_SubscriptionAdd (elementAddress, address, modelIdentifier);

      break;

    case SubscriptionAck_State:
       /* Need to check if all SIG Models are subscribed ? */
      ConfigClient_ResetTrials();
      
      numSIGmodels = GetNumberofSIGModels(elementIndex);
      numVendorModels = GetNumberofVendorModels(elementIndex);
      elementAddress = GetNodeElementAddress();
      elementAddress += elementIndex;
     
      
      if (indexSIGmodels < numSIGmodels )
      { /* Even when all SIG Models are serviced, we need to start for Vendor Models */
        indexSIGmodels++; 
        indexVendormodels =0;  /* Reset back, bcoz, we are still process the SIG Models */
      }
      else if (indexVendormodels < numVendorModels)
      {
        indexVendormodels++; /* When SIG Models and Vendor Models are processed
                                the loop condition will become true */
      }
      
      if (indexSIGmodels < numSIGmodels )
      {/* if index is still less, then we have scope of reading 1 more index */
       
        /* Get the Next Model and Bind it again till all SIG Models are binded */
        modelIdentifier = GetSIGModelFromCompositionData(elementIndex,indexSIGmodels);
        
        /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;
        ConfigClient_SaveMsgSendingTime();
        ConfigClient_SubscriptionAdd (elementAddress, address, modelIdentifier);
      }
      else if (indexVendormodels < numVendorModels)
      {
        modelIdentifier = GetVendorModelFromCompositionData(elementIndex,indexVendormodels );
        
        /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;
        ConfigClient_SaveMsgSendingTime();
        ConfigClient_SubscriptionAdd (elementAddress, address, modelIdentifier);
      }
      else
      {
        /* Now, the element index is handled, change the element index */
        elementIndex++;
        numofElements = ConfigClient_GetNodeElements();  
        if (elementIndex ==  numofElements)
        {/* we are comparing Index whose counting started from 0, becomes equal, 
            then exit the loop */
           eServerRespRecdState = SubscriptionAckCompleted_State; 
        }
        else if (elementIndex < numofElements)
        { /* When the Element Index is still less than the total number of 
             elements in the Node: So, Restart the cycle */
          
          eServerRespRecdState = NodeIdle_State; 
          indexSIGmodels =0; /* Reset the variable again for the next element */
          indexVendormodels = 0;
        }
      }
      break;
      
    case InProgress_State:
      /* Just wait and let the messages be completed 
         or look for timeout */
      ConfigClient_ChkRetrialState (&eServerRespRecdState);
      break;
      
    default:
      /* Error State */
      break;
    }
    
  return result;
}

/**
* @brief  AppliConfigClient_PublicationSetDefault: This function is application
          used for adding publication settings to the element(node) 
          for default settings
* @param  pGeneric_OnOffParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT AppliConfigClient_PublicationSetDefault (void) 
{
  MOBLEUINT16 publishAddress = DEFAULT_PUBLISH_ADDR;
  MOBLEUINT16 appKeyIndex = DEFAULT_APPKEY_INDEX;
  MOBLEUINT8 credentialFlag = DEFAULT_CREDENTIAL_FLAG;
  MOBLEUINT8 publishTTL = DEFAULT_PUBLISH_TTL;
  MOBLEUINT8 publishPeriod = DEFAULT_PUBLISH_PERIOD;
  MOBLEUINT8 publishRetransmitCount = DEFAULT_PUBLISH_RETRANSMIT_COUNT;
  MOBLEUINT8 publishRetransmitIntervalSteps= DEFAULT_PUBLISH_RETRANSMIT_INTERVAL_STEPS;
  
   static MOBLEUINT16 elementAddress;
   static MOBLEUINT32 modelIdentifier;
   static MOBLEUINT8 elementIndex;
   static MOBLEUINT8  indexSIGmodels;
   static MOBLEUINT8  indexVendormodels;
   MOBLEUINT8  numSIGmodels;
   MOBLEUINT8  numVendorModels;
   MOBLEUINT8  numofElements;        
   MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
      
    switch(eServerRespRecdState)
    {
    case NodeIdle_State:
      /* Start the Publication Add message  */

        elementIndex = 0; /* Initialize it for the complete loop */
        indexSIGmodels = 0; /* Initialize it for the complete loop */
        indexVendormodels = 0;
        elementAddress = GetNodeElementAddress();
        elementAddress += elementIndex;
        modelIdentifier = GetSIGModelFromCompositionData(elementIndex,indexSIGmodels );
         /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;        
        ConfigClient_PublicationSet(elementAddress,
                              publishAddress,
                              appKeyIndex,
                              credentialFlag,
                              publishTTL,
                              publishPeriod,
                              publishRetransmitCount,
                              publishRetransmitIntervalSteps,
                              modelIdentifier);
        
      break;

    case PublicationStatus_State:
       /* Need to check if all SIG Models are subscribed ? */
       /* Need to check if all SIG Models are subscribed ? */
      ConfigClient_ResetTrials();
      
      numSIGmodels = GetNumberofSIGModels(elementIndex);
      numVendorModels = GetNumberofVendorModels(elementIndex);
      elementAddress = GetNodeElementAddress();
      elementAddress += elementIndex;

      if (indexSIGmodels < numSIGmodels )
      { /* Even when all SIG Models are serviced, we need to start for Vendor Models */
        indexSIGmodels++; 
        indexVendormodels =0;  /* Reset back, bcoz, we are still process the SIG Models */
      }
      else if (indexVendormodels < numVendorModels)
      {
        indexVendormodels++; /* When SIG Models and Vendor Models are processed
                                the loop condition will become true */
      }

      
      if (indexSIGmodels < numSIGmodels )
      {/* if index is still less, then we have scope of reading 1 more index */
       
        /* Get the Next Model and Bind it again till all SIG Models are binded */
        modelIdentifier = GetSIGModelFromCompositionData(elementIndex,indexSIGmodels);
        
        /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;
        ConfigClient_SaveMsgSendingTime();
        ConfigClient_PublicationSet(elementAddress,
                              publishAddress,
                              appKeyIndex,
                              credentialFlag,
                              publishTTL,
                              publishPeriod,
                              publishRetransmitCount,
                              publishRetransmitIntervalSteps,
                              modelIdentifier);
      }
      else if (indexVendormodels < numVendorModels)
      {
        modelIdentifier = GetVendorModelFromCompositionData(elementIndex,indexVendormodels );
        
        /* Switch to InProgress_State */
        eServerRespRecdState = InProgress_State;
        ConfigClient_SaveMsgSendingTime();
        ConfigClient_PublicationSet(elementAddress,
                              publishAddress,
                              appKeyIndex,
                              credentialFlag,
                              publishTTL,
                              publishPeriod,
                              publishRetransmitCount,
                              publishRetransmitIntervalSteps,
                              modelIdentifier);
      }
      else
      {
        /* Now, the element index is handled, change the element index */
        elementIndex++;
        numofElements = ConfigClient_GetNodeElements();  
        if (elementIndex ==  numofElements)
        {/* we are comparing Index whose counting started from 0, becomes equal, 
            then exit the loop */
           eServerRespRecdState = PublicationStatusCompleted_State; 
        }
        else if (elementIndex < numofElements)
        { /* When the Element Index is still less than the total number of 
             elements in the Node: So, Restart the cycle */
          
          eServerRespRecdState = NodeIdle_State; 
          indexSIGmodels =0; /* Reset the variable again for the next element */
          indexVendormodels = 0;
        }
      }
      break;
      
    case InProgress_State:
      /* Just wait and let the messages be completed 
         or look for timeout */
      ConfigClient_ChkRetrialState (&eServerRespRecdState);
      break;
      
    default:
      /* Error State */
      break;
    }

  return result;
}


/**
* @brief  Appli_CompositionDataStatusCb: This function is callback from config 
         client middleware on reception of the response
* @param  None
* @retval MOBLE_RESULT
*/ 
void Appli_CompositionDataStatusCb(MOBLE_RESULT status)
{
   eServerRespRecdState = CompositionRecd_State;
}

/**
* @brief  Appli_AppKeyStatusCb: This function is callback from config 
         client middleware on reception of the response
* @param  None
* @retval MOBLE_RESULT
*/ 
void Appli_AppKeyStatusCb(MOBLEUINT8 status)
{
   /* Change the received state for application  */
   eServerRespRecdState = AppkeyAck_State;
}

/**
* @brief  Appli_AppBindModelStatusCb: This function is callback from config 
         client middleware on reception of the response
* @param  None
* @retval MOBLE_RESULT
*/ 
void Appli_AppBindModelStatusCb(MOBLEUINT8 status)
{
   /* Change the received state for application  */
   eServerRespRecdState = AppBindModelAck_State;
}

/**
* @brief  Appli_SubscriptionAddStatusCb: This function is callback from config 
         client middleware on reception of the response
* @param  None
* @retval MOBLE_RESULT
*/ 
void Appli_SubscriptionAddStatusCb(MOBLEUINT8 status)
{
   /* Change the received state for application  */
   eServerRespRecdState = SubscriptionAck_State;
}

/**
* @brief  Appli_PublicationStatusCb: This function is callback from config 
         client middleware on reception of the response
* @param  None
* @retval MOBLE_RESULT
*/ 
void Appli_PublicationStatusCb(MOBLEUINT8 status)
{
   /* Change the received state for application  */
   eServerRespRecdState = PublicationStatus_State;
}




/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

