#pragma once

#include <QDialog>
class RectangleWindow;

class TestDialog : public QDialog
{
	Q_OBJECT

public:
	TestDialog();
	

private slots:
	void onChangeColors();
	void onAnimateColors();

private:
	RectangleWindow* m_rectangleWindow;
};
