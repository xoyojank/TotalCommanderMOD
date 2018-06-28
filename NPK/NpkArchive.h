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

	/// ȡ���ļ�����
	const char* GetArchiveName() const;
	/// ȡ���ļ���ʱ��
	int GetArchiveTime() const;
	/// ȡ�õ�ǰ�ļ���
	const char* GetCurFileName() const;
	/// ȡ�õ�ǰ�ļ���С
	uint GetCurFileSize() const;
	/// ָ����һ���ļ�
	void NextFile();
	/// �Ƿ������һ���ļ�
	bool HasNextFile() const;
	/// ��ѹ��ǰ�ļ�
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
