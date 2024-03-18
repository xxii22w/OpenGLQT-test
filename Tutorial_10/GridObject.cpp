#include "GridObject.h"

#include <QOpenGLShaderProgram>
#include <vector>


void GridObject::create(QOpenGLShaderProgram* shaderProgramm) {


	const unsigned int N = 1000; // 在x和z方向上绘制的线数
	// 宽度是以“空间单位”为单位的，无论这对你来说意味着什么（米、公里、纳米…）
	float width = 5000;
	// 栅格以原点为中心，在-x、+x、-z和+z方向上扩展到宽度/2

	// 创建一个临时缓冲区，其中包含所有网格线的x-z坐标
	std::vector<float>			gridVertexBufferData;
	// 我们有2*N条线，每条线需要两个顶点，每条线有两个浮点数（x和z坐标）。
	m_bufferSize = 2 * N * 2 * 2;
	gridVertexBufferData.resize(m_bufferSize);
	float* gridVertexBufferPtr = gridVertexBufferData.data();
	// 计算z=常量的网格线
	float x1 = -width * 0.5;
	float x2 = width * 0.5;
	for (unsigned int i = 0; i < N; ++i, gridVertexBufferPtr += 4) {
		float z = width / (N - 1) * i - width * 0.5;
		gridVertexBufferPtr[0] = x1;
		gridVertexBufferPtr[1] = z;
		gridVertexBufferPtr[2] = x2;
		gridVertexBufferPtr[3] = z;
	}
	// 使用x=const计算网格线
	float z1 = -width * 0.5;
	float z2 = width * 0.5;
	for (unsigned int i = 0; i < N; ++i, gridVertexBufferPtr += 4) {
		float x = width / (N - 1) * i - width * 0.5;
		gridVertexBufferPtr[0] = x;
		gridVertexBufferPtr[1] = z1;
		gridVertexBufferPtr[2] = x;
		gridVertexBufferPtr[3] = z2;
	}

	m_vao.create();		
	m_vao.bind();		

	m_vbo.create();
	m_vbo.bind();
	m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	int vertexMemSize = m_bufferSize * sizeof(float);
	m_vbo.allocate(gridVertexBufferData.data(), vertexMemSize);

	// layout(location = 0) = vec2 position
	shaderProgramm->enableAttributeArray(0); // array with index/id 0
	shaderProgramm->setAttributeBuffer(0, GL_FLOAT,
		0 /* position/vertex offset */,
		2 /* two floats per position = vec2 */,
		0 /* vertex after vertex, no interleaving */);

	m_vao.release();
	m_vbo.release();
}


void GridObject::destroy() {
	m_vao.destroy();
	m_vbo.destroy();
}


void GridObject::render() {
	m_vao.bind();
	// draw the grid lines, m_NVertexes = number of floats in buffer
	glDrawArrays(GL_LINES, 0, m_bufferSize);
	m_vao.release();
}