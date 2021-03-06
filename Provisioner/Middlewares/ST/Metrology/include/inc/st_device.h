/**
  ******************************************************************************
  * @file    st_device.h
  * @author  STMicroelectronics
  * @version V1.0
  * @date    17-May-2016
  * @brief   This file contains all the functions prototypes for metrology
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ST_DEVICE_H
#define __ST_DEVICE_H

#ifdef __cplusplus
 extern "C" {
#endif
 
   
/* Define --------------------------------------------------------------------*/
/* Communication */
#define USART_SPEED		    19200        /*57600*/
#define USART_TIMEOUT		    100
#define SPI_STPM_SPEED              32
#define SPI_TIMEOUT		    10
   
   
 #define POWER_LSB_FACTOR            5067641//9800247    //11556689//2520400//9800247
 #define ENERGY_LSB_FACTOR           6191//26844    //14119//11973
 #define VOLTAGE_LSB_FACTOR          118245//116000//116274//117800    //118245//
 #define CURRENT_LSB_FACTOR          4286//8429    //9774//8429  
   
/* I2C EEPROM */
#define I2C_EEPROM		    I2C1
#define I2C_TIMEOUT		    100
  
#define CDC_POLLING_INTERVAL             4 /* in ms. The max is 65ms and the min is 1ms */
   
 
#define UART_XFER_STPM3X
   //#define POWER_DOWN
/*----------------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Definition of the return platform: */
typedef enum {
  PLTF_UNDEF = 0,
  PLTF_STMET,                   // STMET
  PLTF_STCOMET_COORD = 10,    // COMET
  PLTF_STCOMET_DEVICE,
  PLTF_STCOMET_COORD_METER,
  PLTF_STCOMET_DEVICE_METER,
  PLTF_STM32_METER = 20,      // STM32 + STPM3x
  PLTF_STM32_COORD = 30,      // STM32 + STarCom
  PLTF_STM32_DEVICE,
  PLTF_STARCOM_COORD = 40,    // STarCom
  PLTF_STARCOM_DEVICE,
} platform_t;



/* Exported constants --------------------------------------------------------*/
#define MAJOR_VERSION       1
#define MINOR_VERSION       0
#define PATCH_VERSION       0

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __ST_DEVICE_H */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
