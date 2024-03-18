#pragma once

#include <QPoint>
#include <vector>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class KeyboardMouseHandler
{
public:
	KeyboardMouseHandler();
	virtual ~KeyboardMouseHandler();
	
	// 为我们正在监听的每个键调用此函数
	void addRecognizedKey(Qt::Key k);
	// 清楚按键列表
	void clearRecognizedKeys();

	// 处理按键和鼠标按下事件的功能
	// 如果按下了已识别的键/鼠标按钮，并且场景可能需要更新。
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

	// 每一个键被按都会调用
	bool pressKey(Qt::Key k);
	// 每一个键被释放都会调用
	bool releaseKey(Qt::Key k);
	// 鼠标点击的时候被调用
	bool pressButton(Qt::MouseButton btn, QPoint currentPos);
	// 鼠标释放的时候被调用
	bool releaseButton(Qt::MouseButton btn);

	// 返回，无论该键是被按下还是在上一个查询间隔中被按下。
	bool keyDown(Qt::Key k) const;
	// 返回，无论鼠标按钮是被按下还是在上次查询间隔中被按下。
	bool buttonDown(Qt::MouseButton btn) const;

	/*
		返回按下鼠标按钮时记录的位置（全局位置）。
		使用此函数可确定鼠标是否已移动（通过将其与QCursor:：pos（）进行比较）。
	*/
	QPoint mouseDownPos() const { return m_mouseDownPos; }

	/*
		返回上次和当前鼠标位置之间的差值，并*更新*
		最后一个鼠标位置为currentPos。
		检索到的点（x和y距离）应用于修改基于状态的变换。
	*/
	QPoint resetMouseDelta(const QPoint currentPos);

	/*
		检索到目前为止相加的距离（角度，单位为度）。
		使用此功能可以查询鼠标滚轮是否已转动。
	*/
	int wheelDelta() const;

	/*
		检索上次查询间隔中相加的车轮距离（角度，单位为度）并将其重置为零。
	*/
	int resetWheelDelta();

	/*	这将重置当前标记为“WasPressed”的所有关键状态 */
	void clearWasPressedKeyStates();

private:
	enum KeyStates {
		StateNotPressed,
		StateHeld,
		StateWasPressed
	};

	std::vector<Qt::Key>	m_keys;
	std::vector<KeyStates>	m_keyStates;

	KeyStates				m_leftButtonDown;
	KeyStates				m_middleButtonDown;
	KeyStates				m_rightButtonDown;

	QPoint					m_mouseDownPos;

	int						m_wheelDelta;
};
