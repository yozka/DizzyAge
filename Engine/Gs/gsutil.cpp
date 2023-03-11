#include "stdafx.h"
#include "gsutil.h"


/////////////////////////////////////////////////////////////////////////////////////////
// HASH
/////////////////////////////////////////////////////////////////////////////////////////
gsHash::gsHash()
{
	guard(gsHash::gsHash)
	for(int i=0;i<GSHASH_MAX;i++)
		m_hash[i]=NULL;
	unguard()
}

gsHash::~gsHash()
{
	guard(gsHash::gsHash)
	Reset();
	unguard()
}

void gsHash::Reset()
{
	guard(gsHash::Reset)
	gsHashEntry* n;
	gsHashEntry* t;
	for(int i=0;i<GSHASH_MAX;i++)
	{
		n=m_hash[i];
		while(n!=NULL)
		{
			t = n->m_next;
			gsdelete(n);
			n=t;
		}
		m_hash[i]=NULL;
	}
	unguard()
}

BOOL gsHash::Add( char* key, int data )
{
	guard(gsHash::Add)
	int idx=KeyIdx(key);
	if(idx==-1) return FALSE;
	gsHashEntry* n = gsnew gsHashEntry;
	n->m_key = key;
	n->m_data = data;
	n->m_next = m_hash[idx];
	m_hash[idx] = n;
	return TRUE;
	unguard()
}

BOOL gsHash::Find( char* key, int &data )
{
	guard(gsHash::Find)
	int idx=KeyIdx(key);
	if(idx==-1) return FALSE;
	gsHashEntry* n = m_hash[idx];
	while(n!=NULL)
	{
		if(strcmp(key,n->m_key)==0)
		{
			data = n->m_data;
			return TRUE;
		}
		n = n->m_next;
	}
	return FALSE; // not found
	unguard()
}

BOOL gsHash::Del( char* key )
{
	guard(gsHash::Del)
	int idx=KeyIdx(key);
	if(idx==-1) return FALSE;
	gsHashEntry* n = m_hash[idx];
	gsHashEntry* p = NULL;
	while(n!=NULL)
	{
		if(strcmp(key,n->m_key)==0) break;
		p = n;
		n = n->m_next;
	}
	if(n==NULL) return FALSE; // not found
	
	if(p)	p->m_next = n->m_next; // relink previous
	else	m_hash[idx] = n->m_next;

	gsdelete(n);
	return TRUE;

	unguard()
}

void gsHash::Report()
{
	guard(gsHash::Report)
	gsHashEntry* n;
	gslog( "HASH LOG\n");
	int count = 0;
	for(int i=0;i<GSHASH_MAX;i++)
	{
		int cnt = 0;
		n=m_hash[i];
		while(n!=NULL) 
		{
			cnt++;
			n = n->m_next;
		}
		count+=cnt;
		gslog("  hash[%03i] = %i\n", i, cnt);
	}
	gslog("  total count = %i\n", count);
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
