#pragma once
/*
SARC Files are archive files used in Wii U and 3DS, including Mario Kart 8.
They are similar to the U8 files found in GameCube and Wii games.
Common extensions are .arc and .sarc (general), .pack (when the archive
contains mostly other archive files), .bars (when it contains audio data),
and .bgenv (when it contains shader files). SARC files are sometimes Yaz0
compressed into .szs files.
*/

#include <stdint.h>

// Global SARC header
struct SARC_HEADER
{
	uint32_t magic;
	uint16_t headerLength;
	uint16_t byteOrder;
	uint32_t fileSize;
	uint32_t dataOffset;
	uint32_t unk0;
};

// Header for each node in the archive
struct SFAT_HEADER
{
	uint32_t magic;
	uint16_t headerLength;
	uint16_t nodeCount;
	uint32_t hashMultiplier;
};

// Nodes are basically entities in the archive
struct SFAT_NODE
{
	uint32_t fileNameHash;
	uint8_t  nodeType : 8;
	uint32_t fileNameTableOffset : 24;  // 3-byte offset into string table below
	uint32_t nodeDataBeginOffset;
	uint32_t nodeDataEndOffset;
};

struct SFNT_HEADER
{
	uint32_t magic;
	uint16_t headerLength;
	uint16_t unk0;
};