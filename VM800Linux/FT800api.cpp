#include "FT800api.h"

void initScreen(){
  start(BLACK);
  text(80, 10, 31, 0, "Welcome");
  spinner(150, 150, 0, 1);
  show();
  mainScreen();
}

void autko(){
  printf("Start Autko\n");
  
  printf("Koniec Autko\n");
}

void mainScreen(){
  char str[15];
  printf("Start Mainscreen\n");
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
  button(200, 200, 110, 30, 28, 0, "Save data");
  autko();
  show();
  printf("Koniec Mainscreen\n");
}

void smartMirrorScreen(){
  printf("Start SmartMirror\n");
  start(BLACK);
  button(10, 200, 130, 30, 28, 0, "Back"); 
  button(200, 200, 110, 30, 28, 0, "Save data");
  show();
  printf("Koniec SmartMirror\n");
}

void spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale){  
   printf("Start Spinner\n");  
   ft800memWrite32(RAM_CMD+cmdOffset, CMD_SPINNER);
   cmdOffset=incCMDOffset(cmdOffset, 4);
   ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)y<<16)|(x & 0xffff) );
   cmdOffset=incCMDOffset(cmdOffset, 4);
   ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)scale<<16)|style );
   cmdOffset=incCMDOffset(cmdOffset, 4);
   printf("Koniec Spinner\n");
}

void button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str){
  printf("Start Guzik\n");
  uint16_t i, j, q = 0;
  const uint16_t length = strlen(str);
  if(length == 0) return ;	
	
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
  printf("Koniec Guzik\n");
}

void text( int16_t x,  int16_t y, int16_t font, uint16_t options, const char* str){
  printf("Start Text\n");
  uint16_t i, j, q = 0;
  const uint16_t length = strlen(str);
  if(length == 0) return ;	
	
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
  printf("Koniec Text\n");
}

void number( int16_t x,  int16_t y, int16_t font, uint16_t options, int value){
  printf("Start Number\n");
	
  ft800memWrite32(RAM_CMD+cmdOffset, (DL_BEGIN|CMD_NUMBER));
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)y << 16)|(x & 0xffff));
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset, ((uint32_t)options << 16)|(font & 0xffff) );
  cmdOffset=incCMDOffset(cmdOffset, 4);
  ft800memWrite32(RAM_CMD+cmdOffset, value);
  cmdOffset=incCMDOffset(cmdOffset, 4);
 
  printf("Koniec Number\n");
}

void line(unsigned long color, unsigned long line_x1, unsigned long line_y1, unsigned long line_x2, unsigned long line_y2, unsigned long width){
  printf("Start Linia\n");

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

  printf("Koniec Linia\n");
}

void dot(unsigned long color, unsigned int point_size, unsigned long point_x, unsigned long point_y){ 
 printf("Start Kropka\n"); 

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_POINT_SIZE|point_size));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_BEGIN|FTPOINTS));
  cmdOffset=incCMDOffset(cmdOffset, 4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_COLOR_RGB|color));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_VERTEX2F|(point_x<<15)|point_y));
  cmdOffset=incCMDOffset(cmdOffset,4);

  printf("Koniec Kropka\n");
}

void start(unsigned long color){
  printf("Start Start\n");
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
  printf("Koniec Start\n");
}

void show(){
  printf("Start Wyswietlanie\n");
  ft800memWrite32(RAM_CMD+cmdOffset, (DL_END));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (DL_DISPLAY));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite32(RAM_CMD+cmdOffset, (CMD_SWAP));
  cmdOffset=incCMDOffset(cmdOffset,4);

  ft800memWrite16(REG_CMD_WRITE, (cmdOffset));
  printf("Koniec Wyswietlanie\n");
}
