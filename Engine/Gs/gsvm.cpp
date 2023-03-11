#include "stdafx.h"
#include "gsvm.h"

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////////////
gsVM::gsVM()
{
	guard(gsVM::gsVM)
	m_fp		= -1;
	m_pp		= 0;
	m_sp		= 0;
	m_sb		= 0;
	m_ss		= 0;
	m_stack		= NULL;
	m_cp		= 0;
	m_cb		= 0;
	m_cs		= 0;
	m_call		= NULL;
	m_env		= NULL;
	m_status	= GS_OK;
	m_debug		= GSDBG_LOW;
	m_errhandler= GSErrHandler;
	unguard()
}

void gsVM::Init( int stacksize )
{
	guard(gsVM::Init)
	m_ss	= stacksize;
	m_stack	= (gsObj*)gsmalloc(m_ss * sizeof(gsObj));
	m_cs	= stacksize;
	m_call	= (gsCE*)gsmalloc(m_cs * sizeof(gsCE));
	unguard()
}

void gsVM::Done()
{
	guard(gsVM::Done)
	gsassert( Recover() ); // clear stacks (hopefully to the ground level)
	if(m_stack) { gsfree(m_stack); m_stack=NULL; } 
	if(m_call) { gsfree(m_call); m_call=NULL; }
	m_ss = 0;
	m_cs = 0;
	unguard()
}

BOOL gsVM::Recover()
{
	guard(gsVM::Recover)
	while(m_cp>m_cb) Return();
	gsassert(m_sp==0 && m_sb==0); // safety
	return(m_cp==0);
	unguard()
}

// helper macros
#define BADOPA1( o1 )		Error(GSE_BADARGTYPE, o1.type, 0);
#define BADOPA2( o1, o2 )	Error(GSE_BADARGTYPE, o1.type, o2.type);

/////////////////////////////////////////////////////////////////////////////////////////
// RUN
/////////////////////////////////////////////////////////////////////////////////////////
int	gsVM::Run()
{
	guard(gsVM::Run)
	gsassert(m_env);
	if(m_fp==-1) return GS_OK; // nothing to run

	int i;
	int a,b;
	gsObj A,B,C;
	gsObj* pA;
	m_status = GS_OK;

	while(m_status == GS_OK) // running while no error occure
	{
		gsCode* code = m_env->GetCodeAddr( m_fp, m_pp );
		if(code==NULL) return GS_FAIL; // is this ever reached in normal conditions ?
		int op = code->op;

		#ifdef _DEBUG // used for debug purpose
			int		crt_line = m_env->GetLineInfo( m_fp, m_pp );
			char*	crt_file = m_env->GetFn(m_fp)->m_file;
			char*	crt_func = m_env->GetFn(m_fp)->m_name;
		#endif
		
		switch(op)
		{
			// system
			case GSOP_NOP:
			{
				m_pp++;
				break;
			}
			case GSOP_SET:
			{
				B = Pop();
				A = PopRef();
				if(!gso_isnul(A))
				{
					if(!gso_isnul(B)) // ignore set to NUL
					{
						if(gso_isnul(*A.o) || gso_istype(B, A.o->type)) // check compatibility (allow NUL=enything (for table set))
						{
							gso_del(*A.o); // delete old
							*A.o = B; // set new
						}
						else
						{
							Error(GSE_BADTYPE,A.o->type,B.type);
							gso_del(B); // set failed, don't leak B
						}
					}
				}
				else
				{
					gso_del(B); // set failed, don't leak B
				}
				m_pp++;
				break;
			}
			case GSOP_GET:
			{
				A = PopRef();
				if(!gso_isnul(A))
					Push(gso_dup(*A.o));
				else
					Push(gsNUL);
				m_pp++;
				break;
			}
			case GSOP_VAR:
			{
				A = PopInt();
				if(!gso_isnul(A))
				{
					gsVar* var = m_env->GetVar(A.i);
					if(var!=NULL)
						Push( gsObj(&var->m_obj) );
					else
					{
						Error(GSE_UNKNOWNGLB,A.i,0);
						Push( gsNUL );
					}
				}
				else
					Push(gsNUL);
				m_pp++;
				break;
			}
			case GSOP_SETI:
			{
				C = PopInt();
				B = PopInt();
				A = PopRef(GS_STR);
				if(!gso_isnul(A) && !gso_isnul(B) && !gso_isnul(C))
				{
					i = B.i;
					char chr = (char)C.i;
					if(A.o->s!=NULL && 0<=i && i<A.o->size)
					{
						A.o->s[i] = chr;
					}
					else
					{
						Error(GSE_OUTBOUND,i,A.o->size);
					}
				}
				m_pp++;
				break;
			}
			case GSOP_GETI:
			{
				B = PopInt();
				A = PopRef(GS_STR);
				if(!gso_isnul(A) && !gso_isnul(B))
				{
					i = B.i;
					if(A.o->s!=NULL && 0<=i && i<A.o->size)
					{
						Push( gsObj((int)(A.o->s[i])) );
					}
					else
					{
						Error(GSE_OUTBOUND,i,A.o->size);
						Push(gsNUL);
					}
				}
				else
				{
					Push(gsNUL);
				}
				m_pp++;
				break;
			}	
			case GSOP_IDX:
			{
				B = PopInt();
				A = PopRef(GS_TAB); // ref tab
				if(!gso_isnul(A) && !gso_isnul(B))
				{
					pA = gso_tabi(*A.o,B.i);
					if(pA!=NULL) 
						Push( gsObj(pA) ); // push ref
					else
					{
						Error(GSE_OUTBOUND,B.i,A.o->size);
						Push( gsNUL );
					}
				}
				else
				{
					Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_NEW:
			{
				if( code->i==GS_INT )	Push( gsObj(0) );
				else
				if( code->i==GS_FLT )	Push( gsObj(0.0f) );
				else
				if( code->i==GS_STR || code->i==GS_TAB )
				{
					B = PopInt();
					if(!gso_isnul(B))
					{
						if(code->i==GS_STR) Push(gso_strnew(B.i));
						else
						if(code->i==GS_TAB) Push(gso_tabnew(B.i));
					}
					else
					{
						Push( gsNUL );
					}
				}
				else
				if( code->i==GS_REF )	Push( gsObj((gsObj*)NULL) );
				else
				if( code->i==GS_PTR )	Push( gsObj((void*)NULL) );
				else
					Push( gsNUL );
			
				m_pp++;
				break;
			}
			case GSOP_CAST:
			{
				if(code->i & 128) B = Pop(); else B = gsNUL; // pop extra options if available
				A = Pop();
				if(!gso_isnul(A))
				{
					Push( gso_cast(A, code->i & 127, B.s) );
				}
				else
				{
					Push(gsNUL);
				}
				gso_del(A);
				gso_del(B);
				m_pp++;
				break;
			}
			case GSOP_TYPEOF:
			{
				A = PopRef();
				if(!gso_isnul(A))
				{
					Push( gsObj(A.o->type) );
				}
				else
				{
					Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_SIZEOF:
			{
				A = PopRef();
				if(!gso_isnul(A))
				{
					Push( gsObj(A.o->size) );
				}
				else
				{
					Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_STOP:
			{
				m_status = GSE_STOPPED; // latent stop
				m_pp++;
				break;
			}
			case GSOP_BREAKPOINT:
			{
				__asm int 3
				m_pp++;
				break;
			}

			// stack
			case GSOP_POP:
			{
				A = Pop();
				gso_del(A);
				m_pp++;
				break;
			}
			case GSOP_PUSHNUL:
			{
				Push( gsNUL );
				m_pp++;
				break;
			}
			case GSOP_PUSHINT:
			{
				Push( gsObj(code->i) );
				m_pp++;
				break;
			}
			case GSOP_PUSHFLT:
			{
				Push( gsObj(code->f) );
				m_pp++;
				break;
			}
			case GSOP_PUSHSTR:
			{
				char* sz = GetStr(code->i);
				Push( gso_strdup( gsObj(sz,gsstrsize(sz)) ) );
				m_pp++;
				break;
			}
			case GSOP_PUSHTAB:
			{
				i = code->i;
				A = gso_tabnew(i);
				while(i>0)
				{
					i--;
					A.o[i] = Pop();
				}
				Push(A);
				m_pp++;
				break;
			}
			case GSOP_PUSHLOC:
			{
				if(0<=code->i && m_sb+code->i<m_sp)
					Push( gsObj(&m_stack[m_sb+code->i]) );
				else
				{
					Error(GSE_UNKNOWNLOC,code->i,m_sp-m_sb); // bad error
					Push( gsNUL );
				}
				m_pp++;
				break;
			}
			case GSOP_PUSHGLB:
			{
				gsVar* var = m_env->GetVar(code->i);
				if(var!=NULL)
					Push( gsObj(&var->m_obj) );
				else
				{
					Error(GSE_UNKNOWNGLB,A.i,0); // bad error
					Push( gsNUL );
				}
				m_pp++;
				break;
			}
			case GSOP_PUSHPTR:
			{
				Push( gsObj((void*)NULL) );
				m_pp++;
				break;
			}

			// jumps
			case GSOP_JMP:
			{
				m_pp = code->i;
				break;
			}
			case GSOP_JZ:
			{
				a = PopBool();
				if(!a) 
					m_pp = code->i;
				else 			 
					m_pp++;
				break;
			}
			case GSOP_JNZ:
			{
				a = PopBool();
				if(a) 
					m_pp = code->i;
				else 			 
					m_pp++;
				break;
			}
			case GSOP_CALL:
			{
				A = PopInt();
				if(!gso_isnul(A))
				{
					int ret = Call(A.i,code->i);
					if(ret!=GS_OK) return ret;
				}
				else
					m_pp++;
				break;
			}
			case GSOP_RET:
			{
				A = gsNUL;
				if(code->i) A = Pop();
				Return();
				Push(A); // always push something
				if(m_cp==m_cb) return GS_OK; // done
				break;
			}
			
			// logic (int)
			case GSOP_NOT:
			{
				a = PopBool();
				Push( a ? gsObj(0) : gsObj(1) );
				m_pp++;
				break;
			}
			case GSOP_ANDAND:
			{
				b = PopBool();
				a = PopBool();
				Push( gsObj(a && b) ); 
				m_pp++;
				break;
			}
			case GSOP_OROR:
			{
				b = PopBool();
				a = PopBool();
				Push( gsObj(a || b) );
				m_pp++;
				break;
			}

			// aritmetic (int, flt, str)
			case GSOP_SGN:
			{
				A = Pop();
				if(gso_isint(A))	
					Push( gsObj(-A.i) ); 
				else
				if(gso_isflt(A))	
					Push( gsObj(-A.f) ); 
				else
				{
					BADOPA1(A);
					gso_del(A); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_ADD:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B))	
					Push( gsObj(A.i + B.i) ); 
				else
				if(gso_isflt(A) && gso_isflt(B))	
					Push( gsObj(A.f + B.f) ); 
				else
				if(gso_isstr(A) && gso_isstr(B))
				{
					char* sz=gsstradd(A.s, B.s);
					Push( gsObj(sz, gsstrsize(sz)) );
					gso_del(A);
					gso_del(B);
				}
				else
				if(gso_istab(A) && gso_istab(B))
				{
					Push( gso_tabplus(A, B) );
					gso_del(A);
					gso_del(B);
				}
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_SUB:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B))	
					Push( gsObj(A.i - B.i) ); 
				else
				if(gso_isflt(A) && gso_isflt(B))	
					Push( gsObj(A.f - B.f) );
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_MUL:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B))	
					Push( gsObj(A.i * B.i) ); 
				else
				if(gso_isflt(A) && gso_isflt(B))	
					Push( gsObj(A.f * B.f) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_DIV:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B))
				{
					if(B.i!=0)	
						Push( gsObj(A.i / B.i) );
					else	
					{
						Error(GSE_DIVIDEBYZERO);
						Push(gsNUL);
					}
				}
				else
				if(gso_isflt(A) && gso_isflt(B))
				{
					if(B.f!=0.0f)	
						Push( gsObj(A.f / B.f) );
					else	
					{
						Error(GSE_DIVIDEBYZERO);
						Push(gsNUL);
					}
				}
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_MOD:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B))
				{
					if(B.i!=0)		
						Push( gsObj(A.i % B.i) );
					else			
					{
						Error(GSE_DIVIDEBYZERO);
						Push(gsNUL);
					}
				}
				else
				if(gso_isflt(A) && gso_isflt(B))
				{
					if(B.f!=0.0f)	
						Push( gsObj((float)fmod(A.f, B.f)) );
					else		
					{
						Error(GSE_DIVIDEBYZERO);
						Push(gsNUL);
					}
				}
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_INC:
			{
				A = Pop();
				if(gso_isint(A))	
					Push( gsObj(A.i+1) ); 
				else
				if(gso_isflt(A))	
					Push( gsObj(A.f+1.0f) ); 
				else
				{
					BADOPA1(A);
					gso_del(A); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_DEC:
			{
				A = Pop();
				if(gso_isint(A))	
					Push( gsObj(A.i-1) ); 
				else
				if(gso_isflt(A))	
					Push( gsObj(A.f-1.0f) ); 
				else
				{
					BADOPA1(A);
					gso_del(A); Push(gsNUL);
				}
				m_pp++;
				break;
			}

			// binar
			case GSOP_INV:
			{
				A = Pop();
				if(gso_isint(A)) 
					Push( gsObj(~(dword)A.i) ); 
				else
				{
					BADOPA1(A);
					gso_del(A); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_AND:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( gsObj((dword)A.i & (dword)B.i) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_OR:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( gsObj((dword)A.i | (dword)B.i) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_XOR:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( gsObj((dword)A.i ^ (dword)B.i) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_SHR:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( gsObj((dword)A.i >> (dword)B.i) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_SHL:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( gsObj((dword)A.i << (dword)B.i) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}

			// compare
			case GSOP_CPE:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( (A.i==B.i) ? gsObj(1) : gsObj(0) ); 
				else
				if(gso_isflt(A) && gso_isflt(B)) 
					Push( (A.f==B.f) ? gsObj(1) : gsObj(0) ); 
				else
				if(gso_isstr(A) && gso_isstr(B))
				{
					Push( (gsstrcmp(A.s,B.s)==0) ? gsObj(1) : gsObj(0) ); 
					gso_del(A); gso_del(B);
				}
				else
				if(gso_isptr(A) && gso_isptr(B)) 
					Push( (A.p==B.p) ? gsObj(1) : gsObj(0) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_CPNE:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( (A.i!=B.i) ? gsObj(1) : gsObj(0) ); 
				else
				if(gso_isflt(A) && gso_isflt(B)) 
					Push( (A.f!=B.f) ? gsObj(1) : gsObj(0) ); 
				else
				if(gso_isstr(A) && gso_isstr(B))
				{
					Push( (gsstrcmp(A.s,B.s)!=0) ? gsObj(1) : gsObj(0) ); 
					gso_del(A); gso_del(B);
				}
				else
				if(gso_isptr(A) && gso_isptr(B)) 
					Push( (A.p!=B.p) ? gsObj(1) : gsObj(0) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_CPL:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( (A.i<B.i) ? gsObj(1) : gsObj(0) ); 
				else
				if(gso_isflt(A) && gso_isflt(B)) 
					Push( (A.f<B.f) ? gsObj(1) : gsObj(0) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_CPLE:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( (A.i<=B.i) ? gsObj(1) : gsObj(0) ); 
				else
				if(gso_isflt(A) && gso_isflt(B)) 
					Push( (A.f<=B.f) ? gsObj(1) : gsObj(0) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_CPG:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( (A.i>B.i) ? gsObj(1) : gsObj(0) ); 
				else
				if(gso_isflt(A) && gso_isflt(B)) 
					Push( (A.f>B.f) ? gsObj(1) : gsObj(0) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
			case GSOP_CPGE:
			{
				B = Pop();
				A = Pop();
				if(gso_isint(A) && gso_isint(B)) 
					Push( (A.i>=B.i) ? gsObj(1) : gsObj(0) ); 
				else
				if(gso_isflt(A) && gso_isflt(B)) 
					Push( (A.f>=B.f) ? gsObj(1) : gsObj(0) ); 
				else
				{
					BADOPA2(A,B);
					gso_del(A); gso_del(B); Push(gsNUL);
				}
				m_pp++;
				break;
			}
		
			default:
			{
				Error(GSE_UNKNOWNOP,op,0); // bigbugu
				return GSE_UNKNOWNOP;
			}
		}
	}

	return m_status;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
void gsVM::Error( int err, int param1, int param2, char* text )
{
	guard(gsVM::Error)
	m_status = err;
	if(m_errhandler==NULL) return;
	int line = 0;
	char* file = NULL;
	if(m_env)
	{
		line = m_env->GetLineInfo(m_fp, m_pp);
		if(m_env->GetFn(m_fp)) file = m_env->GetFn(m_fp)->m_file;
	}
	m_errhandler( this, gsErrInfo(err, param1, param2, text, line, file) ); 
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
