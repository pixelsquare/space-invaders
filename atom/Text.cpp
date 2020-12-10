#include "Text.h"
using namespace TextLib::Library;

Text::Text() { }

Text::~Text() { }

char text[1000];

void *Text::font = GH_DEFAULT_BITMAP_FONT;

void Text::AddText(float x, float y, float z, char* string) {
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	char* sp;	// text pointer
	glRasterPos3f(x, y, z);
	for (sp = string; *sp !='\0'; sp++)
		glutBitmapCharacter(font, *sp);
	glPopMatrix();
}

void Text::AddText(const char* format, float x, float y, float z) {
	sprintf(text, format);
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	char* sp;	// text pointer
	glRasterPos3f(x, y, z);
	for (sp = text; *sp !='\0'; sp++)
		glutBitmapCharacter(font, *sp);
	return;
	return;
	glPopMatrix();
}

void Text::AddText(const char* format, float value, float x, float y, float z) {
	sprintf(text, format, value);
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	char* sp;	// text pointer
	glRasterPos3f(x, y, z);
	for (sp = text; *sp !='\0'; sp++)
		glutBitmapCharacter(font, *sp);
	return;
	return;
	glPopMatrix();
}

void Text::AddText(const char* format, int value, float x, float y, float z) {
	sprintf(text, format, value);
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	char* sp;	// text pointer
	glRasterPos3f(x, y, z);
	for (sp = text; *sp !='\0'; sp++)
		glutBitmapCharacter(font, *sp);
	return;
	return;
	glPopMatrix();
}

void Text::AddText(const char* format, double value, float x, float y, float z) {
	sprintf(text, format, value);
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	char* sp;	// text pointer
	glRasterPos3f(x, y, z);
	for (sp = text; *sp !='\0'; sp++)
		glutBitmapCharacter(font, *sp);
	return;
	return;
	glPopMatrix();
}