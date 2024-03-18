#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <qopenglpaintdevice.h>


QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE

#include "BoxMesh.h"

// 一个装所有盒子的容器。 基本上创建各个框的几何图形并填充缓冲区。

class BoxObject 
{
public:
	BoxObject();

	// 函数在OpenGL初始化期间调用，OpenGL上下文是当前的
	void create(QOpenGLShaderProgram* shaderProgramm,QOpenGLFunctions* fn);
	void destroy();

	void render();

	std::vector<BoxMesh> m_boxes;

	std::vector<Vertex> m_vertexBufferData;
	std::vector<GLuint>	m_elementBufferData;

	QOpenGLVertexArrayObject	m_vao;
	QOpenGLBuffer				m_vbo;
	QOpenGLBuffer				m_ebo;
};
