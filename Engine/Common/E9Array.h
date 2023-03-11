///////////////////////////////////////////////////////////////////////////////////////////////////
// E9Array.h
// Array of elements
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __E9ARRAY_H__
#define __E9ARRAY_H__

#include "E9System.h"
#include "D9Debug.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// cArray
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class cArray
{
public:
				cArray		();
			
inline	void	Init		( int size, int grow=32 );
inline	void	Done		();

inline	int		Size		()							{ return m_size; }
inline	void	Resize		( int size );				// loose elements if shrinks

inline	T&		Get			( int idx );
inline	void	Set			( int idx, T& node );
		
inline	T& operator[]		( int idx )					{ return Get(idx); }

		int		m_grow;									// grow amount; if non 0 Add will grow the size if no more space
		int		m_size;									// number of elements
		T*		m_array;								// list
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
cArray<T>::cArray()
{
	guardfast(cArray::cArray)
	m_grow = 32;
	m_size = 0;
	m_array = NULL;
	unguardfast();
}

template <class T>
inline void cArray<T>::Init( int size, int grow )
{
	guardfast(cArray::Init)
	sassert(m_size==0 && m_array==NULL);
	m_grow = grow;
	m_size = size;
	if(m_size==0) return;
	m_array = (T*)smalloc(m_size*sizeof(T));
	memset(m_array,0,m_size*sizeof(T));
	unguardfast()
}

template <class T>
inline void cArray<T>::Done()
{
	guardfast(cArray::Done)
	if(m_array==NULL) return;
	sfree(m_array); m_array=NULL;
	m_size=0;
	unguardfast()
}

template <class T>
inline void cArray<T>::Resize( int size )
{
	guardfast(cArray::Resize)
	if(size<m_size) // shrink
	{
		m_array = (T*)srealloc(m_array,size*sizeof(T));
		m_size = size;
	}
	else
	if(size>m_size) // grow
	{
		m_array = (T*)srealloc(m_array,size*sizeof(T));
		memset(m_array+m_size,0,(size-m_size)*sizeof(T));
		m_size = size;
	}
	unguardfast();
}

template <class T>
inline T& cArray<T>::Get( int idx )
{
	guardfast(cArray::Get)
	sassert(0<=idx && idx<m_size);
	return m_array[idx];
	unguardfast()
}

template <class T>
inline void cArray<T>::Set( int idx, T& node )
{
	guardfast(cArray::Set)
	sassert(0<=idx);
	if(idx>=m_size)	Resize(idx+m_grow);
	m_array[idx] = node;
	unguardfast()
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// cPArray
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class cPArray
{
public:
				cPArray		();
			
inline	void	Init		( int size, int grow=32 );
inline	void	Done		();
inline	void	Empty		();							// just free all slot

inline	int		Size		()							{ return m_size; }
inline	void	Resize		( int size );				// delete elements if shrinks

inline	T*		Get			( int idx );
inline	void	Set			( int idx, T* node );

inline	int		Add			( T* node );				// set on a free slot; return index or -1
inline	void	Del			( int idx );				// delete element and free slot
inline	int		GetFreeIdx	();							// returns free slot or -1
		
inline	T* operator[]		( int idx )					{ return Get(idx); }

		int		m_grow;									// grow amount; if non 0 Add will grow the size if no more space
		int		m_size;									// number of elements
		T**		m_array;								// list

private:
		int		m_last;									// last set, used for searching free slots
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
cPArray<T>::cPArray()
{
	guardfast(cPArray::cPArray)
	m_grow = 32;
	m_size = 0;
	m_array = NULL;
	m_last = 0;
	unguardfast();
}

template <class T>
inline void cPArray<T>::Init( int size, int grow )
{
	guardfast(cPArray::Init)
	sassert(m_size==0 && m_array==NULL);
	m_grow = grow;
	m_size = size;
	if(m_size==0) return;
	m_array = (T**)smalloc(m_size*sizeof(T*));
	memset(m_array,0,m_size*sizeof(T*));
	m_last = 0;
	unguardfast()
}

template <class T>
inline void cPArray<T>::Done()
{
	guardfast(cPArray::Done)
	if(m_array==NULL) return;
	Empty();
	sfree(m_array); m_array=NULL;
	m_size=0;
	unguardfast()
}

template <class T>
inline void cPArray<T>::Empty()
{
	guardfast(cPArray::Empty)
	for(int i=0;i<m_size;i++)
	{
		if(m_array[i]) { sdelete(m_array[i]); m_array[i]=NULL; }
	}
	m_last = 0;
	unguardfast();
}

template <class T>
inline void cPArray<T>::Resize( int size )
{
	guardfast(cPArray::Resize)
	int i;
	if(size<m_size) // shrink
	{
		for(i=size;i<m_size;i++) 
			if(m_array[i]) sdelete(m_array[i]);
		m_array = (T**)srealloc(m_array,size*sizeof(T*));
		m_size = size;
		m_last = 0;
	}
	else
	if(size>m_size) // grow
	{
		m_array = (T**)srealloc(m_array,size*sizeof(T*));
		memset(m_array+m_size,0,(size-m_size)*sizeof(T*));
		m_last = m_size;
		m_size = size;
	}
	unguardfast();
}

template <class T>
inline T* cPArray<T>::Get( int idx )
{
	guardfast(cPArray::Get)
	if(0<=idx && idx<m_size) return m_array[idx];
	return NULL;
	unguardfast()
}

template <class T>
inline void cPArray<T>::Set( int idx, T* node )
{
	guardfast(cPArray::Set)
	if(0<=idx && idx<m_size) m_array[idx] = node;
	m_last = idx;
	unguardfast()
}

template <class T>
inline int cPArray<T>::Add( T* node )
{
	guardfast(cPArray::Add)
	int idx = GetFreeIdx();
	if(idx!=-1) Set(idx,node);
	else
	if(m_grow>0)
	{
		Resize(m_size+m_grow);
		idx = GetFreeIdx();
		if(idx!=-1) Set(idx,node);
	}
	m_last = idx;
	return idx;
	unguardfast()
}

template <class T>
inline void cPArray<T>::Del( int idx )
{
	guardfast(cPArray::Del)
	if(idx<0 || idx>=m_size) return;
	if(m_array[idx]) sdelete(m_array[idx]);
	m_last = idx;
	unguardfast()
}

template <class T>
inline int cPArray<T>::GetFreeIdx()
{
	guardfast(cPArray::GetFreeIdx)
	if(m_size==0) return -1;
	if(m_size==1) return (m_array[0]==NULL)?0:-1;

	unsigned int i=( (unsigned int)m_last )%m_size;
	unsigned int l=( i+m_size-1 )%m_size;
	while(i!=l)
	{
		if(m_array[i]==NULL) return i;
		i = (i+1)%m_size;
	}
	return -1;
	unguardfast()
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
