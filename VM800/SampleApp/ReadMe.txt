; Copyright (c) Future Technology Devices International 2013


; THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED ``AS IS'' AND ANY EXPRESS
; OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
; FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED
; BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
; BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGE.

Objective: 
==========
This package contains the FT800 sample applications targeting Windows PC and Arduino platform.  
The sample application created for tutorial purpose to smooth the learning curve of FT800. 
The users are encouraged to read the source codes first and change the functions 
you would like to experiment.



Release package contents:
=========================
The folder structure is shown as below.
+---Bin
|   \---Msvc_win32
|   	\--libMPSSE.a - MPSSE library file.
|   	\--ftd2xx.dll - ftd2xx library file.
|
|   \---MSVC_FT800Emu
|       \--ft800emu.lib - FT800 Emulator library in release mode
|       \--ft800emud.ib - FT800 Emulator library in debug mode
|
+---Hdr
|   \---\FT_CoPro_Cmds.h  - Includes the FT800 Coprocessor commands.
|   \---\FT_DataTypes.h - Includes the FT800 datatypes.
|   \---\FT_Gpu.h - Includes the FT800 commands.
|   \---\FT_Gpu_Hal.h - Includes the FT800 HAL(Hardware Abstraction Layer) commands.
|   \---\FT_Hal_Utils.h - Includes the HAL utilities.
|   \---\FT_Platform.h - Includes Platform specific macros.
|
|   \---Msvc_win32
|   	\--ftd2xx.h - ftd2xx header file
|   	\--libMPSSE_spi.h - MPSSE SPI library header file
|
|   \---Msvc_FT800Emu
|   	\--ft800emu_emulator.h - FT800 emulator header file
|   	\--ft800emu_inttypes.h - FT800 emulator type definition file
|       \--FT800Emu_main.h     - C interface for FT800 emulator SPII2C module
|       \--FT800Emu_spi_i2c.h  - FT800 emulator SPII2C module
|
+---Project
|   \---Arduino
|       \---SampleApp
| 		    \---SampleApp.ino - Sketch file of sample application
|   	    \---SampleApp.h - Header file for sample application
|   	    \---SampleApp_RawData.cpp - The source file for data used in Sample Application
|    		\---FT_CoPro_Cmds.cpp - Coprocessor commands source file.
|   		\---FT_CoPro_Cmds.h  - Includes the Coprocessor commands.
|  	     	\---FT_DataTypes.h - Includes the FT800 datatypes.
|   		\---FT_Gpu.h - Includes the Gpu commands.
|   		\---FT_Gpu_Hal.cpp - GPU HAL source.
|   		\---FT_Gpu_Hal.h - Includes the GPU HAL commands.
|   		\---FT_Hal_Utils.h - Includes the HAL utilities.
|   		\---FT_Platform.h - Includes Platform specific commands.
|
|   \---Msvc_win32
|       \---SampleApp 
|           \---SampleApp.sln - Sample Application solution file for MSVC IDE
|		    \---SampleApp.vcxproj - Sample Application project file for MSVC IDE 
|
|   \---Msvc_FT800Emu
|       \---SampleApp 
|           \---SampleApp.sln - Sample Application solution file for MSVC IDE(FT800 emulator specific)
|		    \---SampleApp.vcxproj - Sample Application project file for MSVC IDE(FT800 emulator specific) 
+---Src
|   	\---FT_CoPro_Cmds.c - Coprocessor commands source file.
|   	\---FT_Gpu_Hal.c - Gpu HAL source file.
|   	\---FT800Emu_main.cpp - The C wrapper of SPI/I2C interface of FT800 emulator and the main() function.
|       \---SampleApp.c - The API implementation of FT800 sample application
|       \-- SampleApp_RawData.c - The data definition used in sampleApp.c
|
+--Test - folder containing input test files such as .wav, .jpg, .raw etc
---Readme.txt - This file


Configuration Instructions:
===========================
This section contains details regarding various configurations supported by this software.

Two display profiles are supported by this software: (1) WQVGA (480x272) (2) QVGA (320x240)

By default WQVGA display profile is enabled. To enable QVGA, disable macro "SAMAPP_DISPLAY_QVGA" and 
enable macro "SAMAPP_DISPLAY_WQVGA" in respective FT_Platform.h file. To enable WQVGA support, please do vice versa. 

For Arduino platform, please check the source code under "\Project\Arduino" only. Please override the other code.

Installation Instruction:
=========================
The MSVC project file is compatible with Microsoft visual C++ 2012 Express.
Please open the respective sampleApp.sln under project for MSVC(MPSSE) platform and MSVC FT800 platform.

The Arduino sketch file is compatible with Arduino 1.0.5.


Reference Information:
======================
1. AN_245_VM800CB_SampleApp_PC_Introduction.pdf
2. AN_246_VM800CB_SampleApp_Arduino_Introduction.pdf

Extra Information:
==================
1. Sampel Applications for Windows PC and Arduino platform shares share the same code base. But Windows PC has 
more APIs definition since it has no storage limitation. 



Known issues and limitations:
=============================
1. Will work only on SPI mode.
2. The SPI host(Arduino, Windows PC) are assuming the data layout in memory as Little Endian format. 
3. Cannot compile and build MSVC project using the version under Microsoft visual C++ 2012 Express.


Release Notes (Version Significance):
=====================================
1.0.   First formal release
1.0.2  Fix the issue in "Ft_Gpu_CoCmd_Button" 
1.0.3  Add the support of FT800 Emulator, remove Doc folder , new readme.txt
1.0.4  Change the output directory of executable for FT800 Emulator based project
1.0.5  Update emulator library with Audio enabled
1.0.6 - Jun/23/2014 - Added multitouch sets and FT801 emulation support. 

