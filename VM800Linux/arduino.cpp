#include "arduino.h"

void digitalWrite(int portNr, int data){
	char* fullPath;
	FILE *fp;
	const char * path = "/sys/class/gpio";
	fullPath = (char*)malloc(strlen(path)+1+11);
	sprintf(fullPath, "%s/gpio%d/value", path, portNr);
	fp = fopen(fullPath, "w");
	
	if (fp == NULL){
		printf("File doesn't exist: %s\n", fullPath);
		printf("Data to write: %d\n", data);
	} else{
		fprintf(fp, "%d", data);
	}
	
	fclose(fp);
}

void pinMode(int portNr, int type){
	char* fullPath;
	FILE *fp;
	const char * path = "/sys/class/gpio";
	fullPath = (char*)malloc(strlen(path)+1+15);
	sprintf(fullPath, "%s/gpio%d/direction", path, portNr);
	fp = fopen(fullPath, "w");
	if (fp == NULL){
		printf("File doesn't exist: %s\n", fullPath);
		printf("Type to write: %d\n", type);
	} else{
		if(type == 1)
			fprintf(fp, "%s", "in");
		else
			fprintf(fp, "%s", "out");
	}
	fclose(fp);
}

int digitalRead(int portNr){
	/*char* fullPath;
	int value;
	FILE *fp;
	const char * path = "/sys/class/gpio";
	fullPath = (char*)malloc(strlen(path)+1+11);
	sprintf(fullPath, "%s/gpio%d/value", path, portNr);
	fp = fopen(fullPath, "w");
	value = fscanf(fp, "%d", value);
	fclose(fp);

	return value;*/
}

float analogRead(int portNr){
	return 1/1.0;
}

void delay(int val){

}