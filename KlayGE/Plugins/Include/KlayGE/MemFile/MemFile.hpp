// MemFile.hpp
// KlayGE 内存文件类 头文件
// Ver 2.0.0
// 版权所有(C) 龚敏敏, 2003
// Homepage: http://www.enginedev.com
//
// 2.0.0
// 初次建立 (2003.8.10)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#ifndef _MEMFILE_HPP
#define _MEMFILE_HPP

#include <vector>
#include <KlayGE/VFile.hpp>

#pragma comment(lib, "KlayGE_FileSystem_MemFile.lib")

namespace KlayGE
{
	class MemFile : public VFile
	{
	public:
		MemFile();
		MemFile(const void* pData, size_t length);
		MemFile(const MemFile& File);
		~MemFile();

		MemFile& operator=(const MemFile& rhs);

		void Open(const void* pData, size_t length);
		void Close();

		VFilePtr Clone() const;

		size_t Length();
		void Length(size_t NewLen);

		size_t Write(const void* data, size_t count);
		size_t Read(void* data, size_t count);
		size_t CopyFrom(VFile& src, size_t size);

		size_t Seek(size_t offset, SeekMode from);
		size_t Tell();

		void Swap(MemFile& rhs);

	private:
		std::vector<U8>		chunkData_;
		size_t				curPos_;
	};
}

#endif			// _MEMFILE_HPP