#include "OpenGLWindow.h"

#include <QCoreApplication>
#include <QDebug>

#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>

OpenGLWindow::OpenGLWindow(QWindow* parent) :
	QWindow(parent),
	m_context(nullptr)
{
	setSurfaceType(QWindow::OpenGLSurface);
}

void OpenGLWindow::renderLater() {
	// 在事件循环中安排UpdateRequest事件
	// 将与下一个VSync一起发送。
	requestUpdate(); 
}

void OpenGLWindow::renderNow() {
	if (!isExposed())
		return;

	// initialize on first call
	if (m_context == nullptr)
		initOpenGL();

	m_context->makeCurrent(this);

	paintGL(); // call user code

	m_context->swapBuffers(this);
}

bool OpenGLWindow::event(QEvent* event) {
	switch (event->type()) {
	case QEvent::UpdateRequest:
		renderNow();
		return true;
	default:
		return QWindow::event(event);
	}
}

void OpenGLWindow::exposeEvent(QExposeEvent* /*event*/) {

	renderNow(); // 立即更新

	// 注意：如果只是在下一次同步时请求更新，即通过
	// 调用renderLater（）（或requestUpdate（）），我们得到
	// 放大窗口时出现白色小故障。既然我们不想那样，
	// 我们只是立即渲染，以便新的窗口大小
	// 已经被我们渲染到的调整后的视口所反映。
}

void OpenGLWindow::resizeEvent(QResizeEvent* event) {
	QWindow::resizeEvent(event);

	// initialize on first call
	if (m_context == nullptr)
		initOpenGL();

	resizeGL(width(), height());
}

void OpenGLWindow::initOpenGL() {
	assert(m_context == nullptr);

	m_context = new QOpenGLContext(this);
	m_context->setFormat(requestedFormat());
	m_context->create();

	m_context->makeCurrent(this);
	assert(m_context->isValid());

	initializeOpenGLFunctions();
	initializeGL(); // call user code
}