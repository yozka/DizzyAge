///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FileZip.h
// This handles zipped files, from .zip archives (READONLY)
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __F9FILEZIP_H__
#define __F9FILEZIP_H__

#include "F9File.h"
#include "F9FileDisk.h"
#include "..\\Libs\\zlib\\zlib.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////////////////////////
#define ZIP_SMARTOPEN					// if preprocessing name using IFile in Open - allow to use IFile in cFileZip not opened with IFile ?

#define ZIP_INFLATE_BUFFER_SIZE 		(1024*512)	// 512k
#define ZIP_CENTRAL_END_BUFFER_SIZE		0xffffU

#define ZIP_FILE_HEADER_SIGN 			0x02014b50
#define ZIP_LOCAL_FILE_HEADER_SIGN		0x04034b50
#define ZIP_CENTRAL_END_SIGN			0x06054b50

//#define ZIP_FILE_MAX_POOL				5
#define	ZIP_DESCRIPTOR_SIZE				12			// file descriptor block size

#define	ZIP_CM_STORED					0x00		// stored
#define	ZIP_CM_SHRUNK					0x01		// shrunk
#define	ZIP_CM_REDUCED1					0x02		// reduced with factor 1
#define	ZIP_CM_REDUCED2					0x03		// reduced with factor 2
#define	ZIP_CM_REDUCED3					0x04		// reduced with factor 3
#define	ZIP_CM_REDUCED4					0x05		// reduced with factor 4
#define	ZIP_CM_IMPLODED					0x06		// imploded
#define	ZIP_CM_TOKENIZED				0x07		// tokenized
#define	ZIP_CM_DEFLATED					0x08		// deflated
#define	ZIP_CM_RESERVED					0x09		// reserved for enhanced deflating
#define	ZIP_CM_PKLITE					0x0A		// pkware date compression library imp.

#define ZLIB_INIT		 				0			// begin
#define ZLIB_END		  				1			// end

///////////////////////////////////////////////////////////////////////////////////////////////////
// Structs
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4103)
#pragma pack(2)								// force 2 byte aligning for following structures

struct zipLocalFileHeader
{
	dword		sign;				// Local File Header Signature
	word		ver;				// Version needed to extract
	word		gflag;				// General purpose bit flag
	word		compression;		// Compression method
	word		time;				// Last mod file time (MS-DOS)
	word		date;				// Last mod file date (MS-DOS)
	dword		crc;				// CRC-32
	dword		sizecomp;			// Compressed size
	dword		sizeuncomp;			// Uncompressed size
	word		sizename;			// Filename length
	word		sizextra;			// Extra field length
	//char*		name;				// Filename chars (variable size)
	//char*		xtra;				// Extra field (variable size)
};

struct zipCentralEnd
{
	dword		sign;				// End of central dir signature
	word		disk;				// Number of this disk
	word		cdisk;				// Number of disk with start central dir
	word		entriesdisk;		// Total number of entries in central dir on this disk
	word		entries;			// Total number entries in central dir
	dword		size;				// Size of the central directory
	dword		offset;				// Offset of start of central directory with respect to the starting disk number
	word		sizecomment;		// Zipfile comment length
	//char*		comment;			// Zipfile comment
		
	zipCentralEnd():
	sign		(0),
	disk		(0),
	cdisk		(0),
	entriesdisk	(0),
	entries		(0),
	size		(0),
	offset		(0),
	sizecomment (0) {}
};

struct zipFileHeader
{
	dword	sign;					// central file header signature   4 bytes  (0x02014b50)

	word	vermade;				// version made by                 2 bytes
	word	ver;					// version needed to extract       2 bytes
	word	gflag;					// general purpose bit flag        2 bytes
	word	compression;			// compression method              2 bytes
	word	timemod;				// last mod file time              2 bytes
	word	datemod;				// last mod file date              2 bytes

	dword 	crc;					// crc-32                          4 bytes
	dword	sizecomp;				// compressed size                 4 bytes
	dword	sizeuncomp;				// uncompressed size               4 bytes
	
	word	sizename;				// filename length                 2 bytes
	word	sizextra;				// extra field length              2 bytes
	word	sizecomment;					// file comment length             2 bytes
	word	diskstart;				// disk number start               2 bytes
	word	attribint;				// internal file attributes        2 bytes
	
	dword	attribext;				// external file attributes        4 bytes
	dword	offset;					// relative offset of local header 4 bytes

	//	filename (variable size)
	//	extra field (variable size)
	//	file comment (variable size)
};

#pragma pack()										

///////////////////////////////////////////////////////////////////////////////////////////////////
// f9FileZip class
///////////////////////////////////////////////////////////////////////////////////////////////////
class f9FileZip : public f9File
{

public:
				f9FileZip	();
virtual			~f9FileZip	();

virtual	int		Open		( char* name, int mode );
virtual	int		Close		();
virtual	int64	Read		( void* data, int64 size );
virtual	int		Seek		( int64 offset, int origin = F9_SEEK_SET );
virtual	int64	Tell		();
virtual	int64 	Size		();
	
protected:
		BOOL	InitZlib	(int mode);
		void	Reset		();

protected:
		char*				m_arcname;				// just a pointer to zip archive name
		int 				m_offset; 				// offset of file in zip archive (-1 if not initialized); set by archive

		f9FileDisk	 		m_arcfile;				// archive disk file
		zipLocalFileHeader	m_localheader;			// local header info
		z_stream			m_zips;					// zip stream
	
		byte				m_zipBuffer[ZIP_INFLATE_BUFFER_SIZE];

friend	class f9ArchiveZip;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
LOCAL FILE HEADER
-----------------

OFFSET LABEL       TYP  VALUE        DESCRIPTION
------ ----------- ---- ----------- ----------------------------------
00     ZIPLOCSIG   HEX  04034B50    ;Local File Header Signature
04     ZIPVER      DW   0000        ;Version needed to extract
06     ZIPGENFLG   DW   0000        ;General purpose bit flag
08     ZIPMTHD     DW   0000        ;Compression method
0A     ZIPTIME     DW   0000        ;Last mod file time (MS-DOS)
0C     ZIPDATE     DW   0000        ;Last mod file date (MS-DOS)
0E     ZIPCRC      HEX  00000000    ;CRC-32
12     ZIPSIZE     HEX  00000000    ;Compressed size
16     ZIPUNCMP    HEX  00000000    ;Uncompressed size
1A     ZIPFNLN     DW   0000        ;Filename length
1C     ZIPXTRALN   DW   0000        ;Extra field length
1E     ZIPNAME     DS   ZIPFNLN     ;filename
--     ZIPXTRA     DS   ZIPXTRALN   ;extra field

CENTRAL DIRECTORY STRUCTURE
---------------------------

OFFSET LABEL       TYP  VALUE        DESCRIPTION
------ ----------- ---- ----------- ----------------------------------
00     ZIPCENSIG   HEX  02014B50    ;Central file header signature
04     ZIPCVER     DB   00          ;Version made by
05     ZIPCOS      DB   00          ;Host operating system
06     ZIPCVXT     DB   00          ;Version needed to extract
07     ZIPCEXOS    DB   00          ;O/S of version needed for extraction
08     ZIPCFLG     DW   0000        ;General purpose bit flag
0A     ZIPCMTHD    DW   0000        ;Compression method
0C     ZIPCTIM     DW   0000        ;Last mod file time (MS-DOS)
0E     ZIPCDAT     DW   0000        ;Last mod file date (MS-DOS)
10     ZIPCCRC     HEX  00000000    ;CRC-32
14     ZIPCSIZ     HEX  00000000    ;Compressed size
18     ZIPCUNC     HEX  00000000    ;Uncompressed size
1C     ZIPCFNL     DW   0000        ;Filename length
1E     ZIPCXTL     DW   0000        ;Extra field length
20     ZIPCCML     DW   0000        ;File comment length
22     ZIPDSK      DW   0000        ;Disk number start
24     ZIPINT      DW   0000        ;Internal file attributes

       LABEL       BIT        DESCRIPTION
       ----------- --------- -----------------------------------------
       ZIPINT         0       if = 1, file is apparently an ASCII or
                              text file
                      0       if = 0, file apparently contains binary
                              data

                     1-7      unused in version 1.0.

26     ZIPEXT      HEX  00000000    ;External file attributes, host
                                    ;system dependent
2A     ZIPOFST     HEX  00000000    ;Relative offset of local header
                                    ;from the start of the first disk
                                    ;on which this file appears
2E     ZIPCFN      DS   ZIPCFNL     ;Filename or path - should not
                                    ;contain a drive or device letter,
                                    ;or a leading slash. All slashes
                                    ;should be forward slashes '/'
--     ZIPCXTR     DS   ZIPCXTL     ;extra field
--     ZIPCOM      DS   ZIPCCML     ;file comment


END OF CENTRAL DIR STRUCTURE
----------------------------

OFFSET LABEL       TYP  VALUE        DESCRIPTION
------ ----------- ---- ----------- ----------------------------------
00     ZIPESIG     HEX  06064B50    ;End of central dir signature
04     ZIPEDSK     DW   0000        ;Number of this disk
06     ZIPECEN     DW   0000        ;Number of disk with start central dir
08     ZIPENUM     DW   0000        ;Total number of entries in central dir
                                    ;on this disk
0A     ZIPECENN    DW   0000        ;total number entries in central dir
0C     ZIPECSZ     HEX  00000000    ;Size of the central directory
10     ZIPEOFST    HEX  00000000    ;Offset of start of central directory
                                    ;with respect to the starting disk
                                    ;number
14     ZIPECOML    DW   0000        ;zipfile comment length
16     ZIPECOM     DS   ZIPECOML    ;zipfile comment
*/

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
