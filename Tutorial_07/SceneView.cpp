#include "SceneView.h"

#include <QExposeEvent>
#include <QOpenGLShaderProgram>
#include <QDateTime>

#include "DebugApplication.h"
#include "PickObject.h"

#define SHADER(x) m_shaderPrograms[x].shaderProgram()

SceneView::SceneView() :
	m_inputEventReceived(false)
{
	// tell keyboard handler to monitor certain keys
	m_keyboardMouseHandler.addRecognizedKey(Qt::Key_W);
	m_keyboardMouseHandler.addRecognizedKey(Qt::Key_A);
	m_keyboardMouseHandler.addRecognizedKey(Qt::Key_S);
	m_keyboardMouseHandler.addRecognizedKey(Qt::Key_D);
	m_keyboardMouseHandler.addRecognizedKey(Qt::Key_Q);
	m_keyboardMouseHandler.addRecognizedKey(Qt::Key_E);
	m_keyboardMouseHandler.addRecognizedKey(Qt::Key_Shift);

	// *** create scene (no OpenGL calls are being issued below, just the data structures are created.

	// Shaderprogram #0 : regular geometry (painting triangles via element index)
	ShaderProgram blocks(":/shaders/withWorldAndCamera.vert", ":/shaders/simple.frag");
	blocks.m_uniformNames.append("worldToView");
	m_shaderPrograms.append(blocks);

	// Shaderprogram #1 : grid (painting grid lines)
	ShaderProgram grid(":/shaders/grid.vert", ":/shaders/grid.frag");
	grid.m_uniformNames.append("worldToView"); // mat4
	grid.m_uniformNames.append("gridColor"); // vec3
	grid.m_uniformNames.append("backColor"); // vec3
	m_shaderPrograms.append(grid);

	// *** initialize camera placement and model placement in the world

	// move camera a little back (mind: positive z) and look straight ahead
	m_camera.translate(0, 17, 50);
	// look slightly down
	m_camera.rotate(-5, m_camera.right());
	// look slightly left
	m_camera.rotate(-10, QVector3D(0.0f, 1.0f, 0.0f));
}


SceneView::~SceneView() {
	if (m_context) {
		m_context->makeCurrent(this);

		for (ShaderProgram& p : m_shaderPrograms)
			p.destroy();

		m_boxObject.destroy();
		m_gridObject.destroy();
		m_pickLineObject.destroy();

		m_gpuTimers.destroy();
	}
}


void SceneView::initializeGL() {
	FUNCID(SceneView::initializeGL);
	try {
		// initialize shader programs
		for (ShaderProgram& p : m_shaderPrograms)
			p.create();

		// // 背面剔除 优化性能
		glEnable(GL_CULL_FACE);
		//  深度缓冲
		glEnable(GL_DEPTH_TEST);

		// initialize drawable objects
		m_boxObject.create(SHADER(0));
		m_gridObject.create(SHADER(1));
		m_pickLineObject.create(SHADER(0));

		// Timer
		m_gpuTimers.setSampleCount(6);
		m_gpuTimers.create();
	}
	catch (OpenGLException& ex) {
		throw OpenGLException(ex, "OpenGL initialization failed.", FUNC_ID);
	}
}


void SceneView::resizeGL(int width, int height) {
	// 投影矩阵只需要针对窗口大小的变化进行更新
	m_projection.setToIdentity();
	// create projection matrix, i.e. camera lens
	m_projection.perspective(
		/* vertical angle */ 45.0f,
		/* aspect ratio */   width / float(height),
		/* near */           0.1f,
		/* far */            1000.0f
	);
	// 注意:不要在近平面使用0.0，否则深度缓冲和深度测试将无法工作!

	// update cached world2view matrix
	updateWorld2ViewMatrix();
}


void SceneView::paintGL() {
	m_cpuTimer.start();
	if (((DebugApplication*)qApp)->m_aboutToTerminate)
		return;

	// 处理输入，即检查是否有任何键被按下
	if (m_inputEventReceived)
		processInput();

	const qreal retinaScale = devicePixelRatio(); // 有视网膜显示屏的mac电脑需要的
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);
	qDebug() << "SceneView::paintGL(): Rendering to:" << width() << "x" << height();


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	QVector3D backColor(0.1f, 0.15f, 0.3f);
	glClearColor(0.1f, 0.15f, 0.3f, 1.0f);

	QVector3D gridColor(0.5f, 0.5f, 0.7f);

	m_gpuTimers.reset();

	m_gpuTimers.recordSample(); // setup boxes

	// *** render boxes
	SHADER(0)->bind();
	SHADER(0)->setUniformValue(m_shaderPrograms[0].m_uniformIDs[0], m_worldToView);

	m_gpuTimers.recordSample(); // render boxes
	m_boxObject.render();

	m_gpuTimers.recordSample(); // render pickline
	if (m_pickLineObject.m_visible)
		m_pickLineObject.render();

	SHADER(0)->release();

	// *** render grid ***

	m_gpuTimers.recordSample(); // setup grid
	SHADER(1)->bind();
	SHADER(1)->setUniformValue(m_shaderPrograms[1].m_uniformIDs[0], m_worldToView);
	SHADER(1)->setUniformValue(m_shaderPrograms[1].m_uniformIDs[1], gridColor);
	SHADER(1)->setUniformValue(m_shaderPrograms[1].m_uniformIDs[2], backColor);

	m_gpuTimers.recordSample(); // render grid
	m_gridObject.render();
	SHADER(1)->release();

	m_gpuTimers.recordSample(); // done painting


#if 0
	// do some animation stuff
	m_transform.rotate(1.0f, QVector3D(0.0f, 0.1f, 0.0f));
	updateWorld2ViewMatrix();
	renderLater();
#endif

	checkInput();

	QVector<GLuint64> intervals = m_gpuTimers.waitForIntervals();
	for (GLuint64 it : intervals)
		qDebug() << "  " << it * 1e-6 << "ms/frame";
	QVector<GLuint64> samples = m_gpuTimers.waitForSamples();
	qDebug() << "Total render time: " << (samples.back() - samples.front()) * 1e-6 << "ms/frame";

	qint64 elapsedMs = m_cpuTimer.elapsed();
	qDebug() << "Total paintGL time: " << elapsedMs << "ms";
}


void SceneView::keyPressEvent(QKeyEvent* event) {
	m_keyboardMouseHandler.keyPressEvent(event);
	checkInput();
}

void SceneView::keyReleaseEvent(QKeyEvent* event) {
	m_keyboardMouseHandler.keyReleaseEvent(event);
	checkInput();
}

void SceneView::mousePressEvent(QMouseEvent* event) {
	m_keyboardMouseHandler.mousePressEvent(event);
	checkInput();
}

void SceneView::mouseReleaseEvent(QMouseEvent* event) {
	m_keyboardMouseHandler.mouseReleaseEvent(event);
	checkInput();
}

void SceneView::mouseMoveEvent(QMouseEvent* /*event*/) {
	checkInput();
}

void SceneView::wheelEvent(QWheelEvent* event) {
	m_keyboardMouseHandler.wheelEvent(event);
	checkInput();
}


void SceneView::pick(const QPoint& globalMousePos) {
	// local mouse coordinates
	QPoint localMousePos = mapFromGlobal(globalMousePos);
	int my = localMousePos.y();
	int mx = localMousePos.x();

	// viewport dimensions
	const qreal retinaScale = devicePixelRatio(); // needed for Macs with retina display
	qreal halfVpw = width() * retinaScale / 2;
	qreal halfVph = height() * retinaScale / 2;

	// invert world2view matrix, with m_worldToView = m_projection * m_camera.toMatrix() * m_transform.toMatrix();
	bool invertible;
	QMatrix4x4 projectionMatrixInverted = m_worldToView.inverted(&invertible);
	if (!invertible) {
		qWarning() << "Cannot invert projection matrix.";
		return;
	}

	// mouse position in NDC space, one point on near plane and one point on far plane
	QVector4D near(
		(mx - halfVpw) / halfVpw,
		-1 * (my - halfVph) / halfVph,
		-1,
		1.0);

	QVector4D far(
		near.x(),
		near.y(),
		1,
		1.0);

	// transform from NDC to model coordinates
	QVector4D nearResult = projectionMatrixInverted * near;
	QVector4D farResult = projectionMatrixInverted * far;
	// don't forget normalization!
	nearResult /= nearResult.w();
	farResult /= farResult.w();

	// update pick line vertices (visualize pick line)
	m_context->makeCurrent(this);
	m_pickLineObject.setPoints(nearResult.toVector3D(), farResult.toVector3D());

	// now do the actual picking - for now we implement a selection
	selectNearestObject(nearResult.toVector3D(), farResult.toVector3D());
}


void SceneView::checkInput() {
	// 每当发出_any_键/鼠标事件时调用此函数

	// 我们测试，如果键处理程序的当前状态需要一个场景更新
	// (相机移动)，如果是，我们只是设置一个标志，在下次重绘
	// 我们安排重新粉刷

	// trigger key held?
	if (m_keyboardMouseHandler.buttonDown(Qt::RightButton)) {
		// any of the interesting keys held?
		if (m_keyboardMouseHandler.keyDown(Qt::Key_W) ||
			m_keyboardMouseHandler.keyDown(Qt::Key_A) ||
			m_keyboardMouseHandler.keyDown(Qt::Key_S) ||
			m_keyboardMouseHandler.keyDown(Qt::Key_D) ||
			m_keyboardMouseHandler.keyDown(Qt::Key_Q) ||
			m_keyboardMouseHandler.keyDown(Qt::Key_E))
		{
			m_inputEventReceived = true;
			//			qDebug() << "SceneView::checkInput() inputEventReceived";
			renderLater();
			return;
		}

		// has the mouse been moved?
		if (m_keyboardMouseHandler.mouseDownPos() != QCursor::pos()) {
			m_inputEventReceived = true;
			//			qDebug() << "SceneView::checkInput() inputEventReceived: " << QCursor::pos() << m_keyboardMouseHandler.mouseDownPos();
			renderLater();
			return;
		}
	}
	// has the left mouse butten been release
	if (m_keyboardMouseHandler.buttonReleased(Qt::LeftButton)) {
		m_inputEventReceived = true;
		renderLater();
		return;
	}

	// scroll-wheel turned?
	if (m_keyboardMouseHandler.wheelDelta() != 0) {
		m_inputEventReceived = true;
		renderLater();
		return;
	}
}


void SceneView::processInput() {
	// 函数必须仅在接收到输入事件时调用
	Q_ASSERT(m_inputEventReceived);
	m_inputEventReceived = false;
	//	qDebug() << "SceneView::processInput()";

		// check for trigger key
	if (m_keyboardMouseHandler.buttonDown(Qt::RightButton)) {

		// Handle translations
		QVector3D translation;
		if (m_keyboardMouseHandler.keyDown(Qt::Key_W)) 		translation += m_camera.forward();
		if (m_keyboardMouseHandler.keyDown(Qt::Key_S)) 		translation -= m_camera.forward();
		if (m_keyboardMouseHandler.keyDown(Qt::Key_A)) 		translation -= m_camera.right();
		if (m_keyboardMouseHandler.keyDown(Qt::Key_D)) 		translation += m_camera.right();
		if (m_keyboardMouseHandler.keyDown(Qt::Key_Q)) 		translation -= m_camera.up();
		if (m_keyboardMouseHandler.keyDown(Qt::Key_E)) 		translation += m_camera.up();

		float transSpeed = 0.8f;
		if (m_keyboardMouseHandler.keyDown(Qt::Key_Shift))
			transSpeed = 0.1f;
		m_camera.translate(transSpeed * translation);

		// 处理旋转
		// 获取并重置鼠标增量(传递当前鼠标光标位置)
		QPoint mouseDelta = m_keyboardMouseHandler.resetMouseDelta(QCursor::pos()); // 重置内部位置
		static const float rotatationSpeed = 0.4f;
		const QVector3D LocalUp(0.0f, 1.0f, 0.0f); // same as in Camera::up()
		m_camera.rotate(-rotatationSpeed * mouseDelta.x(), LocalUp);
		m_camera.rotate(-rotatationSpeed * mouseDelta.y(), m_camera.right());

	}
	int wheelDelta = m_keyboardMouseHandler.resetWheelDelta();
	if (wheelDelta != 0) {
		float transSpeed = 8.f;
		if (m_keyboardMouseHandler.keyDown(Qt::Key_Shift))
			transSpeed = 0.8f;
		m_camera.translate(wheelDelta * transSpeed * m_camera.forward());
	}

	
	if (m_keyboardMouseHandler.buttonReleased(Qt::LeftButton)) {
		pick(m_keyboardMouseHandler.mouseReleasePos());
	}

	// 最后，重置“WasPressed”键状态
	m_keyboardMouseHandler.clearWasPressedKeyStates();

	updateWorld2ViewMatrix();
	// not need to request update here, since we are called from paint anyway
}


void SceneView::updateWorld2ViewMatrix() {
	// transformation steps:
	//   model space -> transform -> world space
	//   world space -> camera/eye -> camera view
	//   camera view -> projection -> normalized device coordinates (NDC)
	m_worldToView = m_projection * m_camera.toMatrix() * m_transform.toMatrix();
}


void SceneView::selectNearestObject(const QVector3D& nearPoint, const QVector3D& farPoint) {
	QElapsedTimer pickTimer;
	pickTimer.start();

	// 计算视图方向
	QVector3D d = farPoint - nearPoint;

	// 创建拾取对象，距离是一个介于0和1之间的值，所以为了安全起见，初始化为2(非常靠后)。
	PickObject p(2.f, std::numeric_limits<unsigned int>::max());

	// 现在处理所有对象并更新p以保存最近的命中
	m_boxObject.pick(nearPoint, d, p);
	// ... other objects

	// 有什么东西可以挑吗?
	if (p.m_objectId == std::numeric_limits<unsigned int>::max())
		return; // nothing selected

	qDebug().nospace() << "Pick successful (Box #"
		<< p.m_objectId << ", Face #" << p.m_faceId << ", t = " << p.m_dist << ") after "
		<< pickTimer.elapsed() << " ms";

	// 注意:当我们调用这个函数时，OpenGL-context必须是当前的!
	m_boxObject.highlight(p.m_objectId, p.m_faceId);
}