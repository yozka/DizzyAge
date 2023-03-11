#ifndef _GSNODE_
#define _GSNODE_
#include "gsutil.h"

/////////////////////////////////////////////////////////////////////////////////////////
// DEFINES
/////////////////////////////////////////////////////////////////////////////////////////
#define GSNODE_NONE		0
#define	GSNODE_ABORT	1
#define GSNODE_ASG0		2
#define GSNODE_ASG1		3
#define GSNODE_ASG2		4
#define GSNODE_ASG3		5
#define GSNODE_BLOCK	6
#define GSNODE_BREAK	7
#define GSNODE_CALL		8
#define GSNODE_CAST		9
#define GSNODE_COD0		10
#define GSNODE_CONTINUE	11
#define GSNODE_DECVAR	12
#define GSNODE_DECFN	13
#define	GSNODE_DEFINE	14
#define GSNODE_DO		15
#define GSNODE_IDD		16
#define GSNODE_IDX		17
#define GSNODE_IF		18
#define	GSNODE_IFDEF	19
#define GSNODE_IFEXP	20
#define	GSNODE_IFNDEF	21
#define	GSNODE_INCLUDE	22
#define GSNODE_FOR		23
#define	GSNODE_GOTO		24
#define	GSNODE_LABEL	25
#define	GSNODE_NEW		26
#define GSNODE_OP1		27
#define GSNODE_OP2		28
#define GSNODE_OBJ		29
#define GSNODE_RET		30
#define	GSNODE_SCOPE	31
#define	GSNODE_VAR		32
#define GSNODE_WHILE	33

#define GS_MAXNODE		34	// total number of nodes

/////////////////////////////////////////////////////////////////////////////////////////
// GSNODE
// The parser tree's node structure
/////////////////////////////////////////////////////////////////////////////////////////
class gsNode
{
public:
						gsNode		();
						gsNode		( int type, int line, int data, gsNode* child1=NULL, gsNode* child2=NULL, gsNode* child3=NULL, gsNode* child4=NULL );
						gsNode		( int type, int line, int data, int value, gsNode* child1=NULL, gsNode* child2=NULL, gsNode* child3=NULL, gsNode* child4=NULL );
						gsNode		( int type, int line, int data, float value, gsNode* child1=NULL, gsNode* child2=NULL, gsNode* child3=NULL, gsNode* child4=NULL );
						gsNode		( int type, int line, int data, char* value, gsNode* child1=NULL, gsNode* child2=NULL, gsNode* child3=NULL, gsNode* child4=NULL );
	virtual			   ~gsNode		();

			void		LogTree		(int ident=0 );	// logs a parser tree for debug

	int		m_type;		// node type GSNODE_
	int		m_line;		// source file line
	int		m_data;		// some data
	int		m_dataex;	// some more data
	int		m_int;		// int data
	float	m_flt;		// float data
	char*	m_str;		// string data
	gsNode*	m_parent;	// parent
	gsNode*	m_prev;		// prev brother
	gsNode*	m_next;		// next brother
	gsNode*	m_child[4];	// childs

	static	void		InitTree();					// init memory linking
	static	void		DoneTree( BOOL freemem );	// reset memory linking; if freemem is TRUE, clear memory linking list using memlinkhead
	static	void		DestroyTree( gsNode* tree );// recursive destroy a tree 

private:
			gsNode*		m_memlink;		// special memory link used for internal tracking of allocations !
	static	gsNode*		m_memlinkhead;	// head of memory linking list
};

#endif
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
