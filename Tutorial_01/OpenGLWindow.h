#pragma once

#include <QtWidgets/QMainWindow>
#include <qwindow.h>
#include <qopenglfunctions.h>

class OpenGLWindow : public QWindow,protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWindow(QWindow* parent = nullptr);
    virtual void render() = 0;
    virtual void initialize() = 0;

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent* event) Q_DECL_OVERRIDE;
    void exposeEvent(QExposeEvent* event) Q_DECL_OVERRIDE;

    QOpenGLContext* m_context;
};
