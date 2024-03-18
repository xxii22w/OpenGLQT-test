#include "ShaderProgram.h"

#include <QOpenGLShaderProgram>
#include <QDebug>

ShaderProgram::ShaderProgram(const QString& vertexShaderFilePath, const QString& fragmentShaderFilePath) :
	m_vertexShaderFilePath(vertexShaderFilePath),
	m_fragmentShaderFilePath(fragmentShaderFilePath),
	m_program(nullptr)
{
}

void ShaderProgram::create()
{
	assert(m_program == nullptr);

	m_program = new QOpenGLShaderProgram();

	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, m_vertexShaderFilePath))
		qDebug() << "Vertex shader errors:\n" << m_program->log();

	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, m_fragmentShaderFilePath))
		qDebug() << "Fragment shader errors:\n" << m_program->log();

	if (!m_program->link())
		qDebug() << "Shader linker errors:\n" << m_program->log();

	m_uniformIDs.clear();
	for (const QString& uniformName : m_uniformNames)
		m_uniformIDs.append(m_program->uniformLocation(uniformName));
}

void ShaderProgram::destroy() {
	delete m_program;
	m_program = nullptr;
}