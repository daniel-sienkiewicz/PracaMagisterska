#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string.h>
 
int WindowHeight = 500;
int WindowWidth = 800;

struct Button{
	int   x;							/* top left x coord of the button */
	int   y;							/* top left y coord of the button */
	int   w;							/* the width of the button */
	int   h;							/* the height of the button */
	char* label;						/* the text label of the button */
};
typedef struct Button Button;

Button MyButton = {10, 450, 200, 40, "Button"};

void printtext(int x, int y, char* String, int len){
	int i;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WindowWidth, 0, WindowHeight, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(x,y);
    
    for(i = 0; i < len; i++){
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, String[i]);
    }

    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void ButtonDraw(Button *b){
	glColor3f(0.4f,0.4f,0.4f);

		glBegin(GL_LINE_STRIP);
			glVertex2i( b->x     , b->y+b->h );
			glVertex2i( b->x+b->w, b->y+b->h );
			glVertex2i( b->x+b->w, b->y      );
		glEnd();

		glLineWidth(1);

		glColor3f(1,1,1);
		printf("dddd");
		printtext(b->x, b->y, b->label, strlen(b->label));
}
 
void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
	glEnable(GL_DEPTH_TEST);
 
    char welcome[21];
    char temp1[12];
    char temp2[12];
    char temp3[12];
    char gps[4];
    char v[2];
    char s[2];
    sprintf(welcome, "Lurch Computer Borad");
    sprintf(temp1, "Temperatura");
    sprintf(temp2, "Temperatura");
    sprintf(temp3, "Temperatura");
    sprintf(gps, "GPS"); 
    sprintf(v, "V"); 
    sprintf(s, "S"); 
    ButtonDraw(&MyButton);
    printtext(10, 430, temp1, strlen(temp1));
    printtext(10, 400, temp2, strlen(temp2));
    printtext(10, 370, temp3, strlen(temp3));
    printtext(600, 430, v, strlen(v));
    printtext(600, 400, s, strlen(s));
    printtext(10, 480, welcome, strlen(welcome));
 	
	glutSwapBuffers();
}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("OpenGL");
 
	glutDisplayFunc(display);
 
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(70, 1, 1, 100);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 
	gluLookAt(2, 2, 10, 2, 0, 0, 0, 1, 0);
	glutMainLoop();
	return 0;
}