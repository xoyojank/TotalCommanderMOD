#include "SarcArchive.h"
#include "sarc.h"

//------------------------------------------------------------------------------
SarcArchive::SarcArchive(const char* arcName)
	: curIndex(0)
	, archiveName(arcName)
{
	this->hArcFile = CreateFileA(arcName, GENERIC_READ, FILE_SHARE_READ,
	                             0, OPEN_EXISTING, 0, NULL);
	assert(INVALID_HANDLE_VALUE != this->hArcFile);
	ReadFileTime();
}

//------------------------------------------------------------------------------
SarcArchive::SarcArchive(const wchar_t* arcName)
	: curIndex(0)
	, archiveNameW(arcName)
{
	this->hArcFile = CreateFileW(arcName, GENERIC_READ, FILE_SHARE_READ,
	                             0, OPEN_EXISTING, 0, NULL);
	assert(INVALID_HANDLE_VALUE != this->hArcFile);
	ReadFileTime();
	ReadFileHeader();
}

//------------------------------------------------------------------------------
void
SarcArchive::ReadFileTime()
{
	// get file time
	FILETIME time;
	BOOL result = GetFileTime(this->hArcFile, &time, &time, &time);
	assert(TRUE == result);
	FileTimeToLocalFileTime(&time, &time);
	FileTimeToDosDateTime(&time, ((WORD*)&this->archiveTime) + 1, (WORD*)&this->archiveTime);
	ReadFileHeader();
}

//------------------------------------------------------------------------------
wstring
StringToWstring(const string str)
{
	size_t len = str.size() * 2;
	setlocale(LC_CTYPE, "");
	wchar_t *p = new wchar_t[len];
	size_t converted = 0;
	mbstowcs_s(&converted, p, len, str.c_str(), _TRUNCATE);
	std::wstring str1(p);
	delete[] p;
	return str1;
}

//------------------------------------------------------------------------------
bool
SarcArchive::ReadFileHeader()
{
	// reader header
	SARC_HEADER sarcHeader;
	this->ReadData(&sarcHeader, sizeof(sarcHeader));
	if (sarcHeader.magic != 'CRAS')
		return false;
	if (sarcHeader.headerLength != 0x14)
		return false;
	if (sarcHeader.byteOrder != 0xFEFF)
		return false;

	// read file table
	SFAT_HEADER sfatHeader;
	this->ReadData(&sfatHeader, sizeof(sfatHeader));
	if (sfatHeader.magic != 'TAFS')
		return false;
	if (sfatHeader.headerLength != 0xC)
		return false;
	if (sfatHeader.hashMultiplier != 0x65)
		return false;
	this->files.resize(sfatHeader.nodeCount);
	for (int i = 0; i < this->files.size(); ++i)
	{
		SFAT_NODE sfatNode;
		this->ReadData(&sfatNode, sizeof(sfatNode));
		this->files[i].offset = sarcHeader.dataOffset + sfatNode.nodeDataBeginOffset;
		this->files[i].size = sfatNode.nodeDataEndOffset - sfatNode.nodeDataBeginOffset;
	}

	// read filename table
	SFNT_HEADER sfntHeader;
	this->ReadData(&sfntHeader, sizeof(sfntHeader));
	if (sfntHeader.magic != 'TNFS')
		return false;
	if (sfntHeader.headerLength != 0x8)
		return false;
	vector<char> buffer(sarcHeader.dataOffset - this->Tell());
	this->ReadData(buffer.data(), buffer.size());
	size_t pos = 0;
	for (uint32_t i = 0; i < this->files.size(); ++i)
	{
		this->files[i].path = buffer.data()[pos];
		this->files[i].pathW = StringToWstring(this->files[i].path);
		pos += this->files[i].path.size();
		if (i + 1 < this->files.size())
		{
			while (buffer[pos] == 0) //ignore every 0 byte, because why bother calculating the SFNT header offset anyway?
				++pos;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
SarcArchive::~SarcArchive()
{
	this->files.clear();
	this->curIndex = 0;
	CloseHandle(this->hArcFile);
}

//------------------------------------------------------------------------------
void
SarcArchive::ReadData(void* ptr, size_t size)
{
	DWORD bytesRead;
	BOOL result = ReadFile(this->hArcFile, ptr, size, &bytesRead, NULL);
	assert(TRUE == result);
	assert(bytesRead == size);
}

//------------------------------------------------------------------------------
DWORD
SarcArchive::Tell() const
{
	return SetFilePointer(this->hArcFile, 0, NULL, FILE_CURRENT);
}

//------------------------------------------------------------------------------
bool
SarcArchive::ExtractFile(const char* destPath)
{
	// create file
	HANDLE hFile = CreateFileA(destPath, GENERIC_WRITE, FILE_SHARE_READ, 0,
	                           CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;
	WriteFileData(hFile);

	return true;
}

//------------------------------------------------------------------------------
bool
SarcArchive::ExtractFileW(const wchar_t* destPath)
{
	// create file
	HANDLE hFile = CreateFileW(destPath, GENERIC_WRITE, FILE_SHARE_READ, 0,
	                           CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return false;
	WriteFileData(hFile);

	return true;
}

//------------------------------------------------------------------------------
void
SarcArchive::WriteFileData(HANDLE hFile)
{
	// write data
	static vector<byte> buffer;
	const File& file = this->files[this->curIndex];
	buffer.resize(file.size);
	SetFilePointer(this->hArcFile, file.offset, NULL, FILE_BEGIN);
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
}
