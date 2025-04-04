/* spi Serial Peripheral Interface
 *	 7 segments display
 *  Master
 *	WvdE
 * 24 febr 2011
 */
/*
 * Project name		: Demo5_5 : spi - 7 segments display
 * Author			: Avans-TI, WvdE, JW
 * Revision History	: 20110228: - initial release;
 * Description		: This program sends data to 1 4-digit display with spi
 * Test configuration:
     MCU:             ATmega128
     Dev.Board:       BIGAVR6
     Oscillator:      External Clock 08.0000 MHz
     Ext. Modules:    Serial 7-seg display
     SW:              AVR-GCC
 * NOTES			: Turn ON switch 15, PB1/PB2/PB3 to MISO/MOSI/SCK
*/

#define F_CPU 10e6
#include <avr/io.h>
#include <util/delay.h>
#include "segment_display.h"


// wait(): busy waiting for 'ms' millisecond
// used library: util/delay.h
void wait(int ms)
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms(1);
	}
}

void spi_masterInit(void)
{
	DDR_SPI = 0xff;							// All pins output: MOSI, SCK, SS, SS_display as output
	DDR_SPI &= ~BIT(SPI_MISO);				// 	except: MISO input
	PORT_SPI |= BIT(SPI_SS);				// SS_ADC == 1: deselect slave
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);	// or: SPCR = 0b11010010;
											// Enable spi, MasterMode, Clock rate fck/64, bitrate=125kHz
											// Mode = 0: CPOL=0, CPPH=0;
}

// Write a byte from master to slave
void spi_write( unsigned char data )				
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete 
}

// Write a byte from master to slave and read a byte from slave
// nice to have; not used here
char spi_writeRead( unsigned char data )
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete 
	data = SPDR;							// New received data (eventually, MISO) in SPDR
	return data;							// Return received byte
}

// Select device on pinnumer PORTB
void spi_slaveSelect(unsigned char chipNumber)
{
	PORTB &= ~BIT(chipNumber);
}

// Deselect device on pinnumer PORTB
void spi_slaveDeSelect(unsigned char chipNumber)
{

	PORTB |= BIT(chipNumber);
}


// Initialize the driver chip (type MAX 7219)
void displayDriverInit() 
{
	spi_slaveSelect(0);				// Select display chip (MAX7219)
  	spi_write(0x09);      			// Register 09: Decode Mode
  	spi_write(0xFF);				// 	-> 1's = BCD mode for all digits
  	spi_slaveDeSelect(0);			// Deselect display chip

  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0A);      			// Register 0A: Intensity
  	spi_write(0x0F);    			//  -> Level 4 (in range [1..F]) // 04 changed to max intensity
  	spi_slaveDeSelect(0);			// Deselect display chip

  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0B);  				// Register 0B: Scan-limit
  	spi_write(0x03);   				// 	-> 1 = Display digits 0..1 //changed from 01 to 03
  	spi_slaveDeSelect(0);			// Deselect display chip

  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x01); 				// 	-> 1 = Normal operation
  	spi_slaveDeSelect(0);			// Deselect display chip
}

// Set display on ('normal operation')
void displayOn() 
{
  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x01); 				// 	-> 1 = Normal operation
  	spi_slaveDeSelect(0);			// Deselect display chip
}

// Set display off ('shut down')
void displayOff() 
{
  	spi_slaveSelect(0);				// Select display chip
  	spi_write(0x0C); 				// Register 0B: Shutdown register
  	spi_write(0x00); 				// 	-> 1 = Normal operation
  	spi_slaveDeSelect(0);			// Deselect display chip
}

// Write a word = address byte + data byte from master to slave
void spi_writeWord ( unsigned char adress, unsigned char data )
{
	spi_slaveSelect(0);				// Select dispaly chip
	spi_write(adress);      			
	spi_write(data);    			
	spi_slaveDeSelect(0);			// Deselect display chip
}

// Shows the integer value in the 7-segment display
void writeLedDisplay( int value )
{	
	int isNegative = 0;	
	
	// Skip values outside the displayable range (-999 to 9999)
	if(value >= 9999 || value <= -999){
		return;
	}
	
	if (value < 0 ) {		
		isNegative = 1;		// flag as negative
		value = -value;		// make the negative number positive
	}
	
	// Extract individual digits
	int t = value / 1000;	// thousands place
	value %= 1000;			// delete the thousands from value so you have only hundreds over

	int h = value / 100;	// hundreds place
	value %= 100;			// delete the hundreds from value so you have only tens over

	int e = value / 10;		// tens place
	value %= 10;			// delete the tens from value so you have only ones over

	int i = value;			// ones place
	
	 // if its negative add the minus in front
	 if (isNegative) {
		 spi_writeWord(4, 0x0A);	// 0x0A is "-"
		 } else {
		 spi_writeWord(4, t);		// display thousands digiy
	 }
	 
	 spi_writeWord(3, h);// display hundreds digit
	 spi_writeWord(2, e);// display tens digit
	 spi_writeWord(1, i);// display ones digit
}

void clear_display(){
	// clear display (all zero's)
	for (char i =1; i<=4; i++)
	{
		spi_slaveSelect(0); 		// Select display chip
		spi_write(i);  				// 	digit adress: (digit place)
		spi_write(0);				// 	digit value: 0
		spi_slaveDeSelect(0);		// Deselect display chip
	}
}

void setup_segment_display(){
	DDRB=0x01;					  	// Set PB0 pin as output for display select
	spi_masterInit();              	// Initialize spi module
	displayDriverInit();            // Initialize display chip
	clear_display();
}