/** \mainpage
*
****************************************************************************
* Copyright (C) 2011 - 2015 Bosch Sensortec GmbH
*
* File : bmm050.h
*
* Date : 2015/05/21
*
* Revision : 2.0.4 $
*
* Usage: Sensor Driver for BMM050 and BMM150 sensor
*
****************************************************************************
*
* \section License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/
/****************************************************************************/
/*! \file bmm050.h
    \brief BMM050 Sensor Driver Support Header File */

#ifndef __BMC150_H__
#define __BMC150_H__

#include "delay.h"
//#include "drv_i2c_static.h"

#define TRUE	1
#define FALSE   0
                                                //valores tracker    valores placa evaluacion
#define BMC150_I2C_ADRESS 0x20                  //0x20 = (0x10 << 1)    0x22 = (0x11 << 1)
#define BMC150_MAGNETOMETER_I2C_ADRESS 0x24     //0x24 = 0x12 <<1       0x26 = 0x13 <<1
#define I2C_TIMEOUT_MS 20


/**************************************************************/
/**\name	REGISTER ADDRESS DEFINITIONS    */
/**************************************************************/
#define BMA2x2_EEP_OFFSET                       0x16
#define BMA2x2_IMAGE_BASE                       0x38
#define BMA2x2_IMAGE_LEN                        22


#define BMA2x2_CHIP_ID_REG                      0x00
#define BMA2x2_X_AXIS_LSB_REG                   0x02
#define BMA2x2_X_AXIS_MSB_REG                   0x03
#define BMA2x2_Y_AXIS_LSB_REG                   0x04
#define BMA2x2_Y_AXIS_MSB_REG                   0x05
#define BMA2x2_Z_AXIS_LSB_REG                   0x06
#define BMA2x2_Z_AXIS_MSB_REG                   0x07
#define BMA2x2_TEMP_REG							0x08
#define BMA2x2_STAT1_REG						0x09
#define BMA2x2_STAT2_REG						0x0A
#define BMA2x2_STAT_TAP_SLOPE_REG				0x0B
#define BMA2x2_STAT_ORIENT_HIGH_REG				0x0C
#define BMA2x2_STAT_FIFO_REG					0x0E
#define BMA2x2_RANGE_SELECT_REG                 0x0F
#define BMA2x2_BW_SELECT_REG                    0x10
#define BMA2x2_MODE_CTRL_REG                    0x11
#define BMA2x2_LOW_NOISE_CTRL_REG               0x12
#define BMA2x2_DATA_CTRL_REG                    0x13
#define BMA2x2_RST_REG                          0x14
#define BMA2x2_INTR_ENABLE1_REG                 0x16
#define BMA2x2_INTR_ENABLE2_REG                 0x17
#define BMA2x2_INTR_SLOW_NO_MOTION_REG          0x18
#define BMA2x2_INTR1_PAD_SELECT_REG             0x19
#define BMA2x2_INTR_DATA_SELECT_REG             0x1A
#define BMA2x2_INTR2_PAD_SELECT_REG             0x1B
#define BMA2x2_INTR_SOURCE_REG                  0x1E
#define BMA2x2_INTR_SET_REG                     0x20
#define BMA2x2_INTR_CTRL_REG                    0x21
#define BMA2x2_LOW_DURN_REG                     0x22
#define BMA2x2_LOW_THRES_REG                    0x23
#define BMA2x2_LOW_HIGH_HYST_REG                0x24
#define BMA2x2_HIGH_DURN_REG                    0x25
#define BMA2x2_HIGH_THRES_REG                   0x26
#define BMA2x2_SLOPE_DURN_REG                   0x27
#define BMA2x2_SLOPE_THRES_REG                  0x28
#define BMA2x2_SLOW_NO_MOTION_THRES_REG         0x29
#define BMA2x2_TAP_PARAM_REG                    0x2A
#define BMA2x2_TAP_THRES_REG                    0x2B
#define BMA2x2_ORIENT_PARAM_REG                 0x2C
#define BMA2x2_THETA_BLOCK_REG                  0x2D
#define BMA2x2_THETA_FLAT_REG                   0x2E
#define BMA2x2_FLAT_HOLD_TIME_REG               0x2F
#define BMA2x2_FIFO_WML_TRIG                    0x30
#define BMA2x2_SELFTEST_REG                     0x32
#define BMA2x2_EEPROM_CTRL_REG                  0x33
#define BMA2x2_SERIAL_CTRL_REG                  0x34
#define BMA2x2_OFFSET_CTRL_REG                  0x36
#define BMA2x2_OFFSET_PARAMS_REG                0x37
#define BMA2x2_OFFSET_X_AXIS_REG                0x38
#define BMA2x2_OFFSET_Y_AXIS_REG                0x39
#define BMA2x2_OFFSET_Z_AXIS_REG                0x3A
#define BMA2x2_GP0_REG                          0x3B
#define BMA2x2_GP1_REG                          0x3C
#define BMA2x2_FIFO_MODE_REG                    0x3E
#define BMA2x2_FIFO_DATA_OUTPUT_REG             0x3F


/****************************************************/
/**\name  RANGE AND BANDWIDTH SELECT     */
/***************************************************/
#define BMA2x2_RANGE_2G                 3
/**< sets range to +/- 2G mode */
#define BMA2x2_RANGE_4G                 5
/**< sets range to +/- 4G mode */
#define BMA2x2_RANGE_8G                 8
/**< sets range to +/- 8G mode */
#define BMA2x2_RANGE_16G                12
/**< sets range to +/- 16G mode */


#define BMA2x2_BW_7_81HZ        0x08
 /**< sets bandwidth to LowPass 7.81HZ  */
#define BMA2x2_BW_15_63HZ       0x09
/**< sets bandwidth to LowPass 15.63HZ  */
#define BMA2x2_BW_31_25HZ       0x0A
/**< sets bandwidth to LowPass 31.25HZ  */
#define BMA2x2_BW_62_50HZ       0x0B
 /**< sets bandwidth to LowPass 62.50HZ  */
#define BMA2x2_BW_125HZ         0x0C
 /**< sets bandwidth to LowPass 125HZ  */
#define BMA2x2_BW_250HZ         0x0D
/**< sets bandwidth to LowPass 250HZ  */
#define BMA2x2_BW_500HZ         0x0E
/**< sets bandwidth to LowPass 500HZ  */
#define BMA2x2_BW_1000HZ        0x0F
 /**< sets bandwidth to LowPass 1000HZ  */

/******************************************/
/**\name  LATCH DURATION     */
/******************************************/
#define BMA2x2_LATCH_DURN_NON_LATCH    0x00
/* sets LATCH duration to NON LATCH  */
#define BMA2x2_LATCH_DURN_250MS        0x01
/* sets LATCH duration to 250 ms */
#define BMA2x2_LATCH_DURN_500MS        0x02
/* sets LATCH duration to 500 ms */
#define BMA2x2_LATCH_DURN_1S           0x03
 /* sets LATCH duration to 1 s */
#define BMA2x2_LATCH_DURN_2S           0x04
 /* sets LATCH duration to 2 s*/
#define BMA2x2_LATCH_DURN_4S           0x05
 /* sets LATCH duration to 4 s */
#define BMA2x2_LATCH_DURN_8S           0x06
 /* sets LATCH duration to 8 s */
#define BMA2x2_LATCH_DURN_LATCH        0x07
 /* sets LATCH duration to LATCH */
#define BMA2x2_LATCH_DURN_NON_LATCH1   0x08
 /* sets LATCH duration to NON LATCH1 */
#define BMA2x2_LATCH_DURN_250US        0x09
 /* sets LATCH duration to 250 Us */
#define BMA2x2_LATCH_DURN_500US        0x0A
 /* sets LATCH duration to 500 Us */
#define BMA2x2_LATCH_DURN_1MS          0x0B
 /* sets LATCH duration to 1 Ms */
#define BMA2x2_LATCH_DURN_12_5MS       0x0C
/* sets LATCH duration to 12.5 Ms */
#define BMA2x2_LATCH_DURN_25MS         0x0D
/* sets LATCH duration to 25 Ms */
#define BMA2x2_LATCH_DURN_50MS         0x0E
 /* sets LATCH duration to 50 Ms */
#define BMA2x2_LATCH_DURN_LATCH1       0x0F
/* sets LATCH duration to LATCH*/

/******************************************/
/**\name  MODE SETTINGS     */
/******************************************/
#define BMA2x2_MODE_NORMAL             0
#define BMA2x2_MODE_LOWPOWER1          1
#define BMA2x2_MODE_SUSPEND            2
#define BMA2x2_MODE_DEEP_SUSPEND       3
#define BMA2x2_MODE_LOWPOWER2          4
#define BMA2x2_MODE_STANDBY            5
/******************************************/
/**\name  INTERRUPT TYPE SELECTION     */
/******************************************/
#define BMA2x2_LOW_G_INTR       0
/**< enable/disable low-g interrupt*/
#define BMA2x2_HIGH_G_X_INTR    1
/**< enable/disable high_g X interrupt*/
#define BMA2x2_HIGH_G_Y_INTR    2
/**< enable/disable high_g Y interrupt*/
#define BMA2x2_HIGH_G_Z_INTR    3
/**< enable/disable high_g Z interrupt*/
#define BMA2x2_DATA_ENABLE      4
/**< enable/disable data interrupt*/
#define BMA2x2_SLOPE_X_INTR     5
/**< enable/disable slope X interrupt*/
#define BMA2x2_SLOPE_Y_INTR     6
/**< enable/disable slope X interrupt*/
#define BMA2x2_SLOPE_Z_INTR     7
/**< enable/disable slope X interrupt*/
#define BMA2x2_SINGLE_TAP_INTR  8
/**< enable/disable single tap interrupt*/
#define BMA2x2_DOUBLE_TAP_INTR  9
/**< enable/disable double tap interrupt*/
#define BMA2x2_ORIENT_INTR      10
/**< enable/disable orient interrupt*/
#define BMA2x2_FLAT_INTR        11
/**< enable/disable flat interrupt*/
#define BMA2x2_FIFO_FULL_INTR   12
/**< enable/disable fifo full interrupt*/
#define BMA2x2_FIFO_WM_INTR     13

/******************************************/
/**\name  SOURCE REGISTER     */
/******************************************/
#define BMA2x2_SOURCE_LOW_G            0
#define BMA2x2_SOURCE_HIGH_G           1
#define BMA2x2_SOURCE_SLOPE            2
#define BMA2x2_SOURCE_SLOW_NO_MOTION   3
#define BMA2x2_SOURCE_TAP              4
#define BMA2x2_SOURCE_DATA             5

#define BMA2x2_INTR1_OUTPUT      0
#define BMA2x2_INTR2_OUTPUT      1
#define BMA2x2_INTR1_LEVEL       0
#define BMA2x2_INTR2_LEVEL       1

/******************************************/
/**\name  DURATION     */
/******************************************/
#define BMA2x2_LOW_DURN                0
#define BMA2x2_HIGH_DURN               1
#define BMA2x2_SLOPE_DURN              2
#define BMA2x2_SLOW_NO_MOTION_DURN     3

/****************************************************/
/**\name   SLOPE DURATION */
/****************************************************/
#define BMA2x2_SLOPE_DURN__POS                    0
#define BMA2x2_SLOPE_DURN__LEN                    2
#define BMA2x2_SLOPE_DURN__MSK                    0x03
#define BMA2x2_SLOPE_DURN__REG                    BMA2x2_SLOPE_DURN_REG
/******************************************/
/**\name  THRESHOLD     */
/******************************************/
#define BMA2x2_LOW_THRES                0
#define BMA2x2_HIGH_THRES               1
#define BMA2x2_SLOPE_THRES              2
#define BMA2x2_SLOW_NO_MOTION_THRES     3

/**< enable/disab
void I2cStart(void);
void I2cReStart(void);
void I2cStop (void);
void I2cWriteByte (uint8_t Val);
uint8_t I2C_RxOneByte(bool Ack);
void AccelWriteRegisters(uint8_t RegisterAddress, int NumberBytesToWrite, uint8_t* Data);
void AccelReadRegisters(uint8_t RegisterAddress, int NumberBytesToRead, uint8_t* Data);

*/

#endif
