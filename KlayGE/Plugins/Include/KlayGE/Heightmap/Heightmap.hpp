// Heightmap.hpp
// KlayGE Heightmap地形生成类 头文件
// Ver 2.0.0
// 版权所有(C) 龚敏敏, 2003
// Homepage: http://www.enginedev.com
//
// 2.0.0
// 初次建立 (2003.10.5)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#ifndef _HEIGHTMAP_HPP
#define _HEIGHTMAP_HPP

#include <vector>
#include <KlayGE/alloc.hpp>
#include <KlayGE/MathTypes.hpp>

namespace KlayGE
{
	// 高度图地形生成
	/////////////////////////////////////////////////////////////////////////////////
	class Heightmap
	{
	public:
		template <typename HeightFunc>
		void Terrain(float startX, float startY, float width, float height, float spanX, float spanY,
			std::vector<Vector3, alloc<Vector3> >& vertices, std::vector<U16, alloc<U16> >& indices,
			HeightFunc& Height)
		{
			vertices.clear();
			indices.clear();

			for (float y = startY; y < startY + height; y += spanY)
			{
				for (float x = startX; x < startX + width; x += spanX)
				{
					Vector3 vec(MakeVector(x, y, Height(x, y)));

					std::vector<Vector3, alloc<Vector3> >::iterator iter(std::find(vertices.begin(), vertices.end(), vec));
					if (iter == vertices.end())
					{
						indices.push_back(vertices.size());
						vertices.push_back(vec);
					}
					else
					{
						indices.push_back(std::distance(iter, vertices.begin()));
					}
				}
			}
		}
	};
}

#endif		// _HEIGHTMAP_HPP

