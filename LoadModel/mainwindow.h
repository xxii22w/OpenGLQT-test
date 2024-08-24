#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<QOpenGLWidget>
#include<QOpenGLShader>
#include<QDebug>
#include<QOpenGLFunctions>
#include<QOpenGLVertexArrayObject>
#include<QOpenGLBuffer>
#include<QOpenGLTexture>
#include<QTimer>

#include "camera.h"
#include "model.h"
class MainWindow : public QOpenGLWidget,protected QOpenGLFunctions
{
public:
    MainWindow(QWidget *parent=nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;
protected:
    virtual void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w,int h) Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
    QOpenGLShaderProgram ourShader;
    Model *ourModel;
    Camera *camera;
    QTimer* m_pTimer = nullptr;
    int     m_nTimeValue = 0;
    bool m_bLeftPressed;
    QPoint m_lastPos;
    bool soft=false;
};

#endif // MAINWINDOW_H
