#include "PickObject.h"
#include <QDebug>

/* Solves equation system with Cramer's rule:
	 a x + c y = e
	 b x + d y = f
*/
bool solve(double a, double b, double c, double d, double e, double f, double& x, double& y) {
	double det = a * d - b * c;
	if (det == 0.)
		return false;

	x = (e * d - c * f) / det;
	y = (a * f - e * b) / det;
	return true;
}

bool intersectsRect(const QVector3D& a,
	const QVector3D& b,
	const QVector3D& normal,
	const QVector3D& offset,
	const QVector3D& p1,
	const QVector3D& d,
	float& dist)
{
	// 首先是常规测试

	double angle = QVector3D::dotProduct(d, normal) / qAbs(d.length());
	// 条件一，法向量方向相同?
	if (angle >= 0)
		return false; // no intersection possible

	// 计算直线交点
	double t = QVector3D::dotProduct(offset - p1, normal) / QVector3D::dotProduct(d, normal);
	// 情况2:视线范围外?
	if (t < 0 || t > 1)
		return false;

	// 现在确定平面上的位置
	QVector3D x0 = p1 + t * d;

	QVector3D rhs = x0 - offset; // 方程组右侧:a * x + b * y = (x - offset)

	// 我们有三种可能的方法来获得交点，尝试所有的方法直到我们成功
	double x, y;
	// rows 1 and 2
	if (solve(a.x(), a.y(), b.x(), b.y(), rhs.x(), rhs.y(), x, y)) {
		// 条件3:检查是否在rect内
		if (x > 0 && x < 1 && y > 0 && y < 1) {
			dist = t;
			//			qDebug() << "Intersection found (1) at t = " << dist;
			return true;
		}
		else
			return false;
	}
	// rows 1 and 3
	if (solve(a.x(), a.z(), b.x(), b.z(), rhs.x(), rhs.z(), x, y)) {
		// 条件3:检查是否在rect内
		if (x > 0 && x < 1 && y > 0 && y < 1) {
			dist = t;
			//			qDebug() << "Intersection found (2) at t = " << dist;
			return true;
		}
		else
			return false;
	}
	// rows 2 and 3
	if (solve(a.y(), a.z(), b.y(), b.z(), rhs.y(), rhs.z(), x, y)) {
		// Condition 3: check if inside rect
		if (x > 0 && x < 1 && y > 0 && y < 1) {
			dist = t;
			//			qDebug() << "Intersection found (3) at t = " << dist;
			return true;
		}
		else
			return false;
	}

	return false;
}