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
0.1 - date 2013.04.24 - Initial version
0.2 - date 2013.08.19 - added few APIs

*/

#include "FT_Platform.h"


/* API to initialize the SPI interface */
ft_bool_t  Ft_Gpu_Hal_Init(Ft_Gpu_HalInit_t *halinit)
{
#ifdef ARDUINO_PLATFORM_SPI
      pinMode(FT_ARDUINO_PRO_SPI_CS, OUTPUT);
      pinMode(FT800_PD_N, OUTPUT);
      digitalWrite(FT_ARDUINO_PRO_SPI_CS, HIGH);
            digitalWrite(FT800_PD_N, HIGH);
#endif

#ifdef MSVC_PLATFORM_SPI
	/* Initialize the libmpsse */
        Init_libMPSSE();	
	SPI_GetNumChannels(&halinit->TotalChannelNum);
	/* By default i am assuming only one mpsse cable is connected to PC and channel 0 of that mpsse cable is used for spi transactions */
	if(halinit->TotalChannelNum > 0)
	{
        FT_DEVICE_LIST_INFO_NODE devList;
		SPI_GetChannelInfo(0,&devList);
		printf("Information on channel number %d:\n",0);
		/* print the dev info */
		printf(" Flags=0x%x\n",devList.Flags);
		printf(" Type=0x%x\n",devList.Type);
		printf(" ID=0x%x\n",devList.ID);
		printf(" LocId=0x%x\n",devList.LocId);
		printf(" SerialNumber=%s\n",devList.SerialNumber);
		printf(" Description=%s\n",devList.Description);
		printf(" ftHandle=0x%x\n",devList.ftHandle);/*is 0 unless open*/
	}
#endif
	return TRUE;
}
ft_bool_t    Ft_Gpu_Hal_Open(Ft_Gpu_Hal_Context_t *host)
{
#ifdef ARDUINO_PLATFORM_SPI
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
#endif
#ifdef MSVC_PLATFORM_SPI
        ChannelConfig channelConf;			//channel configuration
	FT_STATUS status;					
		
	/* configure the spi settings */
	channelConf.ClockRate = host->hal_config.spi_clockrate_khz * 1000; 
	channelConf.LatencyTimer= 2;       
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00000000;	/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

	/* Open the first available channel */
	SPI_OpenChannel(host->hal_config.channel_no,(FT_HANDLE *)&host->hal_handle);
	status = SPI_InitChannel((FT_HANDLE)host->hal_handle,&channelConf);
	printf("\nhandle=0x%x status=0x%x\n",host->hal_handle,status);	
#endif
	host->ft_cmd_fifo_wp = host->ft_dl_buff_wp = 0;
	host->status = FT_GPU_HAL_OPENED;
	return TRUE;
}
ft_void_t  Ft_Gpu_Hal_Close(Ft_Gpu_Hal_Context_t *host)
{
	host->status = FT_GPU_HAL_CLOSED;
#ifdef MSVC_PLATFORM_SPI	
	/* Close the channel*/
	SPI_CloseChannel(host->hal_handle);
#endif
#ifdef ARDUINO_PLATFORM_SPI
        SPI.end();
#endif
}

ft_void_t Ft_Gpu_Hal_DeInit()
{
#ifdef MSVC_PLATFORM_SPI
   //Cleanup the MPSSE Lib
   Cleanup_libMPSSE();
#endif
}

/*The APIs for reading/writing transfer continuously only with small buffer system*/
ft_void_t  Ft_Gpu_Hal_StartTransfer(Ft_Gpu_Hal_Context_t *host,FT_GPU_TRANSFERDIR_T rw,ft_uint32_t addr)
{
	if (FT_GPU_READ == rw){

#ifdef MSVC_PLATFORM_SPI
		ft_uint8_t Transfer_Array[4];
		ft_uint32_t SizeTransfered;

		/* Compose the read packet */
		Transfer_Array[0] = addr >> 16;
		Transfer_Array[1] = addr >> 8;
		Transfer_Array[2] = addr;

		Transfer_Array[3] = 0; //Dummy Read byte
		SPI_Write((FT_HANDLE)host->hal_handle,Transfer_Array,sizeof(Transfer_Array),&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
#endif
#ifdef ARDUINO_PLATFORM_SPI
		digitalWrite(FT_ARDUINO_PRO_SPI_CS, LOW);
		SPI.transfer(addr >> 16);
		SPI.transfer(highByte(addr));
		SPI.transfer(lowByte(addr));

		SPI.transfer(0); //Dummy Read Byte
#endif
		host->status = FT_GPU_HAL_READING;
	}else{
#ifdef MSVC_PLATFORM_SPI
		ft_uint8_t Transfer_Array[3];
		ft_uint32_t SizeTransfered;

		/* Compose the read packet */
		Transfer_Array[0] = (0x80 | (addr >> 16));
		Transfer_Array[1] = addr >> 8;
		Transfer_Array[2] = addr;
		SPI_Write((FT_HANDLE)host->hal_handle,Transfer_Array,3,&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);		
#endif
#ifdef ARDUINO_PLATFORM_SPI
		digitalWrite(FT_ARDUINO_PRO_SPI_CS, LOW);
		SPI.transfer(0x80 | (addr >> 16));
		SPI.transfer(highByte(addr));
		SPI.transfer(lowByte(addr));
#endif
		host->status = FT_GPU_HAL_WRITING;
	}
}



/*The APIs for writing transfer continuously only*/
  ft_void_t  Ft_Gpu_Hal_StartCmdTransfer(Ft_Gpu_Hal_Context_t *host,FT_GPU_TRANSFERDIR_T rw, ft_uint16_t count)
{
	Ft_Gpu_Hal_StartTransfer(host,rw,host->ft_cmd_fifo_wp + RAM_CMD);
}

ft_uint8_t    Ft_Gpu_Hal_TransferString(Ft_Gpu_Hal_Context_t *host,const ft_char8_t *string)
{
    ft_uint16_t length = strlen(string);
    while(length --){
       Ft_Gpu_Hal_Transfer8(host,*string);
       string ++;
    }
    //Append one null as ending flag
    Ft_Gpu_Hal_Transfer8(host,0);
}


ft_uint8_t    Ft_Gpu_Hal_Transfer8(Ft_Gpu_Hal_Context_t *host,ft_uint8_t value)
{
#ifdef ARDUINO_PLATFORM_SPI
        return SPI.transfer(value);
#endif
#ifdef MSVC_PLATFORM_SPI
	ft_uint32_t SizeTransfered;
	if (host->status == FT_GPU_HAL_WRITING){
		SPI_Write(host->hal_handle,&value,sizeof(value),&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}else{
		SPI_Read(host->hal_handle,&value,sizeof(value),&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}

	if (SizeTransfered != sizeof(value))
		host->status = FT_GPU_HAL_STATUS_ERROR;
        return value;
#endif	
}


ft_uint16_t  Ft_Gpu_Hal_Transfer16(Ft_Gpu_Hal_Context_t *host,ft_uint16_t value)
{
	ft_uint16_t retVal = 0;

        if (host->status == FT_GPU_HAL_WRITING){
		Ft_Gpu_Hal_Transfer8(host,value & 0xFF);//LSB first
		Ft_Gpu_Hal_Transfer8(host,(value >> 8) & 0xFF);
	}else{
		retVal = Ft_Gpu_Hal_Transfer8(host,0);
		retVal |= (ft_uint16_t)Ft_Gpu_Hal_Transfer8(host,0) << 8;
	}

	return retVal;
}
ft_uint32_t  Ft_Gpu_Hal_Transfer32(Ft_Gpu_Hal_Context_t *host,ft_uint32_t value)
{
	ft_uint32_t retVal = 0;
	if (host->status == FT_GPU_HAL_WRITING){
		Ft_Gpu_Hal_Transfer16(host,value & 0xFFFF);//LSB first
		Ft_Gpu_Hal_Transfer16(host,(value >> 16) & 0xFFFF);
	}else{
		retVal = Ft_Gpu_Hal_Transfer16(host,0);
		retVal |= (ft_uint32_t)Ft_Gpu_Hal_Transfer16(host,0) << 16;
	}
	return retVal;
}

ft_void_t   Ft_Gpu_Hal_EndTransfer(Ft_Gpu_Hal_Context_t *host)
{
#ifdef MSVC_PLATFORM_SPI  
	//just disbale the CS - send 0 bytes with CS disable
	SPI_ToggleCS((FT_HANDLE)host->hal_handle,FALSE);
#endif
#ifdef ARDUINO_PLATFORM_SPI
	digitalWrite(FT_ARDUINO_PRO_SPI_CS, HIGH);
#endif
	host->status = FT_GPU_HAL_OPENED;
}


ft_uint8_t  Ft_Gpu_Hal_Rd8(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr)
{
	ft_uint8_t value;
	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_READ,addr);
	value = Ft_Gpu_Hal_Transfer8(host,0);
	Ft_Gpu_Hal_EndTransfer(host);
	return value;
}
ft_uint16_t Ft_Gpu_Hal_Rd16(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr)
{
	ft_uint16_t value;
	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_READ,addr);
	value = Ft_Gpu_Hal_Transfer16(host,0);
	Ft_Gpu_Hal_EndTransfer(host);
	return value;
}
ft_uint32_t Ft_Gpu_Hal_Rd32(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr)
{
	ft_uint32_t value;
	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_READ,addr);
	value = Ft_Gpu_Hal_Transfer32(host,0);
	Ft_Gpu_Hal_EndTransfer(host);
	return value;
}

ft_void_t Ft_Gpu_Hal_Wr8(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, ft_uint8_t v)
{	
	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_WRITE,addr);
	Ft_Gpu_Hal_Transfer8(host,v);
	Ft_Gpu_Hal_EndTransfer(host);
}
ft_void_t Ft_Gpu_Hal_Wr16(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, ft_uint16_t v)
{
	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_WRITE,addr);
	Ft_Gpu_Hal_Transfer16(host,v);
	Ft_Gpu_Hal_EndTransfer(host);
}
ft_void_t Ft_Gpu_Hal_Wr32(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, ft_uint32_t v)
{
	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_WRITE,addr);
	Ft_Gpu_Hal_Transfer32(host,v);
	Ft_Gpu_Hal_EndTransfer(host);
}

ft_void_t Ft_Gpu_HostCommand(Ft_Gpu_Hal_Context_t *host,ft_uint8_t cmd)
{
#ifdef MSVC_PLATFORM_SPI
  ft_uint8_t Transfer_Array[3];
  ft_uint32_t SizeTransfered;

  Transfer_Array[0] = cmd;
  Transfer_Array[1] = 0;
  Transfer_Array[2] = 0;

  SPI_Write(host->hal_handle,Transfer_Array,sizeof(Transfer_Array),&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#endif
#ifdef ARDUINO_PLATFORM_SPI
  digitalWrite(FT_ARDUINO_PRO_SPI_CS, LOW);
  SPI.transfer(cmd);
  SPI.transfer(0);
  SPI.transfer(0);
  digitalWrite(FT_ARDUINO_PRO_SPI_CS, HIGH);
#endif
}

ft_void_t Ft_Gpu_ClockSelect(Ft_Gpu_Hal_Context_t *host,FT_GPU_PLL_SOURCE_T pllsource)
{
   Ft_Gpu_HostCommand(host,pllsource);
}
ft_void_t Ft_Gpu_PLL_FreqSelect(Ft_Gpu_Hal_Context_t *host,FT_GPU_PLL_FREQ_T freq)
{
   Ft_Gpu_HostCommand(host,freq);
}
ft_void_t Ft_Gpu_PowerModeSwitch(Ft_Gpu_Hal_Context_t *host,FT_GPU_POWER_MODE_T pwrmode)
{
   Ft_Gpu_HostCommand(host,pwrmode);
}
ft_void_t Ft_Gpu_CoreReset(Ft_Gpu_Hal_Context_t *host)
{
   Ft_Gpu_HostCommand(host,0x68);
}


ft_void_t Ft_Gpu_Hal_Updatecmdfifo(Ft_Gpu_Hal_Context_t *host,ft_uint16_t count)
{
	host->ft_cmd_fifo_wp  = (host->ft_cmd_fifo_wp + count) & 4095;

	//4 byte alignment
	host->ft_cmd_fifo_wp = (host->ft_cmd_fifo_wp + 3) & 0xffc;
	Ft_Gpu_Hal_Wr16(host,REG_CMD_WRITE,host->ft_cmd_fifo_wp);
}


ft_uint16_t Ft_Gpu_Cmdfifo_Freespace(Ft_Gpu_Hal_Context_t *host)
{
	ft_uint16_t fullness,retval;

	fullness = (host->ft_cmd_fifo_wp - Ft_Gpu_Hal_Rd16(host,REG_CMD_READ)) & 4095;
	retval = (FT_CMD_FIFO_SIZE - 4) - fullness;
	return (retval);
}

ft_void_t Ft_Gpu_Hal_WrCmdBuf(Ft_Gpu_Hal_Context_t *host,ft_uint8_t *buffer,ft_uint16_t count)
{
	ft_uint32_t length =0, SizeTransfered = 0;   

#define MAX_CMD_FIFO_TRANSFER   Ft_Gpu_Cmdfifo_Freespace(host)  
	do {                
		length = count;
		if (length > MAX_CMD_FIFO_TRANSFER){
		    length = MAX_CMD_FIFO_TRANSFER;
		}
      	        Ft_Gpu_Hal_CheckCmdBuffer(host,length);

                Ft_Gpu_Hal_StartCmdTransfer(host,FT_GPU_WRITE,length);

#ifdef ARDUINO_PLATFORM_SPI
                SizeTransfered = 0;
		while (length--) {
                    Ft_Gpu_Hal_Transfer8(host,*buffer);
		    buffer++;
                    SizeTransfered ++;
		}
                length = SizeTransfered;
#endif

#ifdef MSVC_PLATFORM_SPI
		{   
		    SPI_Write(host->hal_handle,buffer,length,&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
                    length = SizeTransfered;
   		    buffer += SizeTransfered;
		}
#endif

		Ft_Gpu_Hal_EndTransfer(host);
		Ft_Gpu_Hal_Updatecmdfifo(host,length);

		Ft_Gpu_Hal_WaitCmdfifo_empty(host);

		count -= length;
	}while (count > 0);
}

ft_void_t Ft_Gpu_Hal_WrCmdBuf_nowait(Ft_Gpu_Hal_Context_t *host,ft_uint8_t *buffer,ft_uint16_t count)
{
	ft_uint32_t length =0, SizeTransfered = 0;   

#define MAX_CMD_FIFO_TRANSFER   Ft_Gpu_Cmdfifo_Freespace(host)  

	do {                
		length = count;
		if (length > MAX_CMD_FIFO_TRANSFER){
		    length = MAX_CMD_FIFO_TRANSFER;
		}
        	Ft_Gpu_Hal_CheckCmdBuffer(host,length);

                Ft_Gpu_Hal_StartCmdTransfer(host,FT_GPU_WRITE,length);

#ifdef ARDUINO_PLATFORM_SPI
                SizeTransfered = 0;
		while (length--) {
                    Ft_Gpu_Hal_Transfer8(host,*buffer);
		    buffer++;
                    SizeTransfered ++;
		}
                length = SizeTransfered;
#endif

#ifdef MSVC_PLATFORM_SPI
		{   
		    SPI_Write(host->hal_handle,buffer,length,&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
                    length = SizeTransfered;
   		    buffer += SizeTransfered;
		}
#endif

		Ft_Gpu_Hal_EndTransfer(host);
		Ft_Gpu_Hal_Updatecmdfifo(host,length);                
		count -= length;
	}while (count > 0);
}







#ifdef ARDUINO_PLATFORM_SPI
ft_void_t Ft_Gpu_Hal_WrCmdBufFromFlash(Ft_Gpu_Hal_Context_t *host,FT_PROGMEM ft_prog_uchar8_t *buffer,ft_uint16_t count)
{
	ft_uint32_t length =0, SizeTransfered = 0;   

#define MAX_CMD_FIFO_TRANSFER   Ft_Gpu_Cmdfifo_Freespace(host)  
	do {                
		length = count;
		if (length > MAX_CMD_FIFO_TRANSFER){
		    length = MAX_CMD_FIFO_TRANSFER;
		}
      	        Ft_Gpu_Hal_CheckCmdBuffer(host,length);

                Ft_Gpu_Hal_StartCmdTransfer(host,FT_GPU_WRITE,length);


                SizeTransfered = 0;
		while (length--) {
                    Ft_Gpu_Hal_Transfer8(host,ft_pgm_read_byte_near(buffer));
		    buffer++;
                    SizeTransfered ++;
		}
                length = SizeTransfered;

    	        Ft_Gpu_Hal_EndTransfer(host);
		Ft_Gpu_Hal_Updatecmdfifo(host,length);

		Ft_Gpu_Hal_WaitCmdfifo_empty(host);

		count -= length;
	}while (count > 0);
}
#endif


ft_void_t Ft_Gpu_Hal_CheckCmdBuffer(Ft_Gpu_Hal_Context_t *host,ft_uint16_t count)
{
   ft_uint16_t getfreespace;
   do{
        getfreespace = Ft_Gpu_Cmdfifo_Freespace(host);
   }while(getfreespace < count);
}
ft_void_t Ft_Gpu_Hal_WaitCmdfifo_empty(Ft_Gpu_Hal_Context_t *host)
{
   while(Ft_Gpu_Hal_Rd16(host,REG_CMD_READ) != Ft_Gpu_Hal_Rd16(host,REG_CMD_WRITE));   
   host->ft_cmd_fifo_wp = Ft_Gpu_Hal_Rd16(host,REG_CMD_WRITE);
}

ft_uint8_t Ft_Gpu_Hal_WaitCmdfifo_empty_status(Ft_Gpu_Hal_Context_t *host)
{
   if(Ft_Gpu_Hal_Rd16(host,REG_CMD_READ) != Ft_Gpu_Hal_Rd16(host,REG_CMD_WRITE))
   {
     return 0;
   }
   else
   {
     host->ft_cmd_fifo_wp = Ft_Gpu_Hal_Rd16(host,REG_CMD_WRITE);
     return 1;
   }  
}

ft_void_t Ft_Gpu_Hal_WaitLogo_Finish(Ft_Gpu_Hal_Context_t *host)
{
    ft_int16_t cmdrdptr,cmdwrptr;

    do{
         cmdrdptr = Ft_Gpu_Hal_Rd16(host,REG_CMD_READ);
         cmdwrptr = Ft_Gpu_Hal_Rd16(host,REG_CMD_WRITE);
    }while ((cmdwrptr != cmdrdptr) || (cmdrdptr != 0));
    host->ft_cmd_fifo_wp = 0;
}


ft_void_t Ft_Gpu_Hal_ResetCmdFifo(Ft_Gpu_Hal_Context_t *host)
{
   host->ft_cmd_fifo_wp = 0;
}


ft_void_t Ft_Gpu_Hal_WrCmd32(Ft_Gpu_Hal_Context_t *host,ft_uint32_t cmd)
{
         Ft_Gpu_Hal_CheckCmdBuffer(host,sizeof(cmd));
      
         Ft_Gpu_Hal_Wr32(host,RAM_CMD + host->ft_cmd_fifo_wp,cmd);
      
         Ft_Gpu_Hal_Updatecmdfifo(host,sizeof(cmd));
}


ft_void_t Ft_Gpu_Hal_ResetDLBuffer(Ft_Gpu_Hal_Context_t *host)
{
           host->ft_dl_buff_wp = 0;
}
/* Toggle PD_N pin of FT800 board for a power cycle*/
ft_void_t Ft_Gpu_Hal_Powercycle(Ft_Gpu_Hal_Context_t *host, ft_bool_t up)
{
	if (up)
	{
#ifdef MSVC_PLATFORM
            FT_WriteGPIO(host->hal_handle, 0xBB, 0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
            Ft_Gpu_Hal_Sleep(20);

            FT_WriteGPIO(host->hal_handle, 0xBB, 0x88);//PDN set to 1
            Ft_Gpu_Hal_Sleep(20);
#endif
#ifdef ARDUINO_PLATFORM      
            digitalWrite(FT800_PD_N, LOW);
            Ft_Gpu_Hal_Sleep(20);

            digitalWrite(FT800_PD_N, HIGH);
            Ft_Gpu_Hal_Sleep(20);
#endif
	}else
	{
#ifdef MSVC_PLATFORM
	        FT_WriteGPIO(host->hal_handle, 0xBB, 0x88);//PDN set to 1
            Ft_Gpu_Hal_Sleep(20);
            
            FT_WriteGPIO(host->hal_handle, 0xBB, 0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
            Ft_Gpu_Hal_Sleep(20);
#endif
#ifdef ARDUINO_PLATFORM
            digitalWrite(FT800_PD_N, HIGH);
            Ft_Gpu_Hal_Sleep(20);
            
            digitalWrite(FT800_PD_N, LOW);
            Ft_Gpu_Hal_Sleep(20);
#endif
	}
}
ft_void_t Ft_Gpu_Hal_WrMemFromFlash(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr,const ft_prog_uchar8_t *buffer, ft_uint32_t length)
{
	ft_uint32_t SizeTransfered = 0;      

	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_WRITE,addr);

#ifdef ARDUINO_PLATFORM
	while (length--) {
            Ft_Gpu_Hal_Transfer8(host,ft_pgm_read_byte_near(buffer));
	    buffer++;
	}
#endif

#ifdef MSVC_PLATFORM_SPI
	{
	    SPI_Write((FT_HANDLE)host->hal_handle,buffer,length,&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}
#endif


	Ft_Gpu_Hal_EndTransfer(host);
}

ft_void_t Ft_Gpu_Hal_WrMem(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr,const ft_uint8_t *buffer, ft_uint32_t length)
{
	ft_uint32_t SizeTransfered = 0;      

	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_WRITE,addr);

#ifdef ARDUINO_PLATFORM
	while (length--) {
            Ft_Gpu_Hal_Transfer8(host,*buffer);
	    buffer++;
	}
#endif

#ifdef MSVC_PLATFORM_SPI
	{
	    SPI_Write((FT_HANDLE)host->hal_handle,buffer,length,&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}
#endif


	Ft_Gpu_Hal_EndTransfer(host);
}


ft_void_t Ft_Gpu_Hal_RdMem(Ft_Gpu_Hal_Context_t *host,ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length)
{
	ft_uint32_t SizeTransfered = 0;      

	Ft_Gpu_Hal_StartTransfer(host,FT_GPU_READ,addr);

#ifdef ARDUINO_PLATFORM
	while (length--) {
	   *buffer = Ft_Gpu_Hal_Transfer8(host,0);
	   buffer++;
	}
#endif

#ifdef MSVC_PLATFORM_SPI
	{
	   SPI_Read((FT_HANDLE)host->hal_handle,buffer,length,&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	}
#endif

	Ft_Gpu_Hal_EndTransfer(host);
}

ft_int32_t Ft_Gpu_Hal_Dec2Ascii(ft_char8_t *pSrc,ft_int32_t value)
{
	ft_int16_t Length;
	ft_char8_t *pdst,charval;
	ft_int32_t CurrVal = value,tmpval,i;
	ft_char8_t tmparray[16],idx = 0;

	Length = strlen(pSrc);
	pdst = pSrc + Length;

	if(0 == value)
	{
		*pdst++ = '0';
		*pdst++ = '\0';
		return 0;
	}

	if(CurrVal < 0)
	{
		*pdst++ = '-';
		CurrVal = - CurrVal;
	}
	/* insert the value */
	while(CurrVal > 0){
		tmpval = CurrVal;
		CurrVal /= 10;
		tmpval = tmpval - CurrVal*10;
		charval = '0' + tmpval;
		tmparray[idx++] = charval;
	}

	for(i=0;i<idx;i++)
	{
		*pdst++ = tmparray[idx - i - 1];
	}
	*pdst++ = '\0';

	return 0;
}



ft_void_t Ft_Gpu_Hal_Sleep(ft_uint16_t ms)
{
#ifdef MSVC_PLATFORM
	Sleep(ms);
#endif
#ifdef ARDUINO_PLATFORM
	delay(ms);
#endif
}






