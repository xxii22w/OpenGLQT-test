#pragma once

#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE

class ShaderProgram
{
public:
	ShaderProgram();
	ShaderProgram(const QString& vertexShaderFilePath,const QString& fragmentShaderFilePath);

	// 创建shader程序。编译链接程序
	void create();
	// 析构opengl资源，函数被调用时，必须将opemgl上下文设置为当前
	void destroy();

	// 访问本机的shader程序
	QOpenGLShaderProgram* shaderProgram() { return m_program; }

	// 在create()函数中调用，来获取vertex shader文件路径
	QString  m_vertexShaderFilePath;
	// ...
	QString  m_fragmentShaderFilePath;

	// 注意：统一处理非常简单，可能最好以某种方式进行包装。
	QStringList m_uniformNames;
	QList<int> m_uniformIDs;

private:
	QOpenGLShaderProgram* m_program;
};
