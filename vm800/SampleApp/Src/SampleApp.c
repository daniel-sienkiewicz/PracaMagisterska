#include <windows.h>
#include <winuser.h>
#include <commctrl.h>
#include <winbase.h>
#include <stdio.h>
#include "FT_Platform.h"
#include "FT_CoPro_Cmds.h"
#include "FT_Gpu_Hal.h"
#include "libMPSSE_SPI.h"
#include "SampleApp.h"

#define MSVC_PLATFORM
#define MSVC_PLATFORM_SPI
#define SAMAPP_DISPLAY_QVGA
#define SAMAPP_DELAY_BTW_APIS (1000)
#define SAMAPP_ENABLE_DELAY() Ft_Gpu_Hal_Sleep(SAMAPP_DELAY_BTW_APIS)
#define SAMAPP_ENABLE_DELAY_VALUE(x) Ft_Gpu_Hal_Sleep(x)

/* Global variables for display resolution to support various display panels */
/* Default is WQVGA - 480x272 */
int16 FT_DispWidth = 480;
int16 FT_DispHeight = 272;
int16 FT_DispHCycle =  548;
int16 FT_DispHOffset = 43;
int16 FT_DispHSync0 = 0;
int16 FT_DispHSync1 = 41;
int16 FT_DispVCycle = 292;
int16 FT_DispVOffset = 12;
int16 FT_DispVSync0 = 0;
int16 FT_DispVSync1 = 10;
uint8 FT_DispPCLK = 5;
char FT_DispSwizzle = 0;
char FT_DispPCLKPol = 1;

/* Global used for buffer optimization */
Ft_Gpu_Hal_Context_t host,*phost;

uint32 Ft_CmdBuffer_Index;
uint32 Ft_DlBuffer_Index;

#ifdef BUFFER_OPTIMIZATION
uint8  Ft_DlBuffer[FT_DL_SIZE];
uint8  Ft_CmdBuffer[FT_CMD_FIFO_SIZE];
#endif

void SAMAPP_fadeout(void);
void SAMAPP_BootupConfig(void);
void Ft_Gpu_Hal_DeInit(void);

void mainScreen(void);
void smartMirror(void);
int saved = 0;
int screenNR = 1;
float tempIn = 0, tempOut = 0, tempEngine = 0;

void Ft_App_WrCoCmd_Buffer(Ft_Gpu_Hal_Context_t *phost,uint32 cmd)
{
#ifdef  BUFFER_OPTIMIZATION
	/* Copy the command instruction into buffer */
	uint32 *pBuffcmd;
	pBuffcmd =(uint32*)&Ft_CmdBuffer[Ft_CmdBuffer_Index];
	*pBuffcmd = cmd;
#endif

	/* Increment the command index */
	Ft_CmdBuffer_Index += FT_CMD_SIZE;
}

void Ft_App_WrDlCmd_Buffer(Ft_Gpu_Hal_Context_t *phost,uint32 cmd)
{
#ifdef BUFFER_OPTIMIZATION
	/* Copy the command instruction into buffer */
	uint32 *pBuffcmd;
	pBuffcmd =(uint32*)&Ft_DlBuffer[Ft_DlBuffer_Index];
	*pBuffcmd = cmd;
#endif

	/* Increment the command index */
	Ft_DlBuffer_Index += FT_CMD_SIZE;
}

void Ft_App_WrCoStr_Buffer(Ft_Gpu_Hal_Context_t *phost, char *s)
{
#ifdef  BUFFER_OPTIMIZATION
	uint16 length;
	length = strlen(s) + 1;//last for the null termination

	strcpy(&Ft_CmdBuffer[Ft_CmdBuffer_Index],s);

	/* increment the length and align it by 4 bytes */
	Ft_CmdBuffer_Index += ((length + 3) & ~3);
#endif
}

void Ft_App_Flush_DL_Buffer(Ft_Gpu_Hal_Context_t *phost)
{
#ifdef  BUFFER_OPTIMIZATION
	if (Ft_DlBuffer_Index > 0)
		Ft_Gpu_Hal_WrMem(phost,RAM_DL,Ft_DlBuffer,Ft_DlBuffer_Index);
#endif
	Ft_DlBuffer_Index = 0;

}

void Ft_App_Flush_Co_Buffer(Ft_Gpu_Hal_Context_t *phost)
{
#ifdef  BUFFER_OPTIMIZATION
	if (Ft_CmdBuffer_Index > 0)
		Ft_Gpu_Hal_WrCmdBuf(phost,Ft_CmdBuffer,Ft_CmdBuffer_Index);
#endif
	Ft_CmdBuffer_Index = 0;
}

ft_void_t SAMAPP_BootupConfig()
{

	/* Do a power cycle for safer side */
	Ft_Gpu_Hal_Powercycle(phost,FT_TRUE);

	/* Access address 0 to wake up the FT800 */
	Ft_Gpu_HostCommand(phost,FT_GPU_ACTIVE_M);  
	Ft_Gpu_Hal_Sleep(20);

	/* Set the clk to external clock */
	Ft_Gpu_HostCommand(phost,FT_GPU_EXTERNAL_OSC);  
	Ft_Gpu_Hal_Sleep(10);


	/* Switch PLL output to 48MHz */
	Ft_Gpu_HostCommand(phost,FT_GPU_PLL_48M);  
	Ft_Gpu_Hal_Sleep(10);

	/* Do a core reset for safer side */
	Ft_Gpu_HostCommand(phost,FT_GPU_CORE_RESET);     


	{
		ft_uint8_t chipid;
		//Read Register ID to check if FT800 is ready. 
		chipid = Ft_Gpu_Hal_Rd8(phost, REG_ID);
		while(chipid != 0x7C)
			chipid = Ft_Gpu_Hal_Rd8(phost, REG_ID);
#ifdef MSVC_PLATFORM
		printf("VC1 register ID after wake up %x\n",chipid);
#endif
	}
	/* Configuration of LCD display */
#ifdef SAMAPP_DISPLAY_QVGA	
	/* Values specific to QVGA LCD display */
	FT_DispWidth = 320;
	FT_DispHeight = 240;
	FT_DispHCycle =  408;
	FT_DispHOffset = 70;
	FT_DispHSync0 = 0;
	FT_DispHSync1 = 10;
	FT_DispVCycle = 263;
	FT_DispVOffset = 13;
	FT_DispVSync0 = 0;
	FT_DispVSync1 = 2;
	FT_DispPCLK = 8;
	FT_DispSwizzle = 2;
	FT_DispPCLKPol = 0;
#endif

	Ft_Gpu_Hal_Wr16(phost, REG_HCYCLE, FT_DispHCycle);
	Ft_Gpu_Hal_Wr16(phost, REG_HOFFSET, FT_DispHOffset);
	Ft_Gpu_Hal_Wr16(phost, REG_HSYNC0, FT_DispHSync0);
	Ft_Gpu_Hal_Wr16(phost, REG_HSYNC1, FT_DispHSync1);
	Ft_Gpu_Hal_Wr16(phost, REG_VCYCLE, FT_DispVCycle);
	Ft_Gpu_Hal_Wr16(phost, REG_VOFFSET, FT_DispVOffset);
	Ft_Gpu_Hal_Wr16(phost, REG_VSYNC0, FT_DispVSync0);
	Ft_Gpu_Hal_Wr16(phost, REG_VSYNC1, FT_DispVSync1);
	Ft_Gpu_Hal_Wr8(phost, REG_SWIZZLE, FT_DispSwizzle);
	Ft_Gpu_Hal_Wr8(phost, REG_PCLK_POL, FT_DispPCLKPol);
	Ft_Gpu_Hal_Wr8(phost, REG_PCLK,FT_DispPCLK);//after this display is visible on the LCD
	Ft_Gpu_Hal_Wr16(phost, REG_HSIZE, FT_DispWidth);
	Ft_Gpu_Hal_Wr16(phost, REG_VSIZE, FT_DispHeight);

	/*Set DISP_EN to 1*/
	Ft_Gpu_Hal_Wr8(phost, REG_GPIO_DIR,0x83 | Ft_Gpu_Hal_Rd8(phost,REG_GPIO_DIR));
	Ft_Gpu_Hal_Wr8(phost, REG_GPIO,0x083 | Ft_Gpu_Hal_Rd8(phost,REG_GPIO));

	/* Touch configuration - configure the resistance value to 1200 - this value is specific to customer requirement and derived by experiment */
	Ft_Gpu_Hal_Wr16(phost, REG_TOUCH_RZTHRESH,1200);


}


void SAMAPP_API_Screen(char *str)
{
	Ft_Gpu_CoCmd_Dlstart(phost);
	Ft_App_WrCoCmd_Buffer(phost,CLEAR_COLOR_RGB(255,255,255));
	Ft_App_WrCoCmd_Buffer(phost,CLEAR(1,1,1));

	Ft_App_WrCoCmd_Buffer(phost,COLOR_RGB(0x80,0x80,0x00));
	Ft_Gpu_CoCmd_Text(phost,(FT_DispWidth/2), FT_DispHeight/2, 31, OPT_CENTERX, str);

	Ft_App_WrCoCmd_Buffer(phost,DISPLAY());
	Ft_Gpu_CoCmd_Swap(phost);

	Ft_App_Flush_Co_Buffer(phost);				// Download the commands into fifo

	Ft_Gpu_Hal_WaitCmdfifo_empty(phost);		//Wait till coprocessor completes the operation
	Ft_Gpu_Hal_Sleep(5000);
}

/* Boot up for FT800 followed by graphics primitive sample cases */
/* Initial boot up DL - make the back ground green color */
uint8 FT_DLCODE_BOOTUP[12] =
{
	0,255,0,2,//GPU instruction CLEAR_COLOR_RGB
	7,0,0,38, //GPU instruction CLEAR
	0,0,0,0,  //GPU instruction DISPLAY
};


#ifdef USEDLL
SPI_GetNumChannelsPtr		SPI_GetNumChannels;
SPI_GetChannelInfoPtr		SPI_GetChannelInfo;
SPI_OpenChannelPtr			SPI_OpenChannel;
SPI_InitChannelPtr			SPI_InitChannel;
SPI_CloseChannelPtr			SPI_CloseChannel;
SPI_ReadPtr						SPI_Read;
SPI_WritePtr					SPI_Write;
SPI_ReadWritePtr				SPI_ReadWrite;
SPI_IsBusyPtr					SPI_IsBusy;
Init_libMPSSEPtr				Init_libMPSSE;
Cleanup_libMPSSEPtr			Cleanup_libMPSSE;
SPI_ChangeCSPtr				SPI_ChangeCS;
SPI_ToggleCSPtr				SPI_ToggleCS;
FT_WriteGPIOPtr				FT_WriteGPIO;
FT_ReadGPIOPtr					FT_ReadGPIO;



//z ftd2xx.dll
FT_WritePtr	FT_Write_DLL;
FT_CreateDeviceInfoListPtr FT_CreateDeviceInfoList_DLL;
FT_GetDeviceInfoListPtr FT_GetDeviceInfoList_DLL;


/**
**********************************************************
* Funkcja ladujaca i inicjujaca funkcje do obslugi libMPSSE.DLL
* @retval HINSTANCE do biblioteki albo NULL
* **********************************************************/
HINSTANCE installMPSSE(void)
{
	HINSTANCE dll=LoadLibrary("mojeMPSSE.DLL");

	if(dll==NULL) return(NULL);

	SPI_GetNumChannels=(SPI_GetNumChannelsPtr)GetProcAddress(dll,"SPI_GetNumChannels");

	SPI_GetChannelInfo=(SPI_GetChannelInfoPtr)GetProcAddress(dll,"SPI_GetChannelInfo");
	SPI_OpenChannel=(SPI_OpenChannelPtr)GetProcAddress(dll,"SPI_OpenChannel");
	SPI_InitChannel=(SPI_InitChannelPtr)GetProcAddress(dll,"SPI_InitChannel");
	SPI_CloseChannel=(SPI_CloseChannelPtr)GetProcAddress(dll,"SPI_CloseChannel");
	SPI_Read=(SPI_ReadPtr)GetProcAddress(dll,"SPI_Read");
	SPI_Write=(SPI_WritePtr)GetProcAddress(dll,"SPI_Write");
	SPI_ReadWrite=(SPI_ReadWritePtr)GetProcAddress(dll,"SPI_ReadWrite");
	SPI_IsBusy=(SPI_IsBusyPtr)GetProcAddress(dll,"SPI_IsBusy");
	Init_libMPSSE=(Init_libMPSSEPtr)GetProcAddress(dll,"Init_libMPSSE");
	Cleanup_libMPSSE=(Cleanup_libMPSSEPtr)GetProcAddress(dll,"Cleanup_libMPSSE");
	SPI_ChangeCS=(SPI_ChangeCSPtr)GetProcAddress(dll,"SPI_ChangeCS");
	SPI_ToggleCS=(SPI_ToggleCSPtr)GetProcAddress(dll,"SPI_ToggleCS");
	FT_WriteGPIO=(FT_WriteGPIOPtr)GetProcAddress(dll,"FT_WriteGPIO");
	FT_ReadGPIO=(FT_ReadGPIOPtr)GetProcAddress(dll,"FT_ReadGPIO");


	//funkcje bladowane bezposrednio z odyginalnej ftd2xx.dll
	HINSTANCE dll2=LoadLibrary("d2xxORG.DLL");
	if(dll2==NULL){
		printf("blad ladowania d2xxorg.dll\n");
		return(NULL);
	}
	FT_Write_DLL=(FT_WritePtr)GetProcAddress(dll2,"FT_Write");
	FT_CreateDeviceInfoList_DLL=(FT_CreateDeviceInfoListPtr)GetProcAddress(dll2,"FT_CreateDeviceInfoList");
	FT_GetDeviceInfoList_DLL=(FT_GetDeviceInfoListPtr)GetProcAddress(dll2,"FT_GetDeviceInfoList");

	return(dll);
}
#endif

void smartMirror(){
	uint32 ReadWord;
	int16 xvalue,yvalue;

	printf("Smart Mirror mode");
		Ft_Gpu_CoCmd_Dlstart(phost);
		Ft_App_WrCoCmd_Buffer(phost,CLEAR_COLOR_RGB(64,64,64));
		Ft_App_WrCoCmd_Buffer(phost,CLEAR(1,1,1));
		Ft_App_WrCoCmd_Buffer(phost,COLOR_RGB(255,255,255));

		Ft_Gpu_CoCmd_Button(phost, 10, 200, 130, 30, 28, 0, "Back");

		if(!saved){
			Ft_Gpu_CoCmd_Button(phost, 200, 200, 110, 30, 28, 0, "Save data");
		} else{
			Ft_Gpu_CoCmd_Button(phost, 200, 200, 110, 30, 28, 0, "Saved");
		}

		Ft_Gpu_CoCmd_Text(phost, 10, 40, 26, 0, "SMART MIRROR SCREEN");
	
		ReadWord = Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_DIRECT_XY);
		yvalue = (int16)(ReadWord & 0xffff);
		xvalue = (int16)((ReadWord>>16) & 0xffff);

		if(xvalue > 70 && xvalue < 460 && yvalue > 70 && yvalue < 190){
			printf("Back \n");
			screenNR = 1;
		} else if(xvalue > 630 && xvalue < 950 && yvalue > 70 && yvalue < 190){
			saved = !saved;
			printf("Save Data = %i\n", saved);
		} else
			printf("x = %i y = %i\n", xvalue, yvalue);

		Ft_App_WrCoCmd_Buffer(phost,DISPLAY());
		Ft_Gpu_CoCmd_Swap(phost);

		/* Download the commands into fifo */
		Ft_App_Flush_Co_Buffer(phost);

		/* Wait till coprocessor completes the operation */
		Ft_Gpu_Hal_WaitCmdfifo_empty(phost);
		Ft_Gpu_Hal_Sleep(30);
}

void mainScreen(void){
	uint32 ReadWord;
	int16 xvalue,yvalue;
	ft_char8_t StringArray[100];

		Ft_Gpu_CoCmd_Dlstart(phost);
		Ft_App_WrCoCmd_Buffer(phost,CLEAR_COLOR_RGB(64,64,64));
		Ft_App_WrCoCmd_Buffer(phost,CLEAR(1,1,1));
		Ft_App_WrCoCmd_Buffer(phost,COLOR_RGB(255,255,255));

		Ft_Gpu_CoCmd_Button(phost, 10, 200, 130, 30, 28, 0, "Smart Mirror");
		if(!saved){
			Ft_Gpu_CoCmd_Button(phost, 200, 200, 110, 30, 28, 0, "Save data");
		} else{
			Ft_Gpu_CoCmd_Button(phost, 200, 200, 110, 30, 28, 0, "Saved");
		}

		StringArray[0] = '\0';
		strcat(StringArray,"Outside: ");
		Ft_Gpu_Hal_Dec2Ascii(StringArray,(ft_int32_t)tempOut);
		strcat(StringArray," C");
		Ft_Gpu_CoCmd_Text(phost, 10, 20, 26, 0, StringArray);
		
		StringArray[0] = '\0';
		strcat(StringArray,"Inside: ");
		Ft_Gpu_Hal_Dec2Ascii(StringArray,(ft_int32_t)tempIn);
		strcat(StringArray," C");
		Ft_Gpu_CoCmd_Text(phost, 10, 40, 26, 0, StringArray);
		
		StringArray[0] = '\0';
		strcat(StringArray,"Engine: ");
		Ft_Gpu_Hal_Dec2Ascii(StringArray,(ft_int32_t)tempEngine);
		strcat(StringArray," C");
		Ft_Gpu_CoCmd_Text(phost, 10, 60, 26, 0, StringArray);

		StringArray[0] = '\0';
		strcat(StringArray,"GPS: ");
		Ft_Gpu_CoCmd_Text(phost, 220, 20, 26, 0, StringArray);

		ReadWord = Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_DIRECT_XY);
		yvalue = (int16)(ReadWord & 0xffff);
		xvalue = (int16)((ReadWord>>16) & 0xffff);

		if(xvalue > 70 && xvalue < 460 && yvalue > 70 && yvalue < 190){
			printf("Smart mirror \n");
			screenNR = 2;
		} else if(xvalue > 630 && xvalue < 950 && yvalue > 70 && yvalue < 190){
			saved = !saved;
			printf("Save Data = %i\n", saved);
		} else
			printf("x = %i y = %i\n", xvalue, yvalue);

		Ft_App_WrCoCmd_Buffer(phost,DISPLAY());
		Ft_Gpu_CoCmd_Swap(phost);

		/* Download the commands into fifo */
		Ft_App_Flush_Co_Buffer(phost);

		/* Wait till coprocessor completes the operation */
		Ft_Gpu_Hal_WaitCmdfifo_empty(phost);
		Ft_Gpu_Hal_Sleep(30);
}

void mainControler(){
	while(1){
		switch(screenNR){
		case 1:
			mainScreen();
			break;
		case 2:
			smartMirror();
			break;
		}
	}
}

int32 main(int32 argc,char *argv[]){
	Ft_Gpu_HalInit_t halinit;

	halinit.TotalChannelNum = 1;

	printf("3.05.2014\n");

#ifdef USEDLL
	if(NULL==installMPSSE()){
		printf("libmpsse.dll not loaded\n");
		exit(1);
	}
	printf("libmpsse.dll installed OK\n");
#endif

	Init_libMPSSE();		//OK

	Ft_Gpu_Hal_Init(&halinit);

	host.hal_config.channel_no=0;
	host.hal_config.spi_clockrate_khz=12000; //in KHz
	Ft_Gpu_Hal_Open(&host);
	phost=&host;

	printf("po Ft_Gpu_Hal_Open: %x\n", host.hal_handle);

	SAMAPP_BootupConfig();

	printf("reg_touch_rz =0x%x ", Ft_Gpu_Hal_Rd16(phost, REG_TOUCH_RZ));
	printf("reg_touch_rzthresh =0x%x ", Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_RZTHRESH));
	printf("reg_touch_tag_xy=0x%x",Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_TAG_XY));
	printf("reg_touch_tag=0x%x",Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_TAG));

	Ft_Gpu_Hal_WrMem(phost,RAM_DL,(uint8 *)FT_DLCODE_BOOTUP,sizeof(FT_DLCODE_BOOTUP));
	Ft_Gpu_Hal_Wr8(phost,REG_DLSWAP,DLSWAP_FRAME);

	Ft_Gpu_Hal_Sleep(1000);

	SAMAPP_API_Screen("Lurch car system");
	mainControler();

	/* Close all the opened handles */
	Ft_Gpu_Hal_Close(phost);
	Ft_Gpu_Hal_DeInit();
	return 0;
}