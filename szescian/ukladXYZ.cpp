#include "ukladXYZ.h"



ukladXYZ::ukladXYZ()
{
}


ukladXYZ::~ukladXYZ()
{
}

void ukladXYZ::uklad(void)
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
