#include "TestDialog.h"
#include <iostream>
#include <ctime>

#include <QApplication>
#include <QDateTime>

#include "OpenGLException.h"
#include "DebugApplication.h"

void qDebugMsgHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
	(void)context;
	QString msgPrefix = "[" + QDateTime::currentDateTime().toString() + "] ";
	switch (type) {
	case QtDebugMsg: msgPrefix += "Debug:    "; break;
	case QtWarningMsg: msgPrefix += "Warning:  "; break;
	case QtCriticalMsg: msgPrefix += "Critical: "; break;
	case QtFatalMsg: msgPrefix += "Fatal:    "; break;
	case QtInfoMsg: msgPrefix += "Info:     "; break;
	}
	QStringList lines = msg.split("\n");
	for (const QString& l : lines)
		std::cout << (msgPrefix + l).toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
	qInstallMessageHandler(qDebugMsgHandler);

	DebugApplication app(argc, argv);

	srand(time(nullptr));

	TestDialog dlg;
	dlg.show();
	return app.exec();
}
