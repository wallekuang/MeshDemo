/**
******************************************************************************
* @file    appli_sensor.c
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Application interface for Sensor Mesh Models 
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

#include "hal_common.h"
#include "types.h"
#include "sensors.h"
#include "light_lc.h"
#include "appli_sensor.h"
#include "mesh_cfg.h"
#include "LPS25HB.h"
#include "string.h"
#include "common.h"
#include "math.h"

/** @addtogroup ST_BLE_Mesh
*  @{
*/

/** @addtogroup Application_Mesh_Models
*  @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
* @brief  PRESSURE init structure definition
*/
PRESSURE_InitTypeDef InitStructure =
{
  LPS25HB_ODR_1Hz,
  LPS25HB_BDU_READ, 
  LPS25HB_DIFF_ENABLE,  
  LPS25HB_SPI_SIM_3W,  
  LPS25HB_P_RES_AVG_32,
  LPS25HB_T_RES_AVG_16 
};

/* Application variables of sensor model definition */
#ifdef ENABLE_SENSOR_MODEL_SERVER

Appli_Sensor_DescriptorStatus_t Appli_Sensor_DescriptorStatus;
Appli_Sensor_SettingSet_t Appli_Sensor_SettingSet;

/* By Default value used for cadence set for testing. */
Sensor_CadenceSet_t Sensor_CadenceSet[NUMBER_OF_SENSOR] = {
                                  {0x0071 , 0x2 , 2 , 2 ,2 ,0 ,0X05 , 0x64},
                                  {0x2A6D , 0x2 , 1 , 1 , 1, 0, 0X258 , 0x3ED}
                                              };
#endif

MODEL_Property_IDTableParam_t Property_ID_Table[NUMBER_OF_SENSOR] = {
                                                                      {TEMPERATURE_PID},
                                                                      {PRESSURE_PID}
                                                                    }; 
MOBLEUINT8 Occupancy_Flag = FALSE;
extern MOBLEUINT8 NumberOfElements;
extern MOBLEUINT8 ProvisionFlag;

/* Temperature and Pressure init structure*/
PRESSURE_DrvTypeDef* xLPS25HBDrv = &LPS25HBDrv; 

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#ifdef ENABLE_SENSOR_MODEL_SERVER

/**
* @brief  Appli_Sensor_Cadence_Set: This function is callback for Application
*         when sensor cadence Set message is received
* @param  pCadence_param: Pointer to the parameters received for message
* @param  property_ID: Property is of sensor coming in data packet
* @param  length: Received data length.
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Sensor_Cadence_Set(Sensor_CadenceParam_t* pCadence_param, MOBLEUINT16 property_ID, MOBLEUINT32 length)                                    
{  
  if(property_ID == (MOBLEUINT16)TEMPERATURE_PID)
  {
    Sensor_CadenceSet[0].Property_ID = pCadence_param->Property_ID;
    Sensor_CadenceSet[0].FastCadenceDevisor = pCadence_param->FastCadenceDevisor ;
    Sensor_CadenceSet[0].StatusTriggerType = pCadence_param->StatusTriggerType ;
    Sensor_CadenceSet[0].triggerDeltaDown = pCadence_param->triggerDeltaDown;
    Sensor_CadenceSet[0].triggerDeltaUp = pCadence_param->triggerDeltaUp;
    Sensor_CadenceSet[0].StatusMinInterval = pCadence_param->StatusMinInterval;
    Sensor_CadenceSet[0].FastCadenceLow = pCadence_param->FastCadenceLow;
    Sensor_CadenceSet[0].FastCadenceHigh = pCadence_param->FastCadenceHigh;
  }
  else if(property_ID == (MOBLEUINT16)PRESSURE_PID)
  {
    
    Sensor_CadenceSet[1].Property_ID = pCadence_param->Property_ID;
    Sensor_CadenceSet[1].FastCadenceDevisor = pCadence_param->FastCadenceDevisor ;
    Sensor_CadenceSet[1].StatusTriggerType = pCadence_param->StatusTriggerType ;
    Sensor_CadenceSet[1].triggerDeltaDown = pCadence_param->triggerDeltaDown;
    Sensor_CadenceSet[1].triggerDeltaUp = pCadence_param->triggerDeltaUp;
    Sensor_CadenceSet[1].StatusMinInterval = pCadence_param->StatusMinInterval;
    Sensor_CadenceSet[1].FastCadenceLow = pCadence_param->FastCadenceLow;
    Sensor_CadenceSet[1].FastCadenceHigh = pCadence_param->FastCadenceHigh;
  }
  else
  {
    /* No Comments */
  }
  
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Appli_Sensor_Setting_Set: This function is callback for Application
*         when sensor setting Set message is received
* @param  pSensor_SettingParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Sensor_Setting_Set(Sensor_SettingParam_t* pSensor_SettingParam,
                                      MOBLEUINT8 OptionalValid)
{  
  Appli_Sensor_SettingSet.Property_ID = pSensor_SettingParam->Property_ID;
  Appli_Sensor_SettingSet.Sensor_Setting_ID = pSensor_SettingParam->Sensor_Setting_ID;
  Appli_Sensor_SettingSet.Sensor_Setting_Access = pSensor_SettingParam->Sensor_Setting_Access;
  Appli_Sensor_SettingSet.Sensor_Setting_Value = pSensor_SettingParam->Sensor_Setting_Value;
  
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Appli_Sensor_Data_Status: This function is callback for Application
*         when sensor get message is received
* @param  sensor_Data: Pointer to the parameters to be send in message
* @param  pLength: Length of the parameters to be sent in response
* @param  prop_ID: Property is of sensor coming in data packet
* @param  length: Received data length
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Sensor_Data_Status(MOBLEUINT8* sensor_Data , MOBLEUINT32* pLength, 
                                      MOBLEUINT16 prop_ID , MOBLEUINT32 length)
{
  MOBLE_RESULT result = MOBLE_RESULT_FALSE;                                  
  MOBLEUINT32 temperatureData ;
  MOBLEUINT32 pressureData ;
  MOBLEUINT8 data_Length = 0x03;
  MOBLEUINT32 distance = 0x000000C8; 
  
  LPS25HB_GetTemperature((float*)&temperatureData);
  
  LPS25HB_GetPressure((float*)&pressureData); 
  
  result = Check_Property_ID(Property_ID_Table , prop_ID);
  
  if((prop_ID == TEMPERATURE_PID ) && (length > 0))
  {
    /*(prop_Id_Temp & 0x07) << 5) | (Len <<1) Format A 
    Property calculation is done like above line
    */
    *(sensor_Data) = ((TEMPERATURE_PID & 0x07) << 5) | (data_Length <<1) ; 
    *(sensor_Data+1) = (TEMPERATURE_PID >> 3) & 0xFF; 
    
    memcpy(&sensor_Data[2],(void*)&temperatureData,4);
    
    *pLength  =6;    
  } 
  else if((prop_ID == PRESSURE_PID) && (length > 0))
  {
    /* Format B for Pressure sensor */
    *(sensor_Data+0) = ((data_Length <<1) | 0x01); 
    *(sensor_Data+1) = (MOBLEUINT8)PRESSURE_PID ;
    *(sensor_Data+2) = (MOBLEUINT8)(PRESSURE_PID >> 8);
    
    memcpy(&sensor_Data[3],(void*)&pressureData,4);
    
    *pLength  =7;    
  }
  else if((prop_ID == TIME_OF_FLIGHT_PID) && (length > 0))
  {
    /* Format B for Pressure sensor */
    *(sensor_Data+0) = ((data_Length <<1) | 0x01); 
    *(sensor_Data+1) = (MOBLEUINT8)TIME_OF_FLIGHT_PID ;
    *(sensor_Data+2) = (MOBLEUINT8)(TIME_OF_FLIGHT_PID >> 8);
    
    memcpy(&sensor_Data[3],(void*)&distance,4);
    
    *pLength  =7;    
  }
  else if((result == MOBLE_RESULT_FALSE) && (length == 0))
  {
    /*(prop_Id_Temp & 0x07) << 5) | (Len <<1) Format A 
    Property calculation is done like above line
    */
    *(sensor_Data) = ((TEMPERATURE_PID & 0x07) << 5) | (data_Length <<1) ; 
    *(sensor_Data+1) = (TEMPERATURE_PID >> 3) & 0xFF; 
    
    memcpy(&sensor_Data[2],(void*)&temperatureData,4);
    
    /* Format B for Pressure sensor */
    *(sensor_Data+6) = ((data_Length <<1) | 0x01); 
    *(sensor_Data+7) = (MOBLEUINT8)PRESSURE_PID ;
    *(sensor_Data+8) = (MOBLEUINT8)(PRESSURE_PID >> 8);
    
    memcpy(&sensor_Data[9],(void*)&pressureData,4);
    
    /* Format B for Pressure sensor */
    *(sensor_Data+13) = ((data_Length <<1) | 0x01); 
    *(sensor_Data+14) = (MOBLEUINT8)TIME_OF_FLIGHT_PID ;
    *(sensor_Data+15) = (MOBLEUINT8)(TIME_OF_FLIGHT_PID >> 8);
    
    memcpy(&sensor_Data[16],(void*)&distance,4);
    
    *pLength  =20;    
  }  
  else
  {
    /* No Comments */
  }
  
  TRACE_M(TF_SENSOR,"the temperature reading from sender in hex 0x%08x \n\r ", temperatureData);
  TRACE_M(TF_SENSOR,"the pressure reading from sender in hex 0x%08x \n\r", pressureData );  
  
  return MOBLE_RESULT_SUCCESS;
}


/**
* @brief  Appli_Sensor_Descriptor_Status: This function is callback for Application
*         when sensor get message is received
* @param  sensor_Descriptor: Pointer to the parameters to be send in message
* @param  pLength: Length of the parameters to be sent in response
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Sensor_Descriptor_Status(MOBLEUINT8* sensor_Descriptor , MOBLEUINT32* pLength)
{
  Appli_Sensor_DescriptorStatus_t Appli_Sensor_DescriptorStatus1[] = {{PRESSURE_PID,0xABC,0xDEF,0x03,0x04,0x05},
                                                                       {TEMPERATURE_PID,0xc56,0xd78,0x06,0x07,0x08},
                                                                        {TIME_OF_FLIGHT_PID,0xD23,0xE45,0x06,0x07,0x08}};
    MOBLEUINT32 tolerance;
    tolerance = Appli_Sensor_DescriptorStatus1[0].NegativeTolerance;
    tolerance = (tolerance << 12 ) | Appli_Sensor_DescriptorStatus1[0].PositiveTolerance;
    
  *(sensor_Descriptor) = Appli_Sensor_DescriptorStatus1[0].Prop_ID;
  *(sensor_Descriptor+1) = Appli_Sensor_DescriptorStatus1[0].Prop_ID >> 8;
  *(sensor_Descriptor+2) = tolerance;
  *(sensor_Descriptor+3) = tolerance >> 8;
  *(sensor_Descriptor+4) = tolerance >> 16;
  *(sensor_Descriptor+5) = Appli_Sensor_DescriptorStatus1[0].SamplingFunction;
  *(sensor_Descriptor+6) = Appli_Sensor_DescriptorStatus1[0].MeasurementPeriod;
  *(sensor_Descriptor+7) = Appli_Sensor_DescriptorStatus1[0].UpdateInterval;
  
   tolerance = Appli_Sensor_DescriptorStatus1[1].NegativeTolerance;
   tolerance = (tolerance << 12 ) | Appli_Sensor_DescriptorStatus1[1].PositiveTolerance ;
    
  *(sensor_Descriptor+8) = Appli_Sensor_DescriptorStatus1[1].Prop_ID;
  *(sensor_Descriptor+9) = Appli_Sensor_DescriptorStatus1[1].Prop_ID >> 8;
  *(sensor_Descriptor+10) = tolerance;
  *(sensor_Descriptor+11) = tolerance >> 8;
  *(sensor_Descriptor+12) = tolerance >> 16;
  *(sensor_Descriptor+13) = Appli_Sensor_DescriptorStatus1[1].SamplingFunction;
  *(sensor_Descriptor+14) = Appli_Sensor_DescriptorStatus1[1].MeasurementPeriod;
  *(sensor_Descriptor+15) = Appli_Sensor_DescriptorStatus1[1].UpdateInterval;
  
   tolerance = Appli_Sensor_DescriptorStatus1[2].NegativeTolerance;
   tolerance = (tolerance << 12 ) | Appli_Sensor_DescriptorStatus1[2].PositiveTolerance ;
    
  *(sensor_Descriptor+16) = Appli_Sensor_DescriptorStatus1[2].Prop_ID;
  *(sensor_Descriptor+17) = Appli_Sensor_DescriptorStatus1[2].Prop_ID >> 8;
  *(sensor_Descriptor+18) = tolerance;
  *(sensor_Descriptor+19) = tolerance >> 8;
  *(sensor_Descriptor+20) = tolerance >> 16;
  *(sensor_Descriptor+21) = Appli_Sensor_DescriptorStatus1[2].SamplingFunction;
  *(sensor_Descriptor+22) = Appli_Sensor_DescriptorStatus1[2].MeasurementPeriod;
  *(sensor_Descriptor+23) = Appli_Sensor_DescriptorStatus1[2].UpdateInterval;
  
  *pLength = 24;
  
  return MOBLE_RESULT_SUCCESS;
}

#endif

/**
* @brief  Sensor Process Function will continuously monitor the sensors.
*         Function used for the Publishing, data monitoring..
* @param  void
* @retval void
*/ 
void Sensor_Process(void)
{  
  
#ifdef ENABLE_SENSOR_PUBLICATION    
  float sensorValue[NUMBER_OF_SENSOR];
  if(ProvisionFlag == 1)
  {
    Read_Sensor_Data(&sensorValue[0]);
    Sensor_Publication_Process(&sensorValue[0], &Property_ID_Table[0]);
  }
#endif

/* Occupancy_Flag become True when ever sensor detect occupancy and get interrupt
   and make flag True to run this routine.
*/ 
  if(Occupancy_Flag == TRUE) 
  {
    if(BlueNrg_waitPeriod(CONTROLLER_WAIT_TIME))
    {
/* publishing the command for LC Light occupancy set message in the sensor status 
   message .
*/     
      Sensor_LC_Light_Publish();  
      Occupancy_Flag = FALSE;   
    }  
  }  
}

/**
* @brief  Function check for the couupancy in the location and send the status
*         message with the ocuppancy value, when the interrupt is detected.
* @param  void     
* @retval void
*/  
void Sensor_LC_Light_Publish(void)
{
  MOBLEUINT8 occupancyData = 0x1;
  MOBLEUINT8 sensor_Data[5];
  MOBLE_ADDRESS srcAdd;
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  
  sensor_Data[1] = (MOBLEUINT8)(PRESENCE_DETECTED_PROPERTY << 8);
  sensor_Data[0] = (MOBLEUINT8)PRESENCE_DETECTED_PROPERTY;
  sensor_Data[2] = occupancyData;
  
  srcAdd = BluenrgMesh_GetAddress();
  
  result = BluenrgMesh_SetRemotePublication(LIGHT_MODEL_SERVER_LC_MODEL_ID, srcAdd ,
                            SENSOR_STATUS , 
                            sensor_Data,3,
                            MOBLE_FALSE, MOBLE_FALSE);
  
  if(result)
  {
    TRACE_M(TF_LIGHT_LC,"Publication Error \r\n");
  }
   
}

#if defined ENABLE_SENSOR_PUBLICATION && defined ENABLE_SENSOR_MODEL_SERVER
/**
* @brief  Function read the particular sensor value which are called inside.
* @param  pSensorValue: pointer to the sensor data array.       
* @retval void
*/ 
void Read_Sensor_Data(float *pSensorValue)
{
  float temp,press;
  LPS25HB_GetTemperature(&temp);
  pSensorValue[0] = temp;
  LPS25HB_GetPressure(&press); 
  pSensorValue[1] = press;
}

/**
* @brief  Sensor Publication Process function
* @param  Function will publish the sensor data according to the given conditions.
* @param  void type function pointer.
* @param  pSensorData: Pointer to the sensor data array.
* @param  pProp_ID: Pointer to the Property id of sensor array.
* @retval void
*/
void Sensor_Publication_Process(float* pSensorData, MODEL_Property_IDTableParam_t* pProp_ID)
{
  static MOBLEUINT8 sensor_Count = 0;
  displayFloatToInt_t out_value;
  MOBLEUINT8 devisorValue;
  MOBLEUINT32 publishTime;
  static MOBLEUINT32 cadenceDurationTick[NUMBER_OF_SENSOR];
  static float previousDataValue[NUMBER_OF_SENSOR];
  static PublishingDataFlag_t PublishingDataFlag[NUMBER_OF_SENSOR] = {MOBLE_FALSE};
  
      floatToInt(pSensorData[sensor_Count], &out_value, 2);
      
      /* Taking the timestamp for the cadence publication and making flag high */
      if(PublishingDataFlag[sensor_Count].CadenceDurationFlag == MOBLE_FALSE)
      {
        cadenceDurationTick[sensor_Count] = Clock_Time();
        PublishingDataFlag[sensor_Count].CadenceDurationFlag = MOBLE_TRUE;          
      }
      /* Taking the sensor value and store it for comparing present sensor value with
      particular difference of increasing or decreasing. and making flag high.
      */
      if(PublishingDataFlag[sensor_Count].DeltaDataFlag == MOBLE_FALSE)
      {
        previousDataValue[sensor_Count] = pSensorData[sensor_Count];
        PublishingDataFlag[sensor_Count].DeltaDataFlag = MOBLE_TRUE;
      }
      /*
      This condition is checking for the difference of present sensor value 
      with prestored sensor value with user defined difference,if this condition 
      is true then it publish the sensor data.And making the delta flag low again.
      */       
      if((pSensorData[sensor_Count] >= (previousDataValue[sensor_Count] + Sensor_CadenceSet[sensor_Count].triggerDeltaUp)) ||
         (pSensorData[sensor_Count] <= (previousDataValue[sensor_Count] - Sensor_CadenceSet[sensor_Count].triggerDeltaDown)))
      {        
        SensorDataPublish((MOBLEUINT32*)&pSensorData[sensor_Count] , &pProp_ID[sensor_Count].Property_ID);
        
        PublishingDataFlag[sensor_Count].DeltaDataFlag = MOBLE_FALSE;
        TRACE_M(TF_SENSOR,"previous value data %.3f \r\n",previousDataValue[sensor_Count]);
        TRACE_M(TF_SENSOR,"Delta publication of data %.3f\r\n",*((float*)&pSensorData[sensor_Count]));
        sensor_Count++;
      }
      /*
      This condition is continuously checking the sensor value range, if that 
      value is within the user defined range then publishing duration or rate will
      be divided by user definedcadence devisor value and rate of publishing will 
      become high.And making the cadence flag low again.
      */
      if(((out_value.out_int <= Sensor_CadenceSet[sensor_Count].FastCadenceHigh) && 
          (out_value.out_int >= Sensor_CadenceSet[sensor_Count].FastCadenceLow)) ||
         (Sensor_CadenceSet[sensor_Count].FastCadenceHigh < Sensor_CadenceSet[sensor_Count].FastCadenceLow))
      {
        devisorValue = (MOBLEUINT8)pow(2 ,Sensor_CadenceSet[sensor_Count].FastCadenceDevisor);
        publishTime = SENSOR_PUBLISH_PERIOD/devisorValue;  
        
        if(((Clock_Time()- cadenceDurationTick[sensor_Count]) >= publishTime))
        {                   
          SensorDataPublish((MOBLEUINT32*)&pSensorData[sensor_Count] , &pProp_ID[sensor_Count].Property_ID);                 
          PublishingDataFlag[sensor_Count].CadenceDurationFlag = MOBLE_FALSE;           
          TRACE_M(TF_SENSOR,"Cadence publication of data %.2f \r\n",*((float*)&pSensorData[sensor_Count])); 
          sensor_Count++;
        } 
      }
      else
      {
        publishTime = SENSOR_PUBLISH_PERIOD ;     
        
        if(((Clock_Time()- cadenceDurationTick[sensor_Count]) >= SENSOR_PUBLISH_PERIOD))
        {            
          SensorDataPublish((MOBLEUINT32*)&pSensorData[sensor_Count] , &pProp_ID[sensor_Count].Property_ID);
          
          PublishingDataFlag[sensor_Count].CadenceDurationFlag = MOBLE_FALSE;                
          TRACE_M(TF_SENSOR,"Regular publication of data %.3f \r\n",*((float*)&pSensorData[sensor_Count]));
        }  
      } 
     if(sensor_Count > 1)
     {
       sensor_Count = 0;
     }

}

/**
* @brief  BluenrgMesh Sensor Publication function
* @param  Function will decide the publish address and element.
* @param  pSensor_Value: Pointer to the sensor data array 
* @param  pProp_ID: pointer to the property id of sensor array.
* @retval void
*/
void SensorDataPublish(MOBLEUINT32 *pSensor_Value , MOBLEUINT16* pProp_ID)
{
  MOBLEUINT32 length;
  MOBLEUINT8 sensor_Data[8];
  MOBLE_ADDRESS srcAdd;
  MOBLE_RESULT result = MOBLE_RESULT_SUCCESS;
  
  srcAdd = BluenrgMesh_GetAddress();
 
  switch(*pProp_ID)
  {
  case TEMPERATURE_PID:
    { 
      /*(prop_Id_Temp & 0x07) << 5) | (Len <<1) Format A 
      Property calculation is done like above line
      */
      sensor_Data[0] = ((TEMPERATURE_PID & 0x07) << 5) | (3 <<1) ; 
      sensor_Data[1] = (TEMPERATURE_PID >> 3) & 0xFF; 
      
      memcpy(&sensor_Data[2],(void*)&pSensor_Value[0],4);
      length  =6;  
      break;
    }
  case PRESSURE_PID:
    {
      /* Format B for Pressure sensor */
      sensor_Data[0] = ((0x03 <<1) | 0x01); 
      sensor_Data[1] = (MOBLEUINT8)PRESSURE_PID ;
      sensor_Data[2] = (MOBLEUINT8)(PRESSURE_PID >> 8);
      
      memcpy(&sensor_Data[3],(void*)&pSensor_Value[0],4);  
      length  =7;     
      break;
    }
  default:
    break;
  }
  
  result = BluenrgMesh_SetRemotePublication(SENSOR_SERVER_MODEL_ID, srcAdd,
                                            SENSOR_STATUS , 
                                            sensor_Data,length,
                                            MOBLE_FALSE, MOBLE_FALSE);
  
  if(result)
  {
    TRACE_M(TF_SENSOR,"Publication Error \r\n");
  }
  
}

#endif

#ifdef ENABLE_SENSOR_MODEL_SERVER
/**
* @brief  Appli_Sensor_GetSettingStatus: This function is callback for Application
*         when sensor setting numbers status message is to be provided
* @param  pSetting_Status: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Sensor_GetSettingStatus(MOBLEUINT8* pSetting_Status)                                        
{
  
  *pSetting_Status = Appli_Sensor_SettingSet.Property_ID;
  *(pSetting_Status+1) = Appli_Sensor_SettingSet.Property_ID >> 8;
  *(pSetting_Status+2) = Appli_Sensor_SettingSet.Sensor_Setting_ID; 
  *(pSetting_Status+3) = Appli_Sensor_SettingSet.Sensor_Setting_ID >> 8;
  
  return MOBLE_RESULT_SUCCESS; 
}

/**
* @brief  Appli_Sensor_GetSetting_IDStatus: This function is callback for Application
*         when sensor setting numbers and row value status message is to be provided
* @param  pSetting_Status: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Sensor_GetSetting_IDStatus(MOBLEUINT8* pSetting_Status)                                        
{
  
  *pSetting_Status = Appli_Sensor_SettingSet.Property_ID;
  *(pSetting_Status+1) = Appli_Sensor_SettingSet.Property_ID >> 8;
  *(pSetting_Status+2) = Appli_Sensor_SettingSet.Sensor_Setting_ID; 
  *(pSetting_Status+3) = Appli_Sensor_SettingSet.Sensor_Setting_ID >> 8; 
  *(pSetting_Status+4) = Appli_Sensor_SettingSet.Sensor_Setting_Access; 
  *(pSetting_Status+5) = Appli_Sensor_SettingSet.Sensor_Setting_Value;
  *(pSetting_Status+6) = Appli_Sensor_SettingSet.Sensor_Setting_Value >> 8;
  
  return MOBLE_RESULT_SUCCESS; 
}

#endif

/**
* @brief  Check_Property_ID: This function is used for checking the Property id 
*         of sensor available in table.
* @param  prop_ID_Table: address of the property id table array.
* @param  prop_ID:received property id of sensor.
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Check_Property_ID(const MODEL_Property_IDTableParam_t prop_ID_Table[] 
                               , MOBLEUINT16 prop_ID)
{
   MOBLE_RESULT status = MOBLE_RESULT_FALSE;
   
  for(int i=0;i<NUMBER_OF_SENSOR;i++)    
  {
    if(prop_ID_Table[i].Property_ID == prop_ID)
    {       
      status =  MOBLE_RESULT_SUCCESS;
      break;
    }
  }
  return status;
}

/**
* @brief  Appli_Sensor_Init: This function is callback for Initialisation of 
*         Application interface
* @param  void 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Sensor_Init(void)                                        
{
  LPS25HB_Init(&InitStructure);
  
  return MOBLE_RESULT_SUCCESS; 
}


/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

