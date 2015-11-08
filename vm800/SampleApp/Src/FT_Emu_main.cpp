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
0.2 - date 2013.08.19 - few minor edits

*/

#include "FT_Platform.h"

#ifdef MSVC_FT800EMU
#include <FT_Emulator.h>
#include <FT_Emulator_spi_i2c.h>




extern "C" void setup();
extern "C" void loop();

ft_int32_t main(ft_int32_t argc,ft_char8_t *argv[])
{
	FT800EMU::EmulatorParameters params;
	params.Setup = setup;
	params.Loop = loop;
	params.Flags = 
		FT800EMU::EmulatorEnableKeyboard 
		| FT800EMU::EmulatorEnableMouse 
		| FT800EMU::EmulatorEnableAudio 
		| FT800EMU::EmulatorEnableDebugShortkeys
		| FT800EMU::EmulatorEnableRegRotate
	    | FT800EMU::EmulatorEnableCoprocessor
		| FT800EMU::EmulatorEnableGraphicsMultithread
		| FT800EMU::EmulatorEnableRegPwmDutyEmulation
#if defined FT_801_ENABLE
	    | FT800EMU::EmulatorFT801
#endif
		;
	FT800EMU::Emulator.run(params);
	//FT800EMU::SPII2C.end();
}

void Ft_GpuEmu_SPII2C_begin()
{
	FT800EMU::SPII2C.begin();
}

void Ft_GpuEmu_SPII2C_csLow()
{
	FT800EMU::SPII2C.csLow();
}

void Ft_GpuEmu_SPII2C_csHigh()
{
	FT800EMU::SPII2C.csHigh();
}

void Ft_GpuEmu_SPII2C_end()
{
	FT800EMU::SPII2C.end();
}

uint8_t Ft_GpuEmu_SPII2C_transfer(uint8_t data)
{
	return FT800EMU::SPII2C.transfer(data);
}


void  Ft_GpuEmu_SPII2C_StartRead(uint32_t addr)
{
	Ft_GpuEmu_SPII2C_csLow();
	Ft_GpuEmu_SPII2C_transfer((addr >> 16) & 0xFF);
	Ft_GpuEmu_SPII2C_transfer((addr >> 8) & 0xFF);
	Ft_GpuEmu_SPII2C_transfer(addr & 0xFF);

	Ft_GpuEmu_SPII2C_transfer(0); //Dummy Read Byte
}

void  Ft_GpuEmu_SPII2C_StartWrite(uint32_t addr)
{
	Ft_GpuEmu_SPII2C_csLow();
	Ft_GpuEmu_SPII2C_transfer(((addr >> 16) & 0xFF) | 0x80);
	Ft_GpuEmu_SPII2C_transfer((addr >> 8) & 0xFF);
	Ft_GpuEmu_SPII2C_transfer(addr & 0xFF);
	
}
#endif