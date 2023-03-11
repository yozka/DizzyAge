#include "stdafx.h"
#include "gsasm.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////
// OBJ TOOLS
/////////////////////////////////////////////////////////////////////////////////////////
void gso_tabresize( gsObj& a, int size )
{
	guard(gso_tabresize)
	int i;
	gsassert(a.type==GS_TAB);
	if(size==a.size) return; // already that size
	if(size<=0) { gso_del(a); return; } // delete all
	// del eventual remains
	for(i=size; i<a.size; i++) gso_del(a.o[i]);
	// realloc
	a.o = (gsObj*)gsrealloc(a.o,size*sizeof(gsObj));
	// fill eventual new ones
	for(i=a.size; i<size; i++) a.o[i]=gsNUL;
	a.size = size;
	unguard()
}

void gso_tabdel( gsObj& a, int pos, int count )
{
	guard(gso_tabdel)
	int i;
	gsassert(a.type==GS_TAB);
	gsassert(0<=pos && pos<a.size);

	// accept invalid counts to delete all to the end
	if(count<0) count=a.size-pos;
	if(pos+count>a.size) count=a.size-pos;
	if(count==0) return; // delete nothing
	if(count==a.size) { gso_del(a); return; } // delete all
	
	// del
	for(i=pos;i<pos+count;i++)
		gso_del(a.o[i]);

	// shift
	for(i=pos+count; i<a.size; i++)
		a.o[i-count] = a.o[i];

	// realloc
	a.o = (gsObj*)gsrealloc(a.o, (a.size-count)*sizeof(gsObj));
	a.size -= count;
	unguard()
}

void gso_tabins( gsObj& a, int pos, int count )
{
	guard(gso_tabins)
	int i;
	gsassert(a.type==GS_TAB);
	gsassert(0<=pos && pos<=a.size);
	if(count<=0) return; // insert nothing

	// realloc
	a.o = (gsObj*)gsrealloc(a.o, (a.size+count)*sizeof(gsObj));
	a.size += count;
	// move
	for(i=a.size-1; i>=pos+count; i--) a.o[i] = a.o[i-count];
	// fill
	for(i=pos; i<pos+count; i++) a.o[i] = gsNUL;
	unguard()
}

void gso_tabadd( gsObj& a, int count )
{
	guard(gso_tabadd)
	gso_tabins(a,a.size,count);
	unguard()
}

gsObj gso_tabplus( gsObj& a, gsObj& b )
{
	guard(gso_tabplus)
	gsassert(a.type==GS_TAB);
	gsassert(b.type==GS_TAB);
	int i;
	gsObj c = gso_tabnew(a.size+b.size);
	for(i=0;i<a.size;i++)
		c.o[i]=gso_dup(a.o[i]);
	for(i=0;i<b.size;i++)
		c.o[a.size+i]=gso_dup(b.o[i]);
	return c;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG HELPERS
/////////////////////////////////////////////////////////////////////////////////////////
char* gs_nametype( int type )
{
	guard(gs_nametype)
	static char sz[8][4] = { "NUL", "INT", "FLT", "STR", "TAB", "REF", "PTR", "???" };
	if(0<=type && type<7) return sz[type];
	return sz[7];
	unguard()
}

char* gs_nameop( int op )
{
	guard(gs_nameop)
	static char sz[GS_MAXOP][16] =
	{
		"NOP", "SET", "GET", "VAR", "SETI", "GETI", "IDX", "NEW", "CAST", "TYPEOF", "SIZEOF",
		"STOP", "BREAKPOINT",
		"POP", "PUSHNUL", "PUSHINT", "PUSHFLT", "PUSHSTR", "PUSHTAB", "PUSHLOC", "PUSHGLB", "PUSHPTR",
		"JMP", "JZ", "JNZ", "CALL", "RET",		
		"NOT", "ANDAND", "OROR", 
		"SGN", "ADD", "SUB", "MUL", "DIV", "MOD", "INC", "DEC",		
		"INV", "AND", "OR", "XOR", "SHR", "SHL", 
		"CPE", "CPNE", "CPL", "CPLE", "CPG", "CPGE"
	};
	if(0<=op && op<GS_MAXOP) return sz[op];
	return NULL;
	unguard()
}

void gs_logobj( gsObj& a )
{
	guard(gso_logobj)
	switch(a.type)
	{
		case GS_NUL:	gslog("NUL"); break;
		case GS_INT:	gslog("%i",a.i); break;
		case GS_FLT:	gslog("%f",a.f); break;
		case GS_STR:	gslog("%s",(a.s!=NULL)?a.s:"VOID"); break;
		case GS_TAB:	gslog("[");
						if(a.o!=NULL)
						{
							for(int i=0;i<a.size;i++) 
							{
								gs_logobj(a.o[i]);
								if(i<a.size-1) gslog(",");
							}
						}
						gslog("]");
						break;
		case GS_REF:	if(a.o!=NULL)
						{
							gslog("@");
							gs_logobj(*a.o);
						}
						else
						{
							gslog("@");
						}
						break;
		case GS_PTR:	if(a.p!=NULL)
							gslog("0x%X",(intptr)a.p); 
						else
							gslog("NULL"); 
						break;
		default:		gslog("<unknown type %i>",a.type);
	}
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
