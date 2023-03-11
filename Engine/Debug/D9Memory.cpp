///////////////////////////////////////////////////////////////////////////////////////////////////
// D9Memory.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D9Memory.h"
#include "D9Log.h"
#include "D9Guard.h"

#define	D9_MEMORY_HASHSIZE	256

int	d9Memory::m_countMalloc	= 0;
int	d9Memory::m_countFree	= 0;
int	d9Memory::m_countNew	= 0;
int	d9Memory::m_countDelete	= 0;
int	d9Memory::m_totalMalloc	= 0;
int	d9Memory::m_totalNew	= 0;
int	d9Memory::m_total		= 0;
int	d9Memory::m_totalMax	= 0;

d9MemoryTrack** d9Memory::m_hash = NULL;
HSEMAPHORE d9Memory::m_semaphore = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////

void d9Memory::Init()
{
	guardfast(d9Memory::Init);
	#ifdef D9_ENABLE_MEMORYTRACK
	m_hash = (d9MemoryTrack**)malloc(D9_MEMORY_HASHSIZE*sizeof(d9MemoryTrack*));
	for(int i=0;i<D9_MEMORY_HASHSIZE;i++) m_hash[i]=NULL;
	#endif
	m_semaphore = sys_createsemaphore();
	unguardfast();
}

void d9Memory::Done()
{
	guardfast(d9Memory::Done);
	sys_destroysemaphore(m_semaphore);
	m_semaphore = NULL;

	// clear hash
	if(m_hash!=NULL)
	{
		d9MemoryTrack* n;
		d9MemoryTrack* t;
		for(int i=0;i<D9_MEMORY_HASHSIZE;i++)
		{
			n=m_hash[i];
			while(n!=NULL)
			{
				t = n->m_next;
				delete(n);
				n=t;
			}
			m_hash[i]=NULL;
		}
		free(m_hash); 
		m_hash = NULL;
	}

	unguardfast();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void* d9Memory::Malloc( int size, const char* file, int line, const char* func )
{
	guardfast(Malloc);
	sys_waitsemaphore(m_semaphore,2000);

	void* addr;
	int memsize;

	#ifdef _DEBUG
	addr = _malloc_dbg(size, _NORMAL_BLOCK, file, line);
	memsize = (int)_msize_dbg(addr, _NORMAL_BLOCK);
	#else
	addr = malloc(size);
	memsize = (int)_msize(addr);
	#endif

	if( addr==NULL ) { sys_releasesemaphore(m_semaphore); errorexit("Out of memory"); return NULL; }

	#ifdef D9_ENABLE_MEMORYTRACK
	TrackAdd( addr, memsize, file, line, func, 0 );
	#endif

	#ifdef D9_ENABLE_MEMORYCOUNT
	m_total			+= memsize;
	m_totalMalloc	+= memsize;
	m_totalMax		= m_totalMax>m_total?m_totalMax:m_total;
	m_countMalloc++;
	#endif

	sys_releasesemaphore(m_semaphore);
	return addr;
	unguardfast();
}

void* d9Memory::Realloc( void* oldaddr, int size, const char* file, int line, const char* func )
{
	guardfast(Realloc);
	sys_waitsemaphore(m_semaphore,2000);

	void* addr;
	int memsize;
	int oldsize;

	#ifdef _DEBUG
	
	if(oldaddr==NULL)
	{
		oldsize = 0;
		addr = _malloc_dbg(size, _NORMAL_BLOCK, file, line);
	}
	else
	{
		oldsize = (int)_msize_dbg(oldaddr, _NORMAL_BLOCK);
		addr = _realloc_dbg(oldaddr, size, _NORMAL_BLOCK, file, line);
	}
	memsize = (int)_msize_dbg(addr, _NORMAL_BLOCK);

	#else

	if(oldaddr==NULL)
	{
		oldsize = 0;
		addr = malloc(size);
	}
	else
	{
		oldsize = (int)_msize(oldaddr);
		addr = realloc(oldaddr, size);
	}
	memsize = (int)_msize(addr);
	#endif

	if(addr==NULL) { sys_releasesemaphore(m_semaphore); errorexit( "Out of memory" ); return NULL; }

	#ifdef D9_ENABLE_MEMORYTRACK
	if(oldaddr!=NULL) TrackDel(oldaddr, oldsize, file, line, func, 0);
	TrackAdd(addr, memsize, file, line, func, 0);
	#endif

	#ifdef D9_ENABLE_MEMORYCOUNT
	m_total			-= oldsize;
	m_total			+= memsize;
	m_totalMalloc	-= oldsize;
	m_totalMalloc	+= memsize;
	if(oldaddr==NULL) m_countMalloc++;
	#endif

	sys_releasesemaphore(m_semaphore);
	return addr;
	unguardfast();
}

void d9Memory::Free( void* addr, const char* file, int line, const char *func )
{
	guardfast(Free);
	sys_waitsemaphore(m_semaphore,2000);

	if(addr==NULL)
	{
		dlog( LOGSYS, "Free NULL pointer [%s; %d; %s]\n", func, line, file );
		sys_releasesemaphore(m_semaphore);
		return;
	}

	#ifdef D9_ENABLE_MEMORYCHECK
	if( IsBadReadPtr(addr, 1) || IsBadWritePtr(addr, 1) )
	{
		dlog( LOGSYS, "Free inaccesible pointer [%s; %d; %s]\n", func, line, file );
		sys_releasesemaphore(m_semaphore);
		errorexit("Free inaccesible pointer");
		return;
	}
	#endif

	int oldsize;
	#ifdef _DEBUG
	oldsize = (int)_msize_dbg(addr, _NORMAL_BLOCK);
	#else
	oldsize = (int)_msize(addr);
	#endif

	#ifdef D9_ENABLE_MEMORYTRACK
	TrackDel(addr, oldsize, file, line, func, 0);
	#endif

	#ifdef D9_ENABLE_MEMORYCOUNT
	m_total			-= oldsize;
	m_totalMalloc	-= oldsize;
	m_countFree++;
	#endif

	#ifdef _DEBUG
	_free_dbg(addr, _NORMAL_BLOCK);
	#else
	free(addr);
	#endif

	sys_releasesemaphore(m_semaphore);
	unguardfast();
}

void* d9Memory::New( int size, const char* file, int line, const char* func )
{
	guardfast(New);
	sys_waitsemaphore(m_semaphore,2000);

	void* addr;
	#ifdef _DEBUG
	addr = _malloc_dbg(size, _NORMAL_BLOCK, file, line);
	#else
	addr = malloc(size);
	#endif

	if( addr==NULL ) { sys_releasesemaphore(m_semaphore); errorexit("Out of memory"); return NULL; }

	int memsize;
	#ifdef _DEBUG
	memsize = (int)_msize_dbg(addr, _NORMAL_BLOCK);
	#else
	memsize = (int)_msize(addr);
	#endif

	#ifdef D9_ENABLE_MEMORYTRACK
	TrackAdd( addr, memsize, file, line, func, 1 );
	#endif

	#ifdef D9_ENABLE_MEMORYCOUNT
	m_total			+= memsize;
	m_totalNew		+= memsize;
	m_totalMax		= m_totalMax>m_total ? m_totalMax : m_total;
	m_countNew++;
	#endif

	sys_releasesemaphore(m_semaphore); 
	return addr;
	unguardfast();
}

void d9Memory::Delete( void* addr, const char* file, int line, const char* func )
{
	guardfast(Delete);
	sys_waitsemaphore(m_semaphore,2000);

	if(addr==NULL) 
	{ 
		dlog( LOGSYS, "Delete NULL pointer [%s; %d; %s]\n", func, line, file ); 
		sys_releasesemaphore(m_semaphore); 
		return; 
	}

	#ifdef D9_ENABLE_MEMORYCHECK
	if( IsBadReadPtr(addr, 1) || IsBadWritePtr(addr, 1) )
	{
		dlog( LOGSYS, "Delete inaccesible pointer [%s; %d; %s]\n", func, line, file );
		sys_releasesemaphore(m_semaphore);
		errorexit("Delete inaccesible pointer");
		return;
	}
	#endif

	int oldsize;
	#ifdef _DEBUG
	oldsize = (int)_msize_dbg(addr, _NORMAL_BLOCK);
	#else
	oldsize = (int)_msize(addr);
	#endif

	#ifdef D9_ENABLE_MEMORYTRACK
	TrackDel(addr, oldsize, file, line, func, 1);
	#endif

	#ifdef D9_ENABLE_MEMORYCOUNT
	m_total		-= oldsize;
	m_totalNew	-= oldsize;
	m_countDelete++;
	#endif

	// can't delete addr here, because the destructor will not be called

	sys_releasesemaphore(m_semaphore); 
	unguardfast();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int	d9Memory::TrackCount()
{
	guardfast(d9Memory::TrackCount);
	if(m_hash==NULL) return 0;
	d9MemoryTrack* mt;
	int count = 0;
	for(int i=0;i<D9_MEMORY_HASHSIZE;i++)
	{
		mt = m_hash[i];
		while(mt!=NULL)
		{
			count++;
			mt = mt->m_next;
		}
	}
	return count;
	unguardfast();
}

d9MemoryTrack* d9Memory::TrackGet( void* addr )
{
	guardfast(d9Memory::TrackGet);
	if(m_hash==NULL) return NULL;

	int idx = (int)((sizet)addr % D9_MEMORY_HASHSIZE);
	d9MemoryTrack* mt = m_hash[idx];
	while(mt!=NULL)
	{
		if(mt->m_addr==addr) return mt;
		mt = mt->m_next;
	}
	return NULL; // not found
	unguardfast();
}

BOOL d9Memory::TrackAdd( void* addr, int size, const char* file, int line, const char* func, int mode )
{
	guardfast(d9Memory::TrackAdd);
	if(m_hash==NULL) return 0;

	int idx = (int)((sizet)addr % D9_MEMORY_HASHSIZE);
	d9MemoryTrack* mt = new d9MemoryTrack;

	mt->m_addr		= addr;
	mt->m_size		= size;
	mt->m_mode		= mode;
	mt->m_line		= line;
	mt->m_file		= (char*)file;
	mt->m_func		= (char*)func;

	// link in hash
	mt->m_next		= m_hash[idx];
	m_hash[idx]		= mt;
	return TRUE;
	unguardfast();
}

BOOL d9Memory::TrackDel( void* addr, int size, const char* file, int line, const char* func, int mode )
{
	guardfast(d9Memory::TrackDel);
	if(m_hash==NULL) return FALSE;

	int idx = (int)((sizet)addr % D9_MEMORY_HASHSIZE);

	d9MemoryTrack* n = m_hash[idx];
	d9MemoryTrack* p = NULL;
	while(n!=NULL)
	{
		if(addr==n->m_addr) break;
		p = n;
		n = n->m_next;
	}
	
	if(n==NULL) // not found
	{
		dlog( LOGSYS, "Releasing unregistered (or released) pointer [%s; %i; %s]\n", func, line, file );
		return FALSE;
	}

	if( size!=n->m_size || mode!=n->m_mode )
	{
		dlog( LOGSYS, "Releasing pointer that don't match:   "
			"THIS: [%s; %d; %s] size=%d, mode=%d   "
			"TRACK:[%s; %d; %s] size=%d, mode=%d\n",
			func, line, file, size, mode,
			n->m_func, n->m_line, n->m_file, n->m_size, n->m_mode );
	}
	
	if(p)	p->m_next = n->m_next; // relink previous
	else	m_hash[idx] = n->m_next;

	delete(n);
	return TRUE;
	unguardfast();
}

int d9Memory::TrackSort(const void* a, const void* b )
{
	int k = strcmp( (*(d9MemoryTrack**)a)->m_func, (*(d9MemoryTrack**)b)->m_func);
	if (k==0) return ((*(d9MemoryTrack**)a)->m_size - (*(d9MemoryTrack**)b)->m_size);
	return k;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void d9Memory::Report( BOOL full )
{
	guardfast(d9Memory::Report);

	int	leakcount = 0;
	#ifdef D9_ENABLE_MEMORYTRACK
	leakcount = TrackCount();
	#else
	leakcount = (m_countMalloc-m_countFree) + (m_countNew-m_countDelete);
	#endif

	dlog(LOGDBG, "----------------------------------------------------------------------\n");
	dlog(LOGDBG, "MEMORY REPORT  (%i Kb used)\n", m_totalMax/1024 );

	if( full && leakcount>0 )
	{
		// SHOW FULL LIST OF LEAKS
		#ifdef D9_ENABLE_MEMORYTRACK

		d9MemoryTrack** blocks = (d9MemoryTrack**)malloc(leakcount*sizeof(d9MemoryTrack*));

		// read hash
		int i;
		int count=0;
		d9MemoryTrack* mt;
		for(i=0;i<D9_MEMORY_HASHSIZE;i++)
		{
			mt = m_hash[i];
			while(mt!=NULL)
			{
				blocks[count]=mt;
				mt = mt->m_next;
				count++;
			}
		}

		qsort( &(blocks[0]), count, sizeof(d9MemoryTrack *), TrackSort );

		if(count>0)
		{
			dlog( LOGDBG, "There are %d memory blocks registered.\n", count );
			dlog( LOGDBG, "----------------------------------------------------------------------\n" );
			dlog( LOGDBG, "%4s NEW %8s %8s %12s %20s %8s %40s %16s\n", "NO", "ADDR", "ID", "SIZE", "FUNC", "LINE", "FILE", "CONTENT" );


			for( i=0; i<count; i++ )
			{
				int j=0;
				char content[16];
				for(j=0;j<15;j++) content[j]='.';
				content[15]=0;			

				#ifdef D9_ENABLE_MEMORYCONTENT
				BOOL inaccessible = FALSE;
				#ifdef D9_ENABLE_MEMORYCHECK
				inaccessible = ( IsBadReadPtr(blocks[i]->m_addr, 1) || IsBadWritePtr(blocks[i]->m_addr, 1) );
				#endif
				if(!inaccessible)
				{
					try 
					{
					if(blocks[i]->m_addr!=NULL)
						strncpy(content, (char*)blocks[i]->m_addr, blocks[i]->m_size<=15 ? blocks[i]->m_size : 15);
					}
					catch(...)
					{
						inaccessible = TRUE;
					}
					for(j=0;j<15 && content[j]!=0;j++)
					{
						if(content[j]<32 || content[j]>127)
							content[j]='.';
					}
				}
				if(inaccessible)
					strcpy(content,"inaccesible");
				#endif

				long blockid=-1;
				if(!_CrtIsMemoryBlock( blocks[i]->m_addr, (dword)_msize(blocks[i]->m_addr), &blockid, NULL, NULL )) blockid=-1;

				dlog(LOGDBG, "%4d  %c  %08x %8x %12d %20s %8d %40s %16s ", 
					i+1,
					blocks[i]->m_mode ? '*' : ' ',
					blocks[i]->m_addr,
					blockid,
					blocks[i]->m_size,
					blocks[i]->m_func,
					blocks[i]->m_line,
					blocks[i]->m_file,
					content);

				dlog(LOGDBG, "\n");
			}
		}
		else
		{
			dlog(LOGDBG, "There are no memory blocks registered.\n");
		}
	
		free(blocks);
		#endif

		#ifdef D9_ENABLE_MEMORYCOUNT
		dlog(LOGDBG, "----------------------------------------------------------------------\n");
		dlog(LOGDBG, "size             = %d = %d (malloc) + %d (new)\n", m_total, m_totalMalloc, m_totalNew );
		dlog(LOGDBG, "calls            = %d (malloc) / %d (free)\n", m_countMalloc, m_countFree);
		dlog(LOGDBG, "calls            = %d (new) / %d (delete)\n", m_countNew, m_countDelete);
		#endif
	}

	// basic report
	if(leakcount>0)
	{
		dlog( LOGDBG, "----------------------------------------------------------------------\n");
		dlog( LOGSYS, "%-8i                                      MEMORY LEAKS DETECTED :(\n", leakcount);
	}

	dlog( LOGDBG, "----------------------------------------------------------------------\n");

	unguardfast();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
