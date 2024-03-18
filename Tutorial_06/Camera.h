#pragma once

#include "Transform3D.h"

class Camera : public Transform3D
{
public:
	/*! 返回前向矢量（相对于摄影机的局部坐标/视图系统） */
	QVector3D forward() const {
		const QVector3D LocalForward(0.0f, 0.0f, -1.0f);
		return m_rotation.rotatedVector(LocalForward);
	}

	/*! 返回指向上的矢量（相对于摄影机的局部坐标/视图系统） */
	QVector3D up() const {
		const QVector3D LocalUp(0.0f, 1.0f, 0.0f);
		return m_rotation.rotatedVector(LocalUp);
	}

	/*! 返回指向右侧的矢量（相对于摄影机的局部坐标/视图系统） */
	QVector3D right() const {
		const QVector3D LocalRight(1.0f, 0.0f, 0.0f);
		return m_rotation.rotatedVector(LocalRight);
	}

	// 左乘世界坐标时从世界坐标转换为视图坐标的变换矩阵。注意：没有应用缩放。
	const QMatrix4x4& toMatrix() const {
		if (m_dirty) {
			m_dirty = false;
			m_world.setToIdentity();
			m_world.rotate(m_rotation.conjugated());
			m_world.translate(-m_translation);
		}
		return m_world;
	}
};