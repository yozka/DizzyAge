///////////////////////////////////////////////////////////////////////////////////////////////////
// DizDebug.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GAMEDEBUG_H__
#define __GAMEDEBUG_H__

#include "E9System.h"
#include "D9Debug.h"
#include "E9Math.h"

#define CON_LINESIZE		80		// characters per line
#define CON_LINES			2048	// total lines
#define SLOT_COUNT			16		// total slots
#define SLOT_SIZE			48		// slot line size
#define	INFO_LINES			2		// info slot lines
#define INPUT_SIZE			256		// input cmd size
#define INPUT_HISTORY		16		// input history count

#define	IS_DEVELOPER()		(cDizDebug::m_developer)

///////////////////////////////////////////////////////////////////////////////////////////////////
// cDizDebug
///////////////////////////////////////////////////////////////////////////////////////////////////
class cDizDebug
{
public:
				cDizDebug();

		BOOL	Init();
		void	Done();
		BOOL	Update();
		void	Draw();
		void	Layout();					// get render size from render; call it if render changes


		BOOL	DeveloperKey();

		// info
		void	InfoDraw();

		// navigation
		void	NavigationUpdate();

		// console
		iRect	ConsoleGetRect();
		void	ConsoleUpdate();
		void	ConsoleDraw();
		void	ConsolePush( int ch, char* msg );
static	void	Con_LogCallback( int ch, char* msg );
		char*	m_con_lines;								// messages lines storage
		int		m_con_pagetop;								// current visible line
		int		m_con_nextline;								// next free line (loops from beginning when full)

		// slots
		iRect	SlotGetRect();
		void	SlotDraw();
		void	SlotSet( int slot, char* text );
		char	m_slot[SLOT_COUNT][SLOT_SIZE];

		// input
inline	BOOL	InputIsOpened()								{ return m_input_open; }
		void	InputUpdate();
		void	InputDraw();
		void	InputExecute();								// execute command
		void	InputSkipWord( int dir );					// move cursor over a word; dir=-1/1
		void	InputAutoComplete();						// auto-complete function
		BOOL	m_input_open;								// input dialog opend
		int		m_input_crt;								// input currsor
		int		m_input_complete;							// auto-complete count (0=first, 1=next, etc)
		char	m_input_cmd[INPUT_SIZE];					// input command string
		char	m_input_historycrt;							// current history cursor
		char	m_input_historycnt;							// current history count
		char	m_input_history[INPUT_HISTORY][INPUT_SIZE];	// input history list of commands

		// util
		void	ConsumeInput();

		int		m_renderw;
		int		m_renderh;
		BOOL	m_console;									// developer console active
static	BOOL	m_developer;								// developer debug active (set from ini)
};

extern cDizDebug g_dizdebug;

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
