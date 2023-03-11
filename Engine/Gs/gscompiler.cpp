#include "stdafx.h"
#include "gscompiler.h"
#include "gs.h"

/////////////////////////////////////////////////////////////////////////////////////////
// DEFINES
/////////////////////////////////////////////////////////////////////////////////////////
#define GS_GOTOTAB_GROW		128 //16
#define GS_SYMTAB_GROW		128 //32
#define GS_LINKTAB_GROW		128 //32
#define GS_STRMAX_GROW		128 //32
#define GS_CODEMAX_GROW		128 //64
#define GS_LINEMAX_GROW		128 //64

#define GS_JMP_BREAK		-10000
#define GS_JMP_CONTINUE		-10001

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////////////

gsCompiler::gsCompiler()
{
	guard(gsCompiler::gsCompiler)
	m_mode		= GSC_NONE;
	m_fn		= -1;
	m_codemax	= 0;
	m_strmax	= 0;
	m_linemax	= 0;
	m_symmax	= 0;
	m_symcnt	= 0;
	m_symtab	= NULL;
	m_symhash	= NULL;
	m_gotomax	= 0;
	m_gotocnt	= 0;
	m_gototab	= NULL;
	m_vm		= NULL;
	m_env		= NULL;
	m_line		= 0;
	m_file		= NULL;
	unguard()
}

gsCompiler::~gsCompiler()
{
	guard(gsCompiler::~gsCompiler)
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES
/////////////////////////////////////////////////////////////////////////////////////////
int	gsCompiler::VarFind( char* name )
{
	guard(gsCompiler::VarFind)
	gsassert(m_env!=NULL);
	return m_env->FindVar(name);
	unguard()
}

int gsCompiler::VarAdd( char* name, int type )
{
	guard(gsCompiler::VarAdd)
	gsassert(m_env!=NULL);
	int i = m_env->AddVar(name);
	if(i==-1) return -1;
	gsVar* var = m_env->GetVar(i);
	var->m_flags = 0;
	var->m_obj.type = type;
	return i;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////
int	gsCompiler::FnFind( char* name )
{
	guard(gsCompiler::FnFind)
	gsassert(m_env!=NULL);
	return m_env->FindFn(name);
	unguard()
}

int gsCompiler::FnAdd( char* name )
{
	guard(gsCompiler::FnAdd)
	gsassert(m_env!=NULL);
	return m_env->AddFn(name);
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// STRINGS
/////////////////////////////////////////////////////////////////////////////////////////
int	gsCompiler::StrAdd( char* str )
{
	guard(gsCompiler::StrAdd)
	gsassert(m_env!=NULL);
	
	gsFn* fn = GetFn(); if(fn==NULL) return -1;
	
	if(fn->m_strsize==m_strmax) // realloc
	{
		m_strmax += GS_STRMAX_GROW;
		fn->m_str = (char**)gsrealloc(fn->m_str, m_strmax*sizeof(char**));
	}

	fn->m_str[fn->m_strsize] = str;
	fn->m_strsize++;
	return fn->m_strsize-1;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// SYMBOL TABLE
/////////////////////////////////////////////////////////////////////////////////////////
void gsCompiler::SymDestroy()
{
	guard(gsCompiler::SymDestroy)
	m_symmax = 0;
	m_symcnt = 0;
	if(m_symhash)	{ gsdelete(m_symhash); m_symhash=NULL; }
	if(m_symtab)	{ gsfree(m_symtab); m_symtab=NULL; }
	unguard()
}

void gsCompiler::SymClear()
{
	guard(gsCompiler::SymClear)
	m_symcnt = 0;
	if(m_symhash) m_symhash->Reset();
	unguard()
}

int gsCompiler::SymFind( char* name )
{
	guard(gsCompiler::SymFind)
	gsassert(name && name[0]);
	if(m_symcnt==0) return -1;
	int i=-1;
	if( !m_symhash->Find(name,i) ) return -1; // hash find
	gsassert( i!=-1 && i<m_symcnt ); // safety test (consistency)
	return i;
	unguard()
}

int	gsCompiler::SymAdd( char* name, int type )
{
	guard(gsCompiler::SymAdd)
	gsassert(name && name[0]);
	int i = SymFind(name);
	if(i==-1) // new
	{
		if(m_symcnt==m_symmax)
		{
			m_symmax += GS_SYMTAB_GROW;
			m_symtab = (gscSym*)gsrealloc(m_symtab, m_symmax*sizeof(gscSym));
		}
		i=m_symcnt;
		m_symcnt++;
	}

	m_symtab[i].m_name = name;
	m_symtab[i].m_type = type;

	if(m_symhash==NULL) m_symhash = gsnew gsHash(); // create hash
	BOOL ret = m_symhash->Add(name,i); // hash add
	gsassert(ret); // safety

	return i;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// GOTO TABLE
/////////////////////////////////////////////////////////////////////////////////////////
void gsCompiler::GotoDestroy()
{
	guard(gsCompiler::GotoDestroy)
	m_gotomax = 0;
	m_gotocnt = 0;
	if(m_gototab!=NULL) { gsfree(m_gototab); m_gototab=NULL; }
	unguard()
}

void gsCompiler::GotoClear()
{
	guard(gsCompiler::GotoClear)
	m_gotocnt = 0;
	unguard()
}

int gsCompiler::GotoFind( char* name )
{
	guard(gsCompiler::GotoFind)
	if(m_gotocnt==0) return -1;

	for(int i=0;i<m_gotocnt;i++)
	{
		if( strcmp(m_gototab[i].m_name,name)==0 ) return i;
	}

	return -1;
	unguard()
}

int	gsCompiler::GotoAdd( char* name, int addr, int line )
{
	guard(gsCompiler::GotoAdd)
	if(m_gotocnt==m_gotomax)
	{
		m_gotomax += GS_GOTOTAB_GROW;
		m_gototab = (gscGoto*)gsrealloc(m_gototab, m_gotomax*sizeof(gscGoto));
	}

	m_gototab[m_gotocnt].m_name = name;
	m_gototab[m_gotocnt].m_addr = addr;
	m_gototab[m_gotocnt].m_line = line;
	m_gotocnt++;

	return m_gotocnt-1;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// CODE OPS
/////////////////////////////////////////////////////////////////////////////////////////
int gsCompiler::Code( gsCode& cod )
{
	guard(gsCompiler::Code)
	gsassert(m_env!=NULL);

	gsFn* fn = GetFn(); 
	if(fn==NULL) return -1;
	
	if(fn->m_codesize==m_codemax) // realloc
	{
		m_codemax += GS_CODEMAX_GROW;
		fn->m_code = (gsCode*)gsrealloc(fn->m_code, m_codemax*sizeof(gsCode));
	}

	fn->m_code[fn->m_codesize] = cod;
	fn->m_codesize++;
	
	LineInfo( fn->m_codesize-1, m_line );

	return fn->m_codesize-1;
	unguard()
}

int gsCompiler::LineInfo( int addr, int line )
{
	guard(gsCompiler::LineInfo)
	gsassert(m_env!=NULL);

	gsFn* fn = GetFn(); 
	if(fn==NULL) return -1;

	if(fn->m_linesize>0)
	{
		int ln = fn->m_line[fn->m_linesize-1] >> 16;
		if(ln==line) return -1; // same line
	}
	
	if(fn->m_linesize==m_linemax) // realloc
	{
		m_linemax += GS_LINEMAX_GROW;
		fn->m_line = (int*)gsrealloc(fn->m_line, m_linemax*sizeof(int));
	}

	fn->m_line[fn->m_linesize] = (int)(addr | (line<<16));
	fn->m_linesize++;
	return fn->m_linesize-1;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// COMPILING FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////
int	gsCompiler::CompileProgram( gsVM* vm, gsNode* tree )
{
	guard(gsCompiler::CompileProgram)
	gsassert(vm!=NULL);
	gsassert(vm->m_env!=NULL);
	gsassert(tree!=NULL);
	gsassert(m_mode==GSC_NONE); // already must not be compile mode
	int v,f;
	gsNode* n;

	m_mode = GSC_PROGRAM;
	m_vm = vm;
	m_env = vm->m_env;

	int		ret = GS_OK;
	BOOL	abort = FALSE;
	gsNode* node = tree;

	while(node!=NULL && ret==GS_OK && !abort)
	{
		m_line = node->m_line;
		switch(node->m_type)
		{
			case GSNODE_NONE: break;
			case GSNODE_DECVAR:
			{
				// idd list
				n = node->m_child[0];
				while(n!=NULL)
				{
					gsassert(n->m_type==GSNODE_IDD);
					f=FnFind(n->m_str);	if( f!=-1 ) { Error(GSE_REDECLARATION, 1, 2, n->m_str); return GSE_REDECLARATION; } // exists as function
					v=VarFind(n->m_str); 
					if( v!=-1 )
					{
						int t=VarGet(v)->m_obj.type;
						if( t!=GS_NUL && t!=node->m_data ) // redeclaration (exists non-NUL)
						{ 
							Error(GSE_REDECLARATION, 1, 1, n->m_str); 
							return GSE_REDECLARATION; 
						}
					}
					v=VarAdd(n->m_str, node->m_data); // declaration
					gsassert(v!=-1); 
					n = n->m_next;
				}
				break;
			}
			case GSNODE_DECFN:
			{
				if(node->m_str==NULL) // empty declaration - idd list
				{
					n = node->m_child[0];
					while(n!=NULL)
					{
						gsassert(n->m_type==GSNODE_IDD);
						v=VarFind(n->m_str);	if( v!=-1 ) { Error(GSE_REDECLARATION, 2, 1, n->m_str); ret = GSE_REDECLARATION; break; } // exists as global
						f=FnFind(n->m_str);
						if( f==-1 )
						{
							f = FnAdd(n->m_str); // declaration
							gsassert(f!=-1);
						}
						n = n->m_next;
					}
					break;
				}

				// full declaration
				v=VarFind(node->m_str);	if( v!=-1 ) { Error(GSE_REDECLARATION, 2, 1, node->m_str); ret = GSE_REDECLARATION; break; } // exists as global
				f=FnFind(node->m_str);
				if( f!=-1 && m_env->IsValidFn(f) ) // redeclaration
				{
					Error(GSE_REDECLARATION, 2, 2, node->m_str); 
					ret = GSE_REDECLARATION; 
					break; 
				}
				m_fn = FnAdd(node->m_str); // declaration
				gsassert(m_fn!=-1);
				gsFn* fn = GetFn();
				fn->m_flags = GSFN_SCRIPTED;

				m_codemax = 0;
				m_strmax = 0;
				m_linemax = 0;

				// idd list; must not be duplicates (allow same as globals or functions)
				n = node->m_child[0];
				while(n!=NULL)
				{
					gsassert(n->m_type==GSNODE_IDD);
					f=FnFind(n->m_str); if( f!=-1 ) { Error(GSE_REDECLARATION, 0, 2, n->m_str); ret = GSE_REDECLARATION; break; } // exists as function
					v=VarFind(n->m_str); if( v!=-1 ) { Error(GSE_REDECLARATION, 0, 1, n->m_str); ret = GSE_REDECLARATION; break; } // exists as global
					v=SymFind(n->m_str); if( v!=-1 ) { Error(GSE_REDECLARATION, 0, 0, n->m_str); ret = GSE_REDECLARATION; break; } // exists as local (probably duplicate param)
					v=SymAdd(n->m_str); gsassert(v!=-1); // declaration
					fn->m_parsize++;
					n = n->m_next;
				}
				if(ret==GS_OK)
				{
					fn->m_file = gsstrdup(m_file); // processed file is set as debug info
					ret = CompileCode( node->m_child[1] );
				}
				else // clear this fn
				{
					m_env->DelFn(m_fn);
					m_fn = -1;
					SymClear();
				}
				break;
			}
			
			default:
			{
				ret = CompileDir(node, abort);
			}
		}

		node = node->m_next;
	}

	// close
	SymDestroy();
	GotoDestroy();
	m_env	= NULL;
	m_vm	= NULL;
	m_mode	= GSC_NONE;
	
	return ret;
	unguard()
}

int	gsCompiler::CompileFunction( gsVM* vm, gsNode* tree, int& fn )
{
	guard(gsCompiler::CompileFunction)
	gsassert(vm!=NULL);
	gsassert(vm->m_env!=NULL);
	gsassert(tree!=NULL);
	gsassert(m_mode==GSC_NONE); // already must not be compile mode

	fn = -1;
	m_mode = GSC_FUNCTION;
	m_vm = vm;
	m_env = vm->m_env;

	// create temp function
	static int fn_count = 0;
	char fn_name[32];
	sprintf(fn_name, "__fn%i",fn_count);
	fn_count++;

	m_fn = FnAdd(fn_name); gsassert(m_fn!=-1); // declaration
	m_codemax = 0;
	m_strmax = 0;
	m_linemax = 0;

	GetFn()->m_flags = GSFN_SCRIPTED | GSFN_TEMPORARY;

	int ret = CompileCode( tree ); // compile code content

	// close
	SymDestroy();
	GotoDestroy();
	m_env	= NULL;
	m_vm	= NULL;
	m_mode	= GSC_NONE;

	fn = m_fn;
	return ret;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// COMPILATION UTILS
/////////////////////////////////////////////////////////////////////////////////////////
int gsCompiler::CompileDir( gsNode* node, BOOL& abort )
{
	guard(gsCompiler::CompileDir)
	int v,f;
	abort = FALSE;
	int ret;
	
	if(node==NULL) return GS_OK;

	m_line = node->m_line;
	switch(node->m_type)
	{
		case GSNODE_DEFINE:
		{
			f=FnFind(node->m_str);	if( f!=-1 ) { Error(GSE_REDECLARATION, 1, 2, node->m_str); return GSE_REDECLARATION; } // exists as function
			v=VarFind(node->m_str); 
			if( v!=-1 )
			{
				int t1=VarGet(v)->m_obj.type;
				int t2=node->m_child[0]->m_data;
				gsObj* obj = &VarGet(v)->m_obj;
				// nothing more to set
				if(t2==GS_NUL) break;
				// redeclaration error if found as non-NUL and types doesn't match
				if( t1!=GS_NUL && t1!=t2 )
				{ 
					Error(GSE_REDECLARATION, 1, 1, node->m_str); 
					return GSE_REDECLARATION;
				}
				// apply type, in case it was NUL
				obj->type = t2;
				// overwrite new value
				if(t2==GS_INT) obj->i = node->m_child[0]->m_int;
				if(t2==GS_FLT) obj->f = node->m_child[0]->m_flt;
				if(t2==GS_STR) 
				{
					if(obj->s!=NULL) gsfree(obj->s); // free old
					obj->s = node->m_child[0]->m_str;
					obj->size = gsstrsize(obj->s);
					node->m_child[0]->m_str = NULL; // prevent deletion
				}
				break;
			}
			v=VarAdd(node->m_str, node->m_child[0]->m_data); gsassert(v!=-1); // declaration
			VarGet(v)->m_flags = GSVAR_DEFINE;
			gsObj* obj = &VarGet(v)->m_obj;
			if(obj->type==GS_INT) obj->i = node->m_child[0]->m_int;
			if(obj->type==GS_FLT) obj->f = node->m_child[0]->m_flt;
			if(obj->type==GS_STR) 
			{
				obj->s = node->m_child[0]->m_str;
				obj->size = gsstrsize(obj->s);
				node->m_child[0]->m_str = NULL; // prevent deletion
			}
			break;
		}
		case GSNODE_IFDEF:
		{
			v=VarFind(node->m_str); 
			if( v!=-1 )
			{
				ret = CompileDir(node->m_child[0],abort);
				if(ret!=GS_OK) return ret;
			}
			break;
		}
		case GSNODE_IFNDEF:
		{
			v=VarFind(node->m_str); 
			if( v==-1 )	
			{
				ret = CompileDir(node->m_child[0],abort);
				if(ret!=GS_OK) return ret;
			}
			break;
		}
		case GSNODE_INCLUDE:
		{
			ret = gs_compilefile(m_vm, node->m_str);
			if(ret!=GS_OK) return ret;
			break;
		}
		case GSNODE_ABORT:
		{
			abort = TRUE;
			break;
		}
		case GSNODE_SCOPE:
		{
			m_env->m_scope = node->m_int;
			break;
		}
		default:
		{
			Error(GSE_UNKNOWNNODE, node->m_type, 0); // unknown node
			return GSE_UNKNOWNNODE;
		}
	}
	return GS_OK;
	unguard()
}

int gsCompiler::CompileCode( gsNode* node )
{
	guard(gsCompiler::CompileCode)
	gsFn* fn = GetFn(); gsassert(fn!=NULL);
	m_line = 1;

	int ret = CompileNodes(node);
	if( ret==GS_OK )
	{
		ret = LinkGoto(); // link function's code
		if( ret==GS_OK )
		{
			int a=GetCodePos();
			// if(a==0 || GetCode(a-1)->op!=GSOP_RET)
			CodeInt(GSOP_RET,0); // must always insert op_ret !

			// local symbols
			fn->m_localsize = m_symcnt;
		}
	}

	if(ret!=GS_OK) // if failed to compile we delete the current function !
	{
		m_env->DelFn(m_fn);
		m_fn = -1;
	}

	SymClear();
	GotoClear();

	return ret;
	unguard()
}

int gsCompiler::CompileNodes( gsNode* node )
{
	guard(gsCompiler::CompileNodes)
	int a,b,c;
	gsNode* n;
	int ret;

	while(node!=NULL)
	{
		m_line = node->m_line;
		switch(node->m_type)
		{
			case GSNODE_NONE: break;
			case GSNODE_CALL:
			{
				ret = CompileCall(node);
				if(ret!=GS_OK) return ret;
				break;
			}	
			case GSNODE_RET:
			{
				if(node->m_child[0]) 
				{
					ret = CompileExp(node->m_child[0]);
					if(ret!=GS_OK) return ret;
				}
				CodeInt( GSOP_RET, node->m_data );
				break;
			}
			case GSNODE_ASG0:
			{
				n = node->m_child[0]; gsassert(n!=NULL); 
				ret = CompileExp(n); // reference
				if(ret!=GS_OK) return ret;
				n = node->m_child[1]; gsassert(n!=NULL);
				ret = CompileExp(n); // value
				if(ret!=GS_OK) return ret;
				CodeInt( GSOP_SET, 0 );
				break;
			}
			case GSNODE_ASG1:
			{
				n = node->m_child[0]; gsassert(n!=NULL);
				ret = CompileExp(n); // reference
				if(ret!=GS_OK) return ret;
				ret = CompileExp(n); // reference
				if(ret!=GS_OK) return ret;
				CodeInt( GSOP_GET, 0 ); // value
				CodeInt( node->m_data, 0 ); // op1
				CodeInt( GSOP_SET, 0 );
				break;
			}
			case GSNODE_ASG2:
			{
				n = node->m_child[0]; gsassert(n!=NULL);
				ret = CompileExp(n); // reference
				if(ret!=GS_OK) return ret;
				ret = CompileExp(n); // reference
				if(ret!=GS_OK) return ret;
				CodeInt( GSOP_GET, 0 ); // value
				n = node->m_child[1]; gsassert(n!=NULL);
				ret = CompileExp(n); // value
				if(ret!=GS_OK) return ret;
				CodeInt( node->m_data, 0 ); // op2
				CodeInt( GSOP_SET, 0 );
				break;
			}
			case GSNODE_ASG3:
			{
				n = node->m_child[0]; gsassert(n!=NULL);
				ret = CompileExp(n); // reference
				if(ret!=GS_OK) return ret;
				n = node->m_child[1]; gsassert(n!=NULL);
				ret = CompileExp(n); // value
				if(ret!=GS_OK) return ret;
				n = node->m_child[2]; gsassert(n!=NULL);
				ret = CompileExp(n); // value
				if(ret!=GS_OK) return ret;
				CodeInt( node->m_data, 0 ); // op3
				break;
			}
			case GSNODE_COD0:
			{
				CodeInt( node->m_data, 0 );
				break;
			}
			case GSNODE_BLOCK:
			{
				n = node->m_child[0];
				if(n!=NULL)	
				{
					ret = CompileNodes(n);
					if(ret!=GS_OK) return ret;
				}
				break;
			}
			case GSNODE_IF:
			{
				BOOL have_else = (node->m_child[2]!=NULL);
				n = node->m_child[0]; gsassert(n!=NULL);
				ret = CompileExp(n);
				if(ret!=GS_OK) return ret;
				a = CodeInt(GSOP_JZ,-1); // unknown jump
				ret = CompileNodes(node->m_child[1]); // compile TRUE code
				if(ret!=GS_OK) return ret;
				if(have_else) b = CodeInt(GSOP_JMP,-1); // unknown jump
				GetCode(a)->i = GetCodePos(); // set jump pos
				if(have_else) 
				{
					ret = CompileNodes(node->m_child[2]); // compile FALSE code (else)
					if(ret!=GS_OK) return ret;
					GetCode(b)->i = GetCodePos(); // set jump pos
				}
				break;
			}
			case GSNODE_DO:
			{
				a = GetCodePos();
				ret = CompileNodes(node->m_child[0]);
				if(ret!=GS_OK) return ret;
				b = GetCodePos();
				ret = CompileExp(node->m_child[1]);
				if(ret!=GS_OK) return ret;
				CodeInt(GSOP_JNZ,a);
				Link(a,b-1,GetCodePos(),b);
				break;
			}
			case GSNODE_WHILE:
			{
				a = GetCodePos();
				ret = CompileExp(node->m_child[0]);
				if(ret!=GS_OK) return ret;
				b = CodeInt(GSOP_JZ,-1); // unknown jump
				ret = CompileNodes(node->m_child[1]);
				if(ret!=GS_OK) return ret;
				CodeInt(GSOP_JMP,a);
				GetCode(b)->i = GetCodePos(); // set jump pos
				Link(b+1,GetCodePos()-2,GetCodePos(),a);
				break;
			}
			case GSNODE_FOR:
			{
				ret = CompileNodes(node->m_child[0]); // init
				if(ret!=GS_OK) return ret;
				a = GetCodePos();
				ret = CompileExp(node->m_child[1]); // condition
				if(ret!=GS_OK) return ret;
				b = CodeInt(GSOP_JZ,-1); // unknown jump
				ret = CompileNodes(node->m_child[3]); // code
				if(ret!=GS_OK) return ret;
				c = GetCodePos();
				ret = CompileNodes(node->m_child[2]); // increment
				if(ret!=GS_OK) return ret;
				CodeInt(GSOP_JMP,a);
				GetCode(b)->i = GetCodePos(); // set jump pos
				Link(b+1,c-1,GetCodePos(),c);
				break;
			}
			case GSNODE_BREAK:
			{
				CodeInt(GSOP_JMP,GS_JMP_BREAK); // special jump to be linked later as break
				break;
			}
			case GSNODE_CONTINUE:
			{
				CodeInt(GSOP_JMP,GS_JMP_CONTINUE); // special jump to be linked later as continue
				break;
			}
			case GSNODE_GOTO:
			{
				int i = GotoFind(node->m_str);
				if(i!=-1) // found
				{
					if(GotoGet(i)->m_addr>=0) // valid addr
					{
						CodeInt(GSOP_JMP, GotoGet(i)->m_addr);
					}
					else // invalid addr
					{
						CodeInt(GSOP_JMP, -i-1); // special jump to be linked later as goto
					}
				}
				else // not found
				{
					i = GotoAdd(node->m_str,-1, m_line); // add with invalid addr
					CodeInt(GSOP_JMP, -i-1); // special jump to be linked later as goto
				}
				break;
			}
			case GSNODE_LABEL:
			{
				int i = GotoFind(node->m_str);
				if(i!=-1) // found
				{
					if(GotoGet(i)->m_addr>=0) // valid addr -> error 
					{
						Error(GSE_LABELEXISTS, node->m_str); 
						return GSE_LABELEXISTS; 
					}
					else // invalid addr (added by a goto command)
					{
						GotoGet(i)->m_addr = GetCodePos(); // set correct addr
					}
				}
				else // not found
				{
					GotoAdd(node->m_str, GetCodePos(), m_line); // add with correct addr
				}
				break;
			}

			default:
			{
				Error(GSE_UNKNOWNNODE,node->m_type); // unknown node
				return GSE_UNKNOWNNODE;
			}
		}

		node = node->m_next;
	}

	return GS_OK;
	unguard()
}

int gsCompiler::CompileExp( gsNode* node )
{
	guard(gsCompiler::CompileExp)
	int i,v,f;
	int a,b;
	int ret;
	gsNode* n;
	gsassert(node!=NULL); // node must be valid

	m_line = node->m_line;
	switch(node->m_type)
	{
		case GSNODE_CALL:
		{
			ret = CompileCall(node);
			if(ret!=GS_OK) return ret;
			break;
		}	
		case GSNODE_OP2:
		{
			n = node->m_child[0]; gsassert(n!=NULL); // expr
			ret = CompileExp(n);
			if(ret!=GS_OK) return ret;
			n = node->m_child[1]; gsassert(n!=NULL); // right op
			ret = CompileExp(n);
			if(ret!=GS_OK) return ret;
			CodeInt(node->m_data,0);
			break;
		}
		case GSNODE_OP1:
		{
			n = node->m_child[0]; gsassert(n!=NULL); // expr
			ret = CompileExp(n);
			if(ret!=GS_OK) return ret;
			CodeInt(node->m_data,0);
			break;
		}
		case GSNODE_OBJ:
		{
			if(node->m_data==GS_NUL)	CodeInt(GSOP_PUSHNUL,0);
			else
			if(node->m_data==GS_INT)	CodeInt(GSOP_PUSHINT,node->m_int);
			else
			if(node->m_data==GS_FLT)	CodeFlt(GSOP_PUSHFLT,node->m_flt);
			else
			if(node->m_data==GS_STR)	
			{
				CodeInt(GSOP_PUSHSTR,StrAdd(node->m_str));
				node->m_str = NULL; // prevent deletion
			}
			else
			if(node->m_data==GS_TAB)
			{
				// push all members
				n = node->m_child[0];
				i = 0;
				while(n!=NULL)
				{
					ret = CompileExp(n);
					if(ret!=GS_OK) return ret;
					n = n->m_next;
					i++;
				}
				CodeInt(GSOP_PUSHTAB,i);
			}
			else
			if(node->m_data==GS_PTR)	CodeInt(GSOP_PUSHPTR,0);
			break;
		}
		case GSNODE_NEW:
		{
			if(node->m_data==GS_STR || node->m_data==GS_TAB)
			{
				ret = CompileExp(node->m_child[0]);
				if(ret!=GS_OK) return ret;
			}
			CodeInt(GSOP_NEW,node->m_data);
			break;
		}
		case GSNODE_VAR:
		{
			if(node->m_str!=NULL) // normal addressing (by name)
			{
				// local
				v=SymFind(node->m_str);
				if(v!=-1)
				{
					CodeInt( GSOP_PUSHLOC, v );
					break;
				}

				// global
				v=VarFind(node->m_str);
				if(v!=-1)
				{
					CodeInt( GSOP_PUSHGLB, v );
					break;
				}

				// add local
				f=FnFind(node->m_str);	if( f!=-1 ) { Error(GSE_REDECLARATION, 0, 2, node->m_str); return GSE_REDECLARATION; } // exists as function
				v=SymAdd(node->m_str,GS_NUL); gsassert(v!=-1);
				CodeInt( GSOP_PUSHLOC, v ); 
				break; 
			}
			else // dynamic addressing (by idd)
			{
				ret = CompileExp(node->m_child[0]); // global idx
				if(ret!=GS_OK) return ret;
				CodeInt( GSOP_VAR, 0 ); // get global ref
			}
			break;
		}
		case GSNODE_IDX: 
		{
			ret = CompileExp(node->m_child[0]); // push reference
			if(ret!=GS_OK) return ret;
			ret = CompileExp(node->m_child[1]); // push value
			if(ret!=GS_OK) return ret;
			CodeInt( GSOP_IDX, 0 );
			break;
		}
		case GSNODE_CAST:
		{
			ret = CompileExp(node->m_child[0]); // expr
			if(ret!=GS_OK) return ret;
			if(node->m_str!=NULL) // extra options
			{
				CodeInt(GSOP_PUSHSTR,StrAdd(node->m_str));
				node->m_str = NULL; // prevent deletion
				CodeInt( GSOP_CAST, node->m_data | 128 ); // use extra bit for extra options
			}
			else
			{
				CodeInt( GSOP_CAST, node->m_data );
			}
			break;
		}
		case GSNODE_IFEXP:
		{
			ret = CompileExp(node->m_child[0]); // exp cmp
			if(ret!=GS_OK) return ret;
			a = CodeInt(GSOP_JZ,-1); // unknown jump
			ret = CompileExp(node->m_child[1]); // exp true
			if(ret!=GS_OK) return ret;
			b = CodeInt(GSOP_JMP,-1); // unknown jump
			GetCode(a)->i = GetCodePos(); // set jump pos
			ret = CompileExp(node->m_child[2]); // exp false
			if(ret!=GS_OK) return ret;
			GetCode(b)->i = GetCodePos(); // set jump pos
			break;
		}
		default:
		{
			Error(GSE_UNKNOWNNODE, node->m_type, 0);
			return GSE_UNKNOWNNODE;
		}
	}

	return GS_OK;
	unguard()
}


int gsCompiler::CompileCall( gsNode* node )
{
	guard(gsCompiler::CompileCall)
	int i,f;
	int ret;
	gsNode* n;

	gsassert(node!=NULL);
	gsassert(node->m_type==GSNODE_CALL);
	gsassert(node->m_str!=NULL);

	m_line = node->m_line;

	// push all params
	i=0;
	n = node->m_child[0];
	while(n!=NULL)
	{
		ret = CompileExp(n);
		if(ret!=GS_OK) return ret;
		n = n->m_next;
		i++;
	}

	// special functions
	if( strcmp(node->m_str,"call")==0 ) // dynamic call (fid was the first param)
	{
		if(i==0) // at least one (fid) param is required
		{
			Error(GSE_BADARGCOUNT, 1, 0);
			return GSE_BADARGCOUNT;
		}

		CodeInt(GSOP_CALL, i-1); // call fn with i-1 params
	}
	else // normal call
	{
		f=FnFind(node->m_str);
		if(f==-1) f=FnAdd(node->m_str);

		CodeInt(GSOP_PUSHINT,f);	// fid
		CodeInt(GSOP_CALL, i);		// call fn with i params
	}

	return GS_OK;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// LINKING
/////////////////////////////////////////////////////////////////////////////////////////
void gsCompiler::Link( int a0, int a1, int br, int cn )
{
	guard(gsCompiler::Link)
	gsCode* code;
	if(a1<a0) return;
	for(int i=a0;i<=a1;i++)
	{
		code = GetCode(i); gsassert(code!=NULL);
		if(code->op==GSOP_JMP && code->i==GS_JMP_BREAK) code->i=br;
		else
		if(code->op==GSOP_JMP && code->i==GS_JMP_CONTINUE) code->i=cn;
	}

	unguard()
}

int gsCompiler::LinkGoto()
{
	guard(gsCompiler::LinkGoto)

	gsCode* code;
	gsFn* fn = GetFn(); gsassert(fn!=NULL);

	for(int i=0; i<fn->m_codesize; i++)
	{
		code = &(fn->m_code[i]);
		if(code->op==GSOP_JMP && code->i<0) // invalid jump
		{
			int l = -code->i-1;
			if(l<0 || l>=m_gotocnt) // invalid label (continue or break outside the loop)
			{
				m_line = m_env->GetLineInfo( m_fn, i );
				Error(GSE_LABELNOTFOUND, code->i, 0, "invalid jump");
				return GSE_LABELNOTFOUND;
			}
			else
			{
				if(GotoGet(l)->m_addr<0) // label not found
				{
					m_line = GotoGet(l)->m_line;
					Error(GSE_LABELNOTFOUND, GotoGet(l)->m_name);
					return GSE_LABELNOTFOUND;
				}
				else
				{
					code->i = GotoGet(l)->m_addr;
				}
			}
		}
	}

	return GS_OK;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
