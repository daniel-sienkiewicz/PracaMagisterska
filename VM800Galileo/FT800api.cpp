#include "FT800api.h"

void initScreen(){
  start(BLACK);
  text(80, 10, 31, 0, "Welcome");
  spinner(150, 150, 0, 1);
  show();
  mainScreen();
}

void autko(){
  Serial.println("Start Autko");
  line(WHITE, 1500, 1700, 3500, 1700, 50);
  line(WHITE, 1500, 1700, 1500, 2500, 50);
  line(WHITE, 3500, 1700, 3500, 2500, 50);
  line(WHITE, 1500, 2500, 3500, 2500, 50);
  
  if(audi->doors & 16)
    line(WHITE, 3500, 1700, 2600, 1400, 50);
  if(audi->doors & 8)
    line(WHITE, 3500, 2500, 2600, 2800, 50);
  if(audi->doors & 4)
    line(WHITE, 2600, 1700, 1500, 1400, 50);
  if(audi->doors & 2)
    line(WHITE, 2600, 2500, 1500, 2800, 50);
  if(audi->doors & 1){
    line(WHITE, 1500, 1700, 1200, 1900, 50);
    line(WHITE, 1500, 2500, 1200, 2300, 50);
    line(WHITE, 1200, 1900, 1200, 2300, 50);
  }
  if(audi->seatbelts)
    text(105, 120, 21, 0, "Fasten Seatbelts");
  if(audi->lights){
    text(250, 110, 21, 0, "Turn on");
    text(250, 130, 21, 0, "lights");
  }
    
  Serial.println("Koniec Autko");
}

void mainScreen(){
  char str[15];
  Serial.println("Start Mainscreen");
  start(BLACK);
  text(10, 10, 21, 0, "Temp Out:");
  number(80, 10, 21, 0, (int)(audi->tempOut)%100);
  text(10, 30, 21, 0, "Temp In:");
  number(70, 30, 21, 0, (int)(audi->tempIn)%100);
  text(10, 50, 21, 0, "Temp Engine:");
  number(99, 50, 21, 0, (int)(audi->tempEngine)%100);
  text(250, 10, 21, 0, "GPS:");
  text(200, 30, 21, 0, "54.360N 18.639E");
  button(10, 200, 130, 30, 28, 0, "Smart Mirror"); 
  button(200, 200, 110, 30, 28, 0, "Options");
  autko();
  show();
  Serial.println("Koniec Mainscreen");
}

void smartMirrorScreen(){
  Serial.println("Start SmartMirror");
  start(BLACK);
  text(10, 10, 21, 0, "Smart mirror");
  button(10, 200, 130, 30, 28, 0, "Back"); 
  show();
  Serial.println("Koniec SmartMirror");
}

void opctionsScreen(){
  Serial.println("Start Options");
  start(BLACK);
  text(10, 30, 21, 0, "Data format");
  switch(dataFormat){
    case 1: button(100, 30, 130, 25, 21, 0, "CSV");
            break;
    case 2: button(100, 30, 130, 25, 21, 0, "XML");
            break;
    case 3: button(100, 30, 130, 25, 21, 0, "JSON");
            break;
  }
  
  text(10, 70, 21, 0, "Saving data");
  if(!saveData)
    button(100, 70, 130, 25, 21, 0, "OK");
  else
    button(100, 70, 130, 25, 21, 0, "NO");
    
  text(10, 110, 21, 0, "Refresh time");
  switch(timeR){
    case 1: button(100, 110, 130, 25, 21, 0, "5s");
            break;
    case 2: button(100, 110, 130, 25, 21, 0, "10s");
            break;
    case 3: button(100, 110, 130, 25, 21, 0, "30s");
            break;
    case 4: button(100, 110, 130, 25, 21, 0, "1min");
            break;
    case 5: button(100, 110, 130, 25, 21, 0, "15min");
            break;
  }
   
  button(200, 200, 110, 30, 28, 0, "Calibrate");
  button(10, 200, 130, 30, 28, 0, "Back"); 
  show();
  Serial.println("Koniec Options");
}

void spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale){  
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
  
  Serial.println("Koniec Guzik");
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
  Serial.println("Koniec Text");
}

void number( int16_t x,  int16_t y, int16_t font, uint16_t options, int value){
  Serial.println("Start Number");
	
  ft800memWrite32(RAM_CMD+cmdOffset, (DL_BEGIN|CMD_NUMBER));
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)y << 16)|(x & 0xffff));
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)options << 16)|(font & 0xffff) );
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset, value);
  cmdOffset=incCMDOffset(cmdOffset, 4);
 
  Serial.println("Koniec Number");
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

void slider(unsigned long x, unsigned long y, unsigned long w, unsigned long h, uint16_t options, uint16_t val, uint16_t range){
  Serial.println("Start slider");
  
  ft800memWrite32(RAM_CMD + cmdOffset, (DL_BEGIN|CMD_SLIDER));
  cmdOffset=incCMDOffset(cmdOffset,4);
  ft800memWrite32(RAM_CMD + cmdOffset, ((uint32_t)y << 16) | (x & 0xffff));
  cmdOffset=incCMDOffset(cmdOffset,4);
  ft800memWrite32(RAM_CMD + cmdOffset, ((uint32_t)h << 16) | (w & 0xffff));
  cmdOffset=incCMDOffset(cmdOffset,4);
  ft800memWrite32(RAM_CMD + cmdOffset, ((uint32_t)val << 16) | (options & 0xffff));
  cmdOffset=incCMDOffset(cmdOffset,4);
  ft800memWrite32(RAM_CMD + cmdOffset, (uint32_t)range);
  cmdOffset=incCMDOffset(cmdOffset,4);
  
  Serial.println("Koniec Slider");
}

void calibrate(){
  Serial.println("Start Calibrate");
  
  start(BLACK);
  ft800memWrite32(RAM_CMD + cmdOffset, (DL_BEGIN|CMD_CALIBRATE));
  cmdOffset=incCMDOffset(cmdOffset,4);
  show();
  
  Serial.println("Koniec Calibrate"); 
}

void start(unsigned long color){
  Serial.println("Start Start");
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
  Serial.println("Koniec Start");
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
