#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>


QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE

class GridObject 
{
public:
	// 函数在OpenGL初始化期间调用，OpenGL上下文是当前的。
	void create(QOpenGLShaderProgram* shaderProgramm);
	void destroy();

	// 绑定缓冲区和绘画
	void render();

	unsigned int m_bufferSize;


	// 包裹OpenGL VertexArrayObject，该对象引用顶点坐标
	QOpenGLVertexArrayObject m_vao;
	// 保持网格线的位置
	QOpenGLBuffer m_vbo;
};
