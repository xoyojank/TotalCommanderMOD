#pragma once
/*
	Yaz0 is a run length encoding (RLE compression) method.

	Data structure
	Header
	The header of a Yaz0 file is always 16 bytes long. All numeric values stored as big endian.

	Offset	Type	Description
	0x00	Char[4]	magic. Always Yaz0 in ASCII.
	0x04	UInt32	Size in bytes of the uncompressed data.
	0x08	Byte[8]	Reserved for special use. Always 0 in Mario Kart Wii.
	GNU C example
	typedef struct yaz0_header_t
	{
		char	magic[4];		// always "Yaz0"
		be32_t	uncompressed_size;	// total size of uncompressed data
		char	padding[8];		// always 0?
	}
	__attribute__ ((packed)) yaz0_header_t;

	Data Groups
	The complete compressed data is organized in data groups. Each data group consists of 1 group header byte and 8 chunks.

	N	Size	Description
	1	1 byte	the group header byte
	8	1-3 bytes	8 chunks
	Each bit of the group header corespondents to one chunk:

	The MSB (most significant bit, 0x80) corespondents to chunk 1
	The LSB (lowest significant bit, 0x01) corespondents to chunk 8
	A set bit (=1) in the group header means, that the chunk is exact 1 byte long. This byte must be copied to the output stream 1:1. A cleared bit (=0) defines, that the chunk is 2 or 3 bytes long interpreted as a back reference to already decompressed data that must be copied.

	Size	Data Bytes	Size Calculation
	2 bytes	NR RR	N = 1..f	SIZE = N+2 (=3..0x11)
	3 bytes	0R RR NN	N = 00..ff	SIZE = N+0x12 (=0x12..0x111)
	RRR is a value between 0x000 and 0xfff. Go back RRR+1 bytes in the output stream to find the start of the data to be copied.
	SIZE is calculated from N (see above) and declares the number of bytes to be copied.
	It is important to know, that a chunk may reference itself. For example if RRR=1 (go back 1+1=2) and SIZE=10 the previous 2 bytes are copied 10/2=5 times.
	Decoding data groups and chunks is done until the end of the destination data is reached.
*/
typedef unsigned char u8;
typedef unsigned long u32;

u32 yaz0_decode(u8 *src, u8 *dst, int& fileSizeCompressed);
int yaz0_get_size(u8 * src);
void yaz0_encode(u8 * src, int src_size, u8 *dst, int *dst_size);