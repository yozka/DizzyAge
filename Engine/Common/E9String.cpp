///////////////////////////////////////////////////////////////////////////////////////////////////
// E9String.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "E9String.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////////////////////////
char* sprint(char* szFormat, ...)
{
	static char		szString[1024];
	va_list			vaArg;
	
	va_start (vaArg, szFormat);
	vsprintf (szString, szFormat, vaArg);
	va_end (vaArg);

	return szString;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// cStr class
///////////////////////////////////////////////////////////////////////////////////////////////////
cStr::cStr()
{
	m_data = NULL;
	Alloc(0);
}

cStr::cStr(const cStr &str)
{
	m_data = NULL;
	Alloc(str.m_size);
	strcpy(m_data, str.m_data);
}

cStr::cStr(LPCSTR sz)
{
	sassert(sz != NULL);
	m_data = NULL;
	Alloc((int)strlen(sz));
	strcpy(m_data, sz);
}

cStr::cStr(LPCSTR sz1, LPCSTR  sz2)
{
	sassert(sz1 != NULL);
	sassert(sz2 != NULL);
	int l1=(int)strlen(sz1);
	int l2=(int)strlen(sz2);
	Alloc(l1+l2);
	memcpy(m_data, sz1, l1);
	memcpy(m_data+l1, sz2, l2);
	m_data[m_size] = 0; // ???
}


cStr::cStr(int n)		{ m_data = sstrdup(sprint("%i",n)); Refresh(); }
cStr::cStr(dword n)		{ m_data = sstrdup(sprint("%u",n)); Refresh(); }
cStr::cStr(float n)		{ m_data = sstrdup(sprint("%f",n)); Refresh(); }

cStr::~cStr()
{
	Release();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Utils
///////////////////////////////////////////////////////////////////////////////////////////////////

void cStr::Ins(int nIndex, char chChar )
{
	guard(cStr::Ins)

	sassert(m_data != NULL);
	sassert(nIndex >= 0 && nIndex <= m_size);

	Ins( nIndex, &chChar, 1 );
	

	unguard()
}

void cStr::Ins(int nIndex, char* sz )
{
	guard(cStr::Ins)

	sassert(m_data != NULL);
	sassert(nIndex >= 0 && nIndex <= m_size);
	sassert(sz!=NULL);

	Ins( nIndex, sz, (int)strlen(sz) );

	unguard()
}

void cStr::Ins(int nIndex, char* sz, int szlen )
{
	guard(cStr::Ins)

	sassert(m_data != NULL);
	sassert(nIndex >= 0 && nIndex <= m_size);
	sassert(sz!=NULL);
	if(szlen==0) return;
	

	int l0 = m_size;
	Realloc(l0+szlen);
	m_data[m_size] = 0;

	for(int i=l0;i>nIndex;i--) m_data[i]=m_data[i-1];
	memcpy(m_data+nIndex, sz, szlen);

	unguard()
}

void cStr::Del(int nIndex)
{
	guard(cStr::Del)

	sassert(m_data != NULL);
	sassert(nIndex >= 0 && nIndex <= m_size);
	
	for(int i=nIndex;i<m_size;i++) 
		m_data[i]=m_data[i+1];

	Realloc(m_size-1);
	m_data[m_size] = 0;

	unguard()
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Parsing utils
///////////////////////////////////////////////////////////////////////////////////////////////////
char* parser_skipchar( char* buffer, char* charlist, int& parsedsize )
{
	guard(parser_skipchar);
	parsedsize = 0;
	while(*buffer!=0)
	{
		int i=0;
		while(charlist[i]!=0)
		{
			if(*buffer==charlist[i])
				break;
			i++;
		}
		if(charlist[i]==0) // none matched
			return buffer;
		buffer++;
		parsedsize++;
	}
	return buffer;
	unguard();
}

char* parser_skiptochar( char* buffer, char* charlist, int& parsedsize )
{
	guard(parser_skiptochar);
	parsedsize = 0;
	while(*buffer!=0)
	{
		int i=0;
		while(charlist[i]!=0)
		{
			if(*buffer==charlist[i])
				break;
			i++;
		}
		if(charlist[i]!=0) // one matched
			return buffer;
		buffer++;
		parsedsize++;
	}
	return buffer;
	unguard();
}

char* parser_skipline( char* buffer, int& parsedsize )
{
	guard(parser_skipline);
	char* bufferstart = buffer;
	buffer = parser_skiptochar(buffer,"\n\r", parsedsize);
	buffer = parser_skipchar(buffer,"\n\r", parsedsize);
	parsedsize = (int)(intptr)(buffer-bufferstart);
	return buffer;
	unguard();
}


char* parser_skipspace( char* buffer, int& parsedsize )
{
	guard(parser_skipspace);
	return parser_skipchar(buffer," \t\r\n",parsedsize);
	unguard();
}

char* parser_skiptotoken( char* buffer, char* token, int& parsedsize )
{
	guard(parser_skiptotoken);
	parsedsize=0;
	while(*buffer!=0)
	{
		int i=0;
		while(buffer[i]==token[i] && buffer[i]!=0 && token[i]!=0)
			i++;
		if(token[i]==0) return buffer; // got it !
		buffer++;
		parsedsize++;
	}
	return buffer;
	unguard();
}

BOOL parser_readtoken( char* buffer, char* token, int& parsedsize )
{
	guard(parser_readtoken);
	parsedsize=0;
	while(*buffer==*token)
	{
		buffer++;
		token++;
		parsedsize++;
	}
	return *token==0;
	unguard();
}

BOOL parser_readword( char* buffer, char* value, int valuesize, int& parsedsize )
{
	guard(parser_readword);
	parsedsize=0;
	while(*buffer!=0 && *buffer!=' ' && *buffer!='\t' && *buffer!='\n' && *buffer!='\r' && parsedsize<valuesize-1)
	{
		*value = *buffer;
		buffer++;
		value++;
		parsedsize++;
	}
	*value=0;
	return parsedsize>0;
	unguard();
}

BOOL parser_readline( char* buffer, char* value, int valuesize, int& parsedsize )
{
	guard(parser_readline);
	int read=0;
	parsedsize=0;
	while(*buffer!=0 && *buffer!='\n' && *buffer!='\r' && read<valuesize-1)
	{
		*value=*buffer;
		buffer++;
		value++;
		read++;
		parsedsize++;
	}
	if(*buffer=='\r')
	{
		buffer++;
		parsedsize++;
	}
	if(*buffer=='\n')
	{
		buffer++;
		parsedsize++;
	}
	*value = 0;
	return parsedsize>0;
	unguard();
}

BOOL parser_readvarstr( char* buffer, char* name, char* value, int valuesize, int& parsedsize )
{
	guard(parser_readvarstr);
	char* bufferstart = buffer;
	
	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,name,parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,"=",parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	parser_readline(buffer,value,valuesize,parsedsize);
	buffer += parsedsize;

	parsedsize = (int)(intptr)(buffer-bufferstart);
	return TRUE;
	unguard();
}

BOOL parser_readvarint( char* buffer, char* name, int* value, int& parsedsize )
{
	guard(parser_readvarint);
		char* bufferstart = buffer;
	
	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,name,parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,"=",parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	char sztmp[64];	sztmp[0]=0;
	parser_readword(buffer,sztmp,64,parsedsize);
	buffer += parsedsize;
	buffer=parser_skipline(buffer,parsedsize);

	int ret = sscanf(sztmp,"%i",value);
	if(ret!=1) return FALSE;

	parsedsize = (int)(intptr)(buffer-bufferstart);
	return TRUE;
	unguard();
}

BOOL parser_readvarfloat( char* buffer, char* name, float* value, int& parsedsize )
{
	guard(parser_readvarfloat);
		char* bufferstart = buffer;
	
	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,name,parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	if(!parser_readtoken(buffer,"=",parsedsize)) return FALSE;
	buffer += parsedsize;

	buffer=parser_skipspace(buffer,parsedsize);

	char sztmp[64];	sztmp[0]=0;
	parser_readword(buffer,sztmp,64,parsedsize);
	buffer += parsedsize;
	buffer=parser_skipline(buffer,parsedsize);

	int ret = sscanf(sztmp,"%f",value);
	if(ret!=1) return FALSE;

	parsedsize = (int)(intptr)(buffer-bufferstart);
	return TRUE;
	unguard();
}

void parser_trimbackspace( char* buffer, int& pos )
{
	guard(parser_trimbackspace);
	while(pos>0 && (buffer[pos-1]==' ' || buffer[pos-1]=='\t' || buffer[pos-1]=='\r' || buffer[pos-1]=='\n') )
		pos--;
	buffer[pos]=0;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
