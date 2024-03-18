#pragma once

#include <qwindow.h>
#include <qopenglfunctions.h>

#include <QOpenGLDebugLogger>

QT_BEGIN_NAMESPACE
class QOpenGLContext;
QT_END_NAMESPACE

/*
OpenGLWindow与QOpenGLWindow非常相似，但更轻一些。
此外，函数initializeGL（）和paintGL（）也受到保护，因为它们在
QOpenGLWidget。因此，以后可以很容易地切换到QOpenGLWidget类，
如果你需要的话。

*/

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
public:
	explicit OpenGLWindow(QWindow* parent = nullptr);
	
public slots:
	/*
		重定向到slot requestUpdate（），它在事件循环中注册UpdateRequest事件以与下一个VSync一起发布。
	*/
	void renderLater();

	/* 立即直接重新绘制视图（此函数是从event（）和exposeEvent（）调用的。*/
	void renderNow();

protected:
	bool event(QEvent* event) override;
	void exposeEvent(QExposeEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

	// 在窗口的第一次显示时调用。用自己的重新实施OpenGL初始化代码。
	virtual void initializeGL() = 0;

	/*
	每当视图端口更改（窗口几何图形）时调用。重新实施在您自己的代码中，例如更新投影矩阵。
	此函数是从resizeEvent（）调用的，因此在paintGL（）之前调用。
	param width从width（）返回的窗口宽度（以像素为单位）
	param height从height（）返回的窗口高度（以像素为单位）
	*/
	virtual void resizeGL(int width, int height) { Q_UNUSED(width) Q_UNUSED(height) }

	// 在OpenGL上下文成为当前上下文后立即调用。重新实施于派生类来进行实际绘制。

	virtual void paintGL() = 0;


	QOpenGLContext* m_context;

private slots:

	/*! Receives debug messages from QOpenGLDebugLogger */
	void onMessageLogged(const QOpenGLDebugMessage& msg);

private:
	void initOpenGL();

	QOpenGLDebugLogger* m_debugLogger;
};
