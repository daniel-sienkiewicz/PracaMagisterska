#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string.h>
 
int WindowHeight = 500;
int WindowWidth = 800;

struct Button{
	float x;
	float y;
	int w;
	int h;
	char* label;
};
typedef struct Button Button;

Button MyButton = {0, 0, 200, 40, "Button"};

void printtext(float x, float y, char* String){
	int i;
    
	glMatrixMode( GL_PROJECTION ) ;
	glPushMatrix() ; // save
	glLoadIdentity();// and clear
	glMatrixMode( GL_MODELVIEW ) ;
	glPushMatrix() ;
	glLoadIdentity() ;
	glDisable( GL_DEPTH_TEST ) ; // also disable the depth test so renders on top
	glRasterPos2f(0, 0) ; // center of screen. (-1,0) is center left.
    
    for(i = 0; i < strlen(String); i++){
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, String[i]);
    }

    glEnable( GL_DEPTH_TEST ) ; // Turn depth testing back on

	glMatrixMode( GL_PROJECTION ) ;
	glPopMatrix() ; // revert back to the matrix I had before.
	glMatrixMode( GL_MODELVIEW ) ;
	glPopMatrix() ;
}

void ButtonDraw(Button *b){
		printtext(b->x, b->y, b->label);
}

void onMouseButton(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    	printf("Mysz guzik: %i %i\n", x, y);
		printtext(10, 430, "Temperatura");
    	printtext(10, 400, "Temperatura");
    	printtext(10, 370, "Temperatura");
    	printtext(600, 430, "V");
    	printtext(600, 400, "S");
    	printtext(10, 480, "Lurch Computer Borad");
    	glutSwapBuffers();
	}else{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glutSwapBuffers();
	}
}
 
void onMouseMotion(int x, int y){
    printf("Mysz przeciagniecie: %i %i\n", x, y);
}

void MousePassiveMotion(int x, int y){
	 printf("Mysz: %i %i\n", x, y);
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ButtonDraw(&MyButton);
	glutSwapBuffers();
}


int main(int argc, char *argv[]){
	printf("Zaczynamy zabawe\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("OpenGL");
	glutDisplayFunc(display);
 	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glutMainLoop();
	return 0;
}