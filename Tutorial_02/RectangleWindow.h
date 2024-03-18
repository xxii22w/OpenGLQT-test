#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLWindow>
#include <qopenglfunctions.h>

/*
    这是显示两个三角形形成矩形的窗口。
    我们从 QOpenGLWindow 基类派生并实现
    虚拟 initializeGL（） 和 paintGL（） 函数。

*/


class RectangleWindow : public QOpenGLWindow,protected QOpenGLFunctions {
public:
    RectangleWindow();
    virtual ~RectangleWindow() override;

    void initializeGL() override;
    void paintGL() override;

    // 更新场景并请求画图更新
    void updateScene();

    void animateColorsTo(const std::vector<QColor>& toColors);

    // 保存下次调用 updateScene（） 时设置的顶点颜色
    std::vector<QColor> m_vertexColors;

private:
    // 稍微修改一下场景，然后调用 updateScene（）
    // 当已经处于最终状态时，不做任何事情
    void animate();

    // VAO 顶点数组对象
    QOpenGLVertexArrayObject	m_vao;
    // 顶点缓冲区
    QOpenGLBuffer				m_vertexBufferObject;
    // 索引缓冲区去画两个矩形
    QOpenGLBuffer				m_indexBufferObject;
    // 着色器程序
    QOpenGLShaderProgram* m_program;

    std::vector<float> m_vertexBufferData;

    // 存储我们针对的目标颜色进行动画处理
    std::vector<QColor> m_toColors;
    // 存储我们制作动画的目标颜色
    std::vector<QColor> m_fromColors;
    // 用于动画的帧数
    unsigned int m_frameCount;
};