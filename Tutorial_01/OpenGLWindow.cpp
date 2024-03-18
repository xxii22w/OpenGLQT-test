#include "OpenGLWindow.h"
#include <qcoreapplication.h>
#include <qopenglcontext.h>
#include <qopenglpaintdevice.h>
#include <qpainter.h>
#include <QEvent>

OpenGLWindow::OpenGLWindow(QWindow*parent)
    : QWindow(parent),m_context(nullptr)
{
    setSurfaceType(QWindow::OpenGLSurface);
}


void OpenGLWindow::renderLater()
{
    // 在事件循环中计划 UpdateRequest 事件
    // 将与下一个 VSync 一起发送。
    requestUpdate();    // 调用公共槽函数 requestUpdate()
}

bool OpenGLWindow::event(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::UpdateRequest:
            renderNow();
            return true;
    default:
        return QWindow::event(event);
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent* /*event*/) {
    
    renderNow(); // 只需将调用重定向到 renderNow（）
}

void OpenGLWindow::renderNow() {

    if (!isExposed())
        return;

    bool needsInitialize = false;
    if (m_context == nullptr)
    {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize)
    {
        initializeOpenGLFunctions();    // 那一堆函数指针
        initialize(); // user code
    }

    render();

    m_context->swapBuffers(this);
}

