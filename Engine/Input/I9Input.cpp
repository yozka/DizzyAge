///////////////////////////////////////////////////////////////////////////////////////////////////
// I9Input.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "I9Input.h"
#include "I9InputDX.h"

i9Input* i9_input = NULL;

i9Input::i9Input()
{
	guard(i9Input::i9Input);
	m_hwnd		= NULL;
	m_hinstance = NULL;
	m_frm		= 0;
	m_time		= 0.0f;
	m_nkq		= 0;
	m_key		= NULL;
	m_axe		= NULL;
	m_keyq		= NULL;
	unguard();
}

i9Input::~i9Input()
{
	guard(i9Input::~i9Input);
	unguard();
}

BOOL i9Input::Init( HWND hwnd, HINSTANCE hinstance )
{
	guard(i9Input::Init);
	m_hwnd = hwnd;
	m_hinstance = hinstance;

	m_key	= (i9Key*)smalloc( I9_KEYS * sizeof(i9Key) );
	m_axe	= (i9Axe*)smalloc( I9_AXES * sizeof(i9Axe) );
	m_keyq	= (dword*)smalloc( I9_QUEUESIZE * sizeof(dword) );
	memset( m_key, 0, I9_KEYS * sizeof(i9Key) );
	memset( m_axe, 0, I9_AXES * sizeof(i9Axe) );
	memset( m_keyq, 0, I9_QUEUESIZE * sizeof(dword) );

	return TRUE;
	unguard();
}

void i9Input::Done()
{
	guard(i9Input::Done);
	for(int i=0;i<I9_DEVICES;i++)
		if(DeviceIsPresent(i))	
			DeviceDone(i);

	sfree(m_key); m_key=NULL;
	sfree(m_axe); m_axe=NULL;
	sfree(m_keyq); m_keyq=NULL;
	unguard();
}

void i9Input::Update( float dtime )
{
	guard(i9Input::Update);
	m_frm++;
	m_time += dtime;

	ClearKeyQ(); // clear que buffer
	
	for(int i=0;i<I9_DEVICES;i++)
		if(DeviceIsPresent(i))
			DeviceUpdate(i);

	if( dtime > 4.0f ) // to long since last refresh - ignore data
	{
		// dlog(LOGINP, "INPUT: Too much time without refresh:\n" );
		Clear();
	}
	unguard();
}

void i9Input::Clear()
{	
	guard(i9Input::Clear);
	for(int i=0;i<I9_DEVICES;i++)
		if(DeviceIsPresent(i))
			DeviceClear(i);

	ClearKeyQ();
	m_time = 0.0f;
	unguard();
}

void i9Input::Acquire()
{
	guard(i9Input::Acquire);
	//dlog(LOGINP, "INPUT: Acquire\n");
	ClearKeyQ();
	unguard();
}

void i9Input::Unacquire()
{
	guard(i9Input::Unacquire);
	//dlog(LOGINP, "INPUT: Unacquire\n");
	ClearKeyQ();	
	unguard();
}

BOOL i9Input::DeviceInit( int device )						{ return FALSE; }
void i9Input::DeviceDone( int device )						{}	
BOOL i9Input::DeviceIsPresent( int device )					{ return FALSE; }
void i9Input::DeviceUpdate( int device )					{}	
void i9Input::DeviceClear( int device )						{}
BOOL i9Input::DeviceFFInit( int device )					{ return FALSE; }
void i9Input::DeviceFFSet( int device, int mag, int per )	{}
void i9Input::DeviceFFPlay( int device )					{}
void i9Input::DeviceFFStop( int device )					{}
BOOL i9Input::DeviceFFIsPlaying( int device )				{ return FALSE; }

///////////////////////////////////////////////////////////////////////////////////////////////////
void i9Input::PushKey( int key, BOOL value )
{
	guard(i9Input::PushKey);

	m_key[key].m_value	= value;
	m_key[key].m_frm	= m_frm;		
	
	// que buffer
	if( m_nkq >= I9_QUEUESIZE )
	{
		dlog(LOGINP, "INPUT: Key que full - input lost.\n" );
		return;
	}

	m_keyq[ m_nkq ] = key | ( value ? I9_KEYMASK_VALUE : 0 );
	m_nkq++;
	
	unguard();
}

dword i9Input::PopKey()
{
	guard(i9Input::PopKey);
	if( m_nkq == 0 ) return 0;
	int key = m_keyq[0];
	m_nkq--;
	if(m_nkq>0) memcpy( &m_keyq[0], &m_keyq[1], m_nkq * sizeof(int) );
	return key;
	unguard();
}

dword i9Input::PeekKey()
{
	guard(i9Input::PeekKey);
	if( m_nkq == 0 ) return 0;
	return m_keyq[0];
	unguard();
}

int i9Input::FindKeyByAscii( char ascii )
{
	guard(i9Input::FindKeyByAscii);
	ascii = ascii | 32; // lower
	for(int i=0;i<I9_KEYS;i++)
		if(m_keyname[i].m_ascii == ascii)
			return i;
	return 0;
	unguard();
}

int	i9Input::FindKeyByName( char* name )
{
	guard(i9Input::FindKeyByName);
	if(name==NULL) return 0;
	for(int i=0;i<I9_KEYS;i++)
		if( 0==stricmp(name, m_keyname[i].m_name) )
			return i;
	return 0;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
i9KeyName i9Input::m_keyname[ I9_KEYS ] = 
{
	{0,		0		,I9_KEYUNKNOWN},		// 0x00
	{0, 	0		,"escape"},				// 0x01	escape
	{'1',	'!'		,"1"},					// 0x02
	{'2',	'@'		,"2"},					// 0x03
	{'3',	'#'		,"3"},					// 0x04
	{'4',	'$'		,"4"},					// 0x05
	{'5',	'%'		,"5"},					// 0x06
	{'6',	'^'		,"6"},					// 0x07
	{'7',	'&'		,"7"},					// 0x08
	{'8',	'*'		,"8"},					// 0x09
	{'9',	'('		,"9"},					// 0x0A
	{'0',	')'		,"0"},					// 0x0B
	{'-',	'_'		,"minus"},				// 0x0C	/* - on main keyboard */
	{'=',	'+'		,"equals"},				// 0x0D
	{0,		0		,"back"},				// 0x0E	/* backspace */
	{0,		0		,"tab"},				// 0x0F
	{'q',	'Q'		,"q"},					// 0x10
	{'w',	'W'		,"w"},					// 0x11
	{'e',	'E'		,"e"},					// 0x12
	{'r',	'R'		,"r"},					// 0x13
	{'t',	'T'		,"t"},					// 0x14
	{'y',	'Y'		,"y"},					// 0x15
	{'u',	'U'		,"u"},					// 0x16
	{'i',	'I'		,"i"},					// 0x17
	{'o',	'O'		,"o"},					// 0x18
	{'p',	'P'		,"p"},					// 0x19
	{'[',	'{'		,"lbracket"},			// 0x1A
	{']',	'}'		,"rbracket"},			// 0x1B
	{0,		0		,"return"},				// 0x1C	  /* Enter on main keyboard */
	{0,		0		,"lcontrol"},			// 0x1D
	{'a',	'A'		,"a"},					// 0x1E
	{'s',	'S'		,"s"},					// 0x1F
	{'d',	'D'		,"d"},					// 0x20
	{'f',	'F'		,"f"},					// 0x21
	{'g',	'G'		,"g"},					// 0x22
	{'h',	'H'		,"h"},					// 0x23
	{'j',	'J'		,"j"},					// 0x24
	{'k',	'K'		,"k"},					// 0x25
	{'l',	'L'		,"l"},					// 0x26
	{';',	':'		,"semicolon"},			// 0x27
	{'\'',	'\"'	,"apostrophe"},			// 0x28
	{'`',	'~'		,"grave"},				// 0x29	  /* accent grave */
	{0,		0		,"lshift"},				// 0x2A
	{'\\',	'|'		,"backslash"},			// 0x2B
	{'z',	'Z'		,"z"},					// 0x2C
	{'x',	'X'		,"x"},					// 0x2D
	{'c',	'C'		,"c"},					// 0x2E
	{'v',	'V'		,"v"},					// 0x2F
	{'b',	'B'		,"b"},					// 0x30
	{'n',	'N'		,"n"},					// 0x31
	{'m',	'M'		,"m"},					// 0x32
	{',',	'<'		,"comma"},				// 0x33
	{'.',	'>'		,"period"},				// 0x34    /* . on main keyboard */
	{'/',	'?'		,"slash"},				// 0x35	/* / on main keyboard */
	{0,		0		,"rshift"},				// 0x36
	{'*',	'*'		,"multiply"},			// 0x37	 /* * on numeric keypad */
	{0,		0		,"l-alt"},				// 0x38	  /* left Alt */
	{' ',	' '		,"space"},				// 0x39
	{0,		0		,"caps lock"},			// 0x3A
	{0,		0		,"F1"},					// 0x3B
	{0,		0		,"F2"},					// 0x3C
	{0,		0		,"F3"},					// 0x3D
	{0,		0		,"F4"},					// 0x3E
	{0,		0		,"F5"},					// 0x3F
	{0,		0		,"F6"},					// 0x40
	{0,		0		,"F7"},					// 0x41
	{0,		0		,"F8"},					// 0x42
	{0,		0		,"F9"},					// 0x43
	{0,		0		,"F10"},				// 0x44
	{0,		0		,"numlock"},			// 0x45
	{0,		0		,"scroll"},				// 0x46	  /* Scroll Lock */
	{'7',	'7'		,"numpad7"},			// 0x47		// numpad VVVVVVVVVVVV
	{'8',	'8'		,"numpad8"},			// 0x48
	{'9',	'9'		,"numpad9"},			// 0x49
	{'-',	'-'		,"subtract"},			// 0x4A	  /* - on numeric keypad */
	{'4',	'4'		,"numpad4"},			// 0x4B
	{'5',	'5'		,"numpad5"},			// 0x4C
	{'6',	'6'		,"numpad6"},			// 0x4D
	{'+',	'+'		,"add"},				// 0x4E	  /* + on numeric keypad */
	{'1',	'1'		,"numpad1"},			// 0x4F
	{'2',	'2'		,"numpad2"},			// 0x50
	{'3',	'3'		,"numpad3"},			// 0x51
	{'0',	'0'		,"numpad0"},			// 0x52	numpad ^^^^^^^^^^^^^^^^
	{'.',	'.'		,"decimal"},			// 0x53	  /* . on numeric keypad */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"oem_102"},			// 0x56	/* < > | on UK/Germany keyboards */
	{0,		0		,"F11"},				// 0x57
	{0,		0		,"F12"},				// 0x58
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"F13"},				// 0x64	/*					   (NEC PC98) */
	{0,		0		,"F14"},				// 0x65	/*					   (NEC PC98) */
	{0,		0		,"F15"},				// 0x66	/*					   (NEC PC98) */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"kana"},				// 0x70    /* (Japanese keyboard)			 */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"abnt_c1"},			// 0x73	/* / ? on Portugese (Brazilian) keyboards */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"convert"},			// 0x79	/* (Japanese keyboard)			  */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"noconvert"},			// 0x7B	  /* (Japanese keyboard)			*/
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"yen"},				// 0x7D	/* (Japanese keyboard)			  */
	{0,		0		,"abnt_c2"},			// 0x7E	/* Numpad . on Portugese (Brazilian) keyboards */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{'=',	'='		,"numpadequals"},		// 0x8D	 /* = on numeric keypad (NEC PC98) */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"prevtrack"},			// 0x90	  /* Previous Track (K_CIRCUMFLEX on Japanese keyboard) */
	{0,		0		,"at"},					// 0x91	 /* 					(NEC PC98) */
	{0,		0		,"colon"},				// 0x92	  /*					 (NEC PC98) */
	{'_',	'_'		,"underline"},			// 0x93	/*					   (NEC PC98) */
	{0,		0		,"kanji"},				// 0x94	  /* (Japanese keyboard)			*/
	{0,		0		,"stop"},				// 0x95    /*					  (NEC PC98) */
	{0,		0		,"ax"},					// 0x96	 /* 					(Japan AX) */
	{0,		0		,"unlabeled"},			// 0x97	  /*						(J3100) */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"nexttrack"},			// 0x99	  /* Next Track */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"numenter"},			// 0x9C	 /* Enter on numeric keypad */
	{0,		0		,"rcontrol"},			// 0x9D
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"mute"},				// 0xA0    /* Mute */
	{0,		0		,"calculator"},			// 0xA1	 /* Calculator */
	{0,		0		,"playpause"},			// 0xA2	  /* Play / Pause */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"mediastop"},			// 0xA4	  /* Media Stop */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"volumedown"},			// 0xAE	 /* Volume - */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"volumeup"},			// 0xB0    /* Volume + */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"webhome"},			// 0xB2	/* Web home */
	{',',	','		,"numpadcomma"},		// 0xB3	  /* , on numeric keypad (NEC PC98) */
	{0,		0		,I9_KEYUNKNOWN},		//
	{'/',	'/'		,"divide"},				// 0xB5    /* / on numeric keypad */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"sysrq"},				// 0xB7
	{0,		0		,"r-alt"},				// 0xB8	  /* right Alt */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"pause"},				// 0xC5	  /* Pause */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"home"},				// 0xC7    /* Home on arrow keypad */
	{0,		0		,"up"},					// 0xC8	 /* UpArrow on arrow keypad */
	{0,		0		,"PgUp"},				// 0xC9	  /* PgUp on arrow keypad */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"left"},				// 0xCB    /* LeftArrow on arrow keypad */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"right"},				// 0xCD	  /* RightArrow on arrow keypad */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"end"},				// 0xCF	/* End on arrow keypad */
	{0,		0		,"down"},				// 0xD0    /* DownArrow on arrow keypad */
	{0,		0		,"PgDown"},				// 0xD1    /* PgDn on arrow keypad */
	{0,		0		,"insert"},				// 0xD2	 /* Insert on arrow keypad */
	{0,		0		,"delete"},				// 0xD3	 /* Delete on arrow keypad */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"lwin"},				// 0xDB    /* Left Windows key */
	{0,		0		,"rwin"},				// 0xDC    /* Right Windows key */
	{0,		0		,"apps"},				// 0xDD    /* AppMenu key */
	{0,		0		,"power"},				// 0xDE	  /* System Power */
	{0,		0		,"sleep"},				// 0xDF	  /* System Sleep */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"wake"},				// 0xE3    /* System Wake */
	{0,		0		,I9_KEYUNKNOWN},		//
	{0,		0		,"websearch"},			// 0xE5	/* Web Search */
	{0,		0		,"webfavorites"},		// 0xE6	/* Web Favorites */
	{0,		0		,"webrefresh"},			// 0xE7	/* Web Refresh */
	{0,		0		,"webstop"},			// 0xE8	/* Web Stop */
	{0,		0		,"webforward"},			// 0xE9	/* Web Forward */
	{0,		0		,"webback"},			// 0xEA	/* Web Back */
	{0,		0		,"mycomputer"},			// 0xEB	/* My Computer */
	{0,		0		,"mail"},				// 0xEC	/* Mail */
	{0,		0		,"mediaselect"},		// 0xED	/* Media Select */
	{0,		0		,I9_KEYUNKNOWN},		// 0xee
	{0,		0		,I9_KEYUNKNOWN},		// 0xef
	{0,		0		,I9_KEYUNKNOWN},		// 0xf0
	{0,		0		,I9_KEYUNKNOWN},		// 0xf1
	{0,		0		,I9_KEYUNKNOWN},		// 0xf2
	{0,		0		,I9_KEYUNKNOWN},		// 0xf3
	{0,		0		,I9_KEYUNKNOWN},		// 0xf4
	{0,		0		,I9_KEYUNKNOWN},		// 0xf5
	{0,		0		,I9_KEYUNKNOWN},		// 0xf6
	{0,		0		,I9_KEYUNKNOWN},		// 0xf7
	{0,		0		,I9_KEYUNKNOWN},		// 0xf8
	{0,		0		,I9_KEYUNKNOWN},		// 0xf9
	{0,		0		,I9_KEYUNKNOWN},		// 0xfa
	{0,		0		,I9_KEYUNKNOWN},		// 0xfb
	{0,		0		,I9_KEYUNKNOWN},		// 0xfc
	{0,		0		,I9_KEYUNKNOWN},		// 0xfd
	{0,		0		,I9_KEYUNKNOWN},		// 0xfe
	{0,		0		,I9_KEYUNKNOWN},		// 0xff
	
	{0,		0		,"mouse_b1"},			// 0x100		MOUSE
	{0,		0		,"mouse_b2"},
	{0,		0		,"mouse_b3"},
	{0,		0		,"mouse_b4"},
	{0,		0		,"mouse_b5"},
	{0,		0		,"mouse_b6"},
	{0,		0		,"mouse_b7"},
	{0,		0		,"mouse_b8"},				

	{0,		0		,"joy1_b1"},			// 0x108		JOYSTICK 1
	{0,		0		,"joy1_b2"},
	{0,		0		,"joy1_b3"},
	{0,		0		,"joy1_b4"},
	{0,		0		,"joy1_b5"},
	{0,		0		,"joy1_b6"},
	{0,		0		,"joy1_b7"},
	{0,		0		,"joy1_b8"},
	{0,		0		,"joy1_b9"},
	{0,		0		,"joy1_b10"},
	{0,		0		,"joy1_b11"},
	{0,		0		,"joy1_b12"},
	{0,		0		,"joy1_b13"},
	{0,		0		,"joy1_b14"},
	{0,		0		,"joy1_b15"},
	{0,		0		,"joy1_b16"},
	{0,		0		,"joy1_h1"},
	{0,		0		,"joy1_h2"},
	{0,		0		,"joy1_h3"},
	{0,		0		,"joy1_h4"},

	{0,		0		,"joy2_b1"},			// 0x11C		JOYSTICK 2
	{0,		0		,"joy2_b2"},
	{0,		0		,"joy2_b3"},
	{0,		0		,"joy2_b4"},
	{0,		0		,"joy2_b5"},
	{0,		0		,"joy2_b6"},
	{0,		0		,"joy2_b7"},
	{0,		0		,"joy2_b8"},
	{0,		0		,"joy2_b9"},
	{0,		0		,"joy2_b10"},
	{0,		0		,"joy2_b11"},
	{0,		0		,"joy2_b12"},
	{0,		0		,"joy2_b13"},
	{0,		0		,"joy2_b14"},
	{0,		0		,"joy2_b15"},
	{0,		0		,"joy2_b16"},
	{0,		0		,"joy2_h1"},
	{0,		0		,"joy2_h2"},
	{0,		0		,"joy2_h3"},
	{0,		0		,"joy2_h4"}
											// 0x130
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL I9_Init( HWND hwnd, HINSTANCE hinstance, int api )
{
	guard(I9_Init);
	if(i9_input) return TRUE;
	dlog(LOGINP, "Input init (api=%i).\n",api);
	// test api here if more platformes
	i9_input = snew i9InputDX();
	BOOL ok = i9_input->Init( hwnd, hinstance );
	if(!ok)
	{
		i9_input->Done();
		sdelete(i9_input);
		i9_input = NULL;
	}
	return ok;
	unguard();
}

void I9_Done()
{
	guard(I9_Done);
	if(!i9_input) return;
	i9_input->Done();
	sdelete(i9_input);
	i9_input = NULL;
	dlog(LOGINP, "Input done.\n");
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
