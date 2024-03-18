#pragma once
#include "OpenGLWindow.h"
#include <qopenglvertexarrayobject.h>
#include <qopenglbuffer.h>
#include <qopenglshaderprogram.h>

/*	这是显示三角形的窗口。
	我们从 OpenGLWindow 基类派生并实现
	虚拟 initialize（） 和 render（） 函数。
*/

class TriangleWindow : public OpenGLWindow
{
public:
	TriangleWindow();
	~TriangleWindow() Q_DECL_OVERRIDE;

	void initialize() Q_DECL_OVERRIDE;
	void render() Q_DECL_OVERRIDE;
private:

	// VAO  顶点数组对象
	QOpenGLVertexArrayObject m_VAO;
	// vertex buffer
	QOpenGLBuffer m_vertexBufferObject;
	// 编译shader程序
	QOpenGLShaderProgram* m_program;

};

