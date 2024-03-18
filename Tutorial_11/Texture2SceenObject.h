#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

QT_BEGIN_NAMESPACE
class QOpenGLShaderProgram;
QT_END_NAMESPACE

class Texture2SceenObject
{
public:

	void create(QOpenGLShaderProgram* shaderProgramm);
	void destroy();

	void render();

	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vbo;
};

