#include "mainwindow.h"
#include<QMatrix4x4>
#include<QDebug>
#include<QVector3D>
#include<QKeyEvent>
#include "math.h"
const float Pi=3.14159f;

MainWindow::MainWindow(QWidget *parent):
    QOpenGLWidget (parent)
{
    camera=new Camera(QVector3D(0.0f, 0.0f, 3.0f));
    m_bLeftPressed = false;
    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, [=]{
        m_nTimeValue += 5;
        update();
    });
    m_pTimer->start(40);
}

MainWindow::~MainWindow()
{
    makeCurrent();
    delete ourModel;
    //Camera *camera;
    delete m_pTimer;
    delete camera;
    doneCurrent();
}

void MainWindow::initializeGL()
{
    this->initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    ourShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/model_loading.vert");
    ourShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/model_loading.frag");
    ourShader.link();

    ourModel=new Model("./rock/rock.obj");
}

void MainWindow::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}

void MainWindow::paintGL()
{
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->processInput(1.0f);

    ourShader.bind();

    QMatrix4x4 projection;
    projection.perspective(camera->zoom,1.0f*width()/height(),0.1f,100.0f);
    ourShader.setUniformValue("projection",projection);
    ourShader.setUniformValue("view",camera->getViewMatrix());
    ourShader.setUniformValue("model",QMatrix4x4());

    ourModel->Draw(ourShader);
    ourShader.release();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key >= 0 && key < 1024)
        camera->keys[key] = true;
    if(key==Qt::Key::Key_N)
        soft=!soft;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key >= 0 && key < 1024)
        camera->keys[key] = false;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_bLeftPressed = true;
        m_lastPos = event->pos();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_bLeftPressed = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bLeftPressed) {
        int xpos = event->pos().x();
        int ypos = event->pos().y();

        int xoffset = xpos - m_lastPos.x();
        int yoffset = m_lastPos.y() - ypos;
        m_lastPos = event->pos();
        camera->processMouseMovement(xoffset, yoffset);
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    QPoint offset = event->angleDelta();
    camera->processMouseScroll(offset.y()/20.0f);
}
