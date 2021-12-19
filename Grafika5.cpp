#define _USE_MATH_DEFINES
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
using namespace std;

//zmienne ogolne
typedef float point3[3];
typedef float point9[9];
int testedObject = 4; //rysowany obiekt 
static int x_pos_old = 0;       // pozycje kursora myszy
static int delta_x = 0;
static int y_pos_old = 0;
static int delta_y = 0;
static GLfloat pix2angle;     // przelicznik pikseli na stopnie 
const char piramida[] = "D1_t.tga";//tekstury
const char jajko[] = "P3_t.tga";
int dopasowanie = 0;//tryb dopasowanie tektury dla jajka


//zmienne obiektu
int n = 50;//ilosc punktow jajka
float scale = 3.0;//wielkosc obiektu
point3** points;//siatka punktow
point3** vectors;//wektory punktow powierzchni jajka

//zmienne obrotu obiektu
static GLint statusMiddle = 0;//stan srdkowego klawisza
float rViewer = 10;//R wokol punktu obserwowanego 
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };//pozycja punktu widzenia
static GLfloat azymuth = 0;    //katy obserwacji punktu obserwowanego
static GLfloat elevation = 0;
static GLint statusLeft = 0;       // stan klawisza lewego
static GLint statusRight = 0;	   // stan klawisza prawego


void Axes(void)
{
	point3  x_min = { -50.0, 0.0, 0.0 };//wspolrzedne koncow osi
	point3  x_max = { 50.0, 0.0, 0.0 };
	point3  y_min = { 0.0, -50.0, 0.0 };
	point3  y_max = { 0.0,  50.0, 0.0 };
	point3  z_min = { 0.0, 0.0, -50.0 };
	point3  z_max = { 0.0, 0.0,  50.0 };

	glColor3f(1.0f, 0.0f, 0.0f);//os x
	glBegin(GL_LINES);
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);//os y
	glBegin(GL_LINES);
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);//os z
	glBegin(GL_LINES);
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

void egg() {
	float u = 0, v = 0;
	float diff = 1.0 / n; //n - liczba punktow na powierzchni jajka
	glTranslated(0, (-(160 * pow(0.5, 4) - 320 * pow(0.5, 3) + 160 * pow(0.5, 2)) / 2) * (scale + 7) / 10, 0);//obnizenie środka figury do centrum ukladu wspolrzednych

	for (int i = 0; i <= n; i++) {//punkty powierzchni
		v = 0;//obliczenie poteg w celu ulatwienia kodu
		float u2 = pow(u, 2);
		float u3 = pow(u, 3);
		float u4 = pow(u, 4);
		float u5 = pow(u, 5);

		for (int ii = 0; ii <= n; ii++) {//obliczenie wspolrzednych punktow
			points[i][ii][0] = ((-90 * u5 + 225 * u4 - 270 * u3 + 180 * u2 - 45 * u) * cos(M_PI * v)) * (scale + 7) / 10;
			points[i][ii][1] = (160 * u4 - 320 * u3 + 160 * u2) * (scale + 7) / 10;
			points[i][ii][2] = ((-90 * u5 + 225 * u4 - 270 * u3 + 180 * u2 - 45 * u) * sin(M_PI * v)) * (scale + 7) / 10;
			v = v + diff;
		}
		u = u + diff;
	}

	u = 0;
	for (int i = 0; i <= n; i++) {//wektory normalne
		v = 0;//obliczenie poteg w celu ulatwienia kodu
		float u2 = pow(u, 2);
		float u3 = pow(u, 3);
		float u4 = pow(u, 4);
		float u5 = pow(u, 5);

		for (int ii = 0; ii <= n; ii++) {
			point9 vector;
			vector[0] = (-450 * u4 + 900 * u3 - 810 * u2 + 360 * u - 45) * cos(M_PI * v);
			vector[1] = M_PI * (90 * u5 - 225 * u4 + 270 * u3 - 180 * u2 + 45) * sin(M_PI * v);
			vector[2] = 640 * u3 - 960 * u2 + 320 * u;
			vector[3] = 0;
			vector[4] = (-450 * u4 + 900 * u3 - 810 * u2 + 360 * u - 45) * sin(M_PI * v);
			vector[5] = -1 * M_PI * (90 * u5 - 225 * u4 + 270 * u3 - 180 * u2 + 45) * cos(M_PI * v);

			vector[6] = vector[2] * vector[5] - vector[4] * vector[3];//wektory 
			vector[7] = vector[4] * vector[1] - vector[0] * vector[5];
			vector[8] = vector[0] * vector[3] - vector[2] * vector[1];
			float vectorSize = sqrt(pow(vector[6], 2) + pow(vector[7], 2) + pow(vector[8], 2));//normalizacja wektora
			if (vectorSize == 0) {
				vectorSize = 1;
			}

			vectors[i][ii][0] = vector[6] / vectorSize;
			vectors[i][ii][1] = vector[7] / vectorSize;
			vectors[i][ii][2] = vector[8] / vectorSize;

			v = v + diff;
		}
		u = u + diff;
	}

	u = 0;
	for (int i = 0; i < n; i++) {//rysowanie 
		v = 0;
		for (int ii = 0; ii < n; ii++) {
			float pozycja[4];
			if (dopasowanie == 0) {
				pozycja[0] = u;
				pozycja[1] = u + diff;
				pozycja[2] = v;
				pozycja[3] = v + diff;
			}
			else {
				pozycja[0] = sin(M_PI * u);
				pozycja[1] = sin(M_PI * (u + diff));
				pozycja[2] = v;
				pozycja[3] = v + diff;
			}

			if (i < n / 2) {
				glCullFace(GL_BACK);
			}
			else {
				glCullFace(GL_FRONT);
			}
			glBegin(GL_TRIANGLES);//rysowanie pierwszego trojkata
			glNormal3fv(vectors[i][ii]);
			glTexCoord2f(pozycja[0], pozycja[2]);
			glVertex3f(points[i][ii][0], points[i][ii][1], points[i][ii][2]);
			glNormal3fv(vectors[i + 1][ii]);
			glTexCoord2f(pozycja[1], pozycja[2]);
			glVertex3f(points[i + 1][ii][0], points[i + 1][ii][1], points[i + 1][ii][2]);
			glNormal3fv(vectors[i + 1][ii + 1]);
			glTexCoord2f(pozycja[1], pozycja[3]);
			glVertex3f(points[i + 1][ii + 1][0], points[i + 1][ii + 1][1], points[i + 1][ii + 1][2]);
			glEnd();


			if (i < n / 2) {
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}
			glBegin(GL_TRIANGLES);//rysowanie drugiego trojkata
			glNormal3fv(vectors[i][ii]);
			glTexCoord2f(pozycja[0], pozycja[2]);
			glVertex3f(points[i][ii][0], points[i][ii][1], points[i][ii][2]);
			glNormal3fv(vectors[i][ii + 1]);
			glTexCoord2f(pozycja[0], pozycja[3]);
			glVertex3f(points[i][ii + 1][0], points[i][ii + 1][1], points[i][ii + 1][2]);
			glNormal3fv(vectors[i + 1][ii + 1]);
			glTexCoord2f(pozycja[1], pozycja[3]);
			glVertex3f(points[i + 1][ii + 1][0], points[i + 1][ii + 1][1], points[i + 1][ii + 1][2]);
			glEnd();
			v = v + diff;
		}
		u = u + diff;
	}
}

void triangle() {
	glBegin(GL_TRIANGLES);

	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(4, 0, 0);

	glNormal3f(0, 0, 1);
	glTexCoord2f(0.5f, 0.75f);
	glVertex3f(0, 6, 0);

	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-4, 0, 0);

	glEnd();
}

void piramid() {

	float triangleHeight = 2 * sqrt(3);
	point3 vector = { 0, 0.5, 0.5 * sqrt(3) };
	float piramidHeight = triangleHeight / 2 * sqrt(3);

	glCullFace(GL_FRONT);
	glBegin(GL_POLYGON);//podstawa
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2, -1, 2);

	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(2, -1, -2);

	glNormal3f(0, -1, 0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-2, -1, -2);

	glNormal3f(0, -1, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-2, -1, 2);
	glEnd();


	glBegin(GL_TRIANGLES);//trojkaty prawy
	glNormal3f(vector[0], vector[1], -1 * vector[2]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2, -1, -2);

	glNormal3f(vector[0], vector[1], -1 * vector[2]);
	glTexCoord2f(0.5f, triangleHeight);
	glVertex3f(0, piramidHeight, 0);

	glNormal3f(vector[0], vector[1], -1 * vector[2]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2, -1, -2);
	glEnd();


	glBegin(GL_TRIANGLES);//trojkaty lewy
	glNormal3f(vector[0], vector[1], vector[2]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-2, -1, 2);

	glNormal3f(vector[0], vector[1], vector[2]);
	glTexCoord2f(0.5f, triangleHeight);
	glVertex3f(0, piramidHeight, 0);

	glNormal3f(vector[0], vector[1], vector[2]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2, -1, 2);
	glEnd();


	glBegin(GL_TRIANGLES);//trojkaty przod
	glNormal3f(vector[2], vector[1], vector[0]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(2, -1, 2);

	glNormal3f(vector[2], vector[1], vector[0]);
	glTexCoord2f(0.5f, triangleHeight);
	glVertex3f(0, piramidHeight, 0);

	glNormal3f(vector[2], vector[1], vector[0]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2, -1, -2);
	glEnd();


	glCullFace(GL_BACK);
	glBegin(GL_TRIANGLES);//trojkaty tyl
	glNormal3f(-1 * vector[2], vector[1], vector[0]);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-2, -1, 2);

	glNormal3f(-1 * vector[2], vector[1], vector[0]);
	glTexCoord2f(0.5f, triangleHeight);
	glVertex3f(0, piramidHeight, 0);

	glNormal3f(-1 * vector[2], vector[1], vector[0]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-2, -1, -2);
	glEnd();
}

void zadanie() {
	if (statusMiddle == 1) {//obracanie obiektu
		elevation += 0.01 * delta_y * pix2angle;
		azymuth += 0.01 * delta_x * pix2angle;
	}

	if (statusRight == 1) {//zoom 
		if (delta_y > 0) {
			rViewer += 0.2;
		}
		else {
			rViewer -= 0.2;
		}
	}

	viewer[0] = rViewer * cos(azymuth) * cos(elevation);//obliczenie pozycji punktu widzenia
	viewer[1] = rViewer * sin(elevation);
	viewer[2] = rViewer * sin(azymuth) * cos(elevation);


	switch (testedObject) {//wybranie obiektu do wyswietlania
	case 1:
		glutSolidTeapot(scale);
		break;
	case 2:
		egg();
		break;
	case 3:
		triangle();
		break;
	case 4:
		piramid();
		break;
	}
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//deklaracja buforow
	glLoadIdentity();
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);//ustawienie pozycji punktu widzenia i pozycji punktu obserwowanego

	Axes();
	zadanie();

	glutSwapBuffers();//zmiana buforow i wyswietlanie
}

void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal prawy klawisz
		x_pos_old = x;
		y_pos_old = y;
		statusRight = 1;        //ustawienie flagi przycisku   
	}
	else {
		statusRight = 0;		//ustawienie flagi przycisku  
	}

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal lewy klawisz
		x_pos_old = x;
		y_pos_old = y;
		statusLeft = 1;    //ustawienie flagi przycisku        
	}
	else {
		statusLeft = 0;		//ustawienie flagi przycisku  
	}

	if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {//sprawdzenie czy przycisniety zostal lewy klawisz
		x_pos_old = x;
		y_pos_old = y;
		statusMiddle = 1;    //ustawienie flagi przycisku        
	}
	else {
		statusMiddle = 0;		//ustawienie flagi przycisku  
	}

	RenderScene();
}

void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;//zmiana pozycji x
	x_pos_old = x;

	delta_y = y - y_pos_old;//zmiana pozycji y
	y_pos_old = y;

	glutPostRedisplay();
}

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{

	/*************************************************************************************/

	// Struktura dla nag堯wka pliku  TGA


#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;


	/*************************************************************************************/

	// Wartoi domyne zwracane w przypadku b喚du

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

#pragma warning(suppress : 4996)
	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;

	/*************************************************************************************/
	// Przeczytanie nag堯wka pliku 


	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);


	/*************************************************************************************/

	// Odczytanie szerokoi, wysokoi i g喚bi obrazu

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;


	/*************************************************************************************/
	// Sprawdzenie, czy g喚bia spe軟ia za這穎ne warunki (8, 24, lub 32 bity)

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/

	// Obliczenie rozmiaru bufora w pami璚i


	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;


	/*************************************************************************************/

	// Alokacja pami璚i dla danych obrazu


	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}


	/*************************************************************************************/

	// Ustawienie formatu OpenGL


	switch (sDepth)

	{

	case 3:

		*ImFormat = GL_BGR_EXT;

		*ImComponents = GL_RGB8;

		break;

	case 4:

		*ImFormat = GL_BGRA_EXT;

		*ImComponents = GL_RGBA8;

		break;

	case 1:

		*ImFormat = GL_LUMINANCE;

		*ImComponents = GL_LUMINANCE8;

		break;

	};



	fclose(pFile);



	return pbitsperpixel;

}

void tesktura(const char plik[]) {//plik to nazwa wczytywanego pliku
	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	pBytes = LoadTGAImage(plik, &ImWidth, &ImHeight, &ImComponents, &ImFormat);//wczytanie tekstury
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);//zdefiniowanie tekstury
	free(pBytes);

	glEnable(GL_CULL_FACE);//uruchomienie teksturowania jednostronnego
	glCullFace(GL_FRONT);//uruchomienie teksturowania frontu
	glEnable(GL_TEXTURE_2D);//uruchomienie tekstur
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//tryb teksturowania
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//sposob nakladania tekstur
}

void MyInit()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	tesktura(piramida);

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };//okreslenie parametrow materialu
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess = { 20.0 };

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);//ustawienie parametrow materialu
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };//pocztkoawa pozycja punktow swietlnych

	GLfloat att_constant = { 1.0 };//okreslenia parametrow swiatla
	GLfloat att_linear = { 0.05 };
	GLfloat att_quadratic = { 0.001 };
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };


	GLfloat redlight_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };//parametry swiatla czerwonego
	GLfloat redlight_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat redLightPosition[] = { 0.0, 0.0, 10.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);//ustawienie GL_LIGHT0 na czerwone zrodlo swiatla
	glLightfv(GL_LIGHT0, GL_DIFFUSE, redlight_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, redlight_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, redLightPosition);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


	//uruchomienie oswietlenia
	glShadeModel(GL_SMOOTH); // wlaczenie lagodnego cieniowania
	glEnable(GL_LIGHTING);   // wlaczenie systemu oietlenia sceny
	glEnable(GL_LIGHT0);     // wlaczenie zrodla czerwonego
	glEnable(GL_DEPTH_TEST); // wlaczenie mechanizmu z-bufora


	/*************************************************************************************/

}

void keys(unsigned char key, int x, int y)
{
	if (key == 'c') {
		testedObject = 1;
	}
	if (key == 'd') {
		if (dopasowanie == 0) {
			dopasowanie = 1;
		}
		else {
			dopasowanie = 0;
		}
	}
	if (key == 'j') {
		tesktura(jajko);
		testedObject = 2;
	}
	if (key == 't') {
		tesktura(piramida);
		testedObject = 3;
	}
	if (key == 'p') {
		tesktura(piramida);
		testedObject = 4;
	}
	RenderScene();
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;//usatwienie wielkosci do obracania obiektu
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1.0, 1.0, 100000.0);

	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void main(void)
{
	srand(time(NULL));
	points = new  point3 * [n + 1];//tablica punktow
	vectors = new point3 * [n + 1];//tablice
	for (int i = 0; i <= n; i++) {
		points[i] = new point3[n + 1];
		vectors[i] = new point3[n + 1];
	}

	cout << "Obsluga programu:\np - piramida\nt - trojkat\nj - jajko\nlewy przycisk myszy - obrot\nsrodkowy przycisk myszy - zoom\nd - wlacz/wylacz dopasowanie tekstury" << endl;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Teksturowanie");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	MyInit();
	glutMouseFunc(Mouse);//"lapanie" akcji na przyciskach myszy
	glutMotionFunc(Motion);//"lapanie" ruchu myszki
	glutKeyboardFunc(keys);//"lapanie" akcji na klawiaturze
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}