// Gl_template.c
//Wy³šczanie b³êdów przed "fopen"
#define  _CRT_SECURE_NO_WARNINGS



// £adowanie bibliotek:

#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif




// Ustalanie trybu tekstowego:
#ifdef _MSC_VER        // Check if MS Visual C compiler
#   ifndef _MBCS
#      define _MBCS    // Uses Multi-byte character set
#   endif
#   ifdef _UNICODE     // Not using Unicode character set
#      undef _UNICODE 
#   endif
#   ifdef UNICODE
#      undef UNICODE 
#   endif
#endif
#include <windows.h>            // Window defines
#include <gl\gl.h>              // OpenGL
#include <gl\glu.h>             // GLU library
#include <math.h>				// Define for sqrt
#include <stdio.h>
#include "resource.h"           // About box resource identifiers.


#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)
#define BITMAP_ID 0x4D42		// identyfikator formatu BMP
#define GL_PI 3.14

//--------------MOJE KLASY--------------------//


//<----------------------------------------------------------PROTOTYPY FUNKCJI
void kolo(GLfloat x, GLfloat y, GLfloat z, GLfloat srednicaXZ, int red, int green, int blue);

//<-----------------------------------------------------------Koniec Prototypy
// Color Palette handle
HPALETTE hPalette = NULL;
int licznik = 0;
float sinus = 0, cosinus = 0;

// Application name and instance storeage
static LPCTSTR lpszAppName = "GL Template";
static HINSTANCE hInstance;

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


static GLsizei lastHeight;
static GLsizei lastWidth;

// Opis tekstury
BITMAPINFOHEADER	bitmapInfoHeader;	// nag³ówek obrazu
unsigned char*		bitmapData;			// dane tekstury
unsigned int		texture[2];			// obiekt tekstury


// Declaration for Window procedure
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam);

// Dialog procedure for about box
BOOL APIENTRY AboutDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);


// Reduces a normal vector specified as a set of three coordinates,
 //to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
{
	float length;

	// Calculate the length of the vector		
	length = (float)sqrt((vector[0] * vector[0]) +
		(vector[1] * vector[1]) +
		(vector[2] * vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if (length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
{
	float v1[3], v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
}



// Change viewing volume and viewport.  Called when window is resized
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat nRange = 100.0f;
	GLfloat fAspect;
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	lastWidth = w;
	lastHeight = h;

	fAspect = (GLfloat)w / (GLfloat)h;
	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);

	// Establish perspective: 
	/*
	gluPerspective(60.0f,fAspect,1.0,400);
	*/

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC()
{
	// Light values and coordinates
	//GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	//GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	//GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	//GLfloat	 lightPos[] = { 0.0f, 150.0f, 150.0f, 1.0f };
	//GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	//glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Enable lighting
	//glEnable(GL_LIGHTING);

	// Setup and enable light 0
	//glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	//glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	//glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	//glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	//glEnable(GL_LIGHT0);

	// Enable color tracking
	//glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	//glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
	//glMateriali(GL_FRONT,GL_SHININESS,128);


	// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Black brush
	glColor3f(0.0, 0.0, 0.0);
}



// LoadBitmapFile
// opis: ³aduje mapê bitow¹ z pliku i zwraca jej adres.
//       Wype³nia strukturê nag³ówka.
//	 Nie obs³uguje map 8-bitowych.
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;							// wskaŸnik pozycji pliku
	BITMAPFILEHEADER	bitmapFileHeader;		// nag³ówek pliku
	unsigned char		*bitmapImage;			// dane obrazu
	int					imageIdx = 0;		// licznik pikseli
	unsigned char		tempRGB;				// zmienna zamiany sk³adowych

	// otwiera plik w trybie "read binary"
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// wczytuje nag³ówek pliku
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// sprawdza, czy jest to plik formatu BMP
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// wczytuje nag³ówek obrazu
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// ustawia wskaŸnik pozycji pliku na pocz¹tku danych obrazu
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// przydziela pamiêæ buforowi obrazu
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// sprawdza, czy uda³o siê przydzieliæ pamiêæ
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// wczytuje dane obrazu
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// sprawdza, czy dane zosta³y wczytane
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// zamienia miejscami sk³adowe R i B 
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// zamyka plik i zwraca wskaŸnik bufora zawieraj¹cego wczytany obraz
	fclose(filePtr);
	return bitmapImage;
}



/// -------> Figury PODSTAWOWE które potem mozna wykorzstywac do budowania innych figur poprzez zmiane parametrow tych funkcji, zmieniamy rozmiar, po³o¿enie itp --------<//
void walecOtwarty(GLfloat x, GLfloat y, GLfloat z, GLfloat srednicaXZ, GLfloat wysokoscY, int red, int green, int blue)
{

	glColor3ub(red, green, blue);
	glBegin(GL_TRIANGLE_STRIP);
	for (float k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 24)
	{
		glVertex3d(x + sin(k) * srednicaXZ, y + wysokoscY, z + cos(k) * srednicaXZ);
		glVertex3d(x + sin(k) * srednicaXZ, y, z + cos(k) * srednicaXZ);
	}
	glEnd();
}

void walecZamkniety(GLfloat x, GLfloat y, GLfloat z, GLfloat srednicaXZ, GLfloat wysokoscY, int red, int green, int blue)
{

	glColor3ub(red, green, blue);
	glBegin(GL_TRIANGLE_STRIP);
	for (float k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 24)
	{
		glVertex3d(x + sin(k) * srednicaXZ, y + wysokoscY, z + cos(k) * srednicaXZ);
		glVertex3d(x + sin(k) * srednicaXZ, y, z + cos(k) * srednicaXZ);
	}
	glEnd();
	kolo(x, y, z, srednicaXZ, red, green, blue);
	kolo(x, y + wysokoscY, z, srednicaXZ, red, green, blue);
}

void kolo(GLfloat x, GLfloat y, GLfloat z, GLfloat srednicaXZ, int red, int green, int blue)
{
	glColor3ub(red, green, blue);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x, y, z);
	for (float k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 24)
	{
		glVertex3f(x + cos(k) * srednicaXZ, y, z + sin(k) * srednicaXZ);
	}
	glEnd();
}

void okrag(GLfloat x, GLfloat y, GLfloat z, GLfloat srednicaXZ, GLfloat gruboscLini, int red, int green, int blue)
{
	glColor3ub(red, green, blue);
	glLineWidth(gruboscLini);
	glBegin(GL_LINE_STRIP);
	for (float k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 24)
	{
		glVertex3f(x + cos(k) * srednicaXZ, y, z + sin(k) * srednicaXZ);
	}
	glEnd();
}

void stozek(GLfloat srodekPodstawyX, GLfloat srodekPodstawyY, GLfloat srodekPodstawyZ, GLfloat srednicaXZ, GLfloat wysokoscY, int red, int green, int blue)
{
	glColor3ub(red, green, blue);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(srodekPodstawyX, srodekPodstawyY + wysokoscY, srodekPodstawyZ);
	for (float k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 24)
	{
		glVertex3f(srodekPodstawyX + cos(k) * srednicaXZ, srodekPodstawyY, srodekPodstawyZ + sin(k) * srednicaXZ);
	}
	glEnd();
}


/// --------> koniec figur podstawowych -------- ///



//rysuje pojedynczy wirnik z silnikiem
//parametry okreslaj¹, w którym miejscu narysowaæ ten wirnik na ka¿dej z osi
void wiatrak(GLfloat x, GLfloat y, GLfloat z, GLfloat promienLopatki)
{

	//dwie "lopatki" smigla
	glColor3ub(180, 180, 180);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x, y+5.0, z);
	glVertex3f(x + cos(GL_PI / 18 * 9) * promienLopatki, y + 5, z + sin(GL_PI / 18 * 9) * promienLopatki); //90 stopni
	glVertex3f(x+cos(GL_PI / 18 * 11) * promienLopatki, y+5, z + sin(GL_PI / 18 * 11) * promienLopatki); //110
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x, y + 5.0, z);
	glVertex3f(x + cos(GL_PI / 18 * 27) * promienLopatki, y + 5, z + sin(GL_PI / 18 *27) * promienLopatki); //270
	glVertex3f(x + cos(GL_PI / 18 * 29) * promienLopatki, y + 5, z + sin(GL_PI / 18 * 29) * promienLopatki); //290
	glEnd();

	//nakretka na smigle
	walecZamkniety(x, y + 4.5, z, 1, 1.5, 0, 0, 0);
	stozek(x, y + 6, z, 0.7, 1.2, 130, 130, 130);

	//czerwone kolo pod smiglem
	walecZamkniety(x, y + 4, z, 1, 0.5, 160, 50, 50);
	walecZamkniety(x, y + 3, z, 2.5, 1, 160, 50, 50);

	//czarne kolo pod czerwonym + polaczenie z uchwytem
	walecZamkniety(x, y + 2, z, 2.5, 1, 0, 0, 0);
	walecZamkniety(x, y + 1, z, 1, 1, 20, 20, 20);
}

void lacznikSmigloBaza(GLfloat smigloX, GLfloat smigloY, GLfloat smigloZ)
{
	if (smigloX == 0 && smigloY == 0 && smigloZ == 0)
	{
		walecZamkniety(smigloX, smigloY, smigloZ, 3, 1, 70, 110, 110);
		glColor3ub(20, 80, 70);
		glBegin(GL_POLYGON);
		glVertex3f(smigloX + 3, smigloY, smigloZ-1);
		glVertex3f(smigloX + 12, smigloY - 2, smigloZ + 5);
		glVertex3f(smigloX + 5, smigloY - 2, smigloZ + 12);
		glVertex3f(smigloX-1, smigloY, smigloZ + 3);
		glEnd();
	}
	else if (smigloX == 40 && smigloY == 0 && smigloZ == 0)
	{
		walecZamkniety(smigloX, smigloY, smigloZ, 3, 1, 70, 110, 110);
		glColor3ub(20, 80, 70);
		glBegin(GL_POLYGON);
		glVertex3f(smigloX + 3, smigloY, smigloZ+1);
		glVertex3f(smigloX - 5, smigloY - 2, smigloZ + 11);
		glVertex3f(smigloX -12, smigloY - 2, smigloZ + 5);
		glVertex3f(smigloX-3, smigloY, smigloZ-1);
		glEnd();
	}
	else if (smigloX == 40 && smigloY == 0 && smigloZ == 40)
	{
		walecZamkniety(smigloX, smigloY, smigloZ, 3, 1, 70, 110, 110);
		glColor3ub(20, 80, 70);
		glBegin(GL_POLYGON);
		glVertex3f(smigloX + 3, smigloY, smigloZ -1);
		glVertex3f(smigloX - 5, smigloY - 2, smigloZ - 11);
		glVertex3f(smigloX - 12, smigloY - 2, smigloZ - 5);
		glVertex3f(smigloX - 3, smigloY, smigloZ + 1);
		glEnd();
	}
	else if (smigloX == 0 && smigloY == 0 && smigloZ == 40)
	{
		walecZamkniety(smigloX, smigloY, smigloZ, 3, 1, 70, 110, 110);
		glColor3ub(20, 80, 70);
		glBegin(GL_POLYGON);
		glVertex3f(smigloX + 3, smigloY, smigloZ + 1);
		glVertex3f(smigloX + 12, smigloY - 2, smigloZ - 5);
		glVertex3f(smigloX + 5, smigloY - 2, smigloZ - 12);
		glVertex3f(smigloX - 3, smigloY, smigloZ - 1);
		glEnd();
	}
}



//w sumie to nwm co to bedzie jeszcze
// parametry domyslnie ustawiac na zera
void kamera(GLfloat x, GLfloat y, GLfloat z)
{
	//gorna sciana
	glColor3f(0, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(x + 35.0f, y + -5.0f, z + 30.0f);
	glVertex3f(x + 45.0f, y + -5.0f, z + 30.0f);
	glVertex3f(x + 45.0f, y + -5.0f, z + 50.0f);
	glVertex3f(x + 35.0f, y + -5.0f, z + 50.0f);
	glEnd();

	//dolna sciana
	glBegin(GL_QUADS);
	glVertex3f(x + 35.0f, y + -15.0f, z + 30.0f);
	glVertex3f(x + 45.0f, y + -15.0f, z + 30.0f);
	glVertex3f(x + 45.0f, y + -15.0f, z + 50.0f);
	glVertex3f(x + 35.0f, y + -15.0f, z + 50.0f);
	glEnd();

	/*---------------sciany boczne---------------*/
	glColor3f(0.1f, 0.4f, 0.9f);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(x + 35.0f, y + -5.0f, z + 30.0f);
	glVertex3f(x + 45.0f, y + -5.0f, z + 30.0f);
	glVertex3f(x + 35.0f, y + -15.0f, z + 30.0f);
	glVertex3f(x + 45.0f, y + -15.0f, z + 30.0f);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(x + 35.0f, y + -5.0f, z + 30.0f);
	glVertex3f(x + 35.0f, y + -5.0f, z + 50.0f);
	glVertex3f(x + 35.0f, y + -15.0f, z + 30.0f);
	glVertex3f(x + 35.0f, y + -15.0f, z + 50.0f);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(x + 35.0f, y + -5.0f, z + 50.0f);
	glVertex3f(x + 45.0f, y + -5.0f, z + 50.0f);
	glVertex3f(x + 35.0f, y + -15.0f, z + 50.0f);
	glVertex3f(x + 45.0f, y + -15.0f, z + 50.0f);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(x + 45.0f, y + -5.0f, z + 30.0f);
	glVertex3f(x + 45.0f, y + -5.0f, z + 50.0f);
	glVertex3f(x + 45.0f, y + -15.0f, z + 30.0f);
	glVertex3f(x + 45.0f, y + -15.0f, z + 50.0f);
	glEnd();

	/*---------------pierwszy walec do kamerki---------------*/
	glColor3f(1.0f, 0.2f, 0.3f);
	glBegin(GL_TRIANGLE_STRIP);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 3, y + -10.5f + sin(k) * 3, z + 45.0f);
		glVertex3f(x + 40.0f + cos(k) * 3, y + -10.5f + sin(k) * 3, z + 55.0f);
	}
	glEnd();

	//kolo w pierwszym walcu
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x + 40.0f, y + -10.5f, z + 55.0f);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 3, y + -10.5f + sin(k) * 3, z + 55.0f);
	}
	glEnd();

	/*---------------drugi walec do kamerki---------------*/
	glColor3f(0.6f, 0.0f, 0.4f);
	glBegin(GL_TRIANGLE_STRIP);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 2.15, y + -10.5f + sin(k) * 2.15, z + 55.0f);
		glVertex3f(x + 40.0f + cos(k) * 2.15, y + -10.5f + sin(k) * 2.15, z + 59.0f);
	}
	glEnd();

	//kolo w drugim walcu
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x + 40.0f, y + -10.5f, z + 59.0f);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 2.15, y + -10.5f + sin(k) * 2.15, z + 59.0f);
	}
	glEnd();

	/*---------------trzeci walec do kamerki---------------*/
	glColor3f(0.9f, 0.0f, 0.2f);
	glBegin(GL_TRIANGLE_STRIP);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -10.5f + sin(k) * 1.5, z + 59.0f);
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -10.5f + sin(k) * 1.5, z + 62.0f);
	}
	glEnd();

	//kolo w trzecim walcu
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x + 40.0f, y + -10.5f, z + 62.0f);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -10.5f + sin(k) * 1.5, z + 62.0f);
	}
	glEnd();

	/*---------------stopki na bazie walca---------------*/

	//pionowy pret pod obiektem								// <--- tu pozniej detali wiecej dorobie ----------------------------------
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(x + 40.0f, y + -15.0f, z + 40.0f);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -15.0f, z + 40.0f + sin(k) * 1.5);
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -25.0f, z + 40.0f + sin(k) * 1.5);
	}
	glEnd();

	//pierwsza stopka
	glColor3f(0.4f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	//glVertex3f(x + 40.0f, y + -22.0f, z + 40.0f);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -22.0f, z + 40.0f + sin(k) * 1.5);
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -26.0f, z + 50.0f + sin(k) * 1.5);
	}
	glEnd();

	//druga stopka
	glColor3f(0.4f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	//glVertex3f(x + 40.0f, y + -22.0f, z + 40.0f);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -22.0f, z + 40.0f + sin(k) * 1.5);
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -26.0f, z + 30.0f + sin(k) * 1.5);
	}
	glEnd();

	//trzecia stopka
	glColor3f(0.4f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	//glVertex3f(x + 40.0f, y + -22.0f, z + 40.0f);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -22.0f, z + 40.0f + sin(k) * 1.5);
		glVertex3f(x + 30.0f + cos(k) * 1.5, y + -26.0f, z + 40.0f + sin(k) * 1.5);
	}
	glEnd();

	//czwarta stopka
	glColor3f(0.4f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	//glVertex3f(x + 40.0f, y + -22.0f, z + 40.0f);
	for (GLfloat k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 1.5, y + -22.0f, z + 40.0f + sin(k) * 1.5);
		glVertex3f(x + 50.0f + cos(k) * 1.5, y + -26.0f, z + 40.0f + sin(k) * 1.5);
	}
	glEnd();


	/*---------------nwm co to ale wyglada zajebiscie---------------*/
	/*glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_LINE_LOOP);
	for (float k = 0; k < 2 * GL_PI + GL_PI / 12; k += GL_PI / 12)
	{
		glVertex3f(x + 40.0f + cos(k) * 10, y + 10.0f, z + 40.0f + sin(k) * 10);
		glVertex3f(x + 40.0f + sin(k) * 10, y + 10.0f + cos(k) * 10, z + 40.0f);
	}
	glEnd();*/
}

//x,y,z - wspolrzedne r,h - rozmiary
void pierscien(GLfloat x, GLfloat y, GLfloat z, GLdouble r, GLdouble h)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		GLfloat a, b;
		glColor3f(0.0f, 0.0f, 0.5f);
		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat k = 0; k <= 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r* sin(k);
			b = r* cos(k);
			glVertex3d(x + a,y + 0,z + b);
			a *= 1.2;
			b *= 1.2;
			glVertex3d(x + a,y + 0,z + b);
		}
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat k = 0; k <= 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r* sin(k);
			b = r* cos(k);
			glVertex3d(x + a,y + h,z + b);
			a *= 1.2;
			b *= 1.2;
			glVertex3d(x + a,y + h,z + b);
		}
		glEnd();

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat k = 0; k <= 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r* sin(k);
			b = r* cos(k);
	     	glVertex3d(x + a, y + 0, z + b);
			glVertex3d(x + a, y + h, z + b);
		}
		glEnd();

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat k = 0; k <= 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r* sin(k);
			b = r* cos(k);
			a *= 1.2;
			b *= 1.2;
			glVertex3d(x + a,y + 0, z + b);
			glVertex3d(x + a,y + h, z + b);
		}
		glEnd();
	}
}


void krzyz(GLfloat x, GLfloat y, GLfloat z, GLfloat r)
{
	GLfloat a, b;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		//walec wzdluz osi x
		glColor3f(0.0f, 0.5f, 0.5f);
		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat k = 0; k < 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r *sin(k);
			b = r *cos(k);
			glVertex3f(x - 20, y + b, z + a);
			glVertex3f(x + 20, y + b, z + a);
		}
		glEnd();

		//kolo na jednym zakonczeniu walca
		glColor3f(0.0f, 0.5f, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for (GLfloat k = 0; k < 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r *sin(k);
			b = r *cos(k);
			glVertex3f(x - 20, y + b, z + a);
		}
		glEnd();

		//kolo na drugim zakonczeniu walca
		glColor3f(0.0f, 0.5f, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for (GLfloat k = 0; k < 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r *sin(k);
			b = r *cos(k);
			glVertex3f(x + 20, y + b, z + a);
		}
		glEnd();

		//walec wzdluz osi z
		glColor3f(0.0f, 0.5f, 0.5f);
		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat k = 0; k < 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r *sin(k);
			b = r *cos(k);
			glVertex3f(x + a, y + b, z -20);
			glVertex3f(x + a, y + b, z + 20);
		}
		glEnd();

		//kolo na jednym zakonczeniu walca
		glColor3f(0.0f, 0.5f, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for (GLfloat k = 0; k < 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r *sin(k);
			b = r *cos(k);
			glVertex3f(x + a, y + b, z - 20);
		}
		glEnd();

		//kolo na drugim zakonczeniu walca
		glColor3f(0.0f, 0.5f, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for (GLfloat k = 0; k < 2 * GL_PI; k += GL_PI / 16.0)
		{
			a = r *sin(k);
			b = r *cos(k);
			glVertex3f(x + a, y + b, z + 20);
		}
		glEnd();
	}
}



//cos bede dalej tutaj robil :D
//void podstawa()
//{
//	glColor3f(0.1, 0.4, 0.0);
//	glBegin(GL_TRIANGLE_FAN);
//
//}


//mozna wylaczac w RenderScene jak przeszkadza, ogolnie pomaga ogarniac wymiary
//x - czerwony,
//y - zielony,
//z - niebieski
void uklad(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow
		glLineWidth(1);
		GLfloat sx1[3] = { -50.0f,0.0f,0.0f };
		GLfloat sx2[3] = { 50.0f,0.0f,0.0f };
		GLfloat sy1[3] = { 0.0f,-50.0f,0.0f };
		GLfloat sy2[3] = { 0.0f,50.0f,0.0f };
		GLfloat sz1[3] = { 0.0f,0.0f,-50.0f };
		GLfloat sz2[3] = { 0.0f,0.0f,50.0f };

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3fv(sx1);
		glVertex3fv(sx2);
		glEnd();

		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3fv(sy1);
		glVertex3fv(sy2);
		glEnd();

		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex3fv(sz1);
		glVertex3fv(sz2);
		glEnd();
	}
}

// Called to draw scene
void RenderScene(void)
{
	//float normal[3];	// Storeage for calculated surface normal

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	/////////////////////////////////////////////////////////////////
	// MIEJSCE NA KOD OPENGL DO TWORZENIA WLASNYCH SCEN:		   //
	/////////////////////////////////////////////////////////////////


	//Sposób na odróŸnienie "przedniej" i "tylniej" œciany wielok¹ta:
	glPolygonMode(GL_BACK, GL_LINE);

	

	uklad();
	kamera(-20, -4, -20);	// wspol zerowe ustawiaja obiekt dokladnie pomiedzy czterema wiatrakami
	wiatrak(0, 0, 0, 15);
	wiatrak(40, 0, 40, 15);
	wiatrak(0, 0, 40, 15);
	wiatrak(40, 0, 0, 15);
	pierscien(20, -4, 20, 15, 2.5); //pierwsze 3 to wspolrzedne, 2 od prawej to rozmiary
	pierscien(20, -30.3, 20, 15, 2.6);
	krzyz(20, -29, 20, 1.25);
	lacznikSmigloBaza(0, 0, 0);  //lacznik wirnika o podanych wspolrzednych itd.
	lacznikSmigloBaza(40, 0, 0);
	lacznikSmigloBaza(40, 0, 40);
	lacznikSmigloBaza(0, 0, 40);



	

	//tak na probe
	/*glRotatef(90, 1, 0, 0);
	kolo(20, 50, 15, 4, 0.0, 1.0, 0.0);

	glRotatef(90, 0, 0, 1);
	{
		okrag(-15, 0, -15, 5, 2, 0.0, 1.0, 1.0);
		okrag(-15, 0, -15, 10, 2, 0.0, 1.0, 1.0);
		okrag(-15, 0, -15, 15, 2, 0.0, 1.0, 1.0);
		okrag(-15, 0, -15, 20, 2, 0.0, 1.0, 1.0);
		walecOtwarty(-15, 0, -15, 2.5, 20, 1, 1, 1);
		walecOtwarty(-15, 0, -15, 2.5, -20, 1, 1, 1);

		okrag(-15, 20, -15, 5, 2, 0.0, 1.0, 1.0);
		okrag(-15, 20, -15, 10, 2, 0.0, 1.0, 1.0);
		okrag(-15, 20, -15, 15, 2, 0.0, 1.0, 1.0);
		okrag(-15, 20, -15, 20, 2, 0.0, 1.0, 1.0);

		okrag(-15, -20, -15, 5, 2, 0.0, 1.0, 1.0);
		okrag(-15, -20, -15, 10, 2, 0.0, 1.0, 1.0);
		okrag(-15, -20, -15, 15, 2, 0.0, 1.0, 1.0);
		okrag(-15, -20, -15, 20, 2, 0.0, 1.0, 1.0);
	}*/

	
	//Uzyskanie siatki:
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Flush drawing commands
	glFlush();
}

// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // Size of this structure
		1,                                                              // Version of this structure    
		PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,                       // Double buffered
		PFD_TYPE_RGBA,                          // RGBA Color mode
		24,                                     // Want 24bit color 
		0,0,0,0,0,0,                            // Not used to select mode
		0,0,                                    // Not used to select mode
		0,0,0,0,0,                              // Not used to select mode
		32,                                     // Size of depth buffer
		0,                                      // Not used to select mode
		0,                                      // Not used to select mode
		PFD_MAIN_PLANE,                         // Draw in main plane
		0,                                      // Not used to select mode
		0,0,0 };                                // Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}



// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange, GreenRange, BlueRange;
	// Range for each color entry (7,7,and 3)


// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if (!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) - 1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) - 1;

	// Loop through all the palette entries
	for (i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double)pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char)NULL;
	}


	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC, hRetPal, FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
}


// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE       hInst,
	HINSTANCE       hPrevInstance,
	LPSTR           lpCmdLine,
	int                     nCmdShow)
{
	MSG                     msg;            // Windows message structure
	WNDCLASS        wc;                     // Windows class structure
	HWND            hWnd;           // Storeage for window handle

	hInstance = hInst;

	// Register Window style
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// No need for background brush for OpenGL window
	wc.hbrBackground = NULL;

	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wc.lpszClassName = lpszAppName;

	// Register the window class
	if (RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,

		// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,

		// Window position and size
		50, 50,
		400, 400,
		NULL,
		NULL,
		hInstance,
		NULL);

	// If window was not created, quit
	if (hWnd == NULL)
		return FALSE;


	// Display the window
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}




// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam)
{
	static HGLRC hRC;               // Permenant Rendering context
	static HDC hDC;                 // Private GDI Device context

	switch (message)
	{
		// Window creation, setup for OpenGL
	case WM_CREATE:
		// Store the device context
		hDC = GetDC(hWnd);

		// Select the pixel format
		SetDCPixelFormat(hDC);

		// Create palette if needed
		hPalette = GetOpenGLPalette(hDC);

		// Create the rendering context and make it current
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		SetupRC();
		glGenTextures(2, &texture[0]);                  // tworzy obiekt tekstury			

		// ³aduje pierwszy obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\checker.bmp", &bitmapInfoHeader);

		glBindTexture(GL_TEXTURE_2D, texture[0]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ³aduje drugi obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\crate.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[1]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ustalenie sposobu mieszania tekstury z t³em
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;

		// Window is being destroyed, cleanup
	case WM_DESTROY:
		// Deselect the current rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);

		// Delete the palette if it was created
		if (hPalette != NULL)
			DeleteObject(hPalette);

		// Tell the application to terminate after the window
		// is gone.
		PostQuitMessage(0);
		break;

		// Window is resized.
	case WM_SIZE:
		// Call our function which modifies the clipping
		// volume and viewport
		ChangeSize(LOWORD(lParam), HIWORD(lParam));
		break;


		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
	case WM_PAINT:
	{
		// Call OpenGL drawing code
		RenderScene();

		SwapBuffers(hDC);

		// Validate the newly painted client area
		ValidateRect(hWnd, NULL);
	}
	break;

	// Windows is telling the application that it may modify
	// the system palette.  This message in essance asks the 
	// application for a new palette.
	case WM_QUERYNEWPALETTE:
		// If the palette was created.
		if (hPalette)
		{
			int nRet;

			// Selects the palette into the current device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries from the currently selected palette to
			// the system palette.  The return value is the number 
			// of palette entries modified.
			nRet = RealizePalette(hDC);

			// Repaint, forces remap of palette in current window
			InvalidateRect(hWnd, NULL, FALSE);

			return nRet;
		}
		break;


		// This window may set the palette, even though it is not the 
		// currently active window.
	case WM_PALETTECHANGED:
		// Don't do anything if the palette does not exist, or if
		// this is the window that changed the palette.
		if ((hPalette != NULL) && ((HWND)wParam != hWnd))
		{
			// Select the palette into the device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries to system palette
			RealizePalette(hDC);

			// Remap the current colors to the newly realized palette
			UpdateColors(hDC);
			return 0;
		}
		break;

		// Key press, check for arrow keys to do cube rotation.
	case WM_KEYDOWN:
	{
		//jezeli wciskany klawisz to a/A(0x41) wtedy zwiekszaj licznik 
		//i w zaleznosci od jego wartosci ustawiaj sciane pod odpowiednim katem
		if (wParam == 0x41)
		{
			licznik++;
			switch (licznik)
			{
			case 1:
			{
				sinus = sin(3.14 / 12);
				cosinus = cos(3.14 / 12);
				break;
			}
			case 2:
			{
				sinus = sin(3.14 / 6);
				cosinus = cos(3.14 / 6);
				break;
			}
			case 3:
			{
				sinus = sin(3.14 / 4);
				cosinus = cos(3.14 / 4);
				break;
			}
			case 4:
			{
				sinus = sin(3.14 / 3);
				cosinus = cos(3.14 / 3);
				break;
			}
			case 5:
			{
				sinus = sin(3.14 / 12 * 5);
				cosinus = cos(3.14 / 12 * 5);
				break;
			}
			case 6:
			{
				sinus = sin(3.14 / 2);
				cosinus = cos(3.14 / 2);
				break;
			}
			//zabezpieczenie gdy ca³y czas bedziemy wciskaæ a/A
			//sciana bedzie ustawiona caly czas pod katem 90 stopni
			//czyli szescian bedzie calkowicie otwarty
			default:
				if (licznik > 6) licznik = 6;
				break;
			}
		}
		//jezeli wciskany klawisz to b/B(0x42) wtedy zmniejszaj licznik 
		//i w zaleznosci od jego wartosci ustawiaj sciane pod odpowiednim katem
		if (wParam == 0x42)
		{
			licznik--;
			switch (licznik)
			{
			case 0:
			{
				sinus = 0;
				cosinus = 0;
				break;
			}
			case 1:
			{
				sinus = sin(3.14 / 12);
				cosinus = cos(3.14 / 12);
				break;
			}
			case 2:
			{
				sinus = sin(3.14 / 6);
				cosinus = cos(3.14 / 6);
				break;
			}
			case 3:
			{
				sinus = sin(3.14 / 4);
				cosinus = cos(3.14 / 4);
				break;
			}
			case 4:
			{
				sinus = sin(3.14 / 3);
				cosinus = cos(3.14 / 3);
				break;
			}
			case 5:
			{
				sinus = sin(3.14 / 12 * 5);
				cosinus = cos(3.14 / 12 * 5);
				break;
			}
			case 6:
			{
				sinus = sin(3.14 / 2);
				cosinus = cos(3.14 / 2);
				break;
			}
			//zabezpieczenie gdy ca³y czas bedziemy wciskaæ b/B
			//sciana bedzie ustawiona caly czas pod katem 0 stopni
			//czyli szescian bedzie "zamkniety"
			default:
				if (licznik < 0) licznik = 0;
				break;
			}
		}
		if (wParam == VK_UP)
			xRot -= 5.0f;

		if (wParam == VK_DOWN)
			xRot += 5.0f;

		if (wParam == VK_LEFT)
			yRot -= 5.0f;

		if (wParam == VK_RIGHT)
			yRot += 5.0f;

		xRot = (const int)xRot % 360;
		yRot = (const int)yRot % 360;

		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;

	// A menu command
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			// Exit the program
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;

			// Display the about box
		case ID_HELP_ABOUT:
			DialogBox(hInstance,
				MAKEINTRESOURCE(IDD_DIALOG_ABOUT),
				hWnd,
				AboutDlgProc);
			break;
		}
	}
	break;


	default:   // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));

	}

	return (0L);
}




// Dialog procedure.
BOOL APIENTRY AboutDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{

	switch (message)
	{
		// Initialize the dialog box
	case WM_INITDIALOG:
	{
		int i;
		GLenum glError;

		// glGetString demo
		SetDlgItemText(hDlg, IDC_OPENGL_VENDOR, glGetString(GL_VENDOR));
		SetDlgItemText(hDlg, IDC_OPENGL_RENDERER, glGetString(GL_RENDERER));
		SetDlgItemText(hDlg, IDC_OPENGL_VERSION, glGetString(GL_VERSION));
		SetDlgItemText(hDlg, IDC_OPENGL_EXTENSIONS, glGetString(GL_EXTENSIONS));

		// gluGetString demo
		SetDlgItemText(hDlg, IDC_GLU_VERSION, gluGetString(GLU_VERSION));
		SetDlgItemText(hDlg, IDC_GLU_EXTENSIONS, gluGetString(GLU_EXTENSIONS));


		// Display any recent error messages
		i = 0;
		do {
			glError = glGetError();
			SetDlgItemText(hDlg, IDC_ERROR1 + i, gluErrorString(glError));
			i++;
		} while (i < 6 && glError != GL_NO_ERROR);


		return (TRUE);
	}
	break;

	// Process command messages
	case WM_COMMAND:
	{
		// Validate and Make the changes
		if (LOWORD(wParam) == IDOK)
			EndDialog(hDlg, TRUE);
	}
	break;

	// Closed from sysbox
	case WM_CLOSE:
		EndDialog(hDlg, TRUE);
		break;
	}

	return FALSE;
}
