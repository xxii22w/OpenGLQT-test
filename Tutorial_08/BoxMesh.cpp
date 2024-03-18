#include "BoxMesh.h"

void copyPlane2Buffer(Vertex*& vertexBuffer, GLuint*& elementBuffer, unsigned int& elementStartIndex,
	const Vertex& a, const Vertex& b, const Vertex& c, const Vertex& d);

// 表示6个面的8个点
BoxMesh::BoxMesh(float width, float height, float depth, QColor boxColor) {

	m_vertices.push_back(QVector3D(-0.5f * width, -0.5f * height, 0.5f * depth)); // a = 0
	m_vertices.push_back(QVector3D(0.5f * width, -0.5f * height, 0.5f * depth)); // b = 1
	m_vertices.push_back(QVector3D(0.5f * width, 0.5f * height, 0.5f * depth)); // c = 2
	m_vertices.push_back(QVector3D(-0.5f * width, 0.5f * height, 0.5f * depth)); // d = 3

	m_vertices.push_back(QVector3D(-0.5f * width, -0.5f * height, -0.5f * depth)); // e = 4
	m_vertices.push_back(QVector3D(0.5f * width, -0.5f * height, -0.5f * depth)); // f = 5
	m_vertices.push_back(QVector3D(0.5f * width, 0.5f * height, -0.5f * depth)); // g = 6
	m_vertices.push_back(QVector3D(-0.5f * width, 0.5f * height, -0.5f * depth)); // h = 7

	setColor(boxColor);
}

void BoxMesh::transform(const QMatrix4x4& transform)
{
	for (QVector3D& v : m_vertices)
		v = transform * v;
}

void BoxMesh::copy2Buffer(Vertex*& vertexBuffer, GLuint*& elementBuffer, unsigned int& elementStartIndex) const {
	std::vector<QColor> cols;
	Q_ASSERT(!m_colors.empty());
	// 防止纯色框
	if (m_colors.size() == 1) {
		cols = std::vector<QColor>(6, m_colors[0]);
	}
	else {
		Q_ASSERT(m_colors.size() == 6);
		cols = m_colors;
	}

	// 画根据每个面的顶点画每个面

	copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
		Vertex(m_vertices[0], cols[0], 0, 0),
		Vertex(m_vertices[1], cols[0], 1, 0),
		Vertex(m_vertices[2], cols[0], 1, 1),
		Vertex(m_vertices[3], cols[0], 0, 1)
	);

	copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
		Vertex(m_vertices[1], cols[1], 0, 0),
		Vertex(m_vertices[5], cols[1], 1, 0),
		Vertex(m_vertices[6], cols[1], 1, 1),
		Vertex(m_vertices[2], cols[1], 0, 1)
	);

	copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
		Vertex(m_vertices[5], cols[2], 0, 0, 1),
		Vertex(m_vertices[4], cols[2], 2, 0, 1),
		Vertex(m_vertices[7], cols[2], 2, 2, 1),
		Vertex(m_vertices[6], cols[2], 0, 2, 1)
	);

	copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
		Vertex(m_vertices[4], cols[3], 0, 0, 1),
		Vertex(m_vertices[0], cols[3], 2, 0, 1),
		Vertex(m_vertices[3], cols[3], 2, 2, 1),
		Vertex(m_vertices[7], cols[3], 0, 2, 1)
	);

	copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
		Vertex(m_vertices[4], cols[4], 0, 0),
		Vertex(m_vertices[5], cols[4], 1, 0),
		Vertex(m_vertices[1], cols[4], 1, 1),
		Vertex(m_vertices[0], cols[4], 0, 1)
	);

	copyPlane2Buffer(vertexBuffer, elementBuffer, elementStartIndex,
		Vertex(m_vertices[3], cols[5], 0, 0, 2),
		Vertex(m_vertices[2], cols[5], 2, 0, 2),
		Vertex(m_vertices[6], cols[5], 2, 2, 2),
		Vertex(m_vertices[7], cols[5], 0, 2, 2)
	);
}

void copyPlane2Buffer(Vertex*& vertexBuffer, GLuint*& elementBuffer, unsigned int& elementStartIndex,
	const Vertex& a, const Vertex& b, const Vertex& c, const Vertex& d)
{
	// first store the vertex data (a,b,c,d in counter-clockwise order)

	vertexBuffer[0] = a;
	vertexBuffer[1] = b;
	vertexBuffer[2] = c;
	vertexBuffer[3] = d;

	// 调整左下角和右下角节点的颜色
	if (a.y < c.y) {
		vertexBuffer[0].r *= 0.5;
		vertexBuffer[1].r *= 0.5;
		vertexBuffer[0].g *= 0.5;
		vertexBuffer[1].g *= 0.5;
		vertexBuffer[0].b *= 0.5;
		vertexBuffer[1].b *= 0.5;
	}

	// advance vertexBuffer
	vertexBuffer += 4;

	// 我们生成两个三角形的数据：a，b，d和b，c，d

	elementBuffer[0] = elementStartIndex;
	elementBuffer[1] = elementStartIndex + 1;
	elementBuffer[2] = elementStartIndex + 3;
	elementBuffer[3] = elementStartIndex + 1;
	elementBuffer[4] = elementStartIndex + 2;
	elementBuffer[5] = elementStartIndex + 3;

	// advance elementBuffer
	elementBuffer += 6;
	// 4 vertices have been added, so increase start number for next plane
	elementStartIndex += 4;
}