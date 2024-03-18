#pragma once

#include <qvector3d.h>
#include <qquaternion.h>	// 四元数
#include <qmatrix4x4.h>

/*
	一个典型的三维变换类。
	实现惰性求值-调用任何修改函数		惰性求值 == 它允许程序仅在需要时才对表达式进行求值
	将仅存储有关修改的信息-仅通过检索
	matrix与toMatrix（）的组合，返回实际的矩阵。
*/

class Transform3D
{
public:
	Transform3D();

	// transform By (Add/Scale)
	void translate(const QVector3D& dt);
	void translate(float dx, float dy, float dz);
	void scale(const QVector3D& ds);
	void scale(float dx, float dy, float dz);
	void scale(float factor);
	void rotate(const QQuaternion& dr);
	void rotate(float angle, const QVector3D& axis);	// 角度 轴体
	void rotate(float angle, float ax, float ay, float az);	
	void grow(const QVector3D& ds);
	void grow(float dx, float dy, float dz);
	void grow(float factor);

	// Transform To (Setters)
	void setTranslation(const QVector3D& t);
	void setTranslation(float x, float y, float z);
	void setScale(const QVector3D& s);
	void setScale(float x, float y, float z);
	void setScale(float k);
	void setRotation(const QQuaternion& r);
	void setRotation(float angle, const QVector3D& axis);
	void setRotation(float angle, float ax, float ay, float az);

	// Accessors
	const QVector3D& translation() const;
	const QVector3D& scale() const;
	const QQuaternion& rotation() const;
	const QMatrix4x4& toMatrix() const;

protected:
	QVector3D m_translation;
	QVector3D m_scale;
	QQuaternion m_rotation;
	mutable QMatrix4x4 m_world;	// 在const toMatrix（）函数中更新
	mutable bool m_dirty;

#ifndef QT_NO_DATASTREAM
	friend QDataStream& operator<<(QDataStream& out, const Transform3D& transform);
	friend QDataStream& operator>>(QDataStream& in, Transform3D& transform);
#endif // !QT_NO_DATASTREAM

};

Q_DECLARE_TYPEINFO(Transform3D, Q_MOVABLE_TYPE);

inline Transform3D::Transform3D() : m_scale(1.0f,1.0f,1.0f),m_dirty(true){}

// Transform By (Add/Scale)
inline void Transform3D::translate(float dx, float dy, float dz) { translate(QVector3D(dx, dy, dz)); }
inline void Transform3D::scale(float dx, float dy, float dz) { scale(QVector3D(dx, dy, dz)); }
inline void Transform3D::scale(float factor) { scale(QVector3D(factor, factor, factor)); }
inline void Transform3D::rotate(float angle, const QVector3D& axis) { rotate(QQuaternion::fromAxisAndAngle(axis, angle)); }
inline void Transform3D::rotate(float angle, float ax, float ay, float az) { rotate(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }
inline void Transform3D::grow(float dx, float dy, float dz) { grow(QVector3D(dx, dy, dz)); }
inline void Transform3D::grow(float factor) { grow(QVector3D(factor, factor, factor)); }

// Transform To (Setters)
inline void Transform3D::setTranslation(float x, float y, float z) { setTranslation(QVector3D(x, y, z)); }
inline void Transform3D::setScale(float x, float y, float z) { setScale(QVector3D(x, y, z)); }
inline void Transform3D::setScale(float k) { setScale(QVector3D(k, k, k)); }
inline void Transform3D::setRotation(float angle, const QVector3D& axis) { setRotation(QQuaternion::fromAxisAndAngle(axis, angle)); }
inline void Transform3D::setRotation(float angle, float ax, float ay, float az) { setRotation(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }

// Accessors
inline const QVector3D& Transform3D::translation() const { return m_translation; }
inline const QVector3D& Transform3D::scale() const { return m_scale; }
inline const QQuaternion& Transform3D::rotation() const { return m_rotation; }

// QT streams
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const Transform3D& transform);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& operator<<(QDataStream& out, const Transform3D& transform);
QDataStream& operator>>(QDataStream& in, Transform3D& transform);
#endif