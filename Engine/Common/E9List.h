///////////////////////////////////////////////////////////////////////////////////////////////////
// E9List.h
// List of elements
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __E9LIST_H__
#define __E9LIST_H__

#include "E9System.h"
#include "D9Debug.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// cList
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class cList
{
public:
				cList		();
			
inline	void	Init		( int sizemax, int grow=32 );
inline	void	Done		();
inline	void	Empty		();							// just delete all nodes

inline	int		Size		()							{ return m_size; }
inline	T&		Get			( int idx );
inline	void	Set			( int idx, T& node );

inline	int		Add			( T& node );				// return index or -1
inline	void	Ins			( int idx, T& node );		// insert at index; shift rest
inline	void	Del			( int idx );				// shift rest

inline	T& operator[]		( int idx )					{ return Get(idx); }
		
		int		m_grow;		// grow amount
		int		m_size;		// number of elements
		int		m_sizemax;	// list size
		T*		m_list;		// list
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
cList<T>::cList()
{
	guardfast(cList::cList)
	m_grow = 32;
	m_sizemax = 0;
	m_size = 0;
	m_list = NULL;
	unguardfast();
}

template <class T>
inline void cList<T>::Init( int sizemax, int grow )
{
	guardfast(cList::Init)
	sverify(m_size==0 && m_list==NULL);
	m_grow = grow;
	m_size = 0;
	m_sizemax = sizemax;
	if(m_sizemax==0) return;
	m_list = (T*)smalloc(m_sizemax*sizeof(T));
	memset(m_list,0,m_sizemax*sizeof(T));
	unguardfast()
}

template <class T>
inline void cList<T>::Done()
{
	guardfast(cList::Done)
	if(m_list==NULL) return;
	Empty();
	sfree(m_list); m_list=NULL;
	m_sizemax=0;
	unguardfast()
}

template <class T>
inline void cList<T>::Empty()
{
	guardfast(cList::Empty)
	m_size=0;
	unguardfast();
}

template <class T>
inline T& cList<T>::Get( int idx )
{
	guardfast(cList::Get)
	sverify(0<=idx && idx<m_size);
	return m_list[idx];
	unguardfast()
}

template <class T>
inline void cList<T>::Set( int idx, T& node )
{
	guardfast(cList::Set)
	sverify(0<=idx && idx<m_size);
	m_list[idx] = node;
	unguardfast()
}

template <class T>
inline int cList<T>::Add( T& node )
{
	guardfast(cList::Add)
	if(m_size==m_sizemax) // grow
	{
		m_sizemax += m_grow;
		m_list = (T*)srealloc(m_list,m_sizemax*sizeof(T));
		memset(m_list+m_sizemax-m_grow, 0, m_grow*sizeof(T));
	}
	m_list[m_size]=node;
	m_size++;
	return(m_size-1);
	unguardfast()
}

template <class T>
inline void cList<T>::Ins( int idx, T& node )
{
	guardfast(cList::Ins)
	sverify(0<=idx && idx<=m_size) return; // allow Ins[m_size]
	if(m_size==m_sizemax) // grow
	{
		m_sizemax += m_grow;
		m_list = (T*)srealloc(m_list,m_sizemax*sizeof(T));
		memset(m_list+m_sizemax-m_grow, 0, m_grow*sizeof(T));
	}
	// shift
	for(int i=m_size-1; i>=idx; i--)
		m_list[i+1] = m_list[i];
	m_list[idx]=node;
	m_size++;
	unguardfast()
}

template <class T>
inline void cList<T>::Del( int idx )
{
	guardfast(cList::Del)
	sverify(0<=idx && idx<m_size);
	if(idx<m_size-1) // shift if not the last one
		memcpy(&m_list[idx], &m_list[idx+1], (m_size-1-idx)*sizeof(T));
	m_size--;
	unguardfast()
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// cPList
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class cPList
{
public:
				cPList		();
			
inline	void	Init		( int sizemax, int grow=32 );
inline	void	Done		();
inline	void	Empty		();							// just delete all nodes

inline	int		Size		()							{ return m_size; }
inline	T*		Get			( int idx );
inline	void	Set			( int idx, T* node );

inline	int		Add			( T* node );				// return index or -1
inline	void	Ins			( int idx, T* node );		// insert at index; shift rest
inline	void	Del			( int idx );				// shift rest
		
inline	T* operator[]		( int idx )					{ return Get(idx); }

		int		m_grow;		// grow amount
		int		m_size;		// number of elements
		int		m_sizemax;	// list size
		T**		m_list;		// list
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
cPList<T>::cPList()
{
	guardfast(cPList::cPList)
	m_grow = 32;
	m_sizemax = 0;
	m_size = 0;
	m_list = NULL;
	unguardfast();
}

template <class T>
inline void cPList<T>::Init( int sizemax, int grow )
{
	guardfast(cPList::Init)
	sverify(m_size==0 && m_list==NULL);
	m_grow = grow;
	m_size = 0;
	m_sizemax = sizemax;
	if(m_sizemax==0) return;
	m_list = (T**)smalloc(m_sizemax*sizeof(T*));
	memset(m_list,0,m_sizemax*sizeof(T*));
	unguardfast()
}

template <class T>
inline void cPList<T>::Done()
{
	guardfast(cPList::Done)
	if(m_list==NULL) return;
	Empty();
	sfree(m_list); m_list=NULL;
	m_sizemax=0;
	unguardfast()
}

template <class T>
inline void cPList<T>::Empty()
{
	guardfast(cPList::Empty)
	for(int i=0;i<m_size;i++)
	{
		if(m_list[i]) {	sdelete(m_list[i]); m_list[i]=NULL; }
	}
	m_size=0;
	unguardfast();
}

template <class T>
inline T* cPList<T>::Get( int idx )
{
	guardfast(cPList::Get)
	if(0<=idx && idx<m_size) return m_list[idx];
	return NULL;
	unguardfast()
}

template <class T>
inline void cPList<T>::Set( int idx, T* node )
{
	guardfast(cPList::Set)
	if(0<=idx && idx<m_size) m_list[idx] = node;
	unguardfast()
}

template <class T>
inline int cPList<T>::Add( T* node )
{
	guardfast(cPList::Add)
	if(m_size==m_sizemax) // grow
	{
		m_sizemax += m_grow;
		m_list = (T**)srealloc(m_list,m_sizemax*sizeof(T*));
		memset(m_list+m_sizemax-m_grow, 0, m_grow*sizeof(T*));
	}
	m_list[m_size]=node;
	m_size++;
	return(m_size-1);
	unguardfast()
}

template <class T>
inline void cPList<T>::Ins( int idx, T* node )
{
	guardfast(cPList::Ins)
	if(idx<0 || idx>m_size) return; // allow Ins[m_size]
	if(m_size==m_sizemax) // grow
	{
		m_sizemax += m_grow;
		m_list = (T**)srealloc(m_list,m_sizemax*sizeof(T*));
		memset(m_list+m_sizemax-m_grow, 0, m_grow*sizeof(T*));
	}
	// shift
	for(int i=m_size-1; i>=idx; i--)
		m_list[i+1] = m_list[i];
	m_list[idx]=node;
	m_size++;
	unguardfast()
}

template <class T>
inline void cPList<T>::Del( int idx )
{
	guardfast(cPList::Del)
	if(idx<0 || idx>=m_size) return;
	if(m_list[idx]) sdelete(m_list[idx]);
	if(idx<m_size-1) // shift if not the last one
		memcpy(&m_list[idx], &m_list[idx+1], (m_size-1-idx)*sizeof(T*));
	m_size--;
	unguardfast()
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////