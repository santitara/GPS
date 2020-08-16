/*
****************************************************************************
* Copyright (C) 2011 - 2015 Bosch Sensortec GmbH
*
* bmm050.c
* Date: 2015/08/19
* Revision: 2.0.5 $
*
* Usage: Sensor Driver for  BMM050 and BMM150 sensor
*
****************************************************************************
* License:
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
#include "bmc150.h"

unsigned long ErrorCounterI2cStartOrRestart;
unsigned long ErrorCounterI2cWriteByteAck;
unsigned long ErrorCounterI2cWriteByte;
unsigned long ErrorCounterI2cACK_NACKTimeOut;
unsigned long ErrorCounterI2cTimeOut;
unsigned long ErrorCounterI2cPcap;
unsigned char ErrorI2cPcap;

//==============================================================================
void I2cStart(void)
{
DRV_I2C0_MasterStart();
DRV_I2C0_WaitForStartComplete();
}
//==============================================================================
void I2cReStart(void)
{
 DRV_I2C0_MasterRestart();
 DRV_I2C0_WaitForStartComplete();
}
//==============================================================================
void I2cStop (void)
{
 DRV_I2C0_MasterStop();
    DRV_I2C0_WaitForStopComplete();
   }
//==============================================================================
void I2cWriteByte (unsigned char Val)
{
 if (DRV_I2C0_ByteWrite(Val))
 {
    DRV_I2C0_WaitForByteWriteToComplete();
    if (!DRV_I2C0_WriteByteAcknowledged())
        ErrorCounterI2cWriteByteAck++;
 }
 else
 {
     ErrorCounterI2cWriteByte++;
 }
}//function
//==============================================================================
unsigned char I2C_RxOneByte(unsigned char Ack)
{
unsigned char DataByte;
int I2cTimeOut;

DRV_I2C0_SetUpByteRead(); //Set Rx enable in MSTR which causes SLAVE to send data
while(!DRV_I2C0_WaitForReadByteAvailable())
{// Wait till RBF = 1; Which means data is available in I2C2RCV reg
}
DataByte = DRV_I2C0_ByteRead();   //Read from I2CxRCV
if (Ack)
    DRV_I2C0_MasterACKSend();
else
    DRV_I2C0_MasterNACKSend();   //last byte; send NACK to Slave, no more data needed
while (!DRV_I2C0_WaitForACKOrNACKComplete())
{// wait till NACK sequence is complete i.e ACKEN = 0 
}
return (DataByte);
}

/*==============================================================================
void MagnetometerWriteRegister(unsigned char RegisterAddress, unsigned char Data)
{
    while (!DRV_I2C0_MasterBusIdle());
    I2cStart();
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(BMC150_MAGNETOMETER_I2C_ADRESS);
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(RegisterAddress);
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(Data);
    
    while (!DRV_I2C0_MasterBusIdle());
    I2cStop();
}
*///==============================================================================
void AccelWriteRegister(unsigned char RegisterAddress, unsigned char Data)
{
    while (!DRV_I2C0_MasterBusIdle());
    I2cStart();
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(BMC150_I2C_ADRESS);
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(RegisterAddress);
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(Data);
    
    while (!DRV_I2C0_MasterBusIdle());
    I2cStop();
}
/*
void AccelWriteRegisters(unsigned char RegisterAddress, int NumberBytesToWrite, unsigned char* Data)
{
    
    
    int i,I2cTimeOut;

    while (!DRV_I2C0_MasterBusIdle());
    I2cStart();
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(BMC150_I2C_ADRESS);
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(RegisterAddress);
    while (!DRV_I2C0_MasterBusIdle());
    for (i = 0; i < NumberBytesToWrite; i++)
    {
        I2cWriteByte(*Data);
        Data++;    
    }
    while (!DRV_I2C0_MasterBusIdle());
    I2cStop();
    
  
}
*/
//==============================================================================
void AccelReadRegisters(unsigned char RegisterAddress, int NumberBytesToRead, unsigned char *Data)
{
    int i,I2cTimeOut;

    while (!DRV_I2C0_MasterBusIdle());
    I2cStart();
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(BMC150_I2C_ADRESS);
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(RegisterAddress);
    while (!DRV_I2C0_MasterBusIdle());
    I2cReStart();
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(BMC150_I2C_ADRESS | 0x01);
    while (!DRV_I2C0_MasterBusIdle());
    for (i = 0; i < NumberBytesToRead; i++)
    {
        if ((i + 1) < NumberBytesToRead)
            *Data++ = I2C_RxOneByte(TRUE);
        else
            *Data++ = I2C_RxOneByte(FALSE); // last byte => NACK
    }
    while (!DRV_I2C0_MasterBusIdle());
    I2cStop();
}
/*==============================================================================*/
void MagnetometerAccelReadRegisters(unsigned char RegisterAddress, int NumberBytesToRead, unsigned char *Data)
{
    int i,I2cTimeOut;

    while (!DRV_I2C0_MasterBusIdle());
    I2cStart();
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(BMC150_MAGNETOMETER_I2C_ADRESS);
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(RegisterAddress);
    while (!DRV_I2C0_MasterBusIdle());
    I2cReStart();
    while (!DRV_I2C0_MasterBusIdle());
    I2cWriteByte(BMC150_MAGNETOMETER_I2C_ADRESS | 0x01);
    while (!DRV_I2C0_MasterBusIdle());
    for (i = 0; i < NumberBytesToRead; i++)
    {
        if ((i + 1) < NumberBytesToRead)
            *Data++ = I2C_RxOneByte(TRUE);
        else
            *Data++ = I2C_RxOneByte(FALSE); // last byte => NACK
    }
    while (!DRV_I2C0_MasterBusIdle());
    I2cStop();
}
 //*****************************************************************************
void acelerometer_init(void)
{
    AccelWriteRegister(BMA2x2_MODE_CTRL_REG, 0x00);  //normal mode 0x00 ; low power 0x02
    delay_10us_x(10);
    AccelWriteRegister(BMA2x2_RANGE_SELECT_REG, 0x03);  //+-2g range
    delay_10us_x(10);
    AccelWriteRegister(BMA2x2_BW_SELECT_REG, 0x00);  //  7.81Hz
    delay_10us_x(10);
    AccelWriteRegister(BMA2x2_INTR_SOURCE_REG, 0x04);  //acceleration data filtered 0x04 ; no filtered 0x00
    delay_10us_x(10);
    AccelWriteRegister(BMA2x2_INTR_SET_REG,0x00 );  //pins electrical behavior 0x00 push-pull active low; 0x05 push-pull active high
    delay_10us_x(10);
    AccelWriteRegister(BMA2x2_INTR_CTRL_REG,0x04 ); //latched interrupts flags 0x0F latched for 2 sec 0x04
    delay_10us_x(10);
    AccelWriteRegister(BMA2x2_INTR1_PAD_SELECT_REG, 0x04); //any-motion interrupt mapped to pin INT1
    delay_10us_x(10);
	//AccelWriteRegister(BMA2x2_INTR2_PAD_SELECT_REG, 0x04); //any-motion interrupt mapped to pin INT2
    //delay_10us_x(10);
    AccelWriteRegister(BMA2x2_SLOPE_DURN_REG, 0x00);   // 2 samples 0x00->1 to 0x03->4 samples
    delay_10us_x(10);
    //AccelWriteRegister(BMA2x2_SLOPE_THRES_REG, 0x08);    //in +-2g range threshold = 3.91 mg * (value of BMA2x2_SLOPE_THRES_REG)  7.84 cm/s2
    AccelWriteRegister(BMA2x2_SLOPE_THRES_REG, 0x80);   //0.5 m/s2 
    delay_10us_x(10);
	AccelWriteRegister(BMA2x2_INTR_ENABLE1_REG, 0x0F); //enable any-motion interrupt in x, y or z axis
    delay_10us_x(10);
}
