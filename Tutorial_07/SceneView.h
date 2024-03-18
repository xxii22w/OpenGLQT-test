#pragma once

#include <qmatrix4x4.h>
#include <QOpenGLTimeMonitor>
#include <QElapsedTimer>
#include "OpenGLWindow.h"
#include "ShaderProgram.h"
#include "KeyboardMouseHandler.h"
#include "GridObject.h"
#include "BoxObject.h"
#include "PickLineObject.h"
#include "Camera.h"
#include <qopenglfunctions.h>

/*
类SceneView扩展了基元OpenGLWindow
通过添加键盘/鼠标事件处理和不同的渲染
对象（封装着色器程序和缓冲区对象）。
*/

class SceneView  : public OpenGLWindow
{
public:
	SceneView();
	virtual ~SceneView() override;

protected:
	void initializeGL() override;
	void resizeGL(int width, int height) override;
	void paintGL() override;

	// 处理按键和鼠标按下事件的函数，所有工作都在KeyboardMouseHandler类中完成
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

	void pick(const QPoint& globalMousePos);
private:
	// 如果接收到任何相关输入，则进行测试，并记录状态变化。
	void checkInput();

	// 这个函数在paintGL（）例程中首先被调用，并且处理到目前为止接收到的输入并更新相机位置。
	void processInput();

	// 将相机矩阵和投影矩阵相结合，形成世界观矩阵。
	void updateWorld2ViewMatrix();

	/*! 
		确定哪些物体/平面被选中，并相应地给它们上色。
		nearPoint和farPoint定义当前光线，并在模型坐标中给出。
	*/
	void selectNearestObject(const QVector3D& nearPoint, const QVector3D& farPoint);

	// 如果设置为true，则接收到一个输入事件，该事件将在下次重新绘制时进行评估。
	bool m_inputEventReceived;

	// 输入处理程序，用于封装事件处理代码。
	KeyboardMouseHandler m_keyboardMouseHandler;

	// 投影矩阵，每当视口几何图形发生变化时都会更新（在resizeGL（）中）
	QMatrix4x4	m_projection;
	Transform3D m_transform;					// 世界变换矩阵生成器
	Camera	m_camera;		// 相机位置、方向和镜头数据
	QMatrix4x4  m_worldToView;	// 缓存世界以查看转换矩阵

	/*! All shader programs used in the scene. */
	QList<ShaderProgram>		m_shaderPrograms;

	BoxObject					m_boxObject;
	GridObject					m_gridObject;
	PickLineObject				m_pickLineObject;

	// 计算耗时
	QOpenGLTimeMonitor			m_gpuTimers;
	QElapsedTimer				m_cpuTimer;
};
