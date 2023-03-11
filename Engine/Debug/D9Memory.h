///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Memory.h
// Debug memory management functions
// Config: 
// D9_ENABLE_MEMORY, D9_ENABLE_MEMORYTRACK, D9_ENABLE_MEMORYCOUNT, D9_ENABLE_MEMORYCHECK, D9_ENABLE_MEMORYCONTENT
// Interface:
// smalloc, srealoc, sfree, snew, sdelete, srelease
// D9_MemoryInit, D9_MemoryDone, D9_MemoryReport, D9_MemoryDebugReport
// Tips:
// _crtBreakAlloc, _CrtSetBreakAlloc
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __D9MEMORY_H__
#define __D9MEMORY_H__

#include "E9System.h"
#include "E9Engine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// d9Memory class
///////////////////////////////////////////////////////////////////////////////////////////////////
struct d9MemoryTrack
{
	void*			m_addr;			// user addr
	int				m_size;			// user size
	int				m_mode;			// 0=smalloc, 1=snew
	int				m_line;			// line where allocated
	char*			m_file;			// file where allocated
	char*			m_func;			// func where allocated
	d9MemoryTrack*	m_next;			// next node in hash
};

class d9Memory
{
public:

static	void			Init		();		// init hash
static	void			Done		();		// clear and report

// memory functions
static	void*			Malloc		( int size, const char* file, int line, const char* func );
static	void*			Realloc		( void* oldaddr, int size, const char* file, int line, const char* func );
static	void			Free		( void* addr, const char* file, int line, const char* func );
static	void*			New			( int size, const char* file, int line, const char* func );
static	void			Delete		( void* addr, const char* file, int line, const char* func );

// memory hash
static	int				TrackCount	();
static	d9MemoryTrack*	TrackGet	( void* addr );
static	BOOL			TrackAdd	( void* addr, int size, const char* file, int line, const char* func, int mode );					// alloc
static	BOOL			TrackDel	( void* addr, int size, const char* file, int line, const char* func, int mode );					// delete
static int				TrackSort	( const void* a, const void* b );

// memory report
static void				Report		( BOOL full=FALSE );

// memory count
static	int				m_countMalloc;			// no of mallocs
static	int				m_countFree;			// no of frees
static	int				m_countNew;				// no of news
static	int				m_countDelete;			// no of deletes
static	int				m_totalMalloc;			// total size of mallocated memory
static	int				m_totalNew;				// total size of newed memory
static	int				m_total;				// total size of allocated memory (malloc+new)
static	int				m_totalMax;				// max total size of allocated memory (malloc+new)

// hash
static	d9MemoryTrack**	m_hash;					// hash with memory tracks
static	HANDLE			m_semaphore;			// memory semaphore

};

#ifdef D9_ENABLE_MEMORY

inline void *operator new(size_t size, const char* file, int line, const char* func )
{
	return d9Memory::New((int)size, file, line, func);
}
// this operator is not called, just for matching the new operator
inline void operator delete( void* addr, const char* file, int line, const char* func )
{
	d9Memory::Delete(addr, file, line, func);
	free(addr); // hm
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef D9_ENABLE_MEMORY

#define	smalloc(size)			d9Memory::Malloc(size, __FILE__, __LINE__, __FUNC__)
#define srealloc(oldaddr, size)	d9Memory::Realloc(oldaddr, size, __FILE__, __LINE__, __FUNC__)
#define sfree(addr)				d9Memory::Free(addr, __FILE__, __LINE__, __FUNC__), addr=NULL;
#define snew					new (__FILE__, __LINE__, __FUNC__)
#define sdelete(addr)			if(addr!=NULL) { d9Memory::Delete(addr, __FILE__, __LINE__, __FUNC__); 	delete addr; addr=NULL; }

#else

#define	smalloc					malloc
#define srealloc				realloc
#define sfree(addr)				{ if(addr!=NULL) { free(addr); addr=NULL; } }
#define snew					new
#define sdelete(addr)			{ if(addr!=NULL) { delete addr; addr=NULL; } }

#endif

#define	srelease(a)				{ if(a!=NULL){ a->Release(); a=NULL; } }

inline	void	D9_MemoryInit()				{ d9Memory::Init(); }
inline	void	D9_MemoryDone()				{ d9Memory::Done(); }
inline	void	D9_MemoryReport(BOOL full)	{ d9Memory::Report(full); }
inline	void	D9_MemoryDebugReport()		{ _CrtCheckMemory(); _CrtDumpMemoryLeaks(); }

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
