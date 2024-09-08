#ifndef PT_QTGLWIDGET_H
#define PT_QTGLWIDGET_H

/*
* 接入渲染端的 传送过来的数据
*/

#include <qwidget.h>
#include <qtimer.h>
#include <qevent.h>
#include <PotatoEarth.h>

namespace PTEarth {
	class Application;
}

class PT_QtGlwidget : public QWidget
{
	Q_OBJECT
public:
	PT_QtGlwidget(QWidget* parent = 0);
	~PT_QtGlwidget();

	void InitRenderApplication();
protected:
	void paintEvent(QPaintEvent* event) override;

	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void keyReleaseEvent(QKeyEvent* event) override;
	virtual void focusInEvent(QFocusEvent* event) override;
	virtual void focusOutEvent(QFocusEvent* event) override;
	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void moveEvent(QMoveEvent* event) override;
private:
	void Update();
private:
	HDC m_hDC;
	QTimer* m_Timer;
	PTEarth::Application* m_Application;
	bool isRepeatPressed = false;
};

#endif
