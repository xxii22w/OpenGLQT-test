#include "BoxObject.h"
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>

#include "PickObject.h"

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

	const int BoxGenCount = 10;
	const int GridDim = 10; // 必须是int，否则必须使用下面的强制转换

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
		float boxHeight = 4.5;
		BoxMesh b(4, boxHeight, 3);
		b.setFaceColors({ Qt::blue, Qt::red, Qt::yellow, Qt::green, Qt::magenta, Qt::darkCyan });
		trans.setTranslation((-GridDim / 2 + xGrid) * 5, boxCount * 5 + 0.5 * boxHeight, (-GridDim / 2 + zGrid) * 5);
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

void BoxObject::create(QOpenGLShaderProgram* shaderProgramm) {
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

void BoxObject::pick(const QVector3D& p1, const QVector3D& d, PickObject& po) const
{
	// 现在加载所有的盒子对象
	for (unsigned int i = 0; i < m_boxes.size(); ++i) {
		const BoxMesh& bm = m_boxes[i];
		for (unsigned int j = 0; j < 6; ++j) {
			float dist;
			// 交点是否比之前的交点更靠近观众?
			if (bm.intersects(j, p1, d, dist)) {
				qDebug() << QString("Plane %1 of box %2 intersects line at normalized distance = %3").arg(j).arg(i).arg(dist);
				// keep objects that is closer to near plane
				if (dist < po.m_dist) {
					po.m_dist = dist;
					po.m_objectId = i;
					po.m_faceId = j;
				}
			}
		}
	}
}
	
void BoxObject::highlight(unsigned int boxId, unsigned int faceId)
{
	// 我们将选中框的所有顶点的颜色改为浅灰色
	// 选定平面/面的顶点颜色为浅蓝色

	std::vector<QColor> faceCols(6);
	for (unsigned int i = 0; i < 6; ++i) {
		if (i == faceId)
			faceCols[i] = QColor("#b40808");
		else
			faceCols[i] = QColor("#f3f3f3");
	}
	m_boxes[boxId].setFaceColors(faceCols);

	// 将指针和顶点数移动到相应的框位置/编号
	Vertex* vertexBuffer = m_vertexBufferData.data() + boxId * 6 * 4;	// 6个面，每个面4个顶点
	unsigned int vertexCount = boxId * 6 * 4;
	GLuint* elementBuffer = m_elementBufferData.data() + boxId * 6 * 6; // 6 planes, with 2 triangles with 3 indexes each
	m_boxes[boxId].copy2Buffer(vertexBuffer, elementBuffer, vertexCount); // 然后更新vertexbuffer内存的相应部分

	QElapsedTimer t;
	t.start();
	// 现在更新顶点
	m_vbo.bind();
	// 只更新数据的修改部分
	m_vbo.write(boxId * 6 * 4 * sizeof(Vertex), m_vertexBufferData.data() + boxId * 6 * 4, 6 * 4 * sizeof(Vertex));
	// 或者使用下面的调用，它(重新)复制整个缓冲区，这可能会很慢
	// m_vbo.allocate(m_vertexBufferData.data(), m_vertexBufferData.size()*sizeof(Vertex));
	m_vbo.release();
	qDebug() << t.elapsed();
}