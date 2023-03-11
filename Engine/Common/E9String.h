///////////////////////////////////////////////////////////////////////////////////////////////////
// E9String.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __E9STRING_H__
#define __E9STRING_H__

#include "E9System.h"
#include "D9Debug.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////////////////////////
inline char* sstrdup( const char* sz )
{
	if(sz==NULL) return NULL;
	char* szdest = (char*)smalloc( (int)strlen(sz)+1 ); 
	if(!szdest) return NULL;
	strcpy(szdest,sz);
	return szdest;
}

char* sprint(char* szFormat, ...);

///////////////////////////////////////////////////////////////////////////////////////////////////
// cStr
///////////////////////////////////////////////////////////////////////////////////////////////////
class cStr
{
public:
	LPSTR			m_data;
	int				m_size;

public:

					cStr();
					cStr(const cStr &str);
					cStr(LPCSTR sz);
					cStr(LPCSTR sz1, LPCSTR sz2);
					cStr(int n);
					cStr(dword n);
					cStr(float n);
					~cStr();

	inline	void	Release();
	
	inline	void	Refresh();

	inline			operator LPCSTR() const;
	inline			operator LPSTR();

	inline	void	operator= (LPCSTR sz);
	inline	BOOL	operator!=(LPCSTR sz) const;
	inline	BOOL	operator==(LPCSTR sz) const;
	inline	cStr&	operator+=(LPCSTR sz);
	inline	cStr	operator+ (LPCSTR sz);

	inline	void	operator= (const cStr &str);
	inline	BOOL	operator!=(const cStr &str) const;
	inline	BOOL	operator==(const cStr &str) const;
	inline	cStr&	operator+=(const cStr &str);
	inline	cStr	operator+ (const cStr &str);

	inline	void	operator= (char chChar);
	inline	void	operator+=(char chChar);

	inline	void	operator= (int n);
	inline	void	operator+= (int n);

	inline	void	operator= (dword n);
	inline	void	operator+= (dword n);

	inline	void	operator= (float n);
	inline	void	operator+= (float n);

	inline	char	operator[](int nIndex) const;
	inline	char&	operator[](int nIndex);

	inline	int		Len() const;
	inline	BOOL	IsEmpty() const;
	inline	int		Cmp(LPCSTR sz) const;
	inline	int		Cmpi(LPCSTR sz) const;
	inline	int		HasStr(LPCSTR sz, int crt=0) const;					// -1 if doesn't
	inline	int		HasStr(const cStr &str, int crt=0) const;			// -1 if doesn't
	inline	cStr	&Up();
	inline	cStr	&Down();

	inline	void	Left(int nIndex)	{ Mid(0, nIndex-1); }
	inline	void	Right(int nIndex)	{ Mid(nIndex, m_size - 1); }
	inline	void	Mid(int nStart, int nEnd);

			void	Ins(int nIndex, char chChar );
			void	Ins(int nIndex, char* sz );
			void	Ins(int nIndex, char* sz, int szlen );
			void	Del(int nIndex);

	inline	void	SetSize( int nSize ) { Realloc(nSize); }

protected:

	inline	void	Alloc(int nSize);
	inline	void	Realloc(int nSize);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
// inlines
///////////////////////////////////////////////////////////////////////////////////////////////////

inline void cStr::Refresh()
{
	if(m_data!=NULL) 
	{
		m_size = (int)strlen(m_data);
	}
	else
		m_size = -1;
}

inline cStr::operator LPCSTR() const
{
	sassert(m_data != NULL);
	return (LPCSTR)m_data;
}

inline cStr::operator LPSTR()
{
	sassert(m_data != NULL);
	return (LPSTR)m_data;
}

inline void cStr::operator=(LPCSTR sz)
{
	sassert(sz != NULL);
	if (m_data != sz)
	{
		Realloc((int)strlen(sz));
		strcpy(m_data, sz);
	}
}

inline BOOL cStr::operator!=(LPCSTR sz) const
{
	sassert(sz != NULL);
	return Cmp(sz) != 0;
}

inline BOOL cStr::operator==(LPCSTR sz) const
{
	sassert(sz != NULL);
	return Cmp(sz) == 0;
}

inline	cStr& cStr::operator+=(LPCSTR sz)
{
	sassert(sz != NULL);
	int l0 = m_size;
	int l1 = (int)strlen(sz);
	Realloc(l0+l1);
	memcpy(m_data + l0, sz, l1);
	m_data[m_size] = 0;

	return *this;
}

inline	cStr cStr::operator+(LPCSTR sz)
{
	sassert(sz != NULL);
	return cStr(m_data, sz);
}



inline void cStr::operator=(const cStr &str)
{
	if (m_data != str.m_data)
	{
		Realloc(str.m_size);
		strcpy(m_data, str.m_data);
	}
}

inline BOOL cStr::operator!=(const cStr &str) const
{
	return Cmp(str.m_data) != 0;
}

inline BOOL cStr::operator==(const cStr &str) const
{
	return Cmp(str.m_data) == 0;
}

inline	cStr& cStr::operator+=(const cStr &str)
{
	int l0 = m_size;
	int l1 = str.m_size;
	Realloc(l0+l1);
	memcpy(m_data + l0, str.m_data, l1);
	m_data[m_size] = 0;

	return *this;
}

inline	cStr cStr::operator+(const cStr &str)
{
	return cStr(m_data, str.m_data);
}

inline void cStr::operator=(char chChar)
{
	Realloc(1);
	m_data[0] = chChar;
	m_data[1] = 0;
}

inline void cStr::operator+=(char chChar)
{
	Realloc(m_size+1);
	m_data[m_size-1] = chChar;
	m_data[m_size] = 0;
}

inline void cStr::operator=(int n)		{ Realloc(32); itoa( n, m_data, 10); Refresh(); }
inline void cStr::operator+=(int n)		{ int i=m_size;Realloc(m_size+32); itoa(n, m_data+i, 10); Refresh(); }
inline void cStr::operator=(dword n)	{ Realloc(32); ultoa( n, m_data, 10); Refresh(); }
inline void cStr::operator+=(dword n)	{ int i=m_size;Realloc(m_size+32); ultoa(n, m_data+i, 10); Refresh(); }
inline void cStr::operator=(float n)	{ Realloc(32); strcpy(m_data, sprint("%f",n)); Refresh(); }
inline void cStr::operator+=(float n)	{ Realloc(m_size+32); strcat(m_data, sprint("%f",n)); Refresh(); }

inline char &cStr::operator[](int nIndex)
{
	sassert(nIndex >= 0 && nIndex <= m_size);
	sassert(m_data != NULL);
	return m_data[nIndex];
}

inline char cStr::operator[](int nIndex) const
{
	sassert(nIndex >= 0 && nIndex <= m_size);
	sassert(m_data != NULL);
	return m_data[nIndex];
}

inline int cStr::Len() const
{
	sassert(m_size == (int)strlen(m_data));
	return m_size;
}

inline BOOL	cStr::IsEmpty() const
{
	return m_size==0;
}

inline int cStr::Cmp(LPCSTR sz) const
{
	sassert(m_data != NULL);
	sassert(sz != NULL);
	return strcmp(m_data, sz);
}

inline int cStr::Cmpi(LPCSTR sz) const
{
	sassert(m_data != NULL);
	sassert(sz != NULL);
	return stricmp(m_data, sz);
}

inline	int	cStr::HasStr(LPCSTR sz, int crt) const
{
	sassert(m_data!=NULL && sz!=NULL);
	sassert(m_size>0 && strlen(sz)>0);
	sassert(crt<m_size);

	LPSTR s = strstr(m_data+crt, sz);
	if(s==NULL) return -1;
	return (int)(s-m_data);
}

inline	int	cStr::HasStr(const cStr &str, int crt) const
{
	sassert(m_data!=NULL && str.m_data!=NULL);
	sassert(m_size>0 && str.m_size>0);
	sassert(crt<m_size);

	LPSTR s = strstr(m_data+crt, str.m_data);
	if(s==NULL) return -1;
	return (int)(s-m_data);
}

inline cStr& cStr::Up()
{
	if (m_data!=NULL)
		_strupr(m_data);

	return *this;
}

inline cStr& cStr::Down()
{
	if (m_data!=NULL)
		_strlwr(m_data);

	return *this;
}

inline void	cStr::Mid(int nStart, int nEnd)
{
	memmove(m_data, m_data + nStart, nEnd - nStart + 1);
	m_data[nEnd - nStart + 1] = 0;
	Realloc(nEnd - nStart + 1); 
}

inline void cStr::Alloc(int nSize)
{
	guardfast(cStr::Alloc)
	sassert(m_data == NULL);
	m_size = nSize;
	m_data = (LPSTR)smalloc(nSize + 1);
	memset(m_data, 0, nSize + 1);
	unguardfast()
}

inline void cStr::Release()
{
	guardfast(cStr::Release)
	sassert(m_data != NULL);
	sfree(m_data);
	m_size = 0;
	m_data = NULL;
	unguardfast()
}

inline void cStr::Realloc(int nSize)
{
	guardfast(cStr::Realloc)
	if( m_size==nSize ) return;
	m_data = (LPSTR)srealloc(m_data, nSize + 1);
	m_size = nSize;
	unguardfast()
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Parsing utils
///////////////////////////////////////////////////////////////////////////////////////////////////
char*	parser_skipchar( char* buffer, char* charlist, int& parsedsize );		// skip any of those chars
char*	parser_skiptochar( char* buffer, char* charlist, int& parsedsize );		// skip until one of those chars
char*	parser_skipline( char* buffer, int& parsedsize );
char*	parser_skipspace( char* buffer, int& parsedsize );
char*	parser_skiptotoken( char* buffer, char* token, int& parsedsize );		// skip all until token find
BOOL	parser_readtoken( char* buffer, char* token, int& parsedsize );
BOOL	parser_readword( char* buffer, char* value, int valuesize, int& parsedsize );
BOOL	parser_readline( char* buffer, char* value, int valuesize, int& parsedsize );
BOOL	parser_readvarstr( char* buffer, char* name, char* value, int valuesize, int& parsedsize );
BOOL	parser_readvarint( char* buffer, char* name, int* value, int& parsedsize );
BOOL	parser_readvarfloat( char* buffer, char* name, float* value, int& parsedsize );
void	parser_trimbackspace( char* buffer, int& pos );							// cuts end spaces, and updates pos (pos=strlen(buffer))

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
