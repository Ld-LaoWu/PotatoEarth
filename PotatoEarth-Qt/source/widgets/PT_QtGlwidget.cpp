#include "PT_QtGlwidget.h"
#include <PotatoEarth.h>
#include <qpainter.h>

// 加入一些其他的设定
class PTEarthApplication : public PTEarth::Application {
public:
	PTEarthApplication(const HDC& inHDC) : PTEarth::Application(inHDC){
		PT_EARTH_INFO("欢迎来到土豆地球系列");
	}
	~PTEarthApplication() {}
};

PTEarth::Application* PTEarth::CreateApplication(const HDC& inHDC) {
	return new PTEarthApplication(inHDC);
}

extern PTEarth::Application* PTEarth::CreateApplication(const HDC& inHDC);

PT_QtGlwidget::PT_QtGlwidget(QWidget* parent)
	:QWidget(parent)
{

	m_hDC = GetDC((HWND)winId());
	m_Application = PTEarth::CreateApplication(m_hDC);
	m_Timer = new QTimer(this);
	// setFocusPolicy(Qt::ClickFocus);
	grabKeyboard();
}

PT_QtGlwidget::~PT_QtGlwidget()
{

	if (m_Application) {
		delete m_Application;
		m_Application = nullptr;
	}

	if (m_hDC) {
		ReleaseDC((HWND)winId(), m_hDC);
		m_hDC = nullptr;
	}
}


void PT_QtGlwidget::InitRenderApplication() {
	// connect(m_Timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
	connect(m_Timer, &QTimer::timeout, this, &PT_QtGlwidget::Update);
	m_Timer->start(16); // 约 60 FPS  1000ms / 60 ==== 16
}


void PT_QtGlwidget::Update() {
	m_Application->Run();
}


// 
void PT_QtGlwidget::paintEvent(QPaintEvent* event) {
	Q_UNUSED(event);
	QPainter thePainter(this);

	/*
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);*/

	// 这不是高效的方式

	//QPen pen(Qt::blue);
	//pen.setWidth(3);
	//thePainter.setPen(pen);

	//QBrush brush(Qt::yellow);
	//thePainter.setBrush(brush);

	//QRect rect(10, 10, 200, 100);
	//thePainter.drawRect(rect);

	//thePainter.setPen(Qt::black); // 设置文本颜色
	//thePainter.drawText(rect, Qt::AlignCenter, u8"你好我是土豆地球!");

	//PO_EARTH_INFO("欢迎来到土豆地球系列");

	event->accept();
}

void PT_QtGlwidget::resizeEvent(QResizeEvent* event)
{
	/*Q_UNUSED(event);
	if (m_hGLRC) {
		glViewport(0, 0, width(), height());
	}*/


	PTEarth::WindowResizedEvent inEvent(width(), height());
	m_Application->onEvent(inEvent);


}

void PT_QtGlwidget::mousePressEvent(QMouseEvent* event)
{
	int inButto = PTEarth::PTEarth_Button::PTEARTH_MOUSE_BUTTON_NONE;
	switch (event->button())
	{
	case Qt::LeftButton:
		inButto = PTEarth::PTEarth_Button::PTEARTH_MOUSE_BUTTON_LEFT;
		break;
	case Qt::RightButton:
		inButto = PTEarth::PTEarth_Button::PTEARTH_MOUSE_BUTTON_RIGHT;
		break;
	case Qt::MiddleButton:
		inButto = PTEarth::PTEarth_Button::PTEARTH_MOUSE_BUTTON_MIDDLE;
		break;
	default:
		break;
	}

	PTEarth::MousePressEvent mousePress(inButto);
	m_Application->onEvent(mousePress);
}

void PT_QtGlwidget::mouseReleaseEvent(QMouseEvent* event)
{
	int inButto = PTEarth::PTEarth_Button::PTEARTH_MOUSE_BUTTON_NONE;
	switch (event->button())
	{
	case Qt::LeftButton:
		inButto = PTEarth::PTEarth_Button::PTEARTH_MOUSE_BUTTON_LEFT;
		break;
	case Qt::RightButton:
		inButto = PTEarth::PTEarth_Button::PTEARTH_MOUSE_BUTTON_RIGHT;
		break;
	case Qt::MiddleButton:
		inButto = PTEarth::PTEarth_Button::PTEARTH_MOUSE_BUTTON_MIDDLE;
		break;
	default:
		break;
	}
	PTEarth::MouseReleasedEvent mousePress(inButto);
	m_Application->onEvent(mousePress);
}

void PT_QtGlwidget::mouseDoubleClickEvent(QMouseEvent* event)
{ 
	PT_EARTH_INFO("双击666");
}

void PT_QtGlwidget::mouseMoveEvent(QMouseEvent* event)
{
	auto pt = event->pos();
	PTEarth::MouseMovedEvent mouseMove(pt.x(), pt.y());
	m_Application->onEvent(mouseMove);
}

void PT_QtGlwidget::keyPressEvent(QKeyEvent* event)
{
	// 需要一个转换
	PTEarth::KeyPressedEvent keyPressed(event->key(), event->isAutoRepeat());
	m_Application->onEvent(keyPressed);
}

void PT_QtGlwidget::keyReleaseEvent(QKeyEvent* event)
{

	if (!event->isAutoRepeat()) {
		PTEarth::KeyReleasedEvent keyReleased(event->key());
		m_Application->onEvent(keyReleased);
	}

}

void PT_QtGlwidget::focusInEvent(QFocusEvent* event)
{
	PT_EARTH_INFO("选中");
}

void PT_QtGlwidget::focusOutEvent(QFocusEvent* event)
{
	PT_EARTH_INFO("未选中");
}

void PT_QtGlwidget::enterEvent(QEvent* event)
{
	PT_EARTH_INFO("输入");
}

void PT_QtGlwidget::leaveEvent(QEvent* event)
{
	PT_EARTH_INFO("离开");
}

void PT_QtGlwidget::moveEvent(QMoveEvent* event)
{
	PT_EARTH_INFO("移动");
}