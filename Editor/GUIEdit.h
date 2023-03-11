//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIEdit.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GUIEDIT_H__
#define __GUIEDIT_H__

#include "GUIItem.h"
#include "E9String.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////////////////////////////

#define IV_TXTSIZE				(IV_USER+1)

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUIEdit
// Obs:
// 1. uses COLOR1=backgr, COLOR2=gradient, COLOR3=border, COLOR4=selection
// 2. text align forced to GUIALIGN_LEFT|GUIALIGN_CENTERY
//////////////////////////////////////////////////////////////////////////////////////////////////

class cGUIEdit : public cGUIItem
{
public:

						cGUIEdit			();
virtual					~cGUIEdit			();

virtual	void			Build				();						// build
virtual	void			Update				();						// update 
virtual	void			Draw				();						// draw 
	
		// Access
virtual	void			SetTxt				( int idx, char* text );

		// Util
		int				Pos2Chr				(int pos);				// get char pos from pixel pos
		int				Chr2Pos				(int chr);				// get pixel pos from char pos
inline  void			ShiftLeft			(int chr, int count);	// shifts count chars starting with chr 
inline  void			ShiftRight			(int chr, int count);	// shifts count chars starting with chr 
inline	void			SetBkTxt			(char* txt)				{ if(m_bktxt) sfree(m_bktxt); m_bktxt=NULL; if(txt) m_bktxt=sstrdup(txt); }
		void			ClipboardCopy		();						// copy to windows clipboard
		void			ClipboardPaste		();						// paste from windows clipboard
		void			SelectionCut		();						// cut current selection, if any
		void			SelectionPaste		(char* txt);			// paste some text at cursor (and shift right)

		int				m_sel1;										// selected char start pos
		int				m_sel2;										// selected char end pos (after last selected)
		BOOL			m_edit;										// if TRUE it is in edit mode
		char*			m_bktxt;			
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// inline
//////////////////////////////////////////////////////////////////////////////////////////////////
inline  void cGUIEdit::ShiftLeft(int chr, int count)
{
	char *txt = GetTxt(IV_TXT);
	if(chr-count<0) count = chr;
	if(count==0) return;
	int size = (int)strlen(txt);
	for(int i=chr;i<=size;i++)
		txt[i-count] = txt[i];
}

inline  void cGUIEdit::ShiftRight(int chr, int count)
{
	char *txt = GetTxt(IV_TXT);
	int size = (int)strlen(txt);
	if(size+count>GetInt(IV_TXTSIZE)) count = GetInt(IV_TXTSIZE) - size;
	if(count==0) return;
	for(int i=size;i>=chr;i--)
		txt[i+count] = txt[i];
}

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


