#pragma once

#include <qopenglfunctions.h>
#include <qcolor.h>
#include <qmatrix4x4.h>
#include <vector>

#include "Transform3D.h"
#include "Vertex.h"

/*
长方体网格（四边形）。方框是通过x、y和z（宽度、高度、长度/深度）的尺寸定义的，并且以原点为中心构建。
您可以通过调用transform（）来变换长方体，然后它将更新长方体的所有顶点。
可以通过调用setFaceColors（）来调整面部颜色。
常数VertexCount和IndexCount返回存储三角形来绘制长方体。
然后，您可以调用copy2buffer（）来用数据填充所提供的内存块。

*/

class BoxMesh  
{
public:
	BoxMesh(float width = -1,float height = -1,float depth = 1,QColor boxColor = Qt::blue);
	void setColor(QColor c) { m_colors = std::vector<QColor>(1, c); }
	// 给6个不同的面设置不同的颜色，font right back left top bottom
	void setFaceColors(const std::vector<QColor>& c) { assert(c.size() == 6); m_colors = c; }

	// 盒子的变换
	void transform(const QMatrix4x4& transform);

	/*
	在由调用方提供的缓冲区中填充顶点数据。顶点数据是交错存储的，“坐标（vec3）-颜色（vec3)。
	param vertex缓冲区指向要写入的顶点内存数组的指针。将向前移动以指向下一个插入顶点之后的位置。
	param elementBuffer指向要写入的元素内存数组的指针。将向前移动以指向下一个插入顶点之后的索引位置。
	elementStartIndex是开始索引，我们应该用它开始索引新添加的顶点。
	*/
	void copy2Buffer(Vertex* &vertexBuffer,GLuint* & elementBuffer, unsigned int& elementStartIndex) const;
	
	static const unsigned int VertexCount = 6 * 4;		// 6个面，每个4个顶点
	static const unsigned int IndexCount = 6 * 2 * 3;	// 6个面，每个面2个三角形，每个三角形3个索引

private:
	std::vector<QVector3D> m_vertices;	
	std::vector<QColor> m_colors;	// //size 1=统一颜色，size 6=面部颜色
};
