/*

Copyright (c) Future Technology Devices International 2014

THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FTDI DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON FTDI PARTS.

FTDI DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : FTDI 

Revision History: 
0.1 - date 2013.04.24 - Initial Version
0.2 - date 2013.08.19 - few inclusions

File Description: 
    This file defines the generic APIs of host access layer for the FT800 or EVE compatible silicon. 
    Application shall access FT800 or EVE resources over these APIs,regardless of I2C or SPI protocol.   
    I2C and SPI is selected by compiler switch "FT_I2C_MODE"  and "FT_SPI_MODE". In addition, there are 
    some helper functions defined for FT800 coprocessor engine as well as host commands.
 
 */
#ifndef FT_GPU_HAL_H
#define FT_GPU_HAL_H



typedef enum {
	FT_GPU_I2C_MODE = 0,
	FT_GPU_SPI_MODE,

	FT_GPU_MODE_COUNT,
	FT_GPU_MODE_UNKNOWN = FT_GPU_MODE_COUNT
}FT_GPU_HAL_MODE_E;

typedef enum {
	FT_GPU_HAL_OPENED,
	FT_GPU_HAL_READING,
	FT_GPU_HAL_WRITING,
	FT_GPU_HAL_CLOSED,

	FT_GPU_HAL_STATUS_COUNT,
	FT_GPU_HAL_STATUS_ERROR = FT_GPU_HAL_STATUS_COUNT
}FT_GPU_HAL_STATUS_E;

typedef struct {
	union {
		ft_uint8_t spi_cs_pin_no;
		ft_uint8_t i2c_addr;
	};
	union {
		ft_uint16_t spi_clockrate_khz;  //In KHz
		ft_uint16_t i2c_clockrate_khz;  //In KHz
	};
	ft_uint8_t channel_no;
}Ft_Gpu_Hal_Config_t;

typedef struct {
	ft_uint8_t reserved;
}Ft_Gpu_App_Context_t;

typedef struct {
	/* Total number channels for libmpsse */
	ft_uint32_t TotalChannelNum;
}Ft_Gpu_HalInit_t;

typedef enum {
	FT_GPU_READ = 0,
	FT_GPU_WRITE,
}FT_GPU_TRANSFERDIR_T;


typedef struct {	
	ft_uint32_t length; //IN and OUT
	ft_uint32_t address;
	ft_uint8_t  *buffer;
}Ft_Gpu_App_Transfer_t;

typedef struct {
	Ft_Gpu_App_Context_t        app_header;         
	Ft_Gpu_Hal_Config_t         hal_config;

        ft_uint16_t ft_cmd_fifo_wp; //coprocessor fifo write pointer
        ft_uint16_t ft_dl_buff_wp;  //display command memory write pointer

	FT_GPU_HAL_STATUS_E        status;        //OUT
	ft_void_t*                 hal_handle;        //IN/OUT
}Ft_Gpu_Hal_Context_t;

/*The basic APIs Level 1*/
ft_bool_t              Ft_Gpu_Hal_Init(Ft_Gpu_HalInit_t *halinit);
ft_bool_t              Ft_Gpu_Hal_Open(Ft_Gpu_Hal_Context_t *host);

/*The APIs for reading/writing transfer continuously only with small buffer system*/
ft_void_t               Ft_Gpu_Hal_StartTransfer(Ft_Gpu_Hal_Context_t *host,FT_GPU_TRANSFERDIR_T rw,ft_uint32_t addr);
ft_uint8_t              Ft_Gpu_Hal_Transfer8(Ft_Gpu_Hal_Context_t *host,ft_uint8_t value);
ft_uint16_t             Ft_Gpu_Hal_Transfer16(Ft_Gpu_Hal_Context_t *host,ft_uint16_t value);
ft_uint32_t             Ft_Gpu_Hal_Transfer32(Ft_Gpu_Hal_Context_t *host,ft_uint32_t value);
ft_void_t               Ft_Gpu_Hal_EndTransfer(Ft_Gpu_Hal_Context_t *host);

/*Read & Write APIs for both burst and single transfer,depending on buffer size*/
ft_void_t              Ft_Gpu_Hal_Read(Ft_Gpu_Hal_Context_t *host, Ft_Gpu_App_Transfer_t *transfer);
ft_void_t              Ft_Gpu_Hal_Write(Ft_Gpu_Hal_Context_t *host,Ft_Gpu_App_Transfer_t *transfer);

ft_void_t              Ft_Gpu_Hal_Close(Ft_Gpu_Hal_Context_t *host);
ft_void_t              Ft_Gpu_Hal_DeInit();

/*Helper function APIs Read*/
ft_uint8_t  Ft_Gpu_Hal_Rd8(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr);
ft_uint16_t Ft_Gpu_Hal_Rd16(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr);
ft_uint32_t Ft_Gpu_Hal_Rd32(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr);

/*Helper function APIs Write*/
ft_void_t Ft_Gpu_Hal_Wr8(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, ft_uint8_t v);
ft_void_t Ft_Gpu_Hal_Wr16(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, ft_uint16_t v);
ft_void_t Ft_Gpu_Hal_Wr32(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, ft_uint32_t v);

/*******************************************************************************/
/*******************************************************************************/
/*APIs for coprocessor Fifo read/write and space management*/
ft_void_t Ft_Gpu_Hal_Updatecmdfifo(Ft_Gpu_Hal_Context_t *host,ft_uint16_t count);
ft_void_t Ft_Gpu_Hal_WrCmd32(Ft_Gpu_Hal_Context_t *host,ft_uint32_t cmd);
ft_void_t Ft_Gpu_Hal_WrCmdBuf(Ft_Gpu_Hal_Context_t *host,ft_uint8_t *buffer,ft_uint16_t count);
ft_void_t Ft_Gpu_Hal_WaitCmdfifo_empty(Ft_Gpu_Hal_Context_t *host);
ft_void_t Ft_Gpu_Hal_ResetCmdFifo(Ft_Gpu_Hal_Context_t *host);
ft_void_t Ft_Gpu_Hal_CheckCmdBuffer(Ft_Gpu_Hal_Context_t *host,ft_uint16_t count);

ft_void_t Ft_Gpu_Hal_ResetDLBuffer(Ft_Gpu_Hal_Context_t *host);

ft_void_t  Ft_Gpu_Hal_StartCmdTransfer(Ft_Gpu_Hal_Context_t *host,FT_GPU_TRANSFERDIR_T rw, ft_uint16_t count);

ft_void_t Ft_Gpu_Hal_Powercycle(Ft_Gpu_Hal_Context_t *host,ft_bool_t up);


/*******************************************************************************/
/*******************************************************************************/
/*APIs for Host Commands*/
typedef enum {
	FT_GPU_INTERNAL_OSC = 0x48, //default
	FT_GPU_EXTERNAL_OSC = 0x44,
}FT_GPU_PLL_SOURCE_T;
typedef enum {
	FT_GPU_PLL_48M = 0x62,  //default
	FT_GPU_PLL_36M = 0x61,
	FT_GPU_PLL_24M = 0x64,
}FT_GPU_PLL_FREQ_T;

typedef enum {
	FT_GPU_ACTIVE_M =  0x00,  
	FT_GPU_STANDBY_M = 0x41,//default
	FT_GPU_SLEEP_M =   0x42,
	FT_GPU_POWERDOWN_M = 0x50,
}FT_GPU_POWER_MODE_T;

#define FT_GPU_CORE_RESET  (0x68)

ft_int32_t hal_strlen(const ft_char8_t *s);
ft_void_t Ft_Gpu_Hal_Sleep(ft_uint16_t ms);
ft_void_t Ft_Gpu_ClockSelect(Ft_Gpu_Hal_Context_t *host,FT_GPU_PLL_SOURCE_T pllsource);
ft_void_t Ft_Gpu_PLL_FreqSelect(Ft_Gpu_Hal_Context_t *host,FT_GPU_PLL_FREQ_T freq);
ft_void_t Ft_Gpu_PowerModeSwitch(Ft_Gpu_Hal_Context_t *host,FT_GPU_POWER_MODE_T pwrmode);
ft_void_t Ft_Gpu_CoreReset(Ft_Gpu_Hal_Context_t *host);
ft_void_t Ft_Gpu_Hal_StartTransfer(Ft_Gpu_Hal_Context_t *host,FT_GPU_TRANSFERDIR_T rw,ft_uint32_t addr);
ft_void_t Ft_Gpu_Hal_WrMem(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, const ft_uint8_t *buffer, ft_uint32_t length);
ft_void_t Ft_Gpu_Hal_WrMemFromFlash(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr,const ft_prog_uchar8_t *buffer, ft_uint32_t length);
ft_void_t Ft_Gpu_Hal_WrCmdBufFromFlash(Ft_Gpu_Hal_Context_t *host,FT_PROGMEM ft_prog_uchar8_t *buffer,ft_uint16_t count);
ft_void_t Ft_Gpu_Hal_RdMem(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length);
ft_void_t Ft_Gpu_Hal_WaitLogo_Finish(Ft_Gpu_Hal_Context_t *host);
ft_uint8_t    Ft_Gpu_Hal_TransferString(Ft_Gpu_Hal_Context_t *host,const ft_char8_t *string);
ft_void_t Ft_Gpu_HostCommand(Ft_Gpu_Hal_Context_t *host,ft_uint8_t cmd);
ft_int32_t Ft_Gpu_Hal_Dec2Ascii(ft_char8_t *pSrc,ft_int32_t value);
ft_uint8_t Ft_Gpu_Hal_WaitCmdfifo_empty_status(Ft_Gpu_Hal_Context_t *host);
ft_void_t Ft_Gpu_Hal_WrCmdBuf_nowait(Ft_Gpu_Hal_Context_t *host,ft_uint8_t *buffer,ft_uint16_t count);
ft_uint16_t Ft_Gpu_Cmdfifo_Freespace(Ft_Gpu_Hal_Context_t *host);
#endif  /*FT_GPU_HAL_H*/
