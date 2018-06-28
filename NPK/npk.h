#pragma once
//------------------------------------------------------------------------------
/**
    @file	npk.h 
  
   data block definition of npk files. 
    
    (C) 2010 xoyojank
*/
//------------------------------------------------------------------------------
typedef unsigned short	ushort;
typedef unsigned char   byte;
typedef unsigned int    uint;

//------------------------------------------------------------------------------
#define IdHeader	'NPK0'
#define IdDir		'DIR_'
#define IdFile		'FILE'
#define IdEod		'DEND'
#define IdData		'DATA'

//------------------------------------------------------------------------------
struct DataBlock
{
	uint fourcc;			///< four character code
	uint length;			///< number of following bytes in block
};

//------------------------------------------------------------------------------
struct NpkHeader : DataBlock
{
	uint dataOffset;		///< byte offset of data block from beginning of file
};

//------------------------------------------------------------------------------
struct NpkDir: DataBlock
{
	ushort dirNameLength;	///< length of the following name   
	char dirName[0];		///< name of directory   
};

//------------------------------------------------------------------------------
struct NpkFile : DataBlock
{
	uint fileOffset;		///< start of file data inside data block   
	uint fileLength;		///< length of file data in bytes   
	ushort fileNameLength;	///< length of the following name   
	char fileName[0];		///< name of file   
};

//------------------------------------------------------------------------------
struct NpkEndOfDir : DataBlock
{
	// empty
};

//------------------------------------------------------------------------------
struct NpkData : DataBlock
{
	byte data[0];			///< the data   
};