/////////////////////////////////////////////////////////////////////////////////////////
// GSUTIL.H
// General utilities used inside scripter
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GSUTIL_
#define _GSUTIL_

#include "string.h"
#include "gscfg.h"

/////////////////////////////////////////////////////////////////////////////////////////
// STRING TOOLS
// Used for easy and safe use of strings
/////////////////////////////////////////////////////////////////////////////////////////
inline int gsstrsize( char* sz )
{ 
	if(sz) return (int)strlen(sz)+1; else return 0; 
}

inline int gsstrlen( char* sz )
{ 
	if(sz) return (int)strlen(sz); else return 0; 
}
inline char* gsstrdup( char* sz )		
{ 
	if(sz==NULL) return NULL;
	char* sz2 = (char*)gsmalloc((int)strlen(sz)+1);
	strcpy(sz2,sz);
	return sz2;
}

inline char* gsstradd( char* sza, char* szb )
{
	if(sza==NULL && szb==NULL) return NULL;
	char* szc = (char*)gsmalloc( gsstrlen(sza)+gsstrlen(szb)+1 );
	szc[0]=0;
	if(sza) strcat(szc,sza);
	if(szb) strcat(szc,szb);
	return szc;
}

inline int gsstrcmp( char* sza, char* szb )
{
	if(sza==szb) return 0;
	if(sza!=NULL && szb!=NULL) return strcmp(sza,szb);
	return 1;
}



/////////////////////////////////////////////////////////////////////////////////////////
// HASH
// Obs: be carefull not to add duplicates, beause last will be served first !
/////////////////////////////////////////////////////////////////////////////////////////
#define GSHASH_MAX	256

struct gsHashEntry
{
	char*			m_key;
	int				m_data;
	gsHashEntry*	m_next;
};

class gsHash
{
	public:
					gsHash	();
					~gsHash	();
			
			void	Reset	();
			void	Report	();							// log some info for debug purpose

			BOOL	Find	( char* key, int &data );	// return TRUE if success (and set data)
			BOOL	Add		( char* key, int data );	// return TRUE if success
			BOOL	Del		( char* key );				// return TRUE if success

	inline	int		KeyIdx	( char* key );				// -1 if failed, else idx<256

	gsHashEntry*	m_hash[GSHASH_MAX];					// nodes list
};

inline int gsHash::KeyIdx( char* key )
{
	if( key==NULL || key[0]==0 ) return -1; // failed
	dword code = 0;
	dword scramble = 0;
	int i=0;
	while(key[i] && i<16)
	{
		scramble = scramble + (code>>8);
		code = (code<<8) + key[i];
		code = code ^ scramble;
		i++;
	}
	return code % GSHASH_MAX;
}

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
#endif
