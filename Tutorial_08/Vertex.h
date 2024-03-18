#pragma once

#include <qvector3d.h>
#include <qcolor.h>

/*
一个容器类，用于存储顶点的数据（坐标、法线、纹理、颜色），用于交错存储根据需要展开此类。
内存布局（每个字符是一个字节）：xxxxyyyyzzzzrrrrggggbbbb=6*4=24字节
可以定义向量＜顶点＞，并将其直接用作顶点缓冲区的输入。
注意编译器的隐式填充！因此，对于分配用途：-sizeof（顶点）作为步幅-offsetof（顶点，r）作为颜色的开始偏移
只有在结构中使用混合数据类型时，这一点才会变得重要。
阅读http://www.catb.org/esr/structure-packing/进行深入解释。

*/

struct Vertex
{
	Vertex(){}
	Vertex(const QVector3D& coords, const QColor& col) :
		x(float(coords.x())),
		y(float(coords.y())),
		z(float(coords.z())),
		r(float(col.redF())),
		g(float(col.greenF())),
		b(float(col.blueF())),
		texi(0),
		texj(0),
		texID(0)
	{
	}
	Vertex(const QVector3D& coords, const QColor& col, float textureX, float textureY, GLint textureID = 0) :
		x(float(coords.x())),
		y(float(coords.y())),
		z(float(coords.z())),
		r(float(col.redF())),
		g(float(col.greenF())),
		b(float(col.blueF())),
		texi(textureX),
		texj(textureY),
		texID(textureID)
	{
	}

	float x, y, z;
	float r, g, b;
	float texi, texj;
	float texID;
};