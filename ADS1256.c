/*****************************************************************************
* | File      	:   ADS1256.c
* | Author      :   Waveshare team
* | Function    :   ADS1256 driver
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-03-25
* | Info        :
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "spi_if.h"
#include "gpio_if.h"
#include "delay_if.h"
#include <ADS1256.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <ringbuffer.h>

#define GPIO_FILE 0
#define SPI_FILE 0
uint8_t ScanMode = 0;

/******************************************************************************
function:   Module reset
parameter:
Info:
******************************************************************************/
static void ADS1256_reset(void)
{
	int fd = GPIO_FILE;
    gpio_pin_write(fd,DEV_RST_PIN, 1);
    delay_ms(200);
    gpio_pin_write(fd,DEV_RST_PIN, 0);
    delay_ms(200);
    gpio_pin_write(fd,DEV_RST_PIN, 1);
}

/******************************************************************************
function:   send command
parameter: 
        Cmd: command
Info:
******************************************************************************/
static void ADS1256_WriteCmd(uint8_t Cmd)
{
	int fd = GPIO_FILE;
    gpio_pin_write(fd,DEV_CS_PIN, 0);
    spi_write_byte(SPI_FILE,Cmd);
    gpio_pin_write(fd,DEV_CS_PIN, 1);
}

/******************************************************************************
function:   Write a data to the destination register
parameter: 
        Reg : Target register
        data: Written data
Info:
******************************************************************************/
static void ADS1256_WriteReg(uint8_t Reg, uint8_t data)
{
	int fd = GPIO_FILE;
    gpio_pin_write(fd,DEV_CS_PIN, 0);
    spi_write_byte(SPI_FILE,CMD_WREG | Reg);
    spi_write_byte(SPI_FILE,0x00);
    spi_write_byte(SPI_FILE,data);
    gpio_pin_write(fd,DEV_CS_PIN, 1);
}

/******************************************************************************
function:   Read a data from the destination register
parameter: 
        Reg : Target register
Info:
    Return the read data
******************************************************************************/
static uint8_t ADS1256_Read_data(uint8_t Reg)
{
    uint8_t temp = 0;
    int fd = GPIO_FILE;
    gpio_pin_write(fd,DEV_CS_PIN, 0);
    spi_write_byte(SPI_FILE,CMD_RREG | Reg);
    spi_write_byte(SPI_FILE,0x00);
    delay_ms(1);
    temp = spi_read_byte(SPI_FILE);
    gpio_pin_write(fd,DEV_CS_PIN, 1);
    return temp;
}

/******************************************************************************
function:   Waiting for a busy end
parameter: 
Info:
    Timeout indicates that the operation is not working properly.
******************************************************************************/
static void ADS1256_WaitDRDY(void)
{   
    double i = 0;
    for(i=0;i<4000000;i++){
        if(gpio_pin_read(GPIO_FILE,DEV_DRDY_PIN) == 0)
            break;
    }
    if(i >= 4000000){
       //printfTime Out ...\r\n");
    }
}

/******************************************************************************
function:  Read device ID
parameter: 
Info:
******************************************************************************/
uint8_t ADS1256_ReadChipID(void)
{
    uint8_t id;
    ADS1256_WaitDRDY();
    id = ADS1256_Read_data(REG_STATUS);
    return id>>4;
}

/******************************************************************************
function:  Configure ADC gain and sampling speed
parameter: 
    gain : Enumeration type gain
    drate: Enumeration type sampling speed
Info:
******************************************************************************/
void ADS1256_ConfigADC(ADS1256_GAIN gain, ADS1256_DRATE drate)
{
	int fd = GPIO_FILE;
    ADS1256_WaitDRDY();
    uint8_t buf[4] = {0,0,0,0};
    buf[0] = (0<<3) | (1<<2) | (0<<1);
    buf[1] = 0x08;
    buf[2] = (0<<5) | (0<<3) | (gain<<0);
    buf[3] = ADS1256_DRATE_E[drate];
    gpio_pin_write(fd,DEV_CS_PIN, 0);
    spi_write_byte(SPI_FILE,CMD_WREG | 0);
    spi_write_byte(SPI_FILE,0x03);
    
    spi_write_byte(SPI_FILE,buf[0]);
    spi_write_byte(SPI_FILE,buf[1]);
    spi_write_byte(SPI_FILE,buf[2]);
    spi_write_byte(SPI_FILE,buf[3]);
    gpio_pin_write(fd,DEV_CS_PIN, 1);
    delay_ms(1);
}

/******************************************************************************
function:  Set the channel to be read
parameter: 
    Channal : Set channel number
Info:
******************************************************************************/
static void ADS1256_SetChannal(uint8_t Channal)
{
    if(Channal > 7){
        return ;
    }
    ADS1256_WriteReg(REG_MUX, (Channal<<4) | (1<<3));
} 

void ADS1256_SetDiffChannal(uint8_t Channal)
{
    if (Channal == 0){
        ADS1256_WriteReg(REG_MUX, (0 << 4) | 1);	//DiffChannal  AIN0-AIN1
    }
    else if(Channal == 1){
        ADS1256_WriteReg(REG_MUX, (2 << 4) | 3);	//DiffChannal   AIN2-AIN3
    }
    else if(Channal == 2){
        ADS1256_WriteReg(REG_MUX, (4 << 4) | 5); 	//DiffChannal    AIN4-AIN5
    }
    else if(Channal == 3){
        ADS1256_WriteReg(REG_MUX, (6 << 4) | 7); 	//DiffChannal   AIN6-AIN7
    }
}

/******************************************************************************
function:  Setting mode
parameter: 
    Mode : 0 Single-ended input
           1 channel1 Differential input
Info:
******************************************************************************/
void ADS1256_SetMode(uint8_t Mode)
{
    if(Mode == 0){
        ScanMode = 0;
    }
    else{ 
        ScanMode = 1;
    }
}

/******************************************************************************
function:  Device initialization
parameter: 
Info:
******************************************************************************/
uint8_t ADS1256_init(void)
{
    ADS1256_reset();
    if(ADS1256_ReadChipID() == 3){
        //printfID Read success \r\n");
    }
    else{
        //printfID Read failed \r\n");
        return 1;
    }
    ADS1256_ConfigADC(ADS1256_GAIN_1, ADS1256_30000SPS);
    
    
    // printf("ADD 0x02  = 0x%x\r\n",ADS1256_Read_data(0x02));
    // printf("ADD 0x03  = 0x%x\r\n",ADS1256_Read_data(0x03));
    return 0;
}

/******************************************************************************
function:  Read ADC data
parameter: 
Info:
******************************************************************************/
static uint32_t ADS1256_Read_ADC_Data(void)
{
	uint32_t read = 0;
    uint8_t buf[3] = {0,0,0};
    int fd = GPIO_FILE;
    ADS1256_WaitDRDY();
    delay_ms(1);
    gpio_pin_write(fd,DEV_CS_PIN, 0);
    spi_write_byte(SPI_FILE,CMD_RDATA);
    delay_ms(1);
    buf[0] = spi_read_byte(SPI_FILE);
    buf[1] = spi_read_byte(SPI_FILE);
    buf[2] = spi_read_byte(SPI_FILE);
    gpio_pin_write(fd,DEV_CS_PIN, 1);
    read = ((uint32_t)buf[0] << 16) & 0x00FF0000;
    read |= ((uint32_t)buf[1] << 8);  /* Pay attention to It is wrong   read |= (buf[1] << 8) */
    read |= buf[2];
    //printf("%d  %d  %d \r\n",buf[0],buf[1],buf[2]);
    if (read & 0x800000)
        read &= 0xFF000000;
    return read;
}

/******************************************************************************
function:  Read ADC specified channel data
parameter: 
    Channel: Channel number
Info:
******************************************************************************/
uint32_t ADS1256_GetChannalValue(uint8_t Channel)
{
	uint32_t Value = 0;
	//printfGet Data channel %d\n",Channel);
    while(gpio_pin_read(GPIO_FILE,DEV_DRDY_PIN) == 1);
    if(ScanMode == 0){// 0  Single-ended input  8 channel1 Differential input  4 channe 
        if(Channel>=8){
            return 0;
        }
        ADS1256_SetChannal(Channel);
        ADS1256_WriteCmd(CMD_SYNC);
        ADS1256_WriteCmd(CMD_WAKEUP);
        Value = ADS1256_Read_ADC_Data();
    }
    else{
        if(Channel>=4){
            return 0;
        }
        ADS1256_SetDiffChannal(Channel);
        ADS1256_WriteCmd(CMD_SYNC);
        ADS1256_WriteCmd(CMD_WAKEUP);
        Value = ADS1256_Read_ADC_Data();
    }
    return Value;
}

/******************************************************************************
function:  Read data from all channels
parameter: 
    ADC_Value : ADC Value
Info:
******************************************************************************/
void ADS1256_GetAll(uint32_t *ADC_Value)
{
    uint8_t i;
    for(i = 0; i<8; i++){
        ADC_Value[i] = ADS1256_GetChannalValue(i);
    }
}

#define MAX_SAMPLE 10000
#define MAX_CHANNEL 8
uint32_t ADC_value[MAX_CHANNEL][MAX_SAMPLE];
Ringbuff_t ringbuffer[MAX_CHANNEL];

char buff_write[1024];

volatile 	pthread_t thread_id;
pthread_t write_file_thread;

static void* adc_scan_thread(void* argp);
static void* write_file_handle(void *arg);

static void* adc_scan_thread(void* argp){
	uint32_t adc_value[8];
	remove("adc_log.txt");
	while(1){
		ADS1256_GetAll(adc_value);
		for(uint16_t i = 0;i<MAX_CHANNEL;i++){
			ringbuff_put_head(&ringbuffer[i],adc_value[i]);
		}
	    pthread_create((pthread_t *)&write_file_thread, NULL, write_file_handle, NULL);
	    pthread_join(write_file_thread, NULL);
	}
	return NULL;
}
static void* write_file_handle(void *arg){
	uint32_t adc_value[8];
	FILE *file = fopen("adc_log.txt", "a");

	for (uint16_t i = 0; i < MAX_CHANNEL; i++) {
		if (ringbuff_available(&ringbuffer[i]) > 0)
			adc_value[i] = ringbuff_pop_tail(&ringbuffer[i]);
	}
	sprintf(buff_write, "%u,%u,%u,%u,%u,%u,%u,%u\n", adc_value[0], adc_value[1],
			adc_value[2], adc_value[3], adc_value[4], adc_value[5],
			adc_value[6], adc_value[7]);
	fwrite(buff_write, sizeof(char), strlen(buff_write), file);
	fclose(file);
	return NULL;
}

void ADS125x_Start_thread(void){
	ADS1256_init();
	ADS1256_SetMode(0);
	for(uint16_t i = 0;i<MAX_CHANNEL;i++){
		ringbuff_init(&ringbuffer[i], ADC_value[i], MAX_SAMPLE);
	}
	//printfCreate Scan Thread\n");
    pthread_create((pthread_t *)&thread_id, NULL, adc_scan_thread, NULL);
    pthread_join(thread_id, NULL);
}
void ADS125x_Stop_thread(void){
	pthread_cancel(thread_id);
}
