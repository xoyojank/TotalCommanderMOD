#pragma once
//------------------------------------------------------------------------------
/**
    @class NpkArchive
  
    NPK archive file access.
    
    (C) 2010 xoyojank
*/
#include "npk.h"
#include <vector>
#include <string>
using namespace std;
#include <Windows.h>
#include <assert.h>

class NpkArchive
{
public:
	NpkArchive(const char* arcName);
	~NpkArchive();

	/// 取得文件包名
	const char* GetArchiveName() const;
	/// 取得文件包时间
	int GetArchiveTime() const;
	/// 取得当前文件名
	const char* GetCurFileName() const;
	/// 取得当前文件大小
	uint GetCurFileSize() const;
	/// 指向下一个文件
	void NextFile();
	/// 是否存在下一个文件
	bool HasNextFile() const;
	/// 解压当前文件
	bool ExtractFile(const char* destPath);
private:
	struct File
	{
		string path;
		string name;
		uint offset;
		uint size;
	};
private:
	void Load();
	void BeginDir();
	void LoadFile();
	void EndDir();
	string ReadString();
	void ReadData(void* ptr, uint size);
private:
	string archiveName;
	int archiveTime;
	vector<File> files;
	uint curIndex;
	HANDLE hArcFile;
	uint dataOffset;

	string loadingDir;
};

//------------------------------------------------------------------------------
inline const char*
NpkArchive::GetArchiveName() const
{
	assert(this->curIndex < this->files.size());
	return this->archiveName.c_str();
}

//------------------------------------------------------------------------------
inline int
NpkArchive::GetArchiveTime() const
{
	return this->archiveTime;
}

//------------------------------------------------------------------------------
inline const char*
NpkArchive::GetCurFileName() const
{
	assert(this->curIndex < this->files.size());
	return this->files[this->curIndex].path.c_str();
}

//------------------------------------------------------------------------------
inline uint
NpkArchive::GetCurFileSize() const
{
	assert(this->curIndex < this->files.size());
	return this->files[this->curIndex].size;
}

//------------------------------------------------------------------------------
inline void
NpkArchive::NextFile()
{
	++this->curIndex;
}

//------------------------------------------------------------------------------
inline bool
NpkArchive::HasNextFile() const
{
	return (this->curIndex < this->files.size());
}
