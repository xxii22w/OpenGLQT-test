#include "BoxObject.h"
#include <QVector3D>
#include <QOpenGLShaderProgram>


BoxObject::BoxObject() :
	m_vbo(QOpenGLBuffer::VertexBuffer), // actually the default, so default constructor would have been enough
	m_ebo(QOpenGLBuffer::IndexBuffer) // make this an Index Buffer
{


	// 第一个盒子，设置他6个面的颜色，变换到中心
	BoxMesh b(4, 2, 3);
	b.setFaceColors({ Qt::blue, Qt::red, Qt::yellow, Qt::green, Qt::magenta, Qt::darkCyan });
	Transform3D trans;
	trans.setTranslation(0, 1, 0);
	b.transform(trans.toMatrix());
	m_boxes.push_back(b);

	// 创建一样的盒子

	const int BoxGenCount = 100000;
	const int GridDim = 100; // 必须是int，否则必须使用下面的强制转换

	const float GRID_SPACING_XZ = 5;
	const float GRID_SPACING_Y = 2.5;

	// 初始化网格（块计数）
	int boxPerCells[GridDim][GridDim];
	for (unsigned int i = 0; i < GridDim; ++i)
		for (unsigned int j = 0; j < GridDim; ++j)
			boxPerCells[i][j] = 0;
	for (unsigned int i = 0; i < BoxGenCount; ++i) {
		// 在随机化网格中创建其他框，x和z维度固定，高度离散变化
		// 具有维度“GridDim”的网格中的x和z转换，网格（行）间距为5个空间单位
		int xGrid = rand() * double(GridDim) / RAND_MAX;
		int zGrid = rand() * double(GridDim) / RAND_MAX;
		int boxCount = boxPerCells[xGrid][zGrid]++;
		float boxHeight = 2.5;
		BoxMesh b(4, boxHeight, 5);
		b.setFaceColors({ Qt::blue, Qt::red, Qt::yellow, Qt::green, Qt::magenta, Qt::darkCyan });
		trans.setTranslation((-GridDim / 2 + xGrid) * GRID_SPACING_XZ,
			boxCount * GRID_SPACING_Y + 0.5 * boxHeight,
			(-GridDim / 2 + zGrid) * GRID_SPACING_XZ);
		b.transform(trans.toMatrix());
		m_boxes.push_back(b);

	}
	unsigned int NBoxes = m_boxes.size();

	// 调整存储阵列的大小
	m_vertexBufferData.resize(NBoxes * BoxMesh::VertexCount);
	m_elementBufferData.resize(NBoxes * BoxMesh::IndexCount);

	// 更新缓冲区
	Vertex* vertexBuffer = m_vertexBufferData.data();
	unsigned int vertexCount = 0;
	GLuint* elementBuffer = m_elementBufferData.data();
	for (const BoxMesh& b : m_boxes)
		b.copy2Buffer(vertexBuffer, elementBuffer, vertexCount);
}

void BoxObject::create(QOpenGLShaderProgram* shaderProgramm,QOpenGLFunctions* fn) {
	// create and bind Vertex Array Object
	m_vao.create();
	m_vao.bind();

	// create and bind vertex buffer
	m_vbo.create();
	m_vbo.bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	int vertexMemSize = m_vertexBufferData.size() * sizeof(Vertex);
	qDebug() << "BoxObject - VertexBuffer size =" << vertexMemSize / 1024.0 << "kByte";
	m_vbo.allocate(m_vertexBufferData.data(), vertexMemSize);

	// create and bind element buffer
	m_ebo.create();
	m_ebo.bind();
	m_ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	int elementMemSize = m_elementBufferData.size() * sizeof(GLuint);
	qDebug() << "BoxObject - ElementBuffer size =" << elementMemSize / 1024.0 << "kByte";
	m_ebo.allocate(m_elementBufferData.data(), elementMemSize);


	// index 0 = position
	shaderProgramm->enableAttributeArray(0); // array with index/id 0
	shaderProgramm->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(Vertex));
	// index 1 = color
	shaderProgramm->enableAttributeArray(1); // array with index/id 1
	shaderProgramm->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, r), 3, sizeof(Vertex));
	// index 2 = texture
	shaderProgramm->enableAttributeArray(2);
	shaderProgramm->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, texi), 2, sizeof(Vertex));
	// index 3 = textureID
	shaderProgramm->enableAttributeArray(3);
	shaderProgramm->setAttributeBuffer(3, GL_FLOAT, offsetof(Vertex, texID), 1, sizeof(Vertex));


	m_vao.release();
	m_vbo.release();
	m_ebo.release();
}


void BoxObject::destroy() {
	m_vao.destroy();
	m_vbo.destroy();
	m_ebo.destroy();
}


void BoxObject::render() {
	m_vao.bind();

	glDrawElements(GL_TRIANGLES, m_elementBufferData.size(), GL_UNSIGNED_INT, nullptr);

	m_vao.release();
}