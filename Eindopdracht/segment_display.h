/*
 * segment_display.h
 *
 * Created: 4/4/2025 3:20:05 PM
 *  Author: zengi
 */ 
#ifndef SEGMENT_DISPLAY_H_
#define SEGMENT_DISPLAY_H_

#define BIT(x)		( 1<<x )
#define DDR_SPI		DDRB					// spi Data direction register
#define PORT_SPI	PORTB					// spi Output register
#define SPI_SCK		1						// PB1: spi Pin System Clock
#define SPI_MOSI	2						// PB2: spi Pin MOSI
#define SPI_MISO	3						// PB3: spi Pin MISO
#define SPI_SS		0						// PB0: spi Pin Slave Select

void wait(int ms);
void spi_masterInit(void);
void spi_write( unsigned char data );
char spi_writeRead( unsigned char data );
void spi_slaveSelect(unsigned char chipNumber);
void spi_slaveDeSelect(unsigned char chipNumber);
void displayDriverInit();
void displayOn();
void displayOff();
void spi_writeWord ( unsigned char address, unsigned char data );
void writeLedDisplay( int value );
void clear_display();
void setup_segment_display();

#endif /* SEGMENT_DISPLAY_H_ */