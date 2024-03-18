#pragma once

#include <qvector3d.h>

/*! 一个对象，用于保存被单击对象的信息. */
struct PickObject {
	PickObject(float dist, unsigned int id) :
		m_dist(dist), m_objectId(id), m_faceId(0)
	{
	}
	PickObject(float dist, unsigned int id, unsigned int faceId) :
		m_dist(dist), m_objectId(id), m_faceId(faceId)
	{
	}

	float m_dist; // 交点到拾取线起点的归一化距离
	unsigned int m_objectId; // 被点击的对象
	unsigned int m_faceId; // 实际的三角形/平面点击
};

/*
测试一条直线(方程p = p1 + t * d)是否碰到一个平面，定义为
p = x * a + y * b。如果找到交集返回true，并返回
交点与点p1之间的归一化距离t。
*/

bool intersectsRect(const QVector3D& a,
	const QVector3D& b,
	const QVector3D& normal,
	const QVector3D& offset,
	const QVector3D& p1,
	const QVector3D& d,
	float& dist);