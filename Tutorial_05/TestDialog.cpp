#include "TestDialog.h"

#include <qpushbutton.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qlabel.h>

#include "SceneView.h"

TestDialog::TestDialog():
#ifdef Q_OS_WIN
	QDialog(nullptr,Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
#else
	QDialog(nullptr, Qt::Window)
#endif // Q_OS_WIN
{
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(4, 5);
	format.setSamples(4);	// 启用多采样(抗混叠)
	format.setDepthBufferSize(8);	// 使用深度缓冲就需要打开，这是设置进度的

	m_sceneView = new SceneView;
	m_sceneView->setFormat(format);

	QWidget* container = QWidget::createWindowContainer(m_sceneView);
	container->setFocusPolicy(Qt::TabFocus);
	container->setMinimumSize(QSize(640, 400));

	QVBoxLayout* vlay = new QVBoxLayout;
	vlay->setContentsMargins(0, 0, 0, 0);
	vlay->setSpacing(0);
	vlay->addWidget(container);

	QHBoxLayout* hlay = new QHBoxLayout;
	hlay->setContentsMargins(0, 0, 0, 0);

	QLabel* navigationInfo = new QLabel(this);
	navigationInfo->setWordWrap(true);
	navigationInfo->setText("Hold right mouse button for free mouse look and to navigate "
		"with keys WASDQE. Hold shift to slow down. Use scroll-wheel to move quickly forward and backward.");

	hlay->addWidget(navigationInfo);

	QPushButton* closeBtn = new QPushButton(tr("Close"), this);
	connect(closeBtn, &QPushButton::clicked, this, &QDialog::accepted);

	hlay->addWidget(closeBtn);
	hlay->setStretch(0, 1);

	vlay->addLayout(hlay);

	setLayout(vlay);

	resize(700, 450);

	container->setFocus();
}
