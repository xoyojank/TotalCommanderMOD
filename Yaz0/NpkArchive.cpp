//------------------------------------------------------------------------------
//  NpkArchive.cpp
//  (C) 2010 xoyojank
//------------------------------------------------------------------------------
#include "NpkArchive.h"

//------------------------------------------------------------------------------
NpkArchive::NpkArchive(const char* arcName)
: curIndex(0)
, archiveName(arcName)
{
	this->hArcFile = CreateFile(arcName, GENERIC_READ, FILE_SHARE_READ,
		0, OPEN_EXISTING, 0, NULL);
	assert(INVALID_HANDLE_VALUE != this->hArcFile);
	// get file time
	FILETIME time;
	BOOL result = GetFileTime(this->hArcFile, &time, &time, &time);
	assert(TRUE == result);
	FileTimeToDosDateTime(&time, ((WORD*)&this->archiveTime) + 1, (WORD*)&this->archiveTime);

	// reader header
	NpkHeader npkHeader;
	this->ReadData(&npkHeader, sizeof(npkHeader));
	assert(IdHeader == npkHeader.fourcc);
	this->dataOffset = npkHeader.dataOffset;

	// load file informations
	this->Load();
}

//------------------------------------------------------------------------------
NpkArchive::~NpkArchive()
{
	this->files.clear();
	this->curIndex = 0;
	CloseHandle(this->hArcFile);
	this->dataOffset = 0;
}

//------------------------------------------------------------------------------
void
NpkArchive::Load()
{
	DataBlock dataBlock;
	while (SetFilePointer(this->hArcFile, 0, NULL, FILE_CURRENT) < int(this->dataOffset))
	{
		this->ReadData(&dataBlock, sizeof(dataBlock));
		assert(TRUE == result);
		switch(dataBlock.fourcc)
		{
		case IdDir:
			this->BeginDir();
			break;
		case IdFile:
			this->LoadFile();
			break;
		case IdEod:
			this->EndDir();
			break;
		default:
			assert(false);
			SetFilePointer(this->hArcFile, dataBlock.length, NULL, FILE_CURRENT);
			break;
		}
	}
}

//------------------------------------------------------------------------------
void
NpkArchive::BeginDir()
{
	this->loadingDir += this->ReadString();
	this->loadingDir += "\\";
}

//------------------------------------------------------------------------------
void
NpkArchive::LoadFile()
{
	File file;
	this->ReadData(&file.offset, sizeof(file.offset));
	this->ReadData(&file.size, sizeof(file.size));
	file.name = this->ReadString();
	file.path = this->loadingDir + file.name;

	this->files.push_back(file);
}

//------------------------------------------------------------------------------
void
NpkArchive::EndDir()
{
	assert(!this->loadingDir.empty());
	string temp = this->loadingDir;
	char* str = (char*)temp.c_str();
	str[temp.length() - 1] = 0;
	char* lastSlash = strrchr(str, '\\');
	if (NULL == lastSlash)
	{
		this->loadingDir.clear();
	}
	else
	{
		lastSlash[1] = 0;
		this->loadingDir = str;
	}
}

//------------------------------------------------------------------------------
string
NpkArchive::ReadString()
{
	static vector<char> buffer;
	ushort length;
	this->ReadData(&length, sizeof(length));
	buffer.resize(length + 1);
	this->ReadData(&buffer.front(), length);
	buffer.back() = 0;

	return &buffer.front();
}

//------------------------------------------------------------------------------
void
NpkArchive::ReadData(void* ptr, uint size)
{
	DWORD bytesRead;
	BOOL result = ReadFile(this->hArcFile, ptr, size, &bytesRead, NULL);
	assert(TRUE == result);
	assert(bytesRead == size);
}

//------------------------------------------------------------------------------
bool
NpkArchive::ExtractFile(const char* destPath)
{
	// create file
	HANDLE hFile = CreateFile(destPath, GENERIC_WRITE, FILE_SHARE_READ, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;
	// write data
	static vector<byte> buffer;
	const File& file = this->files[this->curIndex];
	buffer.resize(file.size);
	SetFilePointer(this->hArcFile, this->dataOffset + file.offset + sizeof(DataBlock),
		NULL, FILE_BEGIN);
	this->ReadData(&buffer.front(), buffer.size());
	DWORD bytesWrite;
	BOOL result = WriteFile(hFile, &buffer.front(), buffer.size(), &bytesWrite, NULL);
	assert(TRUE == result);
	assert(bytesWrite == buffer.size());
	// set file time
	FILETIME time;
	result = GetFileTime(this->hArcFile, &time, &time, &time);
	assert(TRUE == result);
	result = SetFileTime(hFile, &time, &time, &time);
	assert(TRUE == result);
	// close file
	CloseHandle(hFile);

	return true;
}
