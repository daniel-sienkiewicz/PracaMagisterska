/**
****************************************************************************
* Sterowanie wyswietlaczem FT800 na podstawie AN320 FTDI
*
* typy danych w zaleznosci od kompilatora
* char 8 bitow	-> uchar8
* int 16 bitow 	-> uint16
* long 32 bity	-> uint32
****************************************************************************/
//#define PIC		              // Wybrac gdy uzywamy PIC

// Set LCD display resolution here
#define LCD_QVGA	              // QVGA  = 320 x 240 (VM800B/C 3.5")
//#define LCD_WQVGA		      // WQVGA = 480 x 272 (VM800B/C 4.3" and 5.0")

#include "FT800.h"
#include <stdio.h>

#define xSDI 8     // Linia SDI intrefejsu SPI wejście do Galileo
#define xSDO 9     // Linia SDI intrefejsu SPI wyjście od Galileo
#define xclock 10  // Linia zegarowa wyścia od Galileo
#define xPD 11     // Linia PD wyświatlacza, wyjście od Galileo
#define xCS 12     // Linia Chip select wyświetlacza, wyjście od Galielo
short int screenNR = 1;

#ifdef PIC
#pragma config FOSC = INTRCCLK  // Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#define _XTAL_FREQ 8000000      // Required for _delay() function, internal OSC Max
#endif

// FT800 Chip Commands - use with cmdWrite
#define FT800_ACTIVE		0x00		// Initializes FT800
#define FT800_STANDBY		0x41		// Place FT800 in Standby (clk running)
#define FT800_SLEEP		0x42	        // Place FT800 in Sleep (clk off)
#define FT800_PWRDOWN		0x50		// Place FT800 in Power Down (core off)
#define FT800_CLKEXT		0x44		// Select external clock source
#define FT800_CLK48M		0x62		// Select 48MHz PLL
#define FT800_CLK36M		0x61		// Select 36MHz PLL
#define FT800_CORERST		0x68		// Reset core - all registers default
#define FT800_GPUACTIVE	0x40

// FT800 Memory Commands - use with ft800memWritexx and ft800memReadxx
#define MEM_WRITE		0x80		// FT800 Host Memory Write
#define MEM_READ		0x00		// FT800 Host Memory Read

// Colors - fully saturated colors defined here
#define RED				0xFF0000		// Red
#define GREEN				0x00FF00		// Green
#define BLUE				0x0000FF		// Blue
#define WHITE				0xFFFFFF		// White
#define BLACK				0x000000		// Black

//Global Variables
// LCD display parameters
unsigned int lcdWidth;					// Active width of LCD display
unsigned int lcdHeight;					// Active height of LCD display
unsigned int lcdHcycle;					// Total number of clocks per line
unsigned int lcdHoffset;				// Start of active line
unsigned int lcdHsync0;					// Start of horizontal sync pulse
unsigned int lcdHsync1;					// End of horizontal sync pulse
unsigned int lcdVcycle;					// Total number of lines per screen
unsigned int lcdVoffset;				// Start of active screen
unsigned int lcdVsync0;					// Start of vertical sync pulse
unsigned int lcdVsync1;					// End of vertical sync pulse
unsigned char lcdPclk;					// Pixel Clock
unsigned char lcdSwizzle;				// Define RGB output pins
unsigned char lcdPclkpol;				// Define active edge of PCLK

unsigned long ramDisplayList=RAM_DL;		        // Set beginning of display list memory
unsigned long ramCommandBuffer=RAM_CMD;	                // Set beginning of graphics command memory

unsigned int cmdBufferRd;				// Used to navigate command ring buffer
unsigned int cmdBufferWr=0x0000;	                // Used to navigate command ring buffer
unsigned int cmdOffset=0x0000;		                // Used to navigate command rung buffer
unsigned char ft800Gpio;				// Used for FT800 GPIO register

/**
******************************************************************************
*
* @param us pauza w us
*****************************************************************************/
void delay_us(int us){
  //delayMicroseconds(us);
}

/**
******************************************************************************
*
* @param ms pauza w ms
*****************************************************************************/
void delay_ms(int ms){
  //delay(ms);
}

/**
******************************************************************************
*
* @param data dane do wyslania
*****************************************************************************/
void sendData(int data){
  int i;
  for(i = 0x80; i; i >>= 1){
    digitalWrite(xSDO, data & i);
    delay_us(2);
    digitalWrite(xclock, HIGH);
    delay_us(2);
    digitalWrite(xclock, LOW);
    delay_us(2);
  }
}

/**
******************************************************************************
*
*****************************************************************************/
unsigned char getData(){
  int i, j;
  unsigned char result = 0;
  for(i = 0x80; i; i >>= 1){
    if(digitalRead(xSDI)){
      result |= i;
    }
    digitalWrite(xclock, HIGH);
    delay_ms(2);
    digitalWrite(xclock, LOW);
  }	
  return result;
}

/******************************************************************************
* Function:        void ft800memWritexx(ftAddress, ftDataxx, ftLength)
* PreCondition:    None
* Input:           ftAddress = FT800 memory space address
*                  ftDataxx = a byte, int or long to send
* Output:          None
* Side Effects:    None
* Overview:        Writes FT800 internal address space
* Note:            "xx" is one of 8, 16 or 32
*****************************************************************************/

/**
******************************************************************************
*
* @param ftAddress FT800 memory space address (24 bits)
* @param ftData8 a byte to send
*****************************************************************************/
void ft800memWrite8(unsigned long ftAddress, unsigned char ftData8){
#ifdef PIC
  RC6=0;                    			    // Set CS# low

  delay_us(2);
  SSPBUF=((char)(ftAddress>>16)|MEM_WRITE);         // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress>>8));		    // Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress);			    // Send low address byte
  while(!SSPSTATbits.BF);
  SSPBUF=ftData8;				    // Send data byte
  while(!SSPSTATbits.BF);
  delay_us(2);

  RC6=1;                                            // Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_WRITE));
  sendData((ftAddress >> 8));
  sendData(ftAddress);
  sendData(ftData8);
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
}

/**
******************************************************************************
*
* @param ftAddress FT800 memory space address (24 bits)
* @param ftData16 a word (16 bits) to send
*****************************************************************************/
void ft800memWrite16(unsigned long ftAddress, unsigned int ftData16){
#ifdef PIC
  RC6=0;					      // Set CS# low

  SSPBUF=((char)(ftAddress>>16)|MEM_WRITE);           // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress>>8));		      // Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress));			      // Send low address byte
  while(!SSPSTATbits.BF);

  SSPBUF=((char)(ftData16));			      // Send data low byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftData16>>8));			      // Send data high byte
  while(!SSPSTATbits.BF);
  delay_us(2);

  RC6=1;                                             // Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_WRITE));
  sendData((ftAddress >> 8));
  sendData((ftAddress));
	
  sendData(ftData16);
  sendData((ftData16 >> 8));
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
}

/**
******************************************************************************
*
* @param ftAddress FT800 memory space address (24 bits)
* @param ftData32 long (32 bits) to send
*****************************************************************************/
void ft800memWrite32(unsigned long ftAddress, unsigned long ftData32){
#ifdef PIC
  RC6=0;   					        // Set CS# low

  SSPBUF=((char)(ftAddress>>16)|MEM_WRITE);             // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress>>8));			// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftAddress));				// Send low address byte
  while(!SSPSTATbits.BF);

  SSPBUF=((char)(ftData32));				// Send data low byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftData32>>8)); 			// Send data middle-low byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftData32>>16));			// Send data middle-high byte
  while(!SSPSTATbits.BF);
  SSPBUF=((char)(ftData32>>24));			// Send data high byte
  while(!SSPSTATbits.BF);
  delay_us(2);
  RC6=1;						// Set CS# high

#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_WRITE));
  sendData((ftAddress >> 8));
  sendData(ftAddress);
	
  sendData(ftData32);
  sendData((ftData32 >> 8));
  sendData((ftData32 >> 16));
  sendData((ftData32 >> 24));
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
}

/******************************************************************************
 * Function:        unsigned char ft800memReadxx(ftAddress, ftLength)
 * PreCondition:    None
 * Input:           ftAddress = FT800 memory space address
 * Output:          ftDataxx (byte, int or long)
 * Side Effects:    None
 * Overview:        Reads FT800 internal address space
 * Note:            "xx" is one of 8, 16 or 32
 *****************************************************************************/
/**
******************************************************************************
*
* @param ftAddress FT800 memory space address (24 bits)
* @retval Read byte
*****************************************************************************/
unsigned char ft800memRead8(unsigned long ftAddress){
  unsigned char ftData8=ZERO;
#ifdef PIC
  RC6=0;						// Set CS# low

  SSPBUF=(char)(ftAddress>>16)|MEM_READ;	        // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress>>8);				// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress);				// Send low address byte
  while(!SSPSTATbits.BF);
  SSPBUF=ZERO;
  while(!SSPSTATbits.BF);
  SSPBUF=ZERO;					        // Send dummy byte
  while((SSPSTAT&(1<<0))==0);                           //to samo co while(!SSPSTATbits.BF) ??
  ftData8=SSPBUF;
  RC6=1;						// Set CS# high

#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_READ)&255);
  sendData((ftAddress >> 8)&255);
  sendData(ftAddress&255);
  sendData(0);
  delay_us(2);
        
  ftData8 = getData();
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
  return(ftData8);			                // Return byte read
}

/**
******************************************************************************
*
* @param ftAddress FT800 memory space address (24 bits)
* @retval Read word (16 bits)
*****************************************************************************/
unsigned char ft800memRead16(unsigned long ftAddress){
  unsigned int ftData16, tempData[2];
  ftData16=0x0000;
#ifdef PIC
  RC6=0;						// Set CS# low

  SSPBUF=(char)(ftAddress>>16)|MEM_READ;	        // Send Memory Write plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress>>8);				// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress);				// Send low address byte
  while(!SSPSTATbits.BF);    
  SSPBUF=ZERO;  					// Send dummy byte
  SSPBUF=ZERO;					        // Send dummy byte
  while(!SSPSTATbits.BF);
  SSPBUF=ZERO;					        // Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[0]=SSPBUF;
  ftData16=(tempData[0])|(ftData16);

  SSPBUF=ZERO;						// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[1]=SSPBUF;
  ftData16=(ftData16)|(tempData[1]<<8);

  delay_us(2);						// Read data byte
  RC6=1;						// Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_READ)&255);
  sendData((ftAddress >> 8)&255);
  sendData(ftAddress&255);
  sendData(0);
  delay_us(2);
        
  tempData[0] = getData();
  ftData16 = (tempData[0])|(ftData16);
  sendData(0);
  delay_us(2);
        
  tempData[1] = getData();
  ftData16 = (ftData16) | (tempData[1] << 8);
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
  return(ftData16);					// Return 16 bits
}

/**
******************************************************************************
*
* @param ftAddress FT800 memory space address (24 bits)
* @retval Read long (32 bits)
*****************************************************************************/
unsigned long ft800memRead32(unsigned long ftAddress){
  unsigned long ftData32, tempData[4];

  ftData32=0x00000000;

#ifdef PIC
  RC6=0;						// Set CS# low

  SSPBUF=(char)(ftAddress>>16)|MEM_READ;	        // Send Memory Read plus high address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress>>8);				// Send middle address byte
  while(!SSPSTATbits.BF);
  SSPBUF=(char)(ftAddress);				// Send low address byte
  while(!SSPSTATbits.BF);
 SSPBUF=ZERO;                         		        //send dummy byte
  delay_us(2);

  SSPBUF=ZERO;  					// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[3]=SSPBUF;					// Read data byte
  ftData32=tempData[3]|ftData32;
  SSPBUF=ZERO;  					// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[2]=SSPBUF;					// Read data byte
  ftData32=(tempData[2]<<8)|ftData32;
  SSPBUF=ZERO;  					// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[1]=SSPBUF;                                   // Read data byte
  ftData32=(tempData[1]<<16)|ftData32;
  SSPBUF=ZERO;  					// Send dummy byte
  while(!SSPSTATbits.BF);
  tempData[0]=SSPBUF;                                   // Read data byte
  ftData32=ftData32|(tempData[0]<<24);

  delay_us(2);
  RC6=1;						// Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(((ftAddress >> 16) | MEM_READ));
  sendData((ftAddress >> 8));
  sendData(ftAddress);
  sendData(0);
  delay_us(2);
	
  tempData[3] = getData();
  ftData32 = tempData[3] | ftData32;
  sendData(0);
  delay_us(2);
        
  tempData[2] = getData();
  ftData32 = (tempData[2] << 8) | ftData32;
  sendData(0);
  delay_us(2);
        
  tempData[1] = getData();
  ftData32 = (tempData[1] << 16) | ftData32;
  sendData(0);
  delay_us(2);
        
  tempData[0] = getData();
  ftData32 = ftData32 | (tempData[0] << 24);
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
  return(ftData32);
}

/******************************************************************************
* Function:        void incCMDOffset(currentOffset, commandSize)
* PreCondition:    None
*                    starting a command list
* Input:           currentOffset = graphics processor command list pointer
*                  commandSize = number of bytes to increment the offset
* Output:          newOffset = the new ring buffer pointer after adding the command
* Side Effects:    None
* Overview:        Adds commandSize to the currentOffset.
*                  Checks for 4K ring-buffer offset roll-over
* Note:            None
*****************************************************************************/
unsigned int incCMDOffset(unsigned int currentOffset, unsigned char commandSize){
  unsigned int newOffset;								// used to hold new offset

  newOffset=currentOffset+commandSize;	                                        // Calculate new offset
  if(newOffset>4095){								// If new offset past boundary...
    newOffset=(newOffset-4096);				                // ... roll over pointer
  }
  return(newOffset);								// Return new offset
}

/******************************************************************************
 * Function:        void ft800cmdWrite(ftCommand)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        Sends FT800 command
 * Note:            None
 *****************************************************************************/
void ft800cmdWrite(unsigned char ftCommand){
#ifdef PIC
  RC6=0;							// Set CS# low
  
  SSPBUF=ftCommand;					        // Send command
  while(!SSPSTATbits.BF); 
  SSPBUF=ZERO;
  while(!SSPSTATbits.BF);		                        // Commands consist of two more zero bytes
  SSPBUF=ZERO;
  while(!SSPSTATbits.BF);		                        // Send last zero byte

  RC6=1;							// Set CS# high
#else
  digitalWrite(xCS, LOW);
  delay_us(2);
  sendData(ftCommand);
  delay_us(2);
  sendData(0);
  delay_us(2);
  sendData(0);
  delay_us(2);
  digitalWrite(xCS, HIGH);
#endif
}

/**
******************************************************************************
*
*****************************************************************************/
void setup(void){
  unsigned char duty;
  Serial.begin(9600);
  Serial.println("Poczatek inicjalizacji");
#ifdef PIC
//initial PIC SPI configuration
  SSPEN=0;           	    // diable SSP module to begin configuration
  SSPSTAT=0b01000000;       // 0 - BF<0>, Buffer Full Status bit, SSPBUFF is empty
                            // 1 - CKE<6>, CKP = 1, Data on falling edge of SCK
                            // 0 - SMP<7>, SPI Master Mode, Input data sample in middle of data output time
  SSPCON=0b00000000;        // 0000 - SSPM<3:0>, SPI Master mode, clock = FOSC/4 10MHz.
                            // 1 - CKP<4>, Clock Polarity Select Bit, Idle high state
                            // 1 - SSPEN<5>, Enable serial port and config SCK, SDO, SDI
                            // 0 - SSPOV<6>, Reset Synchronous Serial Port OVerflow, before sending
                            // 0 - WCOL<7>, Clear, No collisions
  SSPEN=1;                  // re-enable SSP module
#else
  pinMode(xSDI, INPUT);
  pinMode(xSDO, OUTPUT);
  pinMode(xclock, OUTPUT);
  pinMode(xPD, OUTPUT);
  pinMode(xCS, OUTPUT);
#endif

// LCD display parameters
#ifdef LCD_QVGA			// QVGA display parameters
  lcdWidth	 = 320;		// Active width of LCD display
  lcdHeight  = 240;		// Active height of LCD display
  lcdHcycle  = 408;		// Total number of clocks per line
  lcdHoffset = 70;		// Start of active line
  lcdHsync0  = 0;		// Start of horizontal sync pulse
  lcdHsync1  = 10;		// End of horizontal sync pulse
  lcdVcycle  = 263;		// Total number of lines per screen
  lcdVoffset = 13;		// Start of active screen
  lcdVsync0  = 0;		// Start of vertical sync pulse
  lcdVsync1  = 2;		// End of vertical sync pulse
  lcdPclk    = 8;		// Pixel Clock
  lcdSwizzle = 2;		// Define RGB output pins
  lcdPclkpol = 0;		// Define active edge of PCLK
#endif

#ifdef LCD_WQVGA		// WQVGA display parameters
  lcdWidth   = 480;		// Active width of LCD display
  lcdHeight  = 272;		// Active height of LCD display
  lcdHcycle  = 548;		// Total number of clocks per line
  lcdHoffset = 43;		// Start of active line
  lcdHsync0  = 0;		// Start of horizontal sync pulse
  lcdHsync1  = 41;		// End of horizontal sync pulse
  lcdVcycle  = 292;		// Total number of lines per screen
  lcdVoffset = 12;		// Start of active screen
  lcdVsync0  = 0;		// Start of vertical sync pulse
  lcdVsync1  = 10;		// End of vertical sync pulse
  lcdPclk    = 5;		// Pixel Clock
  lcdSwizzle = 0;		// Define RGB output pins
  lcdPclkpol = 1;		// Define active edge of PCLK
#endif

//wake up FT800
#ifdef PIC
  RC7=0;				// set PD LOW
  delay_ms(20);
  RC7=1;				// set PD high
  delay_ms(20);
#else
  digitalWrite(xPD, LOW);
  delay_ms(2);
  digitalWrite(xPD, HIGH);
  delay_ms(2);
#endif
  ft800cmdWrite(FT800_ACTIVE);    	// Start FT800
  delay_ms(5);			        // Give some time to process
  
  ft800cmdWrite(FT800_CLKEXT);	        // Set FT800 for external clock
  delay_ms(5);			        //Give some time to process

  ft800cmdWrite(FT800_CLK48M);	        // Set FT800 for 48MHz PLL
  delay_ms(5);

  ft800cmdWrite(FT800_CORERST);	        // Set FT800 for 48MHz PLL
  delay_ms(5);

  ft800cmdWrite(FT800_GPUACTIVE);	// Set FT800 for 48MHz PLL
  delay_ms(5);

  if(ft800memRead8(REG_ID)!=0x7C){      // Read ID register - is it 0x7C?
    Serial.print("Cos nie tak: ");
    Serial.println(ft800memRead8(REG_ID));
    return;
  }
  Serial.println("OK");
  delay_us(2);
          
  ft800memWrite8(REG_PCLK, ZERO);		// Set PCLK to zero - don't clock the LCD until later
  ft800memWrite8(REG_PWM_DUTY, ZERO);		// Turn off backlight

  // Initialize Display
  ft800memWrite16(REG_HSIZE,  lcdWidth);	// active display width
  ft800memWrite16(REG_HCYCLE, lcdHcycle);	// total number of clocks per line, incl front/back porch
  ft800memWrite16(REG_HOFFSET,lcdHoffset);	// start of active line
  ft800memWrite16(REG_HSYNC0, lcdHsync0);	// start of horizontal sync pulse
  ft800memWrite16(REG_HSYNC1, lcdHsync1);	// end of horizontal sync pulse
  ft800memWrite16(REG_VSIZE,  lcdHeight);	// active display height
  ft800memWrite16(REG_VCYCLE, lcdVcycle);	// total number of lines per screen, incl pre/post
  ft800memWrite16(REG_VOFFSET,lcdVoffset);	// start of active screen
  ft800memWrite16(REG_VSYNC0, lcdVsync0);	// start of vertical sync pulse
  ft800memWrite16(REG_VSYNC1, lcdVsync1);	// end of vertical sync pulse
  ft800memWrite8(REG_SWIZZLE, lcdSwizzle);	// FT800 output to LCD - pin order
  ft800memWrite8(REG_PCLK_POL,lcdPclkpol);	// LCD data is clocked in on this PCLK edge
  // Don't set PCLK yet - wait for just after the first display list
  // End of Initialize Display

  // Configure Touch and Audio - not used in this example, so disable both
  //ft800memWrite8(REG_TOUCH_MODE, 0x03);		
  ft800memWrite16(REG_TOUCH_RZTHRESH, 1200);    // Eliminate any false touches

  ft800memWrite8(REG_VOL_PB, ZERO);		// turn recorded audio volume down
  ft800memWrite8(REG_VOL_SOUND, ZERO);		// turn synthesizer volume down
  ft800memWrite16(REG_SOUND, 0x6000);		// set synthesizer to mute
  // End of Configure Touch and Audio

  // Write Initial Display List & Enable Display
  ramDisplayList=RAM_DL;			  // start of Display List
  ft800memWrite32(ramDisplayList, DL_CLEAR_RGB);  // Clear Color RGB 00000010 RRRRRRRR GGGGGGGG BBBBBBBB  (R/G/B = Colour values) default zero / black
  ramDisplayList+=4;															// point to next location
  ft800memWrite32(ramDisplayList, (DL_CLEAR|CLR_COL|CLR_STN|CLR_TAG));	// Clear 00100110 -------- -------- -----CST  (C/S/T define which parameters to clear)
  ramDisplayList+=4;															// point to next location
  ft800memWrite32(ramDisplayList, DL_DISPLAY);		// DISPLAY command 00000000 00000000 00000000 00000000 (end of display list)

  ft800memWrite32(REG_DLSWAP, DLSWAP_FRAME);		// 00000000 00000000 00000000 000000SS  (SS bits define when render occurs)
  
  // Nothing is being displayed yet... the pixel clock is still 0x00
  ramDisplayList=RAM_DL;				// Reset Display List pointer for next list

  ft800Gpio=ft800memRead8(REG_GPIO);			// Read the FT800 GPIO register for a read/modify/write operation
  ft800Gpio=ft800Gpio|0x80;				// set bit 7 of FT800 GPIO register (DISP) - others are inputs
  ft800memWrite8(REG_GPIO, ft800Gpio);			// Enable the DISP signal to the LCD panel
  ft800memWrite8(REG_PCLK, lcdPclk);			// Now start clocking data to the LCD panel

  for(duty=0; duty<127; duty++){
    ft800memWrite8(REG_PWM_DUTY,duty);			// Turn on backlight - ramp up slowly to full brighness
    delay_ms(10);
  }
  Serial.println("Koniec inicjalizacji");
  initScreen();
}

void initScreen(){
  start(BLACK);
  text(80, 10, 31, 0, "Welcome");
  spinner(150, 150, 0, 1);
  show();
  mainScreen();
}

void mainScreen(){
  Serial.println("Zaczynamy rysowanie");
  start(BLACK);
  text(10, 10, 21, 0, "Temp Out:");
  text(10, 30, 21, 0, "Temp In:");
  text(10, 45, 21, 0, "Temp Engine:");
  text(230, 10, 21, 0, "GPS:");
  button(10, 200, 130, 30, 28, 0, "Smart Mirror"); 
  button(200, 200, 110, 30, 28, 0, "Save data");
  show();
}

void smartMirrorScreen(){
  Serial.println("Zaczynamy rysowanie");
  start(BLACK);
  button(10, 200, 130, 30, 28, 0, "Back"); 
  button(200, 200, 110, 30, 28, 0, "Save data");
  show();
}

void spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale)
{  
   Serial.println("Start Spinner");  
   ft800memWrite32(RAM_CMD+cmdOffset, CMD_SPINNER);
   cmdOffset=incCMDOffset(cmdOffset, 4);
   ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)y<<16)|(x & 0xffff) );
   cmdOffset=incCMDOffset(cmdOffset, 4);
   ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)scale<<16)|style );
   cmdOffset=incCMDOffset(cmdOffset, 4);
   Serial.println("Koniec Spinner");
}

void button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str){
  Serial.println("Start Guzik");
  uint16_t i, j, q = 0;
  const uint16_t length = strlen(str);
  if(!length) return ;	
	
  uint32_t* data = (uint32_t*) calloc((length / 4) + 1, sizeof(uint32_t));
	
  for(i = 0; i < (length / 4); i++, q = q + 4){
    data[i] = (uint32_t)str[q + 3] << 24 | (uint32_t)str[q + 2] << 16 | (uint32_t)str[q + 1] << 8 | (uint32_t)str[q];
  }

  for(j = 0; j < (length % 4); j++, q++){
    data[i] |= (uint32_t)str[q] << (j * 8);
  }
	
  ft800memWrite32(RAM_CMD+cmdOffset, CMD_BUTTON);
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset,  ((uint32_t)y << 16)|(x & 0xffff) );
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset,  ((uint32_t)h << 16)|(w & 0xffff) );
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset,  ((uint32_t)options << 16)|(font & 0xffff) );
  cmdOffset=incCMDOffset(cmdOffset, 4);
	
  for(j = 0; j < (length / 4) + 1; j++){
    ft800memWrite32(RAM_CMD+cmdOffset, (data[j]));
    cmdOffset=incCMDOffset(cmdOffset, 4);
  }

  free(data);
  Serial.println("\nKoniec Guzik");
}

void text( int16_t x,  int16_t y, int16_t font, uint16_t options, const char* str){
  Serial.println("Start Text");
  uint16_t i, j, q = 0;
  const uint16_t length = strlen(str);
  if(!length) return ;	
	
  uint32_t* data = (uint32_t*) calloc((length / 4) + 1, sizeof(uint32_t));
	
  for(i = 0; i < (length / 4); i++, q = q + 4){
    data[i] = (uint32_t)str[q + 3] << 24 | (uint32_t)str[q + 2] << 16 | (uint32_t)str[q + 1] << 8 | (uint32_t)str[q];
  }

  for(j = 0; j < (length % 4); j++, q++){
    data[i] |= (uint32_t)str[q] << (j * 8);
  }
	
  ft800memWrite32(RAM_CMD+cmdOffset, (DL_BEGIN|CMD_TEXT));
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)y << 16)|(x & 0xffff));
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)options << 16)|(font & 0xffff) );
  cmdOffset=incCMDOffset(cmdOffset, 4);
  
  for(j = 0; j < (length / 4) + 1; j++){
      ft800memWrite32(RAM_CMD+cmdOffset, data[j]);
      cmdOffset=incCMDOffset(cmdOffset, 4);
  }

  free(data);
  Serial.println("\nKoniec Text");
}

void line(unsigned long color, unsigned long line_x1, unsigned long line_y1, unsigned long line_x2, unsigned long line_y2, unsigned long width){
  Serial.println("Start Linia");

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_BEGIN|LINES));
  cmdOffset=incCMDOffset(cmdOffset, 4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_COLOR_RGB|color));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_LINE_WIDTH|width));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_VERTEX2F|(line_x1<<15)|line_y1));
  cmdOffset=incCMDOffset(cmdOffset,4);
  
  ft800memWrite32(RAM_CMD+cmdOffset, (DL_VERTEX2F|(line_x2<<15)|line_y2));
  cmdOffset=incCMDOffset(cmdOffset,4);	

  Serial.println("Koniec Linia");
}

void dot(unsigned long color, unsigned int point_size, unsigned long point_x, unsigned long point_y){ 
 Serial.println("Start Kropka"); 

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_POINT_SIZE|point_size));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_BEGIN|FTPOINTS));
  cmdOffset=incCMDOffset(cmdOffset, 4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_COLOR_RGB|color));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_VERTEX2F|(point_x<<15)|point_y));
  cmdOffset=incCMDOffset(cmdOffset,4);

  Serial.println("Koniec Kropka");
}

void start(unsigned long color){
  unsigned long ftRead=0x00000000;
  do{
    cmdBufferRd=ft800memRead16(REG_CMD_READ);
    cmdBufferWr=ft800memRead16(REG_CMD_WRITE);
  }while(cmdBufferWr!=cmdBufferRd);

  cmdOffset=cmdBufferWr;

  ft800memWrite32(RAM_CMD+cmdOffset, (CMD_DLSTART));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_CLEAR_RGB|color));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_CLEAR|CLR_COL|CLR_STN|CLR_TAG));
  cmdOffset=incCMDOffset(cmdOffset, 4);
}

void show(){
  Serial.println("Start Wyswietlanie");
  ft800memWrite32(RAM_CMD+cmdOffset, (DL_END));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_DISPLAY));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (CMD_SWAP));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite16(REG_CMD_WRITE, (cmdOffset));
  Serial.println("Koniec Wyswietlanie");
}

/******************************************************************************
* Function:        void loop(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Overview:        None
*****************************************************************************/
void loop(){
  int ReadWord;
  int xvalue, yvalue;
  ReadWord = ft800memRead32(REG_TOUCH_DIRECT_XY);

  Serial.println(ReadWord);
  Serial.println();
}

/**
******************************************************************************
*
*****************************************************************************/
/*void main(void){

#ifdef PIC
	OSCCONbits.IRCF = 0b111;
	ANSEL = 0;

	TRISC4 = 1; //SDI
	TRISC5 = 0; //SDO
	TRISC3 = 0; //clock
	TRISC6 = 0; //SS
	TRISC7 = 0; //PD pin

	RC6 = 1;    //CS
	RC7 = 1;    //PD
#endif

	setup();
	while(1) loop();
}*/

// DEMO
//dot(BLUE, 500, 1500, 2500);
//line(RED, 3000, 900, 1500, 1500, 16);
//line(WHITE, 3000, 900, 4000, 1500, 100);
//text(0, 0, 16, 0, "Daniel");
