#ifndef PT_QTGLWIDGET_H
#define PT_QTGLWIDGET_H

#include <qopenglwidget.h>
#include <qopenglfunctions.h>
#include <qopenglfunctions_4_5_core.h>

class PT_QtGlwidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	PT_QtGlwidget(QWidget* parent = 0);
	~PT_QtGlwidget();

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
};

#endif
