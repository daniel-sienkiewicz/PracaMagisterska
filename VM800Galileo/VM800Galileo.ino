#include "FT800.h"
#include "FT800api.h"
#include "I2C.h"
#include "simulator.h"
#include <stdio.h>
#include <TimerOne.h>

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

short int screenNR = 1;
struct car *audi;
int dataFormat = 3
;                                      // 1 - CSV, 2 - XML, 3 - JSON
/**
******************************************************************************
*
*****************************************************************************/
void setup(void){
  // Setup Car simulator
  pinMode(13, OUTPUT);    
  Serial.begin(9600);
  Serial.println("Setup...");
  pinMode(sda, OUTPUT);
  pinMode(scl, OUTPUT);
  digitalWrite(scl, HIGH);
  digitalWrite(sda, HIGH);
  attachInterrupt(pinInt0, checkChangesDigital, FALLING);                    // Set interrupt - changes in pin0 (HIGH) execute function checkChangesDigital()
     
  audi = readData();
  
  // Setup VM800
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

/******************************************************************************
* Function:        void loop(void)
* PreCondition:    None
* Input:           None
* Output:          None
* Side Effects:    None
* Overview:        None
*****************************************************************************/
void loop(){

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
