#include "stdafx.h"
#include "gsnode.h"

gsNode*	gsNode::m_memlinkhead = NULL;

/////////////////////////////////////////////////////////////////////////////////////////
gsNode::gsNode()
{
	guard(gsNode::gsNode)
	m_type		= 0;
	m_line		= -1;
	m_data		= 0;
	m_dataex	= 0;
	m_int		= 0;
	m_flt		= 0.0f;
	m_str		= NULL;
	m_parent	= NULL;
	m_prev		= NULL;
	m_next		= NULL;
	m_child[0]	= NULL;
	m_child[1]	= NULL;
	m_child[2]	= NULL;
	m_child[3]	= NULL;
	
	if(m_memlinkhead) m_memlink = m_memlinkhead; else m_memlink = NULL;
	m_memlinkhead = this;
	unguard()
}

gsNode::gsNode( int type, int line, int data, gsNode* child1, gsNode* child2, gsNode* child3, gsNode* child4 )
{
	guard(gsNode::gsNode)
	m_type		= type;
	m_line		= line;
	m_data		= data;
	m_dataex	= 0;
	m_int		= 0;
	m_flt		= 0.0f;
	m_str		= NULL;
	m_parent	= NULL;
	m_prev		= NULL;
	m_next		= NULL;
	m_child[0]	= child1;
	m_child[1]	= child2;
	m_child[2]	= child3;
	m_child[3]	= child4;

	if(m_memlinkhead) m_memlink = m_memlinkhead; else m_memlink = NULL;
	m_memlinkhead = this;
	unguard()
}

gsNode::gsNode( int type, int line, int data, int value, gsNode* child1, gsNode* child2, gsNode* child3, gsNode* child4 )
{
	guard(gsNode::gsNode)
	m_type		= type;
	m_line		= line;
	m_data		= data;
	m_dataex	= 0;
	m_int		= value;
	m_flt		= 0.0f;
	m_str		= NULL;
	m_parent	= NULL;
	m_prev		= NULL;
	m_next		= NULL;
	m_child[0]	= child1;
	m_child[1]	= child2;
	m_child[2]	= child3;
	m_child[3]	= child4;

	if(m_memlinkhead) m_memlink = m_memlinkhead; else m_memlink = NULL;
	m_memlinkhead = this;
	unguard()
}

gsNode::gsNode( int type, int line, int data, float value, gsNode* child1, gsNode* child2, gsNode* child3, gsNode* child4 )
{
	guard(gsNode::gsNode)
	m_type		= type;
	m_line		= line;
	m_data		= data;
	m_dataex	= 0;
	m_int		= 0;
	m_flt		= value;
	m_str		= NULL;
	m_parent	= NULL;
	m_prev		= NULL;
	m_next		= NULL;
	m_child[0]	= child1;
	m_child[1]	= child2;
	m_child[2]	= child3;
	m_child[3]	= child4;

	if(m_memlinkhead) m_memlink = m_memlinkhead; else m_memlink = NULL;
	m_memlinkhead = this;
	unguard()
}

extern char* yytext;
extern int yyleng;
gsNode::gsNode( int type, int line, int data, char* value, gsNode* child1, gsNode* child2, gsNode* child3, gsNode* child4 )
{
	guard(gsNode::gsNode)
	m_type		= type;
	m_line		= line;
	m_data		= data;
	m_dataex	= 0;
	m_int		= 0;
	m_flt		= 0.0f;
	m_str		= value; // gsstrdup(value); // important
	m_parent	= NULL;
	m_prev		= NULL;
	m_next		= NULL;
	m_child[0]	= child1;
	m_child[1]	= child2;
	m_child[2]	= child3;
	m_child[3]	= child4;

	if(m_memlinkhead) m_memlink = m_memlinkhead; else m_memlink = NULL;
	m_memlinkhead = this;
	unguard()
}

gsNode::~gsNode()
{
	guard(gsNode::~gsNode)
	if(m_str) gsfree(m_str);
	// childs will be deleted throught the mem linking chain or throught DestroyTree !
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG HELPERS
/////////////////////////////////////////////////////////////////////////////////////////
void gsNode::LogTree( int ident )
{
	guard(gsNode::LogTree)
	static char* nodename[GS_MAXNODE] = 
	{
		"NODE_NONE",
		"NODE_ABORT",
		"NODE_ASG0",	
		"NODE_ASG1",	
		"NODE_ASG2",	
		"NODE_ASG3",	
		"NODE_BLOCK",
		"NODE_BREAK",
		"NODE_CALL",
		"NODE_CAST",	
		"NODE_COD0",	
		"NODE_CONTINUE",	
		"NODE_DECVAR",
		"NODE_DECFN",
		"NODE_DEFINE",
		"NODE_DO",
		"NODE_IDD",
		"NODE_IDX",
		"NODE_IF",
		"NODE_IFEXP",
		"NODE_IFDEF",
		"NODE_IFNDEF",
		"NODE_INCLUDE",
		"NODE_FOR",
		"NODE_GOTO",
		"NODE_LABEL",
		"NODE_NEW",
		"NODE_OP1",
		"NODE_OP2",
		"NODE_OBJ",
		"NODE_RET",
		"NODE_SCOPE",
		"NODE_VAR",
		"NODE_WHILE"
	};

	int i;
	gslog("%03i: ",m_line);
	for(i=0;i<ident;i++) gslog("|");

	gslog(	"%-10s  = %i (%i, %.2f, %s)\n", 
		(0<=m_type && m_type<GS_MAXNODE) ? nodename[m_type] : "<unknown>",
		m_data, m_int, m_flt, m_str );
	
	for(i=0;i<4;i++)
		if(m_child[i]!=NULL) m_child[i]->LogTree( ident+1 );

	if( m_next!=NULL )
		m_next->LogTree( ident );
	unguard()
}


void gsNode::InitTree()
{
	guard(gsNode::InitTree)
	gsassert(m_memlinkhead==NULL); // must be free!
	m_memlinkhead = NULL;
	unguard()
}

void gsNode::DoneTree( BOOL freemem )
{
	guard(gsNode::DoneTree)
	if(freemem)
	{
		gsNode* n = m_memlinkhead;
		while(n!=NULL)
		{
			gsNode* t = n;
			n=n->m_memlink;
			gsdelete(t);
		}
	}
	m_memlinkhead = NULL;
	unguard()
}

void gsNode::DestroyTree( gsNode* tree )
{
	guard(gsNode::DestroyTree)
	if(tree==NULL) return;
	if(tree->m_child[0])	DestroyTree(tree->m_child[0]);
	if(tree->m_child[1])	DestroyTree(tree->m_child[1]);
	if(tree->m_child[2])	DestroyTree(tree->m_child[2]);
	if(tree->m_child[3])	DestroyTree(tree->m_child[3]);
	if(tree->m_next)		DestroyTree(tree->m_next);
	gsdelete(tree);
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
