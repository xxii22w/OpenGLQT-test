#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <qopenglpaintdevice.h>


QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE

#include "BoxMesh.h"
struct PickObject;

// 一个装所有盒子的容器。 基本上创建各个框的几何图形并填充缓冲区。

class BoxObject 
{
public:
	BoxObject();

	// 函数在OpenGL初始化期间调用，OpenGL上下文是当前的
	void create(QOpenGLShaderProgram* shaderProgramm);
	void destroy();

	void render();

	/*!
		线程保存拾取函数。
		检查是否有任何盒子物体表面被定义为“p1 + d[0..]”的射线击中。1)”,
		将数据存储在Po(选择对象)中。
	*/
	void pick(const QVector3D& p1, const QVector3D& d, PickObject& po) const;

	// 改变方块和正面的颜色，以显示方块被点击
	void highlight(unsigned int boxId, unsigned int faceId);

	std::vector<BoxMesh> m_boxes;

	std::vector<Vertex> m_vertexBufferData;
	std::vector<GLuint>	m_elementBufferData;

	QOpenGLVertexArrayObject	m_vao;
	QOpenGLBuffer				m_vbo;
	QOpenGLBuffer				m_ebo;
};
