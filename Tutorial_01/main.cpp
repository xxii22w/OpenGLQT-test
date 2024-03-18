#include "OpenGLWindow.h"
#include <QtWidgets/QApplication>
#include "TriangleWindow.h"
#include <qapplication.h>
#include <qsurfaceformat.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set opengl version information
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);   // 设置API为opengl
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 5);    // 设置版本

    TriangleWindow window;
    // 这个格式必须在show被调用前设置
    window.setFormat(format);
    window.resize(640, 480);
    window.show();
    return a.exec();
}
