
#include <QtWidgets/QApplication>
#include "TestDialog.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	TestDialog dlg;
	// 透明窗口
	dlg.setAttribute(Qt::WA_TranslucentBackground, true);
	// 无边框
	dlg.setWindowFlag(Qt::FramelessWindowHint, true);
	dlg.show();

	return app.exec();
}
