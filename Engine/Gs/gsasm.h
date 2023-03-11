#ifndef _GSASM_
#define _GSASM_
#include "gsutil.h"

/////////////////////////////////////////////////////////////////////////////////////////
// GSOBJ
/////////////////////////////////////////////////////////////////////////////////////////
#define GS_NUL		0				// NUL (unusable)
#define GS_INT		1				// integer
#define GS_FLT		2				// float
#define GS_STR		3				// string
#define GS_TAB		4				// table
#define GS_REF		5				// reference
#define GS_PTR		6				// pointer

struct gsObj
{
	inline	gsObj()					{ type=GS_NUL; i=0; size=0; }
	inline	gsObj( int v )			{ type=GS_INT; i=v; size=0; }
	inline	gsObj( dword v )		{ type=GS_INT; i=v; size=0; }
	inline	gsObj( float v )		{ type=GS_FLT; f=v; size=0; }
	inline	gsObj( char* v, int n )	{ type=GS_STR; s=v; size=n; }
	inline	gsObj( gsObj* v, int n ){ type=GS_TAB; o=v; size=n; }
	inline	gsObj( gsObj* v )		{ type=GS_REF; o=v; size=0; }
	inline	gsObj( void* v )		{ type=GS_PTR; p=v; size=0; }

	union
	{
		int			i;	// int
		float		f;	// flt
		char*		s;	// str
		gsObj*		o;	// tab, ref
		void*		p;	// ptr
	};

	int	type;		// data type
	int	size;		// data count (if STR or TAB)
};

#define gsNUL		gsObj()

/////////////////////////////////////////////////////////////////////////////////////////
// GSOPS
/////////////////////////////////////////////////////////////////////////////////////////
// system
#define GSOP_NOP		0
#define GSOP_SET		1		// ref, obj
#define GSOP_GET		2		// ref -- obj
#define GSOP_VAR		3		// num(idx) -- ref(global)
#define GSOP_SETI		4		// ref(str), num(idx), num(char)
#define GSOP_GETI		5		// ref(str), num(idx) -- int(char)
#define GSOP_IDX		6		// ref(tab), str/num(idx) -- ref
#define	GSOP_NEW		7		// [count] -- obj
#define GSOP_CAST		8		// ref, [str] -- obj
#define	GSOP_TYPEOF		9		// ref -- int(*ref.type)
#define	GSOP_SIZEOF		10		// ref -- int(*ref.size)
#define GSOP_STOP		11
#define GSOP_BREAKPOINT	12

// stack
#define GSOP_POP		13		// obj
#define GSOP_PUSHNUL	14		// -- nul
#define GSOP_PUSHINT	15		// -- int(i)
#define GSOP_PUSHFLT	16		// -- flt(f)
#define GSOP_PUSHSTR	17		// -- str(GetStr(i))
#define GSOP_PUSHTAB	18		// [obj_i,..obj_2,obj_1] -- tab(i)
#define GSOP_PUSHLOC	19		// -- local ref ( to stack[] )
#define GSOP_PUSHGLB	20		// -- global ref ( to stack[] )
#define GSOP_PUSHPTR	21		// -- ptr(p)

// jumps
#define GSOP_JMP		22
#define GSOP_JZ			23		// obj 
#define GSOP_JNZ		24		// obj
#define GSOP_CALL		25		// [obj_i,..obj_2,obj_1], num(fn) -- obj
#define GSOP_RET		26		// [obj] -- obj

// logic
#define GSOP_NOT		27
#define GSOP_ANDAND		28
#define GSOP_OROR		29
// aritmetic
#define GSOP_SGN		30
#define GSOP_ADD		31
#define GSOP_SUB		32
#define GSOP_MUL		33
#define GSOP_DIV		34
#define GSOP_MOD		35
#define GSOP_INC		36
#define GSOP_DEC		37
// binar
#define GSOP_INV		38
#define GSOP_AND		39
#define GSOP_OR			40
#define GSOP_XOR		41
#define GSOP_SHR		42
#define GSOP_SHL		43

// compare
#define GSOP_CPE		44
#define GSOP_CPNE		45
#define GSOP_CPL		46
#define GSOP_CPLE		47
#define GSOP_CPG		48
#define GSOP_CPGE		49

#define GS_MAXOP		50		// total number operations 

/////////////////////////////////////////////////////////////////////////////////////////
// OBJ TOOLS
/////////////////////////////////////////////////////////////////////////////////////////
inline	BOOL	gso_isnul		( gsObj& a )						{ return (a.type==GS_NUL); }	
inline	BOOL	gso_isint		( gsObj& a )						{ return (a.type==GS_INT); }
inline	BOOL	gso_isflt		( gsObj& a )						{ return (a.type==GS_FLT); }
inline	BOOL	gso_isstr		( gsObj& a )						{ return (a.type==GS_STR); }
inline	BOOL	gso_istab		( gsObj& a )						{ return (a.type==GS_TAB); }
inline	BOOL	gso_isref		( gsObj& a )						{ return (a.type==GS_REF); }
inline	BOOL	gso_isptr		( gsObj& a )						{ return (a.type==GS_PTR); }
inline	BOOL	gso_istype		( gsObj& a, int type )				{ return (a.type==type); }

inline	gsObj	gso_cast		( gsObj& a, int type, char* format = NULL ); // cast an object using extra format options if available

inline	void	gso_del			( gsObj& a );						// delete (free) object; strings are set to NULL; tables have all content deleted and removed
inline	gsObj	gso_dup			( gsObj& a );						// duplicate object

inline	gsObj	gso_strnew		( int size );						// alloc new string (size must include eos); string is set to empty
inline	gsObj	gso_strdup		( gsObj& a );						// duplicate string object

inline	gsObj	gso_tabnew		( int size );						// alloc new table; table is filled with gsNUL
inline	gsObj	gso_tabdup		( gsObj& a );						// duplicate tab object; content is also duplicated
inline	gsObj*	gso_tabi		( gsObj& a, int idx );				// get the address of a tab element, by index; if outbound, NULL is returned
		void	gso_tabresize	( gsObj& a, int size );				// resize a table; eventual lost content is deleted; eventual added elements are set to gsNUL
		void	gso_tabdel		( gsObj& a, int pos, int count );	// delete elements from a table; elements are deleted
		void	gso_tabins		( gsObj& a, int pos, int count );	// insert elements in a table; elements are inserted as gsNUL
		void	gso_tabadd		( gsObj& a, int count);				// add elements from at the end of a table; elements are inserted as gsNUL
		gsObj	gso_tabplus		( gsObj& a, gsObj& b );				// concatenate two tables


/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG HELPERS
/////////////////////////////////////////////////////////////////////////////////////////
		char*	gs_nametype		( int type );						// get type's name
		char*	gs_nameop		( int op );							// get op code's name
		void	gs_logobj		( gsObj& a );						// log obj's value

/////////////////////////////////////////////////////////////////////////////////////////
// INLINES
/////////////////////////////////////////////////////////////////////////////////////////
inline gsObj gso_cast( gsObj& a, int type, char* format )
{
	if(type==a.type) return gso_dup(a);
	switch(a.type)
	{
		case GS_INT:
		{
			if(type==GS_FLT) return gsObj((float)a.i); 
			else
			if(type==GS_STR) 
			{
				char sz[128]; sz[127]=0;
				if(format)	_snprintf(sz,127,format,a.i);
				else		_snprintf(sz,127,"%i",a.i);
				return gso_strdup( gsObj(sz,gsstrsize(sz)) );
			}
			break;
		}
		case GS_FLT:
		{
			if(type==GS_INT) return gsObj((int)a.f); 
			else
			if(type==GS_STR) 
			{
				char sz[128]; sz[127]=0;
				if(format)	_snprintf(sz,127,format,a.f);
				else		_snprintf(sz,127,"%f",a.f);
				return gso_strdup( gsObj(sz,gsstrsize(sz)) );
			}
			break;
		}
		case GS_STR:
		{
			if(type==GS_INT) 
			{
				int i=0;
				if(format)	sscanf(a.s,format,&i);
				else		sscanf(a.s,"%i",&i);
				return gsObj(i);
			}
			else
			if(type==GS_FLT)
			{
				float f=0.0f;
				if(format)	sscanf(a.s,format,&f);
				else		sscanf(a.s,"%f",&f);
				return gsObj(f);
			}
			break;
		}
		default:
		{
			if(type==GS_INT) return gsObj(0);
			else
			if(type==GS_FLT) return gsObj(0.0f);
			else
			if(type==GS_STR) return gsObj((char*)NULL,0);
			else
			if(type==GS_PTR) return gsObj((void*)NULL);
		}
	}
	return gsNUL;
}

inline void gso_del( gsObj& a )
{
	if(gso_isstr(a)) 
	{
		if(a.s!=NULL)
		{ 
			gsfree(a.s); 
			a.s = NULL; 
		}
		a.size = 0;
	}
	else
	if(gso_istab(a))
	{
		if(a.o!=NULL)
		{
			for(int i=0;i<a.size;i++) gso_del(a.o[i]);
			gsfree(a.o);
			a.o = NULL;
		}
		a.size = 0;
	}
}

inline gsObj gso_dup( gsObj& a )
{
	if(gso_isstr(a)) return gso_strdup(a);
	if(gso_istab(a)) return gso_tabdup(a);
	return a;
}



inline gsObj gso_strnew( int size )
{
	char* sz = NULL;
	if(size>0)
	{
		sz = (char*)gsmalloc(size+1); sz[size]=0; // safety +1 (not eos)
		sz[0]=0;
	}
	else 
		size = 0;
	return 	gsObj(sz,size);
}

inline gsObj gso_strdup( gsObj& a )
{
	gsassert(a.type==GS_STR);
	gsObj b = gso_strnew(a.size);
	if(a.size>0) memcpy(b.s,a.s,a.size);
	return b;
}

inline gsObj gso_tabnew( int size )
{
	gsObj* o = NULL;
	if(size>0) 
	{
		o = (gsObj*)gsmalloc(size*sizeof(gsObj));
		memset(o,0,size*sizeof(gsObj)); // fill with gsNUL
	}
	else
		size = 0;
	return gsObj(o,size);
}

inline gsObj gso_tabdup( gsObj& a )
{
	gsassert(a.type==GS_TAB);
	gsObj b = gso_tabnew(a.size);
	for(int i=0;i<a.size;i++)
		b.o[i] = gso_dup(a.o[i]);
	return b;
}

inline gsObj* gso_tabi( gsObj& a, int idx )
{
	gsassert(a.type==GS_TAB);
	if(idx<0 || idx>=a.size) return NULL;
	gsassert(a.o!=NULL); // safety
	return &a.o[idx];
}

/////////////////////////////////////////////////////////////////////////////////////////
#endif
