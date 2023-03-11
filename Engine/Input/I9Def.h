///////////////////////////////////////////////////////////////////////////////////////////////////
// I9Def.h
// Defines of keys and axes used in input
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __I9DEF_H__
#define __I9DEF_H__

// counts
#define I9_AXES					( I9_MOUSE_AXES + I9_JOY_AXES * I9_JOY_COUNT )
#define I9_KEYS					( I9_KEYBOARD_KEYS + I9_MOUSE_KEYS + I9_JOY_KEYS * I9_JOY_COUNT )

///////////////////////////////////////////////////////////////////////////////////////////////////
// KEYBOARD
///////////////////////////////////////////////////////////////////////////////////////////////////
#define I9_KEYBOARD_KEYS		256
#define I9_KEYBOARD_FIRSTKEY	0

#define	I9K_UNKNOWN				0x00
#define I9K_ESCAPE          	0x01
#define I9K_1               	0x02
#define I9K_2               	0x03
#define I9K_3               	0x04
#define I9K_4               	0x05
#define I9K_5               	0x06
#define I9K_6               	0x07
#define I9K_7               	0x08
#define I9K_8               	0x09
#define I9K_9               	0x0A
#define I9K_0               	0x0B
#define I9K_MINUS           	0x0C    /* - on main keyboard */
#define I9K_EQUALS          	0x0D
#define I9K_BACK            	0x0E    /* backspace */
#define I9K_TAB             	0x0F
#define I9K_Q               	0x10
#define I9K_W               	0x11
#define I9K_E               	0x12
#define I9K_R               	0x13
#define I9K_T               	0x14
#define I9K_Y               	0x15
#define I9K_U               	0x16
#define I9K_I               	0x17
#define I9K_O               	0x18
#define I9K_P               	0x19
#define I9K_LBRACKET        	0x1A
#define I9K_RBRACKET        	0x1B
#define I9K_RETURN          	0x1C    /* Enter on main keyboard */
#define I9K_LCONTROL        	0x1D
#define I9K_A               	0x1E
#define I9K_S               	0x1F
#define I9K_D               	0x20
#define I9K_F               	0x21
#define I9K_G               	0x22
#define I9K_H               	0x23
#define I9K_J               	0x24
#define I9K_K               	0x25
#define I9K_L               	0x26
#define I9K_SEMICOLON       	0x27
#define I9K_APOSTROPHE      	0x28
#define I9K_GRAVE           	0x29    /* accent grave */
#define I9K_LSHIFT          	0x2A
#define I9K_BACKSLASH       	0x2B
#define I9K_Z               	0x2C
#define I9K_X               	0x2D
#define I9K_C               	0x2E
#define I9K_V               	0x2F
#define I9K_B               	0x30
#define I9K_N               	0x31
#define I9K_M               	0x32
#define I9K_COMMA           	0x33
#define I9K_PERIOD          	0x34    /* . on main keyboard */
#define I9K_SLASH           	0x35    /* / on main keyboard */
#define I9K_RSHIFT          	0x36
#define I9K_MULTIPLY        	0x37    /* * on numeric keypad */
#define I9K_LMENU           	0x38    /* left Alt */
#define I9K_SPACE           	0x39
#define I9K_CAPITAL         	0x3A
#define I9K_F1              	0x3B
#define I9K_F2              	0x3C
#define I9K_F3              	0x3D
#define I9K_F4              	0x3E
#define I9K_F5              	0x3F
#define I9K_F6              	0x40
#define I9K_F7              	0x41
#define I9K_F8              	0x42
#define I9K_F9              	0x43
#define I9K_F10             	0x44
#define I9K_NUMLOCK         	0x45
#define I9K_SCROLL          	0x46    /* Scroll Lock */
#define I9K_NUMPAD7         	0x47
#define I9K_NUMPAD8         	0x48
#define I9K_NUMPAD9         	0x49
#define I9K_SUBTRACT        	0x4A    /* - on numeric keypad */
#define I9K_NUMPAD4         	0x4B
#define I9K_NUMPAD5         	0x4C
#define I9K_NUMPAD6         	0x4D
#define I9K_ADD             	0x4E    /* + on numeric keypad */
#define I9K_NUMPAD1         	0x4F
#define I9K_NUMPAD2         	0x50
#define I9K_NUMPAD3         	0x51
#define I9K_NUMPAD0         	0x52
#define I9K_DECIMAL         	0x53    /* . on numeric keypad */
#define I9K_OEM_102         	0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define I9K_F11             	0x57
#define I9K_F12             	0x58
#define I9K_F13             	0x64    /*                     (NEC PC98) */
#define I9K_F14             	0x65    /*                     (NEC PC98) */
#define I9K_F15             	0x66    /*                     (NEC PC98) */
#define I9K_KANA            	0x70    /* (Japanese keyboard)            */
#define I9K_ABNT_C1         	0x73    /* /? on Brazilian keyboard */
#define I9K_CONVERT         	0x79    /* (Japanese keyboard)            */
#define I9K_NOCONVERT       	0x7B    /* (Japanese keyboard)            */
#define I9K_YEN             	0x7D    /* (Japanese keyboard)            */
#define I9K_ABNT_C2         	0x7E    /* Numpad . on Brazilian keyboard */
#define I9K_NUMPADEQUALS    	0x8D    /* = on numeric keypad (NEC PC98) */
#define I9K_PREVTRACK       	0x90    /* Previous Track (I9K_CIRCUMFLEX on Japanese keyboard) */
#define I9K_AT              	0x91    /*                     (NEC PC98) */
#define I9K_COLON           	0x92    /*                     (NEC PC98) */
#define I9K_UNDERLINE       	0x93    /*                     (NEC PC98) */
#define I9K_KANJI           	0x94    /* (Japanese keyboard)            */
#define I9K_STOP            	0x95    /*                     (NEC PC98) */
#define I9K_AX              	0x96    /*                     (Japan AX) */
#define I9K_UNLABELED       	0x97    /*                        (J3100) */
#define I9K_NEXTTRACK       	0x99    /* Next Track */
#define I9K_NUMPADENTER     	0x9C    /* Enter on numeric keypad */
#define I9K_RCONTROL        	0x9D
#define I9K_MUTE            	0xA0    /* Mute */
#define I9K_CALCULATOR      	0xA1    /* Calculator */
#define I9K_PLAYPAUSE       	0xA2    /* Play / Pause */
#define I9K_MEDIASTOP       	0xA4    /* Media Stop */
#define I9K_VOLUMEDOWN      	0xAE    /* Volume - */
#define I9K_VOLUMEUP        	0xB0    /* Volume + */
#define I9K_WEBHOME         	0xB2    /* Web home */
#define I9K_NUMPADCOMMA     	0xB3    /* , on numeric keypad (NEC PC98) */
#define I9K_DIVIDE          	0xB5    /* / on numeric keypad */
#define I9K_SYSRQ           	0xB7
#define I9K_RMENU           	0xB8    /* right Alt */
#define I9K_PAUSE           	0xC5    /* Pause */
#define I9K_HOME            	0xC7    /* Home on arrow keypad */
#define I9K_UP              	0xC8    /* UpArrow on arrow keypad */
#define I9K_PRIOR           	0xC9    /* PgUp on arrow keypad */
#define I9K_LEFT            	0xCB    /* LeftArrow on arrow keypad */
#define I9K_RIGHT           	0xCD    /* RightArrow on arrow keypad */
#define I9K_END             	0xCF    /* End on arrow keypad */
#define I9K_DOWN            	0xD0    /* DownArrow on arrow keypad */
#define I9K_NEXT            	0xD1    /* PgDn on arrow keypad */
#define I9K_INSERT          	0xD2    /* Insert on arrow keypad */
#define I9K_DELETE          	0xD3    /* Delete on arrow keypad */
#define I9K_LWIN            	0xDB    /* Left Windows key */
#define I9K_RWIN            	0xDC    /* Right Windows key */
#define I9K_APPS            	0xDD    /* AppMenu key */
#define I9K_POWER           	0xDE    /* System Power */
#define I9K_SLEEP           	0xDF    /* System Sleep */
#define I9K_WAKE            	0xE3    /* System Wake */
#define I9K_WEBSEARCH       	0xE5    /* Web Search */
#define I9K_WEBFAVORITES    	0xE6    /* Web Favorites */
#define I9K_WEBREFRESH      	0xE7    /* Web Refresh */
#define I9K_WEBSTOP         	0xE8    /* Web Stop */
#define I9K_WEBFORWARD      	0xE9    /* Web Forward */
#define I9K_WEBBACK         	0xEA    /* Web Back */
#define I9K_MYCOMPUTER      	0xEB    /* My Computer */
#define I9K_MAIL            	0xEC    /* Mail */
#define I9K_MEDIASELECT     	0xED    /* Media Select */

// Alternate names for keys
#define I9K_BACKSPACE       	I9K_BACK            /* backspace */
#define I9K_NUMPADSTAR      	I9K_MULTIPLY        /* * on numeric keypad */
#define I9K_LALT            	I9K_LMENU           /* left Alt */
#define I9K_CAPSLOCK        	I9K_CAPITAL         /* CapsLock */
#define I9K_NUMPADMINUS     	I9K_SUBTRACT        /* - on numeric keypad */
#define I9K_NUMPADPLUS      	I9K_ADD             /* + on numeric keypad */
#define I9K_NUMPADPERIOD    	I9K_DECIMAL         /* . on numeric keypad */
#define I9K_NUMPADSLASH     	I9K_DIVIDE          /* / on numeric keypad */
#define I9K_RALT            	I9K_RMENU           /* right Alt */
#define I9K_UPARROW         	I9K_UP              /* UpArrow on arrow keypad */
#define I9K_PGUP            	I9K_PRIOR           /* PgUp on arrow keypad */
#define I9K_LEFTARROW       	I9K_LEFT            /* LeftArrow on arrow keypad */
#define I9K_RIGHTARROW      	I9K_RIGHT           /* RightArrow on arrow keypad */
#define I9K_DOWNARROW       	I9K_DOWN            /* DownArrow on arrow keypad */
#define I9K_PGDN            	I9K_NEXT            /* PgDn on arrow keypad */


///////////////////////////////////////////////////////////////////////////////////////////////////
// MOUSE
///////////////////////////////////////////////////////////////////////////////////////////////////
// mouse axes
#define I9_MOUSE_AXES			3
#define I9_MOUSE_FIRSTAXE		0
#define	I9_MOUSE_X				( I9_MOUSE_FIRSTAXE + 0 )
#define	I9_MOUSE_Y				( I9_MOUSE_FIRSTAXE + 1 )
#define	I9_MOUSE_Z				( I9_MOUSE_FIRSTAXE + 2 )

// mouse keys
#define	I9_MOUSE_KEYS			10
#define	I9_MOUSE_FIRSTKEY		( I9_KEYBOARD_FIRSTKEY + I9_KEYBOARD_KEYS )
#define I9_MOUSE_B1				( I9_MOUSE_FIRSTKEY + 0 )
#define I9_MOUSE_B2				( I9_MOUSE_FIRSTKEY + 1 )
#define I9_MOUSE_B3				( I9_MOUSE_FIRSTKEY + 2 )
#define I9_MOUSE_B4				( I9_MOUSE_FIRSTKEY + 3 )
#define I9_MOUSE_B5				( I9_MOUSE_FIRSTKEY + 4 )
#define I9_MOUSE_B6				( I9_MOUSE_FIRSTKEY + 5 )
#define I9_MOUSE_B7				( I9_MOUSE_FIRSTKEY + 6 )
#define I9_MOUSE_B8				( I9_MOUSE_FIRSTKEY + 7 )
#define I9_MOUSE_WHEELUP		( I9_MOUSE_FIRSTKEY + 8 )	// fake wheel up   - limited functionality (not qued, not accurate)
#define I9_MOUSE_WHEELDN		( I9_MOUSE_FIRSTKEY + 9 )	// fake wheel down - limited functionality (not qued, not accurate)


///////////////////////////////////////////////////////////////////////////////////////////////////
// JOYSTICKS
///////////////////////////////////////////////////////////////////////////////////////////////////
#define I9_JOY_COUNT			2	// supported joysticks
#define I9_JOY_SLIDERS			2	// supported joystick slider axes
#define I9_JOY_BUTTONS			16	// supported joystick buttons keys
#define I9_JOY_HATS				1	// supported joystick hat/pov keys (4 keys for each supported hat: 0=up, 1=right, 2=down, 3=left)

#define I9_HUT_UP				0	// hut direction up
#define I9_HUT_RIGHT			1	// hut direction right
#define I9_HUT_DOWN				2	// hut direction down
#define I9_HUT_LEFT				3	// hut direction left

// joystick axes
#define I9_JOY_AXES				8
#define I9_JOY_FIRSTAXE( joy )	( I9_MOUSE_FIRSTAXE + I9_MOUSE_AXES + I9_JOY_AXES * joy )
#define I9_JOY_X( joy )			( I9_JOY_FIRSTAXE(joy) + 0 )
#define I9_JOY_Y( joy )			( I9_JOY_FIRSTAXE(joy) + 1 )
#define I9_JOY_Z( joy )			( I9_JOY_FIRSTAXE(joy) + 2 )
#define I9_JOY_RX( joy )		( I9_JOY_FIRSTAXE(joy) + 3 )
#define I9_JOY_RY( joy )		( I9_JOY_FIRSTAXE(joy) + 4 )
#define I9_JOY_RZ( joy )		( I9_JOY_FIRSTAXE(joy) + 5 )
#define I9_JOY_SLIDER1( joy )	( I9_JOY_FIRSTAXE(joy) + 6 )
#define I9_JOY_SLIDER2( joy )	( I9_JOY_FIRSTAXE(joy) + 7 )
//@OBS: on my gamepad axes are stick1:x=0,y=0, stick2:x=5,y=6

// joystick keys
#define	I9_JOY_KEYS				( I9_JOY_BUTTONS + I9_JOY_HATS*4 )
#define I9_JOY_FIRSTKEY( joy )	( I9_MOUSE_FIRSTKEY + I9_MOUSE_KEYS + I9_JOY_KEYS * joy )
#define I9_JOY_B1( joy )		( I9_JOY_FIRSTKEY(joy) + 0 )
#define I9_JOY_B2( joy )		( I9_JOY_FIRSTKEY(joy) + 1 )
#define I9_JOY_B3( joy )		( I9_JOY_FIRSTKEY(joy) + 2 )
#define I9_JOY_B4( joy )		( I9_JOY_FIRSTKEY(joy) + 3 )
#define I9_JOY_B5( joy )		( I9_JOY_FIRSTKEY(joy) + 4 )
#define I9_JOY_B6( joy )		( I9_JOY_FIRSTKEY(joy) + 5 )
#define I9_JOY_B7( joy )		( I9_JOY_FIRSTKEY(joy) + 6 )
#define I9_JOY_B8( joy )		( I9_JOY_FIRSTKEY(joy) + 7 )
#define I9_JOY_H1( joy )		( I9_JOY_FIRSTKEY(joy) + I9_JOY_BUTTONS + I9_HUT_UP )
#define I9_JOY_H2( joy )		( I9_JOY_FIRSTKEY(joy) + I9_JOY_BUTTONS + I9_HUT_RIGHT )
#define I9_JOY_H3( joy )		( I9_JOY_FIRSTKEY(joy) + I9_JOY_BUTTONS + I9_HUT_DOWN )
#define I9_JOY_H4( joy )		( I9_JOY_FIRSTKEY(joy) + I9_JOY_BUTTONS + I9_HUT_LEFT )

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
