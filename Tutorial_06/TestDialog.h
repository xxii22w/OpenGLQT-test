#pragma once

#include <QDialog>

class SceneView;

class TestDialog : public QDialog
{
    Q_OBJECT

public:
    TestDialog();

private:
    SceneView* m_sceneView;
};
