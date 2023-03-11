///////////////////////////////////////////////////////////////////////////////////////////////////
// E9Hash.h
// Obs: 
// 1.keys can be integers or strings, depending on the init mode
// 2.cHashNode must contain a member for next linking ( T* _next; ) and 
//   a function for the key ( void* _key(); )
// 3.also cHashNode can have constructor and destructor if needed
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __E9HASH_H__
#define __E9HASH_H__

#include "E9System.h"
#include "D9Debug.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// class cHash
///////////////////////////////////////////////////////////////////////////////////////////////////
class cHash
{
		struct cHashNode
		{
			void*		m_key;
			void*		m_data;
			cHashNode*	m_next;
			cHashNode()	{ m_key=m_data=m_next=NULL; }
		};

public:
				cHash		()							{ m_mode = 0; m_size = 0; m_hash = NULL; }
			
inline	void	Init		( int size, int mode );		// mode=0(integer)/1(string)
inline 	void	Done		();

// pointer friendly
inline 	BOOL	Find		( void* key, void* &data );	// return node; NULL if not found
inline 	BOOL	Add			( void* key, void* data );	// adds a node
inline 	BOOL	Del			( void* key );				// delete a node by key

// integer friendly
inline 	BOOL	Find		( int key, int &data )		{ void* pdata; if(Find((void*)(intptr)key,pdata)) { data = (int)(intptr)pdata; return TRUE; } else return FALSE; }
inline 	BOOL	Add			( int key, int data )		{ return Add((void*)(intptr)key,(void*)(intptr)data); }
inline 	BOOL	Del			( int key )					{ return Del((void*)(intptr)key); }

// string friendly
inline 	BOOL	Find		( char* key, int &data )	{ void* pdata; if(Find((void*)key,pdata)) { data = (int)(intptr)pdata; return TRUE; } else return FALSE; }
inline 	BOOL	Add			( char* key, int data )		{ return Add((void*)key,(void*)(intptr)data); }
inline 	BOOL	Del			( char* key )				{ return Del((void*)key); }

inline 	BOOL	Enum		( int idx, void* &key, void* &data ); // idx must grow until FALSE is returned; slow

inline	int		GetKeyIdx	( void* key );				// -1 if failed, else idx<size
inline	BOOL	CmpKey		( void* key1, void* key2 );	// TRUE if equal

		int					m_mode;			// key mode
		int					m_size;			// hash size
		cHashNode**			m_hash;			// nodes list
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
inline void cHash::Init( int size, int mode )
{
	guardfast(cHash::Init)
	sassert(m_size==0 && m_hash==NULL);
	m_size = size;
	m_mode = mode;
	if(m_size==0) return;
	m_hash = (cHashNode**)smalloc(m_size*sizeof(cHashNode*));
	memset(m_hash,0,m_size*sizeof(cHashNode*));
	unguardfast()
}

inline void cHash::Done()
{
	guardfast(cHash::Done)
	if(m_hash==NULL) return;
	cHashNode* n;
	cHashNode* t;
	for(int i=0;i<m_size;i++)
	{
		n=m_hash[i];
		while(n)
		{
			t = n;
			n = n->m_next;
			sdelete(t);
		}
		m_hash[i]=NULL;
	}
	sfree(m_hash); m_hash=NULL;
	m_size=0;
	unguardfast()
}

inline BOOL cHash::Find( void* key, void* &data )
{
	guardfast(cHash::Find)
	int idx=GetKeyIdx(key);
	if(idx==-1) return FALSE;
	cHashNode* n = m_hash[idx];
	while(n!=NULL)
	{
		if(CmpKey(key,n->m_key)) { data=n->m_data; return TRUE; }
		n = n->m_next;
	}
	return FALSE; // not found
	unguardfast()
}

inline BOOL cHash::Add( void* key, void* data )
{
	guardfast(cHash::Add)
	int idx=GetKeyIdx(key);
	if(idx==-1) return FALSE;
	cHashNode* n = snew cHashNode();
	n->m_key = key;
	n->m_data = data;
	n->m_next = m_hash[idx];
	m_hash[idx] = n;
	return TRUE;
	unguardfast()
}

inline BOOL cHash::Del( void* key )
{
	guardfast(cHash::Del)
	int idx=GetKeyIdx(key);
	if(idx==-1) return FALSE;
	cHashNode* n = m_hash[idx];
	cHashNode* p = NULL;
	while(n)
	{
		if(CmpKey(key,n->m_key)) break;
		p = n;
		n = n->m_next;
	}
	if(n==NULL) return FALSE; // not found
	
	// relink
	if(p)	p->m_next = n->m_next; 
	else	m_hash[idx] = n->m_next;

	sdelete(n);
	return TRUE;
	unguardfast()
}

inline BOOL cHash::Enum( int idx, void* &key, void* &data )
{
	guard(cHash::Enum);
	int count = 0;
	cHashNode* n=NULL;
	for(int i=0;i<m_size;i++)
	{
		n=m_hash[i];
		if(n==NULL) continue;
		if(idx==count) goto found;
		while(n->m_next)
		{
			n=n->m_next;
			count++;
			if(idx==count) goto found;
		}
	}
	return FALSE; // not found

	found:
	key = n->m_key;
	data = n->m_data;
	return TRUE;
	unguard();
}

inline int cHash::GetKeyIdx( void* key )
{
	guardfast(cHash::GetKeyIdx)
	if(m_size<=0) return -1; // failed
	if(m_mode==0)
		return (int)((dwordptr)(key))%m_size;
	else
	{
		if(key==NULL) return -1; // failed
		int i=0;
		int idx=0;
		while( ((char*)key)[i] ) idx += ((char*)key)[i++];
		return idx % m_size;
	}
	unguardfast()
}

inline BOOL cHash::CmpKey( void* key1, void* key2 )
{
	guardfast(cHash::CmpKey)
	if(m_mode==0)
		return (dwordptr)(key1) == (dwordptr)(key2);
	else
	{
		if(key1==NULL || key2==NULL) return FALSE;
		return (0==strcmp((char*)key1,(char*)key2));
	}
	unguardfast()
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
