/**
******************************************************************************
* @file    appli_light.c
* @author  BLE Mesh Team
* @version V1.12.000
* @date    06-12-2019
* @brief   Application interface for Lighting Mesh Models 
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
#include "appli_light.h"
#include "common.h"
#include "mesh_cfg.h"
#include "PWM_handlers.h"
#include "PWM_config.h"
#include "appli_nvm.h"
#include "math.h"

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


/*******************************************************************************
Following Variables are used for the LIGHTING Lightness MODEL 
*******************************************************************************/
#ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS

  Appli_Light_lightnessSet ApplilightnessSet;
  Appli_Light_lightnessLinearSet ApplilightnessLinearSet;

/******************************************************************************/
#endif /* #ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS  */
/******************************************************************************/

/*******************************************************************************
Following Variables are used for the LIGHTING CTL MODEL 
*******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_CTL

  Appli_Light_CtlSet AppliCtlSet;
  Appli_Light_CtlTemperatureRangeSet AppliCtlTemperatureRangeSet;
  Appli_Light_CtlDefaultSet AppliCtlDefaultSet;

/******************************************************************************/
#endif /* #ifdef ENABLE_LIGHT_MODEL_SERVER_CTL */
/******************************************************************************/

/*******************************************************************************
Following Variables are used for the LIGHTING HSL MODEL 
*******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL
  Appli_Light_HslSet AppliHslSet;
  Appli_Light_HslDefaultSet Appli_HslDefaultSet = {0x7FFF,0x7FFF,0x7FFF};
  Appli_Light_RGBSet Appli_RGBParam;
  Appli_Light_HslRangeSet AppliHslRangeSet;
  
/*******************************************************************************/
#endif  /*End of the LIGHTING HSL MODEL variables */
/*******************************************************************************/

/*******************************************************************************
Following Variables are used for the RGB board. 
*******************************************************************************/
  
Appli_LightPwmValue_t Appli_LightPwmValue;
extern MOBLEUINT8 RestoreFlag;
extern MOBLEUINT8 PowerOnOff_flag;
extern MOBLEUINT8 IntensityFlag;
extern MOBLEUINT16 IntensityValue;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS 
/**
* @brief  Appli_Light_Lightness_Set: This function is callback for Application
*           when Light Lightness Set message is received
* @param  pLight_LightnessParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_Lightness_Set(Light_LightnessStatus_t* pLight_LightnessParam,
                                       MOBLEUINT8 OptionalValid)
{
  MOBLEUINT16 duty;
  static MOBLEUINT16 previousIntensity = 0;
   TRACE_M(TF_SERIAL_CTRL,"#824C!\n\r");
  ApplilightnessSet.PresentState16 = pLight_LightnessParam->PresentValue16;
  
  if(((IntensityValue > previousIntensity) && (IntensityValue <PWM_TIME_PERIOD)) ||
     IntensityValue == INTENSITY_LEVEL_ZERO)
  {
    IntensityFlag = FALSE;
  }
  else
  {
    IntensityFlag = TRUE;
  }
  previousIntensity = IntensityValue; 
  
  if(pLight_LightnessParam->PresentValue16 != 0x00)
  {
    ApplilightnessSet.LastLightness16 = pLight_LightnessParam->PresentValue16;
  }
  
  if(pLight_LightnessParam->PresentValue16 != 0x00)
  {
    ApplilightnessSet.LastLightness16 = pLight_LightnessParam->PresentValue16;
  }
  
  duty = PwmValueMapping(ApplilightnessSet.PresentState16 , 0xfFFF ,0); 
  Appli_LightPwmValue.IntensityValue = duty;
  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
  
  /* set the flag value for NVM store */
  RestoreFlag = LIGHT_LIGHTNESS_NVM_FLAG;

  AppliNvm_SaveMessageParam();
    
  return MOBLE_RESULT_SUCCESS;
  
}

/**
* @brief  Appli_Light_Lightness_Status: This function is callback for Application
*           when Light Lightness status message is received
* @param  pLightness_status: Pointer to the parameters received for message
* @param  pLength: length of data 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_Lightness_Status(MOBLEUINT8 const *pLightness_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_Lightness_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#824E! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

/******************************************************************************/
#endif
/******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS 
/**
* @brief  Appli_Light_Lightness_Linear_Set: This function is callback for Application
*         when Light Lightness Linear Set message is received
* @param  pLight_LightnessLinearParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_Lightness_Linear_Set(Light_LightnessStatus_t* pLight_LightnessLinearParam,
                                              MOBLEUINT8 OptionalValid)
{
  ApplilightnessLinearSet.PresentState16 = pLight_LightnessLinearParam->PresentValue16; 
  
  TRACE_M(TF_SERIAL_CTRL,"#8250!\n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Appli_Light_Lightness_Linear_Status: This function is callback for Application
*         when Light Lightness Linear status message is received
* @param  pLightnessLinear_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Appli_Light_Lightness_Linear_Status(MOBLEUINT8 const *pLightnessLinear_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_Lightness_Linear_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8252! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

/******************************************************************************/
#endif
/******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS_SETUP
/**
* @brief  Appli_Light_Lightness_Default_Set: This function is callback for Application
*         when Light Lightness Default Set message is received
* @param  pLight_LightnessDefaultParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_Lightness_Default_Set(Light_LightnessDefaultParam_t* pLight_LightnessDefaultParam,
                                               MOBLEUINT8 OptionalValid)
{
  SetLed(pLight_LightnessDefaultParam->LightnessDefaultStatus);
  ApplilightnessSet.LightnessDefault = pLight_LightnessDefaultParam->LightnessDefaultStatus;
  
   TRACE_M(TF_SERIAL_CTRL,"#8259!\n\r");
   
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Appli_Light_Lightness_Default_Status: This function is callback for Application
*         when Light Lightness Default status message is received
* @param  pLightnessDefault_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Appli_Light_Lightness_Default_Status(MOBLEUINT8 const *pLightnessDefault_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_Lightness_Default_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8256! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

/******************************************************************************/
#endif
/******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS_SETUP
/**
* @brief  Appli_Light_Lightness_Range_Set: This function is callback for Application
*         when Light Lightness Range Set message is received
* @param  pLight_LightnessRangeParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_Lightness_Range_Set(Light_LightnessRangeParam_t* pLight_LightnessRangeParam,
                                             MOBLEUINT8 OptionalValid)
{
  ApplilightnessSet.StatusCode = pLight_LightnessRangeParam->StatusCode;
  ApplilightnessSet.RangeMin = pLight_LightnessRangeParam->MinRangeStatus; 
  ApplilightnessSet.RangeMax = pLight_LightnessRangeParam->MaxRangeStatus;
  
  TRACE_M(TF_SERIAL_CTRL,"#825B! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Appli_Light_Lightness_Range_Status: This function is callback for Application
*         when Light Lightness range ststus message is received
* @param  pLightnessRange_status: Pointer to the parameters received for message
* @param  pLength: length of data 
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Appli_Light_Lightness_Range_Status(MOBLEUINT8 const *pLightnessRange_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_Lightness_Range_Status callback received \r\n");

  TRACE_M(TF_SERIAL_CTRL,"#8258! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

/******************************************************************************/
#endif
/******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_CTL
/**
* @brief  Appli_Light_Ctl_Set: This function is callback for Application
*         when Light Ctl Set message is received
* @param  pLight_CtlParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_Ctl_Set(Light_CtlStatus_t* pLight_CtlParam,
                                 MOBLEUINT8 OptionalValid)
{
  float colourRatio;
  float brightRatio;
  TRACE_M(TF_SERIAL_CTRL,"#825E! \n\r");
  
  AppliCtlSet.PresentLightness16  = pLight_CtlParam->PresentCtlLightness16;
  AppliCtlSet.PresentTemperature16 = pLight_CtlParam->PresentCtlTemperature16;
  AppliCtlSet.PresentCtlDelta16 = pLight_CtlParam->PresentCtlDelta16;
  colourRatio = Ratio_CalculateValue(AppliCtlSet.PresentTemperature16 ,MAX_CTL_TEMP_RANGE ,MIN_CTL_TEMP_RANGE);
  
  brightRatio = Ratio_CalculateValue(AppliCtlSet.PresentLightness16 ,0XFFFF ,0);
  
  Appli_LightPwmValue.PwmCoolValue = PWM_CoolValue(colourRatio , brightRatio); 
  Appli_LightPwmValue.PwmWarmValue = PWM_WarmValue(colourRatio , brightRatio);
  
  Rgb_LedOffState();
 
  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
  
  /* set the flag value for NVM store */
  RestoreFlag = LIGHT_CTL_NVM_FLAG;

  AppliNvm_SaveMessageParam();
    
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Appli_Light_Ctl_Status: This function is callback for Application
*         when Light CTL status message is received
* @param  pLightCtl_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_Ctl_Status(MOBLEUINT8 const *pLightCtl_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_Ctl_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8260! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

#endif


#ifdef ENABLE_LIGHT_MODEL_SERVER_CTL_TEMPERATURE 
/**
* @brief  Appli_Light_CtlTemperature_Set: This function is callback for Application
*         when Light Ctl Temperature Set message is received
* @param  pLight_CtltempParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_CtlTemperature_Set(Light_CtlStatus_t* pLight_CtltempParam,
                                            MOBLEUINT8 OptionalValid)
{
  float colourRatio;
  float brightRatio;
  TRACE_M(TF_SERIAL_CTRL,"#8264!\n\r");
  AppliCtlSet.PresentTemperature16 = pLight_CtltempParam->PresentCtlTemperature16;
  AppliCtlSet.PresentCtlDelta16 = pLight_CtltempParam->PresentCtlDelta16;
  
  colourRatio = Ratio_CalculateValue(AppliCtlSet.PresentTemperature16 ,MAX_CTL_TEMP_RANGE ,MIN_CTL_TEMP_RANGE);
  
  brightRatio = Ratio_CalculateValue(AppliCtlSet.PresentLightness16 ,0XFFFF ,0);
  
  Appli_LightPwmValue.PwmCoolValue = PWM_CoolValue(colourRatio , brightRatio); 
  Appli_LightPwmValue.PwmWarmValue = PWM_WarmValue(colourRatio , brightRatio);
   
  Rgb_LedOffState();
  
  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
  /* set the flag value for NVM store */
  RestoreFlag = LIGHT_CTL_NVM_FLAG;
  
  AppliNvm_SaveMessageParam();
  
  return MOBLE_RESULT_SUCCESS;
}

/**                                                  
* @brief  Appli_Light_CtlTemperature_Status: This function is callback for Application
*         when Light CTL temperature status message is received
* @param  pLightCtlTemp_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_CtlTemperature_Status(MOBLEUINT8 const *pLightCtlTemp_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_CtlTemperature_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8266! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}


/******************************************************************************/
#endif
/******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_CTL_SETUP
/**
* @brief  Appli_Light_CtlTemperature_Range_Set: This function is callback for Application
*         when Light Ctl Temperature range Set message is received
* @param  pLight_CtlTempRangeParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_CtlTemperature_Range_Set(Light_CtlTemperatureRangeParam_t* pLight_CtlTempRangeParam,
                                                  MOBLEUINT8 OptionalValid)
{
  AppliCtlTemperatureRangeSet.RangeMin = pLight_CtlTempRangeParam->MinRangeStatus; 
  AppliCtlTemperatureRangeSet.RangeMax = pLight_CtlTempRangeParam->MaxRangeStatus;
  AppliCtlTemperatureRangeSet.StatusCode = pLight_CtlTempRangeParam->StatusCode;
  
  TRACE_M(TF_SERIAL_CTRL,"#826B!\n\r");
  
  return MOBLE_RESULT_SUCCESS;
}
/**                                                  
* @brief  Appli_Light_CtlTemperature_Range_Set: This function is callback for Application
*         when Light CTL temperature range status message is received
* @param  pCtlTempRange_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_CtlTemperature_Range_Status(MOBLEUINT8 const *pCtlTempRange_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_CtlTemperature_Range_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8263! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

/******************************************************************************/
#endif
/******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_CTL_SETUP
/**
* @brief  Appli_Light_CtlDefault_Set: This function is callback for Application
*         when Light Ctl Default Set message is received
* @param  pLight_CtlDefaultParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_CtlDefault_Set(Light_CtlDefaultParam_t* pLight_CtlDefaultParam,
                                        MOBLEUINT8 OptionalValid)
{
  AppliCtlDefaultSet.CtlDefaultLightness16 = pLight_CtlDefaultParam->CtlDefaultLightness16; 
  AppliCtlDefaultSet.CtlDefaultTemperature16 = pLight_CtlDefaultParam->CtlDefaultTemperature16;
  AppliCtlDefaultSet.CtlDefaultDeltaUv = pLight_CtlDefaultParam->CtlDefaultDeltaUv;
  
  TRACE_M(TF_SERIAL_CTRL,"#8269!\n\r");
  
  return MOBLE_RESULT_SUCCESS;
} 

/**                                                  
* @brief  Appli_Light_CtlDefault_Status: This function is callback for Application
*         when Light CTL Default status message is received
* @param  pCtlDefault_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_CtlDefault_Status(MOBLEUINT8 const *pCtlDefault_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_Ctl_DefaultStatus callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8268! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

#endif


#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL
/**
* @brief  Appli_Light_Hsl_Set: This function is callback for Application
*         when Light Hsl Set message is received
* @param  pLight_HslParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_Hsl_Set(Light_HslStatus_t* pLight_HslParam,
                                 MOBLEUINT8 OptionalValid)
{
  TRACE_M(TF_SERIAL_CTRL,"#8276!\n\r");
  AppliHslSet.HslLightness16 = pLight_HslParam->PresentHslLightness16;
  AppliHslSet.HslHueLightness16 = pLight_HslParam->PresentHslHueLightness16;
  AppliHslSet.HslSaturation16 = pLight_HslParam->PresentHslSaturation16;
  
  /* Function to convert HSL values in RGB values */
  HSL2RGB_Conversion();
  
  Appli_LightPwmValue.PwmRedValue = PwmValueMapping(Appli_RGBParam.Red_Value ,0xFFFF ,0x00); 
  Appli_LightPwmValue.PwmGreenValue = PwmValueMapping(Appli_RGBParam.Green_Value ,0xFFFF ,0x00); 
  Appli_LightPwmValue.PwmBlueValue = PwmValueMapping(Appli_RGBParam.Blue_Value ,0xFFFF ,0x00); 
  
  /* when HSL is set, make CTL pwm will bw zero */
  Ctl_LedOffState();
  
  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
  /* set the flag value for NVM store */
  RestoreFlag = LIGHT_HSL_NVM_FLAG;
  
  AppliNvm_SaveMessageParam();
    
  return MOBLE_RESULT_SUCCESS;
} 

/**                                                  
* @brief  Appli_Light_Hsl_Status: This function is callback for Application
*         when Light HSL status message is received
* @param  pHsl_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Appli_Light_Hsl_Status(MOBLEUINT8 const *pHsl_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_Hsl_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8278! \n\r");
  
  return MOBLE_RESULT_SUCCESS;
}

#endif


#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL_HUE
/**
* @brief  Appli_Light_HslHue_Set: This function is callback for Application
*         when Light Hsl Hue Set message is received
* @param  pLight_HslHueParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_HslHue_Set(Light_HslStatus_t* pLight_HslHueParam,
                                    MOBLEUINT8 OptionalValid)
{
  TRACE_M(TF_SERIAL_CTRL,"#826F! \n\r");	
  AppliHslSet.HslHueLightness16 = pLight_HslHueParam->PresentHslHueLightness16; 
  
  HSL2RGB_Conversion();
  
  Appli_LightPwmValue.PwmRedValue = PwmValueMapping(Appli_RGBParam.Red_Value ,0xFFFF ,0x00); 
  Appli_LightPwmValue.PwmGreenValue = PwmValueMapping(Appli_RGBParam.Green_Value ,0xFFFF ,0x00); 
  Appli_LightPwmValue.PwmBlueValue = PwmValueMapping(Appli_RGBParam.Blue_Value ,0xFFFF ,0x00); 
  
  Ctl_LedOffState();
  
  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
  /* set the flag value for NVM store */
  RestoreFlag = LIGHT_HSL_NVM_FLAG;

  AppliNvm_SaveMessageParam();
  
  return MOBLE_RESULT_SUCCESS;
   
}

/**                                                  
* @brief  Appli_Light_HslHue_Status: This function is callback for Application
*         when Light HSL HUE status message is received
* @param  pHslHue_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Appli_Light_HslHue_Status(MOBLEUINT8 const *pHslHue_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_HslHue_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8271! \n\r");
    
  return MOBLE_RESULT_SUCCESS;
}

#endif


#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL_SATURATION
/**
* @brief  Appli_Light_HslSaturation_Set: This function is callback for Application
*         when Light Hsl Saturation Set message is received
* @param  pLight_HslSaturationParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_HslSaturation_Set(Light_HslStatus_t* pLight_HslSaturationParam,
                                           MOBLEUINT8 OptionalValid)
{
  TRACE_M(TF_SERIAL_CTRL,"#8273! \n\r");	
  AppliHslSet.HslSaturation16 = pLight_HslSaturationParam->PresentHslSaturation16;
  
  HSL2RGB_Conversion();
  
  Appli_LightPwmValue.PwmRedValue = PwmValueMapping(Appli_RGBParam.Red_Value ,0xFFFF ,0x00); 
  Appli_LightPwmValue.PwmGreenValue = PwmValueMapping(Appli_RGBParam.Green_Value ,0xFFFF ,0x00); 
  Appli_LightPwmValue.PwmBlueValue = PwmValueMapping(Appli_RGBParam.Blue_Value ,0xFFFF ,0x00); 
  
  Ctl_LedOffState();
  
  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
  /* set the flag value for NVM store */
  RestoreFlag = LIGHT_HSL_NVM_FLAG;

  AppliNvm_SaveMessageParam();
    
  return MOBLE_RESULT_SUCCESS;
}

/**                                                  
* @brief  Appli_Light_HslSaturation_Status: This function is callback for Application
*         when Light HSL Saturation status message is received
* @param  pHslSaturation_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Appli_Light_HslSaturation_Status(MOBLEUINT8 const *pHslSaturation_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_HslSaturation_Status callback received \r\n");
  
  TRACE_M(TF_SERIAL_CTRL,"#8275! \n\r");
    
  return MOBLE_RESULT_SUCCESS;
}


#endif


#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL_SETUP
/**
* @brief  Appli_Light_HslDefault_Set: This function is callback for Application
*         when Light Hsl Default Set message is received
* @param  pLight_HslDefaultParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_HslDefault_Set(Light_HslStatus_t* pLight_HslDefaultParam,
                                        MOBLEUINT8 OptionalValid)
{
  TRACE_M(TF_SERIAL_CTRL,"#827F! \n\r");	
  Appli_HslDefaultSet.HslDefaultLightness16 = pLight_HslDefaultParam->PresentHslLightness16;
  Appli_HslDefaultSet.HslDefaultHueLightness16 = pLight_HslDefaultParam->PresentHslHueLightness16;
  Appli_HslDefaultSet.HslDefaultSaturation16 = pLight_HslDefaultParam->PresentHslSaturation16;  
  
  AppliHslSet.HslLightness16 = Appli_HslDefaultSet.HslDefaultLightness16;
  AppliHslSet.HslHueLightness16 = Appli_HslDefaultSet.HslDefaultHueLightness16;
  AppliHslSet.HslSaturation16 = Appli_HslDefaultSet.HslDefaultSaturation16;
  
  HSL2RGB_Conversion();
  
  Appli_LightPwmValue.PwmRedValue = PwmValueMapping(Appli_RGBParam.Red_Value ,0xFFFF ,0x00); 
  Appli_LightPwmValue.PwmGreenValue = PwmValueMapping(Appli_RGBParam.Green_Value ,0xFFFF ,0x00); 
  Appli_LightPwmValue.PwmBlueValue = PwmValueMapping(Appli_RGBParam.Blue_Value ,0xFFFF ,0x00); 
  
  Ctl_LedOffState();
  
  Light_UpdateLedValue(LOAD_STATE , Appli_LightPwmValue);
  /* set the flag value for NVM store */
  RestoreFlag = LIGHT_HSL_NVM_FLAG;
 
  AppliNvm_SaveMessageParam();
    
  return MOBLE_RESULT_SUCCESS;
}

/**                                                  
* @brief  Appli_Light_HslDefault_Status: This function is callback for Application
*          when Light HSL Default status  message is received
* @param  pHslDefault_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Appli_Light_HslDefault_Status(MOBLEUINT8 const *pHslDefault_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_HslDefault_Status callback received \r\n");
  
    TRACE_M(TF_SERIAL_CTRL,"#827C! \n\r");
    
  return MOBLE_RESULT_SUCCESS;
}

#endif 


#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL_SETUP
/**
* @brief  Appli_Light_HslRange_Set: This function is callback for Application
*         when Light Hsl Range Set message is received
* @param  pLight_HslRangeParam: Pointer to the parameters received for message
* @param  OptionalValid: Flag to inform about the validity of optional parameters 
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_HslRange_Set(Light_HslRangeParam_t* pLight_HslRangeParam,
                                      MOBLEUINT8 OptionalValid)
{
  TRACE_M(TF_SERIAL_CTRL,"#8281! \n\r"); 	
  AppliHslRangeSet.HslHueMinRange16 = pLight_HslRangeParam->HslHueMinRange16;
  AppliHslRangeSet.HslHueMaxRange16 = pLight_HslRangeParam->HslHueMaxRange16;
  AppliHslRangeSet.HslMinSaturation16 = pLight_HslRangeParam->HslMinSaturation16;
  AppliHslRangeSet.HslMaxSaturation16 = pLight_HslRangeParam->HslMaxSaturation16;
  
  return MOBLE_RESULT_SUCCESS;
}

/**                                                  
* @brief  Appli_Light_HslRange_Status: This function is callback for Application
*         when Light HSL range status message is received
* @param  pHslRange_status: Pointer to the parameters received for message
* @param  pLength: length of data
* @retval MOBLE_RESULT
*/
MOBLE_RESULT Appli_Light_HslRange_Status(MOBLEUINT8 const *pHslRange_status, MOBLEUINT32 pLength)
{
  TRACE_M(TF_LIGHT,"Light_HslRange_Status callback received \r\n");
  
 TRACE_M(TF_SERIAL_CTRL,"#827E! \n\r");
    
  return MOBLE_RESULT_SUCCESS;
}


#endif            


/*******************************************************************************
Following Functions are used for the LIGHTING Lightness MODEL 
*******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS 

/**
* @brief  Appli_Light_GetLightnessStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lLightnessState: Pointer to the status message
* @retval MOBLE_RESULT
*/  
MOBLE_RESULT Appli_Light_GetLightnessStatus(MOBLEUINT8* lLightnessState)
{
  *(lLightnessState) = ApplilightnessSet.PresentState16;
  *(lLightnessState+1) = ApplilightnessSet.PresentState16 >> 8;
  *(lLightnessState+2) = ApplilightnessSet.LastLightness16 ;
  *(lLightnessState+3) = ApplilightnessSet.LastLightness16 >> 8;
  
  return MOBLE_RESULT_SUCCESS;
}


/**
* @brief  Appli_Light_GetLightnessLinearStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lLightnessState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetLightnessLinearStatus(MOBLEUINT8* lLightnessState)
{
  *(lLightnessState) = ApplilightnessLinearSet.PresentState16;
  *(lLightnessState+1) = ApplilightnessLinearSet.PresentState16 >> 8;
  *(lLightnessState+2) = ApplilightnessSet.LastLightness16 ;
  *(lLightnessState+3) = ApplilightnessSet.LastLightness16 >> 8;
  
  return MOBLE_RESULT_SUCCESS;
}


/**
* @brief  Appli_Light_GetLightnessDefaultStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lDefaultState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetLightnessDefaultStatus(MOBLEUINT8* lDefaultState)
{
  *(lDefaultState) = ApplilightnessSet.LightnessDefault;
  *(lDefaultState+1) = ApplilightnessSet.LightnessDefault >> 8;
  
  return MOBLE_RESULT_SUCCESS;
}


/**
* @brief  Appli_Light_GetLightnessRangeStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lRangeState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetLightnessRangeStatus(MOBLEUINT8* lRangeState)
{
  *(lRangeState) = ApplilightnessSet.StatusCode;
  *(lRangeState+1) = ApplilightnessSet.RangeMin;
  *(lRangeState+2) = ApplilightnessSet.RangeMin >> 8;
  *(lRangeState+3) = ApplilightnessSet.RangeMax;
  *(lRangeState+4) = ApplilightnessSet.RangeMax >> 8;
  
  return MOBLE_RESULT_SUCCESS;
}

#endif /* #ifdef ENABLE_LIGHT_MODEL_SERVER_LIGHTNESS  */

/*******************************************************************************
Following Functions are used for the LIGHTING CTL MODEL 
*******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_CTL

/**
* @brief  Appli_Light_GetCtlLightStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lCtlLightState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetCtlLightStatus(MOBLEUINT8* lCtlLightState)
{
  *(lCtlLightState) = AppliCtlSet.PresentLightness16;
  *(lCtlLightState+1) = AppliCtlSet.PresentLightness16 >> 8;
  *(lCtlLightState+2) = AppliCtlSet.PresentTemperature16;
  *(lCtlLightState+3) = AppliCtlSet.PresentTemperature16 >>8;
  *(lCtlLightState+4) = AppliCtlSet.PresentCtlDelta16 >>8;
  *(lCtlLightState+5) = AppliCtlSet.PresentCtlDelta16 >>8;
  
  return MOBLE_RESULT_SUCCESS;
}  


/**
* @brief  Appli_Light_GetCtlTeperatureStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lCtlTempState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetCtlTemperatureStatus(MOBLEUINT8* lCtlTempState)
{
  *(lCtlTempState) = AppliCtlSet.PresentTemperature16;
  *(lCtlTempState+1) = AppliCtlSet.PresentTemperature16 >> 8;
  *(lCtlTempState+2) = AppliCtlSet.PresentCtlDelta16;
  *(lCtlTempState+3) = AppliCtlSet.PresentCtlDelta16 >>8;
  
  return MOBLE_RESULT_SUCCESS;
}  


/**
* @brief  Appli_Light_GetCtlTemperatureRange: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lCtlTempRange: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetCtlTemperatureRange(MOBLEUINT8* lCtlTempRange)
{   
  *(lCtlTempRange) = AppliCtlTemperatureRangeSet.StatusCode;
  *(lCtlTempRange+1) = AppliCtlTemperatureRangeSet.RangeMin;
  *(lCtlTempRange+2) = AppliCtlTemperatureRangeSet.RangeMin >> 8;
  *(lCtlTempRange+3) = AppliCtlTemperatureRangeSet.RangeMax;
  *(lCtlTempRange+4) = AppliCtlTemperatureRangeSet.RangeMax >>8;
  
  return MOBLE_RESULT_SUCCESS;
}  


/**
* @brief  Appli_Light_GetCtlDefaultStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lCtlDefaultState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetCtlDefaultStatus(MOBLEUINT8* lCtlDefaultState)
{
  *(lCtlDefaultState) = AppliCtlDefaultSet.CtlDefaultLightness16;
  *(lCtlDefaultState+1) = AppliCtlDefaultSet.CtlDefaultLightness16 >> 8;
  *(lCtlDefaultState+2) = AppliCtlDefaultSet.CtlDefaultTemperature16;
  *(lCtlDefaultState+3) = AppliCtlDefaultSet.CtlDefaultTemperature16 >>8;
  *(lCtlDefaultState+4) = AppliCtlDefaultSet.CtlDefaultDeltaUv;
  *(lCtlDefaultState+5) = AppliCtlDefaultSet.CtlDefaultDeltaUv >>8;
  
  return MOBLE_RESULT_SUCCESS;
}  

#endif /* #ifdef ENABLE_LIGHT_MODEL_SERVER_CTL  */

/*******************************************************************************
Following Functions are used for the LIGHTING HSL MODEL 
*******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL
/**
* @brief  Appli_Light_GetHslStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lHslState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetHslStatus(MOBLEUINT8* lHslState)
{
  *(lHslState) = AppliHslSet.HslLightness16;
  *(lHslState+1) = AppliHslSet.HslLightness16 >> 8;
  *(lHslState+2) = AppliHslSet.HslHueLightness16;
  *(lHslState+3) = AppliHslSet.HslHueLightness16 >>8;
  *(lHslState+4) = AppliHslSet.HslSaturation16;
  *(lHslState+5) = AppliHslSet.HslSaturation16 >>8;
  
  return MOBLE_RESULT_SUCCESS;
}  

/**
* @brief  Appli_Light_GetHslHueStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lHslHueState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetHslHueStatus(MOBLEUINT8* lHslHueState)
{  
  *(lHslHueState) = AppliHslSet.HslHueLightness16;
  *(lHslHueState+1) = AppliHslSet.HslHueLightness16 >>8;
  
  return MOBLE_RESULT_SUCCESS;
}    


/**
* @brief  Appli_Light_GetHslSaturationStatus: This function is callback for Application
*         to get the application values in middleware used for transition change
* @param  lHslSaturationState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetHslSaturationStatus(MOBLEUINT8* lHslSaturationState)
{  
  *(lHslSaturationState) = AppliHslSet.HslSaturation16;
  *(lHslSaturationState+1) = AppliHslSet.HslSaturation16 >>8;
  
  return MOBLE_RESULT_SUCCESS;
}  

/**
* @brief  Appli_Light_GetHslDefaultStatus: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lHslDefaultState: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetHslDefaultStatus(MOBLEUINT8* lHslDefaultState)
{
  *(lHslDefaultState) = Appli_HslDefaultSet.HslDefaultLightness16;
  *(lHslDefaultState+1) = Appli_HslDefaultSet.HslDefaultLightness16 >> 8;
  *(lHslDefaultState+2) = Appli_HslDefaultSet.HslDefaultHueLightness16;
  *(lHslDefaultState+3) = Appli_HslDefaultSet.HslDefaultHueLightness16 >>8;
  *(lHslDefaultState+4) = Appli_HslDefaultSet.HslDefaultSaturation16;
  *(lHslDefaultState+5) = Appli_HslDefaultSet.HslDefaultSaturation16 >>8;
  
  return MOBLE_RESULT_SUCCESS;
}  

/**
* @brief  Appli_Light_GetHslSatRange: This function is callback for Application
*         to get the application values in middleware used for transition change
* @param  lHslSatRange: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetHslSatRange(MOBLEUINT8* lHslSatRange)
{   
  *(lHslSatRange) = AppliHslRangeSet.StatusCode;
  *(lHslSatRange+1) = AppliHslRangeSet.HslMinSaturation16;
  *(lHslSatRange+2) = AppliHslRangeSet.HslMinSaturation16 >> 8;
  *(lHslSatRange+3) = AppliHslRangeSet.HslMaxSaturation16;
  *(lHslSatRange+4) = AppliHslRangeSet.HslMaxSaturation16 >>8;
  
  return MOBLE_RESULT_SUCCESS;
}

/**
* @brief  Appli_Light_GetHslHueRange: This function is callback for Application
*         to get the application values in middleware used for transition change.
* @param  lHslHueRange: Pointer to the status message
* @retval MOBLE_RESULT
*/ 
MOBLE_RESULT Appli_Light_GetHslHueRange(MOBLEUINT8* lHslHueRange)
{   
  *(lHslHueRange) = AppliHslRangeSet.StatusCode;
  *(lHslHueRange+1) = AppliHslRangeSet.HslHueMinRange16;
  *(lHslHueRange+2) = AppliHslRangeSet.HslHueMinRange16 >> 8;
  *(lHslHueRange+3) = AppliHslRangeSet.HslHueMaxRange16;
  *(lHslHueRange+4) = AppliHslRangeSet.HslHueMaxRange16 >>8;
  
  return MOBLE_RESULT_SUCCESS;
}  

/******************************************************************************/
#endif    /* ENABLE_LIGHT_MODEL_SERVER_HSL */
/******************************************************************************/

#ifdef ENABLE_LIGHT_MODEL_SERVER_HSL

/**
* @brief  Function to convert the map the RGB values in real values.
* @param  value1: This value corresponds to red led.
* @param  value2: This value corresponds to green led .
* @param  value3: This value corresponds to blue led .
* @retval void
*/
void RgbF_Create(MOBLEUINT16 value1, MOBLEUINT16 value2, MOBLEUINT16 value3)
{
   Appli_RGBParam.Red_Value = (MOBLEUINT16)(65535 * value1/1000);
   Appli_RGBParam.Green_Value = (MOBLEUINT16)(65535 * value2/1000);
   Appli_RGBParam.Blue_Value = (MOBLEUINT16)(65535 * value3/1000);
}

/**
* @brief  Function to convert the HSL values in RGB values.
* @param  void
* @retval void
*/
void HSL2RGB_Conversion(void)
{
  
  MOBLEUINT16 hueValue;
  float lightnessvalue;
  float saturationValue;
  MOBLEUINT16 cValue;  /* chroma */
  MOBLEUINT16 mValue;  /* intermediate value used for creating the r,g,b baseline */
  MOBLEUINT16 xValue;  /* intermediate value used for computing r,g,b */
  
  if(AppliHslSet.HslSaturation16 == 0)
  {
    Appli_RGBParam.Red_Value  = AppliHslSet.HslLightness16  ;
    Appli_RGBParam.Green_Value = AppliHslSet.HslLightness16 ;
    Appli_RGBParam.Blue_Value = AppliHslSet.HslLightness16 ;
  }
  else if((AppliHslSet.HslLightness16 == 0xFFFF) || (AppliHslSet.HslLightness16 == 0x00))
  {
    Appli_RGBParam.Red_Value  = AppliHslSet.HslLightness16  ;
    Appli_RGBParam.Green_Value = AppliHslSet.HslLightness16 ;
    Appli_RGBParam.Blue_Value = AppliHslSet.HslLightness16 ;
  }
  else
  {  
    /* Hue is calculated for angular value. so converting in between 0 to 360 degree */
    hueValue = (MOBLEUINT16)(360 * (float)AppliHslSet.HslHueLightness16/65535);
    lightnessvalue = (float)AppliHslSet.HslLightness16/65535;
    saturationValue = (float)AppliHslSet.HslSaturation16/65535;
    
    /* Formula to get the values used for RGB conversion */
    cValue = (MOBLEUINT16)(((1- fabs(2 * lightnessvalue-1))*saturationValue) * 1000);
    mValue = (MOBLEUINT16)((lightnessvalue * 1000) -  (cValue/2));
    xValue = (MOBLEUINT16)(cValue * (1 - fabs(fmod(hueValue / 60.0, 2.0) - 1)));
    
    /* Conditons applied for the different angle of hue value */
    if (hueValue > 0 && hueValue < (HUE_UPPER_LIMIT / 6))
    {
       RgbF_Create(cValue + mValue, xValue + mValue, mValue);
    }
    else if (hueValue >= (HUE_UPPER_LIMIT / 6) && hueValue < (HUE_UPPER_LIMIT / 3))
    {
       RgbF_Create(xValue + mValue, cValue + mValue, mValue);
    }
    else if (hueValue >= (HUE_UPPER_LIMIT / 3) && hueValue < (HUE_UPPER_LIMIT / 2))
    {
       RgbF_Create(mValue, cValue + mValue, xValue + mValue);
    }
    else if (hueValue >= (HUE_UPPER_LIMIT / 2)
                          && hueValue < (2 * HUE_UPPER_LIMIT / 3))
    {
       RgbF_Create(mValue, xValue + mValue, cValue + mValue);
    }
    else if (hueValue >= (2 * HUE_UPPER_LIMIT / 3)
                          && hueValue < (5 * HUE_UPPER_LIMIT / 6))
    {
       RgbF_Create(xValue + mValue, mValue, cValue + mValue);
    }
    else if ((hueValue >= (5 * HUE_UPPER_LIMIT / 6) && hueValue <= HUE_UPPER_LIMIT) || (hueValue == 0))
    {
       RgbF_Create(cValue + mValue, mValue, xValue + mValue);
    }
    else
    {
       RgbF_Create(mValue, mValue, mValue);
    } 
  }
}

#endif 
                                               
/**
* @brief  Rgb_LedOffState: This function is called while using CTL, makes all the RGB 
*         PWM off state for Application.        
* @param  void: 
* @retval void
*/ 
void Rgb_LedOffState(void)
{
  Appli_LightPwmValue.PwmRedValue = PWM_VALUE_OFF;
  Appli_LightPwmValue.PwmGreenValue = PWM_VALUE_OFF;
  Appli_LightPwmValue.PwmBlueValue = PWM_VALUE_OFF; 
  Appli_LightPwmValue.IntensityValue = PWM_VALUE_OFF;
}

/**
* @brief  Ctl_LedOffState: This function is called while using HSL, makes all the  
*         cool Warm PWM off state for Application.        
* @param  void: 
* @retval void
*/ 
void Ctl_LedOffState(void)
{
  Appli_LightPwmValue.PwmWarmValue = PWM_VALUE_OFF;
  Appli_LightPwmValue.PwmCoolValue = PWM_VALUE_OFF;
  Appli_LightPwmValue.IntensityValue = PWM_VALUE_OFF;
}

/**
* @brief  Function to set the values for PWM for RGB board.
* @param  state: decided the state of PWM according to the condition.
* @retval void
*/
void Light_UpdateLedValue(MOBLEUINT8 state ,Appli_LightPwmValue_t light_state)
{
  
  if(state == RESUME_STATE)
  {
#ifdef USER_BOARD_1LED
    Modify_PWM(SINGLE_LED,  light_state.IntensityValue); 
#endif

#ifdef  USER_BOARD_COOL_WHITE_LED

    Modify_PWM(COOL_LED, light_state.PwmCoolValue); 
    Modify_PWM(WARM_LED, light_state.PwmWarmValue); 
#endif

#ifdef  USER_BOARD_RGB_LED

    Modify_PWM(RED_LED, light_state.PwmRedValue); 
    Modify_PWM(GREEN_LED, light_state.PwmGreenValue); 
    Modify_PWM(BLUE_LED, light_state.PwmBlueValue);  
#endif

  }
  else if(state == RESET_STATE)
  {
    Modify_PWM(SINGLE_LED, PWM_VALUE_OFF);     
    Modify_PWM(COOL_LED, 0); 
    Modify_PWM(WARM_LED, 0); 
    Modify_PWM(RED_LED, PWM_VALUE_OFF); 
    Modify_PWM(GREEN_LED, PWM_VALUE_OFF); 
    Modify_PWM(BLUE_LED, PWM_VALUE_OFF);
  }
  else if(state == LOAD_STATE)
  {
#ifdef USER_BOARD_1LED
    Modify_PWM(SINGLE_LED, light_state.IntensityValue); 
#endif
    
#ifdef  USER_BOARD_COOL_WHITE_LED    
    Modify_PWM(COOL_LED, light_state.PwmCoolValue); 
    Modify_PWM(WARM_LED, light_state.PwmWarmValue); 
#endif
#ifdef  USER_BOARD_RGB_LED    
    Modify_PWM(RED_LED, light_state.PwmRedValue); 
    Modify_PWM(GREEN_LED, light_state.PwmGreenValue); 
    Modify_PWM(BLUE_LED, light_state.PwmBlueValue); 
#endif    
  }
  else
  {
#ifdef USER_BOARD_1LED
    Modify_PWM(SINGLE_LED, light_state.IntensityValue); 
#endif
    
#ifdef  USER_BOARD_COOL_WHITE_LED        
    light_state.PwmCoolValue = PWM_DEFAULT_VALUE;
    
    Modify_PWM(COOL_LED, light_state.PwmCoolValue); 
    Modify_PWM(WARM_LED, light_state.PwmWarmValue); 
#endif
#ifdef  USER_BOARD_RGB_LED    
    Modify_PWM(RED_LED, light_state.PwmRedValue); 
    Modify_PWM(GREEN_LED, light_state.PwmGreenValue); 
    Modify_PWM(BLUE_LED, light_state.PwmBlueValue); 
#endif    
  } 
}

/**
* @}
*/

/**
* @}
*/
/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

