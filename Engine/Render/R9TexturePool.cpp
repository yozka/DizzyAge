///////////////////////////////////////////////////////////////////////////////////////////////////
// R9TexturePool.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "E9String.h"
#include "R9TexturePool.h"

void r9TexturePool::Init()
{
	guard(r9TexturePool::Init);
	m_array.Init(32,32);
	m_hash.Init(256,1);
	unguard();
}

void r9TexturePool::Done()
{
	guard(r9TexturePool::Done);
	// destroy any left content
	for(int i=0;i<m_array.Size();i++) Del(i);
	m_hash.Done();
	m_array.Done();
	unguard();
}

int r9TexturePool::Add( R9TEXTURE texture, char* name )
{
	guard(r9TexturePool::Add);
	sassert(name!=NULL);
	sassert(texture!=NULL);
	// find empty position or add on a new position
	int i;
	for(i=0;i<m_array.Size();i++)
		if(!GetName(i)) break;
	// add entry
	tEntry entry;
	entry.m_texture = texture;
	entry.m_name = sstrdup(name);
	m_array.Set(i,entry);
	// add to hash
	m_hash.Add(entry.m_name,i);
	return i;
	unguard();
}

int r9TexturePool::Load( char* name, BOOL noduplicate )
{
	guard(r9TexturePool::Load);
	sassert(name!=NULL);
	if(noduplicate)
	{
		int idx = Find(name);
		if(idx!=-1) return idx;
	}
	R9TEXTURE texture = R9_TextureLoad(name);
	if(!texture) return -1;
	return Add(texture,name);
	unguard();
}

int r9TexturePool::Find( char* name )
{
	guard(r9TexturePool::Find);
	int idx = -1;
	m_hash.Find(name,idx);
	return idx;
	unguard();
}

void r9TexturePool::Del( int idx )
{
	guard(r9TexturePool::Del);
	sassert(0<=idx && idx<Size());
	if(m_array[idx].m_name)		
	{ 
		m_hash.Del(m_array[idx].m_name);
		sfree(m_array[idx].m_name); 
		m_array[idx].m_name = NULL; 
		if(m_array[idx].m_texture)	
			R9_TextureDestroy(m_array[idx].m_texture); 
		m_array[idx].m_texture = NULL;
	}
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
