#include "PT_QtGlwidget.h"

PT_QtGlwidget::PT_QtGlwidget(QWidget* parent)
	:QOpenGLWidget(parent)
{
}

PT_QtGlwidget::~PT_QtGlwidget()
{
	
}

void PT_QtGlwidget::initializeGL()
{
	initializeOpenGLFunctions();
}

void PT_QtGlwidget::resizeGL(int w, int h)
{
	glViewport(0.0f, 0.0f, w, h);
}

void PT_QtGlwidget::paintGL()
{
	glClearColor(0.1f, 0.8f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
