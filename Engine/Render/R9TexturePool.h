///////////////////////////////////////////////////////////////////////////////////////////////////
// R9TexturePool.h
// Texture pool manager
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __R9TEXTUREPOOL_H__
#define __R9TEXTUREPOOL_H__

#include "R9Render.h"
#include "E9Array.h"
#include "E9Hash.h"

class r9TexturePool
{
struct tEntry
{
	R9TEXTURE	m_texture;
	char*		m_name;
	tEntry()	{ m_texture=NULL; m_name=NULL; }
};

public:
					r9TexturePool()									{};
		void		Init();
		void		Done();

inline	int			Size()											{ return m_array.Size(); }
		int			Add( R9TEXTURE texture, char* name );			// add a specific texture, return index
		int			Load( char* name, BOOL noduplicate=TRUE );		// load a texture by file; if noduplicate and texture already in pool, the same index is returned
		int			Find( char* name );								// search for a texture; return index
		void		Del( int idx );									// delete a texture
inline	R9TEXTURE	GetTexture( int idx )							{ if(0<=idx && idx<Size()) return m_array[idx].m_texture; else return NULL; }
inline	char*		GetName( int idx )								{ if(0<=idx && idx<Size()) return m_array[idx].m_name; else return NULL; }

		cArray<tEntry>	m_array;
		cHash			m_hash;
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
