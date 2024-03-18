#include "TriangleWindow.h"
#include <qdebug.h>

TriangleWindow::TriangleWindow()
	:m_program(nullptr)
{

}

TriangleWindow::~TriangleWindow()
{
	// 资源清理

	// 由于我们发布了与opengl上下文相关的资源
	// 在清理之前，我们会使此上下文保持最新状态
	bool success = m_context->makeCurrent(this);
	if (!success)
	{
		qDebug() << "Cannot make OpenGL context current.";
	}

	m_VAO.destroy();
	m_vertexBufferObject.destroy();
	delete m_program;
}

void TriangleWindow::initialize()
{
	// 此函数在首次显示窗口时调用一次，即当首次呈现窗口内容构建和编译我们的着色器程序
	m_program = new QOpenGLShaderProgram();

	// 从资源中读取着色器程序
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/pass_through.vert"))
		qDebug() << "Vertex shader error:\n" << m_program->log();

	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/uniform_color.frag"))
		qDebug() << "Fragment shader errors:\n" << m_program->log();

	if (!m_program->link())
		qDebug() << "Shader linker errors:\n" << m_program->log();

	// 设置顶点数据（和缓冲区）并配置顶点属性
	// ------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	// 创建 一个新的顶点缓冲区
	m_vertexBufferObject = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);	// VBO
	m_vertexBufferObject.create();
	m_vertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);	// 必须在分配前调用

	m_vertexBufferObject.bind();	// 在上下文中将其设置为活动状态，以便我们可以写入它
	// int bufSize = sizeof(vertices) = 9 * sizeof(float) = 9*4 = 36 bytes
	m_vertexBufferObject.allocate(vertices, sizeof(vertices));

	// 	初始化顶点数组对象 （VAO） 以记录和记住后续属性关联生成的顶点缓冲区
	m_VAO.create();	// 创建底层 OpenGL 对象
	m_VAO.bind();	// 将顶点数组对象当前设置为 OpenGL 上下文，以便它监视属性分配

	// 现在，所有以下 enableAttributeArray（）、disableAttributeArray（） 和 setAttributeBuffer（） 调用都是在当前绑定的 VBA 中“录制”。
	// Enable attribute array at layout location 0
	m_program->enableAttributeArray(0);
	m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3);
	// 这会将我们在 VBO 中设置的数据映射到“position”属性。
	// 0 - 偏移量 - 表示“位置”数据从存储器数组的开头开始
	// 3 - 每个顶点的大小 （=vec3） - 表示每个位置元组的大小为 3 个浮点数（即 3 个坐标，注意：这是GL_FLOAT的大小，而不是以字节为单位的大小！)

	// 释放vao
	m_vertexBufferObject.release();
	m_VAO.release();
}

void TriangleWindow::render()
{
	// 每一帧渲染到屏幕都会调用这个函数
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	// 设置背景颜色
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 使用我们的着色程序
	m_program->bind();
	// 绑定 VAO，而 Vertex Array 对象又绑定 VBO 和在 OpenGL 上下文中设置属性缓冲区
	m_VAO.bind();
	// 现在绘制三角形
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// 最后释放VAO
	m_VAO.release();
}