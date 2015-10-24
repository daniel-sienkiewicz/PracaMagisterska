/****************************************************************************
 * Copyright (C) 2015 by Daniel Sienkiewicz                                 *
 *                                                                          *
 *   PCF8574N is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   PFC8574N is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with Box. If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

/**
 * @file PFC8574N.H
 * @author Daniel Sienkiewicz
 * @date 24 October 2015
 */
 
#ifndef _PCF8574N_H
#define _PCF8574N_H
class PCF8574N{
  public:
  
	/**
	******************************************************************************
	* @details        Object constrctor - creating new PCF object 			     *
	*				  Set sda and scl line as inactive						     *
	* @param          address The address of PCF8574N I/O Expander				 *
	* @param		  sda, scl line using to comminication with PCF				 *
	******************************************************************************
	*/
	PCF8574N(char address, int sda, int scl); 

	/**
	******************************************************************************
	* @details        Object destrctor - deleting PCF object 				     *
	*			      Set sda and scl line as inactive						     *
	******************************************************************************
	*/
	~PCF8574N(char address, int sda, int scl);
	
	/**
	******************************************************************************
	* @details        Reading value from PCF8574N I/O Expander                   *
	* @return         Value from the specified PCF8574N I/O Expander             *
	******************************************************************************
	*/
	int read8(void);
	
	/**
	******************************************************************************
	* @details        Writing value to PCF8574N I/O Expander                     *
	******************************************************************************
	*/
	void write8(int data);
 
  private:
	char _address;
	int _sda;
	int _scl;
};

#endif