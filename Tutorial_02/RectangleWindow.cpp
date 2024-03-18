#include "RectangleWindow.h"
#include <qdebug.h>

RectangleWindow::RectangleWindow() :
	m_vertexColors{QColor("#f6a509"),QColor("#cb2dde"),QColor("#0eeed1"),QColor("#068918")},
	m_program(nullptr),
	m_frameCount(5000)
{

}

RectangleWindow::~RectangleWindow()
{
	makeCurrent();

	m_vao.destroy();
	m_vertexBufferObject.destroy();
	m_indexBufferObject.destroy();
	delete m_program;
}

void RectangleWindow::initializeGL()
{
	// 此函数在首次显示窗口时调用一次，即当
	//	首先呈现窗口内容
	// 构建和编译我们的着色器程序
	// ------------------------------------

	initializeOpenGLFunctions();

	m_program = new QOpenGLShaderProgram();

	// 读取着色器程序
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/pass_through.vert"))
		qDebug() << "Vertex shader errors:\n" << m_program->log();

	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag"))
		qDebug() << "Fragment shader errors:\n" << m_program->log();

	if (!m_program->link())
		qDebug() << "Shader linker errors:\n" << m_program->log();

	// 设置顶点数据（和缓冲区）并配置顶点属性
	// ----------------------------------------------
	float vertices[] = {
		 0.8f,  0.8f, 0.0f,  // top right
		 0.8f, -0.8f, 0.0f,  // bottom right
		-0.8f, -0.8f, 0.0f,  // bottom left
		-0.8f,  0.8f, 0.0f   // top left
	};

	// 为 2 个交错属性创建缓冲区：位置和颜色，4 个顶点，每个顶点 3 个浮点数
	m_vertexBufferData.resize(2 * 4 * 3);
	// 创建新的数据缓冲区 - 以下内存复制内容应
	// 在后面的教程中被放在一些方便的类中
	// 使用模式 p0c0 | p1c1 | p2c2 | p3c3 以交错模式复制数据
	float* buf = m_vertexBufferData.data();
	for (int v = 0; v < 4; ++v, buf += 6) {
		// 坐标
		buf[0] = vertices[3 * v];
		buf[1] = vertices[3 * v + 1];
		buf[2] = vertices[3 * v + 2];
		// colors
		buf[3] = m_vertexColors[v].redF();
		buf[4] = m_vertexColors[v].greenF();
		buf[5] = m_vertexColors[v].blueF();
	}

	// 创建一个新的缓冲区来存放顶点和颜色 交错存储
	m_vertexBufferObject = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	m_vertexBufferObject.create();
	m_vertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_vertexBufferObject.bind();
	// 现在将缓冲区数据复制到：第一个参数指向数据的指针，第二个参数：大小（以字节为单位）
	m_vertexBufferObject.allocate(m_vertexBufferData.data(), m_vertexBufferData.size() * sizeof(float));

	m_vao.create();
	m_vao.bind();

	unsigned int indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	// 为索引创建新的缓冲区
	m_indexBufferObject = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	m_indexBufferObject.create();
	m_indexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_indexBufferObject.bind();
	m_indexBufferObject.allocate(indices, sizeof(indices));

	// 步长
	int stride = 6 * sizeof(float);

	m_program->enableAttributeArray(0);
	m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

	m_program->enableAttributeArray(1);
	int colorOffset = 3 * sizeof(float);
	m_program->setAttributeBuffer(1, GL_FLOAT, colorOffset, 3, stride);
}

void RectangleWindow::paintGL() {

	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();

	m_vao.bind();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	m_vao.release();

	animate();
}

void RectangleWindow::updateScene()
{
	// 目前，我们只更新颜色
	// 首先更新我们的顶点缓冲区内存，但只更新那些实际更改的位置
	float* buf = m_vertexBufferData.data();
	for (int v = 0; v < 4; ++v, buf += 6) {
		// colors
		buf[3] = m_vertexColors[v].redF();
		buf[4] = m_vertexColors[v].greenF();
		buf[5] = m_vertexColors[v].blueF();
	}

	// 设置上下文
	makeCurrent();

	m_vertexBufferObject.bind();
	m_vertexBufferObject.allocate(m_vertexBufferData.data(), m_vertexBufferData.size() * sizeof(float));

	update();
}

void RectangleWindow::animateColorsTo(const std::vector<QColor>& toColors)
{
	// 当前颜色设置为“fromColors”，toColors 存储在 m_toColors 和
	// 动画计数器被重置
	m_fromColors = m_vertexColors;
	m_toColors = toColors;
	m_frameCount = 0;

	animate();
}

void RectangleWindow::animate()
{
	const unsigned int FRAMECOUNT = 120;
	// 如果已在帧计数结束时，请停止
	if (++m_frameCount > FRAMECOUNT)
		return; 

	// 更新颜色
	double alpha = double(m_frameCount) / FRAMECOUNT;

	// HSV空间中的线性混合可能看起来很“有趣”，但它很简单
	for (unsigned int i = 0; i < m_vertexColors.size(); ++i) {
		float fromH, fromS, fromV;
		m_fromColors[i].getHsvF(&fromH, &fromS, &fromV);
		float toH, toS, toV;
		m_toColors[i].getHsvF(&toH, &toS, &toV);

		m_vertexColors[i] = QColor::fromHsvF(toH * alpha + fromH * (1 - alpha),
			toS * alpha + fromS * (1 - alpha),
			toV * alpha + fromV * (1 - alpha));
	}

	updateScene();
}