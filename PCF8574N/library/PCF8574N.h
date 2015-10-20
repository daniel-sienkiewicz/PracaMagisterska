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
 * @date 18 October 2015
 */
 
#ifndef _PCF8574N_H
#define _PCF8574N_H
class PCF8574N{
  public:
	PCF8574N(char address, int sda, int scl); 

	int read8();
	void write8(int data);
  
  private:
	void init(int sda, int scl);
	char _address;
	int _sda;
	int _scl;
};

#endif