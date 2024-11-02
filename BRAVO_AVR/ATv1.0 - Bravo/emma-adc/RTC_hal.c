/*
 * twi_hal.c
 *
 * Created: 2020/11/01 18:07:03
 *  Author: DevilBinder
 */ 
#include "RTC_hal.h"
#include "app_utils.h"

static char print_buffer[64] = {0};
// -------------------{ SEG,  MIN, HORA, diaSem,  DIA,  MES,  AÑO}; Día 1: lunes
uint8_t rtc_data[7] = {0x10, 0x10, 0x13,   0x06, 0x02, 0x11, 0x24}; 
//DARLE GO EN EL MINUTO "MIN" CON 00 SEGUNDOS 
//SEG es 0x29 porque demora 29 segundos en grabar
//----------------- 

static uint8_t twi_start(void)
{
	uint16_t i = 0;
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while( !(TWCR & (1<<TWINT)) )
	{
		_delay_us(1);
		i++;
		if(i >= TWI_TIMEOUT) return TWI_ERROR_START;
	}
	return TWI_OK;
}

static void twi_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

static uint8_t twi_restart(void)
{
	uint16_t i = 0;
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while( !(TWCR & (1<<TWINT)) )
	{
		i++;
		if(i >= TWI_TIMEOUT) return TWI_ERROR_START;
	}
	return TWI_OK;
}

static uint8_t twi_addr_write_ack(void)
{
	uint16_t i = 0;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while( !(TWCR & (1<<TWINT)) )
	{
		i++;
		if(i >= TWI_TIMEOUT) return TWI_ERROR_START;
	}
	return TWI_OK;
}


static uint8_t twi_data_write_ack(void)
{
	uint16_t i = 0;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while( !(TWCR & (1<<TWINT)) )
	{
		i++;
		if(i >= TWI_TIMEOUT) return TWI_ERROR_START;
	}
	return TWI_OK;
}


static uint8_t twi_addr_read_ack(void)
{
	uint16_t i = 0;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while( !(TWCR & (1<<TWINT)) )
	{
		i++;
		if(i >= TWI_TIMEOUT) return TWI_ERROR_START;
	}
	return TWI_OK;
}


static uint8_t twi_data_read_ack(uint8_t ack)
{
	uint16_t i = 0;
	if(ack != 0){
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA) ; //| (1 << TWIE) ;
		while( !(TWCR & (1<<TWINT)) )
		{
			i++;
			if(i >= TWI_TIMEOUT) return TWI_ERROR_START;
		}
	}
	else
	{
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1<<TWINT))) 
		{
			i++;
			if(i >= TWI_TIMEOUT) return TWI_ERROR_START;
		}
		
	}
	return TWI_OK;
}

uint8_t RTC_read(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len){
	
	uint16_t i = 0;
	uint8_t err = TWI_OK;
	
	err = twi_start();
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	TWDR = (addr << 1) | 0;
	
	err = twi_addr_write_ack();
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	TWDR = reg;
	err = twi_data_write_ack();
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	err = twi_restart();
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	TWDR = (addr << 1) | 1;

	err = twi_addr_read_ack();
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	for(i = 0; i < (len - 1);i++)
	{
		err = twi_data_read_ack(1);
		if(err != TWI_OK)
		{
			twi_stop();
			return err;
		}
		data[i] = TWDR;
	}
	
	err = twi_data_read_ack(0);
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	data[i] = TWDR;
	
	twi_stop();
	
	return err;
}


uint8_t RTC_write(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len)
{
	uint16_t i = 0;
	uint8_t err = TWI_OK;
	err = twi_start();
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	TWDR = (addr << 1) | 0;
	
	err = twi_addr_write_ack();
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	TWDR = reg;
	err = twi_data_write_ack();
	if(err != TWI_OK)
	{
		twi_stop();
		return err;
	}
	
	for(i = 0; i < len;i++)
	{
		TWDR = data[i];
		err = twi_data_write_ack();
		if(err != TWI_OK){
			twi_stop();
			return err;
		}
	}
	
	twi_stop();
	
	return err;
}

//modificar """""""rtc_data""""""" para ajustar fecha y hora
void configuraRTC()
{
	uint8_t err = RTC_write(RTC_ADDR,0x00,rtc_data,sizeof(rtc_data));
	if(err != TWI_OK)
	{
		memset(print_buffer,0,sizeof(print_buffer));
		sprintf(print_buffer,"%d error %d\r\n\n", __LINE__,err);
		#ifdef UART_PC
			uart_send_string((uint8_t*)print_buffer);
		#endif
		while(1)
		{
			#ifdef UART_PC
				printf("rtc error...\r\n");
			#endif
			_delay_ms(200);
		}
	}
}

void leeRTC ()
{	
	uint8_t err = RTC_read(RTC_ADDR,0x00,rtc_data,sizeof(rtc_data));
	if(err != TWI_OK)
	{
		while(1)
		{
			#ifdef UART_PC
				printf("rtc read error...");
			#endif
			_delay_ms(200);
		}
	}
	
	memset(print_buffer,0,sizeof(print_buffer));
	sprintf(print_buffer,"%02x/%02x/20%02x %02x:%02x:%02x",
	rtc_data[4], // día
	rtc_data[5], // mes
	rtc_data[6], // año
	rtc_data[2], // horas
	rtc_data[1], // minutos
	rtc_data[0]  // segundos
	);
	/*#ifdef UART_PC
		uart_send_string((uint8_t*)print_buffer);
	#endif*/
	
	//pSEGUNDOS   = rtc_data[0]-6*(print_buffer[17]-48); //resto decenas de SEG
	pMINUTOS    = rtc_data[1]-6*(print_buffer[14]-48); 
	pHORAS	    = rtc_data[2]-6*(print_buffer[11]-48);
	pDIA_SEMANA = rtc_data[3];
	pANIO	    = rtc_data[6]-6*(print_buffer[8]-48);
	pMES	    = rtc_data[5]-6*(print_buffer[3]-48);
	pDIA	    = rtc_data[4]-6*(print_buffer[0]-48);
			
	//#ifdef UART_PC
		//printf("%d%d%d,%d:%d",pANIO, pMES, pDIA, pHORAS,pMINUTOS);
	//#endif
	sprintf(TxBuff,"%d%d%d,%d:%d",pANIO, pMES, pDIA, pHORAS,pMINUTOS);
	mi_puts(TxBuff);
}