///////////////////////////////////////////////////////////////////////////////////////////////////
// F9FileZip.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "F9FileZip.h"
#include "F9FileDisk.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
f9FileZip::f9FileZip()
{
	guard(f9FileZip::f9FileZip);
	m_type			= F9_FILE_ZIP;
	m_arcname		= NULL;
	m_offset		= -1;
	memset( &m_zips, 0, sizeof(z_stream) );
	unguard();
}

f9FileZip::~f9FileZip()
{
	guard(f9FileZip::~f9FileZip);
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// OPEN( .zip, mode )
// before call, you must set:
// m_arcname = name of the zip archive
// m_offset	= offset of the filelocalheader in the zip archive
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL f9FileZip::Open( char* name, int mode )
{
	guard(f9FileZip::Open);
	if(IsOpen()) Close();
	if(name==NULL) return F9_FAIL;
	if(!F9_ISREADONLY(mode)) return F9_FAIL; // readonly

	// archive should set those
	if( !m_arcname || m_offset==-1 ) return F9_FAIL;

	m_mode = mode;

	// arc disk file
	if(m_arcfile.Open( m_arcname, m_mode )!=F9_OK) return F9_FAIL;
	
	// local header
	if( m_arcfile.Seek( m_offset, F9_SEEK_SET )!=F9_OK ||
		(m_arcfile.Read( &m_localheader, sizeof(m_localheader) ) != sizeof(m_localheader) ) ||
		 m_localheader.sign != ZIP_LOCAL_FILE_HEADER_SIGN	)
	{
		m_arcfile.Close();
		return F9_FAIL;
	}
	
	m_arcfile.Seek(m_localheader.sizename, F9_SEEK_CUR); // skip name
	m_arcfile.Seek(m_localheader.sizextra, F9_SEEK_CUR); // skip extra field

	// compression
	switch( m_localheader.compression )
	{
	    case ZIP_CM_DEFLATED:	break;
		case ZIP_CM_STORED:		break;
	    case ZIP_CM_SHRUNK:
	    case ZIP_CM_REDUCED1:
	    case ZIP_CM_REDUCED2:
	    case ZIP_CM_REDUCED3:
	    case ZIP_CM_REDUCED4:
	    case ZIP_CM_IMPLODED:
	    case ZIP_CM_TOKENIZED:
		case ZIP_CM_RESERVED:
		case ZIP_CM_PKLITE:
	    default: // unsupported compress method
		{
			m_arcfile.Close();
			return F9_FAIL;
		}
	}

	// zlib
	if( !InitZlib(ZLIB_INIT) ) 
	{
		m_arcfile.Close();
		return F9_FAIL;
	}

	// position and size
	m_pos	= 0;
	m_size	= m_localheader.sizeuncomp;
	
	m_open = TRUE;
	return F9_OK;
	unguard();
}

int f9FileZip::Close()
{
	guard(f9FileZip::Close);
	if(!IsOpen()) return F9_FAIL;
	InitZlib(ZLIB_END);
	m_arcfile.Close();

	m_arcname	= NULL;
	m_offset	= -1;

	m_open = FALSE;
	return F9_OK;
	unguard();
}

int f9FileZip::Seek( int64 offset, int origin )
{
	guard(f9FileZip::Seek);
	if(!IsOpen()) return F9_FAIL;

	int64 off = offset;
	if( origin==F9_SEEK_SET ) off = offset - m_pos;
	if( origin==F9_SEEK_END ) off = m_size - offset - m_pos;
	if( off==0 ) return F9_OK;
		
	if( m_localheader.compression == ZIP_CM_STORED )
	{
		int64 curpos = m_arcfile.Tell();
		if( m_arcfile.Seek(off, F9_SEEK_CUR)!=F9_OK ) return F9_FAIL;
		m_pos += (m_arcfile.Tell() - curpos);
		return F9_OK;
	}
	
	// move forward
	if(off>=0)
	{
		char c;
		for(int64 i = 0; i < off; i++) Read(&c, 1);
		return F9_OK;
	}
	else 
	// rewind and seek
	{
		int64 oldpos = m_pos;
		Reset();	
		return Seek( oldpos + off, F9_SEEK_SET );
	}
	return F9_OK;
	unguard();
}

int64 f9FileZip::Tell()
{
	guard(f9FileZip::Tell);
	return m_pos;
	unguard();
}

int64 f9FileZip::Size()
{
	guard(f9FileZip::Size);
	return m_size;
	unguard();
}

int64 f9FileZip::Read( void* data, int64 size )
{
	guard(f9FileZip::Read);
	if(!IsOpen() || data==NULL) return 0;

	BOOL  error			= FALSE;
	int64 read_before	= 0;	// ?
	int64 bytes_to_read = size;	// bytes to be readed
	
	byte* src = m_zipBuffer;	// source buffer
	byte* dst = (byte*)data;	// destination buffer
	
	if( m_localheader.compression == ZIP_CM_STORED )
	{
		int64 ret = m_arcfile.Read(data, size);
		m_pos += ret;
		return ret;
	}

	m_zips.next_out		= dst;			
	m_zips.avail_out	= (uInt)bytes_to_read;
    read_before 		= m_zips.total_out;

	while( !error && ((m_zips.total_out - read_before) < bytes_to_read) )
	{	
		if( m_zips.avail_in <= 0 ) 
		{
			// input buffer is empty - refill
			int64 nbytes ;
			nbytes = m_arcfile.Read( src, (ZIP_INFLATE_BUFFER_SIZE < m_localheader.sizecomp) ? ZIP_INFLATE_BUFFER_SIZE : m_localheader.sizecomp );
			if(nbytes <= 0) error = TRUE;

			m_zips.avail_in	= (uInt)nbytes;
			m_zips.next_in		= src;					
		}

		// uncompress 
		int64 ret  = inflate(&m_zips, Z_SYNC_FLUSH);

		switch(ret)
		{
			case Z_OK:			//Uncompress OK
				break;
			case Z_STREAM_END:	//Uncompress Z_STREAM_END
				error = TRUE;				
				break;
			default:			// Uncomprase FAIL (m_zips.msg)
				error = TRUE;
				break;
		}
	}	

	int64 total_read = (m_zips.total_out > read_before)?(m_zips.total_out - read_before):0;
	m_pos += total_read;
	return total_read;
	unguard();
}

BOOL f9FileZip::InitZlib( int mode )
{
	guard(f9FileZip::InitZlib);
	if(mode==ZLIB_INIT)
	{
		memset( &m_zips, 0, sizeof(z_stream) );
		if( inflateInit2(&m_zips, -MAX_WBITS) != Z_OK ) return FALSE;
		return TRUE;
	}
	else
	if(mode==ZLIB_END)
	{
		if( inflateEnd(&m_zips) != Z_OK ) return FALSE;
		return TRUE;
	}
	return FALSE;
	unguard();
}  	

void f9FileZip::Reset()
{
	guard(f9FileZip::Reset);
	if(!IsOpen()) return;

	InitZlib(ZLIB_END);
	InitZlib(ZLIB_INIT);

	// seeking to the data in local header
	m_arcfile.Seek(	m_offset + 
					sizeof(m_localheader) + 
					m_localheader.sizename + 
					m_localheader.sizextra, F9_SEEK_SET);
	m_pos = 0;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
