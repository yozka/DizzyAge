///////////////////////////////////////////////////////////////////////////////////////////////////
// I9DeviceDX.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "I9InputDX.h"

#define I9_KEYBOARD_BUFFERSIZE	64
#define I9_MOUSE_BUFFERSIZE		64
#define I9_JOYSTICK_BUFFERSIZE	64

#define I9_DI					(((i9InputDX*)i9_input)->m_di)
#define I9_LOGERROR( prefix )	dlog( LOGINP, "INPUT: %s (%s)\n", prefix, ((i9InputDX*)i9_input)->ErrorDesc(err) );

///////////////////////////////////////////////////////////////////////////////////////////////////
// DEVICE
///////////////////////////////////////////////////////////////////////////////////////////////////
i9DeviceDX::i9DeviceDX()
{
	guard(i9DeviceDX::i9DeviceDX);
	m_acquired	= FALSE;
	m_idx		= 0;
	m_ff		= NULL;
	m_didevice	= NULL;
	unguard();
}

i9DeviceDX::~i9DeviceDX()
{
	guard(i9DeviceDX:~i9DeviceDX);
	unguard();
}

BOOL i9DeviceDX::Init( int idx, const GUID* guid )
{
	guard(i9DeviceDX::Init);
	return TRUE;
	unguard();
}

void i9DeviceDX::Done()
{
	guard(i9DeviceDX::Done);
	if( m_ff ) { m_ff->Done(); sdelete(m_ff); m_ff=NULL; }
	if( m_didevice ) { m_didevice->Release(); m_didevice = NULL; }
	unguard();
}

BOOL i9DeviceDX::Acquire()
{
	guard(i9DeviceDX::Acquire);
	sassert(m_didevice);
	int err = m_didevice->Acquire();
//	if(FAILED(err))	{ I9_LOGERROR( "InputDevice acquire failed" ); return FALSE; }
	Clear();
	m_acquired = TRUE;
	return TRUE;
	unguard();
}

BOOL i9DeviceDX::Unacquire()
{
	guard(i9DeviceDX::Unacquire);
	sassert(m_didevice);
	int err = m_didevice->Unacquire();
	if(FAILED(err)) { I9_LOGERROR( "InputDevice unacquire failed" ); return FALSE; }
	Clear();
	m_acquired = FALSE;
	return TRUE;
	unguard();
}

/*
void i9DeviceDX::DestroyFFFX( cFFFX* fffx )
{
	guard(i9DeviceDX::DestroyFFFX);
	if(!fffx) return; 
	fffx->Done(); 
	sdelete(fffx); 
	unguard();
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
// KEYBOARD
///////////////////////////////////////////////////////////////////////////////////////////////////
i9DeviceDXKeyboard::i9DeviceDXKeyboard()
{
	guard(i9DeviceDXKeyboard::i9DeviceDXKeyboard)
	i9DeviceDX::i9DeviceDX();
	unguard()
}

i9DeviceDXKeyboard::~i9DeviceDXKeyboard()
{
	guard(i9DeviceDXKeyboard::~i9DeviceDXKeyboard)
	unguard()
}

BOOL i9DeviceDXKeyboard::Init( int idx, const GUID* guid )
{
	guard(i9DeviceDXKeyboard::Init);
	// test the conversion table
	sassert(sizeof(m_dxcode)==I9_KEYBOARD_KEYS*sizeof(int)); 
	//for(int i=0;i<I9_KEYBOARD_KEYS;i++)
	//	dlog("i9k=%x dik=%x %s\n",i,m_dxcode[i],I9_GetKeyName(i));

	sassert(m_didevice==NULL);
	int err;
	m_idx = idx;

	err = I9_DI->CreateDevice( *guid, &m_didevice, NULL );
	if(FAILED(err)) { I9_LOGERROR( "KEYBOARD CreateDevice failed" ); goto error; }

	err = m_didevice->SetDataFormat( &c_dfDIKeyboard );
	if(FAILED(err))	{ I9_LOGERROR( "KEYBOARD SetDataFormat failed" ); goto error; }

	err = m_didevice->SetCooperativeLevel( i9_input->m_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND );
	if(FAILED(err)) { I9_LOGERROR( "KEYBOARD SetCooperativeLevel" ); goto error; }

	DIPROPDWORD	dip;
	dip.diph.dwSize			= sizeof(DIPROPDWORD);
	dip.diph.dwHeaderSize	= sizeof(DIPROPHEADER);
	dip.diph.dwObj			= 0;
	dip.diph.dwHow			= DIPH_DEVICE;
	dip.dwData				= I9_KEYBOARD_BUFFERSIZE;
	err = m_didevice->SetProperty( DIPROP_BUFFERSIZE, &dip.diph );
	if(FAILED(err)) { I9_LOGERROR( "KEYBOARD SetProperty failed" ); goto error; }

	if(!Acquire())  { I9_LOGERROR( "KEYBOARD can't aquire" ); goto error; }

	return TRUE;

error:
	Done();
	return FALSE;
	unguard();
}

void i9DeviceDXKeyboard::Update()
{
	guard(i9DeviceDXKeyboard::Update);
	sassert(m_didevice);
	if( !m_acquired ) return;

	// get data
	DIDEVICEOBJECTDATA didod[I9_KEYBOARD_BUFFERSIZE];
	unsigned long elements = I9_KEYBOARD_BUFFERSIZE;
	int err = m_didevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), didod, &elements, 0 );
	if(FAILED(err))
	{
		if( err==DIERR_INPUTLOST )
		{
			dlog(LOGINP, "Input lost\n");
			Acquire();
			return;
		}
		I9_LOGERROR( "KEYBOARD GetDeviceData failed" );
		return;
	}

	for(int i=0; i<(int)elements; i++)
	{
		if( didod[i].dwData & 0x80 )	i9_input->PushKey( m_dxcode[didod[i].dwOfs], TRUE );
		else							i9_input->PushKey( m_dxcode[didod[i].dwOfs], FALSE );
	}
	unguard();
}

void i9DeviceDXKeyboard::Clear()
{
	guard(i9DeviceDXKeyboard::Clear);
	sassert(m_didevice);
	memset( &(i9_input->m_key[I9_KEYBOARD_FIRSTKEY]), 0, I9_KEYBOARD_KEYS * sizeof(i9Key) );
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// i9DeviceDXMouse
//////////////////////////////////////////////////////////////////////////////////////////////////
i9DeviceDXMouse::i9DeviceDXMouse()
{
	guard(i9DeviceDXMouse::i9DeviceDXMouse);
	i9DeviceDX::i9DeviceDX();
	unguard();
}

i9DeviceDXMouse::~i9DeviceDXMouse()
{
	guard(i9DeviceDXMouse::~i9DeviceDXMouse);
	unguard();
}

BOOL i9DeviceDXMouse::Init( int idx, const GUID* guid )
{
	guard(i9DeviceDXMouse::Init);
	sassert(m_didevice==NULL);
	int err;
	m_idx = idx;

	err = I9_DI->CreateDevice( *guid, &m_didevice, NULL );
	if(FAILED(err)) { I9_LOGERROR( "MOUSE CreateDevice failed" ); goto error; }

	err = m_didevice->SetDataFormat( &c_dfDIMouse2 );
	if(FAILED(err)) { I9_LOGERROR( "MOUSE SetDataFormat failed" ); goto error; }

	err = m_didevice->SetCooperativeLevel( i9_input->m_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND );
	if(FAILED(err))	{ I9_LOGERROR( "MOUSE SetCooperativeLevel failed" ); goto error; }

	DIPROPDWORD	dip;
	dip.diph.dwSize			= sizeof(DIPROPDWORD);
	dip.diph.dwHeaderSize	= sizeof(DIPROPHEADER);
	dip.diph.dwObj			= 0;
	dip.diph.dwHow			= DIPH_DEVICE;
	dip.dwData				= I9_MOUSE_BUFFERSIZE;
	err = m_didevice->SetProperty( DIPROP_BUFFERSIZE, &dip.diph );
	if(FAILED(err)) { I9_LOGERROR( "MOUSE SetProperty failed "); goto error; }

	if(!Acquire())  { I9_LOGERROR( "MOUSE can't aquire" ); goto error; }

	return TRUE;

	error:
	Done();
	return FALSE;
	unguard();
}

void i9DeviceDXMouse::Update()
{
	guard(i9DeviceDXMouse::Update);
	sassert(m_didevice);
	if( !m_acquired ) return;

	// reset axes delta
	i9Axe& axex = i9_input->GetAxe( I9_MOUSE_X );
	i9Axe& axey = i9_input->GetAxe( I9_MOUSE_Y );
	i9Axe& axez = i9_input->GetAxe( I9_MOUSE_Z );
	axex.m_delta = 0;
	axey.m_delta = 0;
	axez.m_delta = 0;
	
	// get data
	DIDEVICEOBJECTDATA didod[I9_MOUSE_BUFFERSIZE];
	unsigned long elements = I9_MOUSE_BUFFERSIZE;
	int err = m_didevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), didod, &elements, 0 );
	if(FAILED(err))
	{
		if( err==DIERR_INPUTLOST )
		{
			dlog(LOGINP, "Input lost\n");
			Acquire();
			return;
		}
		I9_LOGERROR("MOUSE GetDeviceData failed");
		return;
	}

	for(int i=0; i<(int)elements; i++)
	{
		int delta;
		int key;
		switch(didod[i].dwOfs)
		{
		
		// axes
		case DIMOFS_X:
			delta = *((int*)&didod[i].dwData);
			axex.m_delta	+= delta;
			break;
		
		case DIMOFS_Y:
			delta = *((int*)&didod[i].dwData);
			axey.m_delta = delta;
			break;
		
		case DIMOFS_Z:
			delta = *((int*)&didod[i].dwData);
			axez.m_delta = delta;
			break;

		// keys
		case DIMOFS_BUTTON0:
		case DIMOFS_BUTTON1:
		case DIMOFS_BUTTON2:
		case DIMOFS_BUTTON3:
		case DIMOFS_BUTTON4:
		case DIMOFS_BUTTON5:
		case DIMOFS_BUTTON6:
		case DIMOFS_BUTTON7:
			key = I9_MOUSE_B1 + didod[i].dwOfs - DIMOFS_BUTTON0;
			if( didod[i].dwData & 0x80 )	i9_input->PushKey(key, TRUE);
			else							i9_input->PushKey(key, FALSE);
		}
	}

	// fake wheel - not qued
	if(axez.m_delta>0)
	{
		i9_input->m_key[I9_MOUSE_WHEELUP].m_value = TRUE;
		i9_input->m_key[I9_MOUSE_WHEELUP].m_frm = i9_input->m_frm;		
	}
	else
	if(i9_input->m_key[I9_MOUSE_WHEELUP].m_value)
	{
		i9_input->m_key[I9_MOUSE_WHEELUP].m_value = FALSE;	
		i9_input->m_key[I9_MOUSE_WHEELUP].m_frm = i9_input->m_frm; 
	}
	if(axez.m_delta<0)
	{
		i9_input->m_key[I9_MOUSE_WHEELDN].m_value = TRUE;
		i9_input->m_key[I9_MOUSE_WHEELDN].m_frm = i9_input->m_frm;		
	}
	else
	if(i9_input->m_key[I9_MOUSE_WHEELDN].m_value)
	{
		i9_input->m_key[I9_MOUSE_WHEELDN].m_value = FALSE;	
		i9_input->m_key[I9_MOUSE_WHEELDN].m_frm = i9_input->m_frm; 
	}

	// set axes values and clip
	axex.m_value += axex.m_delta * axex.m_speed;
	axey.m_value += axey.m_delta * axey.m_speed;
	axez.m_value += axez.m_delta * axez.m_speed;
	
	if( axex.m_value < axex.m_min ) axex.m_value = axex.m_min;	else
	if( axex.m_value > axex.m_max ) axex.m_value = axex.m_max;
	if( axey.m_value < axey.m_min ) axey.m_value = axey.m_min;	else
	if( axey.m_value > axey.m_max ) axey.m_value = axey.m_max;
	if( axez.m_value < axez.m_min ) axez.m_value = axez.m_min;	else
	if( axez.m_value > axez.m_max ) axez.m_value = axez.m_max;

	unguard();
}


void i9DeviceDXMouse::Clear()
{
	guard(i9DeviceDXMouse::Clear);
	memset( &(i9_input->m_key[I9_MOUSE_FIRSTKEY]), 0, I9_MOUSE_KEYS * sizeof(i9Key) );
	for(int i=I9_MOUSE_FIRSTAXE; i<I9_MOUSE_FIRSTAXE+I9_MOUSE_AXES; i++)
	{
		i9Axe& axe = i9_input->GetAxe(i);
		axe.m_value	= 0;
		axe.m_delta	= 0;
	}
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// JOYSTICK
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EnumJoystickObjectsCallback( LPCDIDEVICEOBJECTINSTANCE dideviceoi, LPVOID pvRef );

i9DeviceDXJoystick::i9DeviceDXJoystick()
{
	guard(i9DeviceDXJoystick::i9DeviceDXJoystick);
	i9DeviceDX::i9DeviceDX();
	m_hat = 0;
	m_needpolling = 0;
	memset(m_axeavail,0,sizeof(m_axeavail));
	memset(m_keyavail,0,sizeof(m_keyavail));
	unguard();
}

i9DeviceDXJoystick::~i9DeviceDXJoystick()
{
	guard(i9DeviceDXJoystick::~i9DeviceDXJoystick);
	unguard();
}

BOOL i9DeviceDXJoystick::Init( int idx, const GUID* guid )
{
	guard(i9DeviceDXJoystick::Init);
	sassert(m_didevice==NULL);
	int err;
	m_idx = idx;

	err = I9_DI->CreateDevice( *guid, &m_didevice, NULL );
	if(FAILED(err)) { I9_LOGERROR( "JOYSTICK CreateDevice failed" ); goto error; }

	err = m_didevice->SetDataFormat( &c_dfDIJoystick );
	if(FAILED(err))	{ I9_LOGERROR( "JOYSTICK SetDataFormat failed" ); goto error; }

	err = m_didevice->SetCooperativeLevel( i9_input->m_hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND );
	if(FAILED(err))	{ I9_LOGERROR( "JOYSTICK SetCooperativeLevel" ); goto error; }

	DIPROPDWORD	dip;
	dip.diph.dwSize			= sizeof(DIPROPDWORD);
	dip.diph.dwHeaderSize	= sizeof(DIPROPHEADER);
	dip.diph.dwObj			= 0;
	dip.diph.dwHow			= DIPH_DEVICE;
	dip.dwData				= I9_JOYSTICK_BUFFERSIZE;
	err = m_didevice->SetProperty(DIPROP_BUFFERSIZE, &dip.diph);
	if(FAILED(err)) { I9_LOGERROR( "JOYSTICK SetProperty failed" ); goto error; }

	DIDEVCAPS dicaps;
	memset(&dicaps, 0, sizeof(DIDEVCAPS));
	dicaps.dwSize = sizeof(DIDEVCAPS);
	m_didevice->GetCapabilities(&dicaps);
	if(dicaps.dwFlags & DIDC_POLLEDDEVICE) m_needpolling = TRUE;

	// enum device objects and set available axes and keys
	m_didevice->EnumObjects( EnumJoystickObjectsCallback, this, DIDFT_AXIS|DIDFT_BUTTON|DIDFT_POV );

	// somehow I can't aquire right now...
	// if(!Acquire())  { I9_LOGERROR( "JOYSTICK can't aquire" ); goto error; }

	return TRUE;

	error:
	Done();
	return FALSE;
	unguard();
}

void i9DeviceDXJoystick::Update()
{
	guard(i9DeviceDXJoystick::Update);
	sassert(m_didevice);
	if( !m_acquired ) return;

	if( m_needpolling )
		m_didevice->Poll();

	// get data
	m_didevice->Acquire(); // make sure it's really acquired
	DIDEVICEOBJECTDATA didod[I9_JOYSTICK_BUFFERSIZE];
	unsigned long elements = I9_JOYSTICK_BUFFERSIZE;
	int err = m_didevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), didod, &elements, 0 );
	if(FAILED(err))
	{
		// don't log anything, in case joystick was removed
		// I9_LOGERROR("JOYSTICK GetDeviceData failed");
		// if( err==DIERR_INPUTLOST )
		// 	dlog(LOGINP, "Input lost\n");
		Clear();
		return;
	}

	int key;
	for(int i=0; i<(int)elements; i++)
	{
		if(didod[i].dwOfs>=DIJOFS_BUTTON0 && didod[i].dwOfs<=DIJOFS_BUTTON0+I9_JOY_BUTTONS)
		{
			key = I9_JOY_B1(m_idx) + (didod[i].dwOfs-DIJOFS_BUTTON0);
			if(didod[i].dwData & 0x80)		i9_input->PushKey(key, TRUE);
			else							i9_input->PushKey(key, FALSE);
		}
		else
		if(didod[i].dwOfs==DIJOFS_POV(0)) // more hats can be supported here...
		{
			int pov; // -1=none, 0=up, 1=up-right, ... 7=left-up
			int povmask[9] = { 0, 1, 1+2, 2, 2+4, 4, 4+8, 8, 8+1 }; // direction bits for each pov value
			if(*((int*)&(didod[i].dwData))>=0)
				pov = didod[i].dwData / 4500;
			else
				pov = -1;
			
			int hat = povmask[pov+1]; // current hat direction bits

			for(int d=0;d<4;d++) // check each direction: if was set push false, 
			{
				key = I9_JOY_H1(m_idx) + d;
				if( (m_hat & (1<<d)) != (hat & (1<<d)) ) // direction has changed
				{
					if(m_hat & (1<<d))	i9_input->PushKey(key, FALSE);
					if(  hat & (1<<d))	i9_input->PushKey(key, TRUE);
				}
			}
			m_hat = hat; // remember setting
		}
		else
		{
			int axe = -1;
			switch(didod[i].dwOfs)
			{
				case DIJOFS_X:			axe = 0; break;
				case DIJOFS_Y:			axe = 1; break;
				case DIJOFS_Z:			axe = 2; break;
				case DIJOFS_RX:			axe = 3; break;
				case DIJOFS_RY:			axe = 4; break;
				case DIJOFS_RZ:			axe = 5; break;
				case DIJOFS_SLIDER(0):	axe = 6; break;
				case DIJOFS_SLIDER(1):	axe = 7; break;
			};
			if(axe!=-1)
			{
				int value = ((int)(didod[i].dwData) - 32767) * 1000 / 32768;
				i9_input->GetAxe(I9_JOY_X(m_idx)+axe).m_value = Filter(value);
			}
		}
	}

	unguard();
}

void i9DeviceDXJoystick::Clear()
{
	guard(i9DeviceDXJoystick::Clear);
	memset( &(i9_input->m_key[ I9_JOY_FIRSTKEY(m_idx) ]), 0, I9_JOY_KEYS * sizeof(i9Key) );
	for(int i=I9_JOY_FIRSTAXE(m_idx); i<I9_JOY_FIRSTAXE(m_idx)+I9_JOY_AXES; i++)
	{
		i9Axe& axe = i9_input->GetAxe(i);
		axe.m_value	= 0;
		axe.m_delta	= 0;
	}

	m_hat = 0;
	unguard();
}

int i9DeviceDXJoystick::Filter( int value )
{
	guard(i9DeviceDXJoystick::Filter);
	int tresh = (int)(I9_TRESHHOLD*1000.f);
	
	if( value<tresh && -value<tresh ) return 0;
	if( value>0 )		{ value -= tresh; if(value<0) return 0; }
	if( value<0 )		{ value += tresh; if(value>0) return 0; }
	if( value<-1000 )	value=-1000;
	if( value>1000 )	value=1000;
	value = value*1000/(1000-tresh);

	return value;
	unguard();
}

/*
cFFFX* i9DeviceDXJoystick::CreateFFFX( int type )
{
	guard(i9DeviceDXJoystick::CreateFFFX);
	cFFFX* fx = snew cFFFX();
	if(!fx->Init(this, type)) { sdelete(fx); return NULL; }
	return fx;
	unguard();
}
*/

BOOL CALLBACK EnumJoystickObjectsCallback( LPCDIDEVICEOBJECTINSTANCE dideviceoi, LPVOID pvRef )
{
	guard(EnumJoystickObjectsCallback);
	i9DeviceDXJoystick* _this = (i9DeviceDXJoystick*)pvRef;

	int idx = DIDFT_GETINSTANCE(dideviceoi->dwType);
	if( dideviceoi->guidType == GUID_XAxis )	_this->m_axeavail[0] = 1; else
	if( dideviceoi->guidType == GUID_YAxis )	_this->m_axeavail[1] = 1; else
	if( dideviceoi->guidType == GUID_ZAxis )	_this->m_axeavail[2] = 1; else
	if( dideviceoi->guidType == GUID_RxAxis )	_this->m_axeavail[3] = 1; else
	if( dideviceoi->guidType == GUID_RyAxis )	_this->m_axeavail[4] = 1; else
	if( dideviceoi->guidType == GUID_RzAxis )	_this->m_axeavail[5] = 1; else
	if( dideviceoi->guidType == GUID_Slider )	_this->m_axeavail[6] = 1; else
	if( dideviceoi->guidType == GUID_Button && (idx>=0 && idx<I9_JOY_BUTTONS) )	_this->m_keyavail[idx] = 1;  else
	if( dideviceoi->guidType == GUID_POV && (idx>=0 && idx<I9_JOY_HATS) )
	{
		for(int i=0;i<4;i++)
			_this->m_keyavail[I9_JOY_BUTTONS + idx*4 + i] = 1;
	}

	dlog(LOGINP, "  DeviceObject: idx=%2d, name=%s, ofs=%d\n", idx, dideviceoi->tszName, dideviceoi->dwOfs);
	return DIENUM_CONTINUE;
	unguard();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
int	i9DeviceDXKeyboard::m_dxcode[I9_KEYBOARD_KEYS] = 
{
	0					,
	DIK_ESCAPE          ,
	DIK_1               ,
	DIK_2               ,
	DIK_3               ,
	DIK_4               ,
	DIK_5               ,
	DIK_6               ,
	DIK_7               ,
	DIK_8               ,
	DIK_9               ,
	DIK_0               ,
	DIK_MINUS           ,
	DIK_EQUALS          ,
	DIK_BACK            ,
	DIK_TAB             ,
	DIK_Q               ,
	DIK_W               ,
	DIK_E               ,
	DIK_R               ,
	DIK_T               ,
	DIK_Y               ,
	DIK_U               ,
	DIK_I               ,
	DIK_O               ,
	DIK_P               ,
	DIK_LBRACKET        ,
	DIK_RBRACKET        ,
	DIK_RETURN          ,
	DIK_LCONTROL        ,
	DIK_A               ,
	DIK_S               ,
	DIK_D               ,
	DIK_F               ,
	DIK_G               ,
	DIK_H               ,
	DIK_J               ,
	DIK_K               ,
	DIK_L               ,
	DIK_SEMICOLON       ,
	DIK_APOSTROPHE      ,
	DIK_GRAVE           ,
	DIK_LSHIFT          ,
	DIK_BACKSLASH       ,
	DIK_Z               ,
	DIK_X               ,
	DIK_C               ,
	DIK_V               ,
	DIK_B               ,
	DIK_N               ,
	DIK_M               ,
	DIK_COMMA           ,
	DIK_PERIOD          ,
	DIK_SLASH           ,
	DIK_RSHIFT          ,
	DIK_MULTIPLY        ,
	DIK_LMENU           ,
	DIK_SPACE           ,
	DIK_CAPITAL         ,
	DIK_F1              ,
	DIK_F2              ,
	DIK_F3              ,
	DIK_F4              ,
	DIK_F5              ,
	DIK_F6              ,
	DIK_F7              ,
	DIK_F8              ,
	DIK_F9              ,
	DIK_F10             ,
	DIK_NUMLOCK         ,
	DIK_SCROLL          ,
	DIK_NUMPAD7         ,
	DIK_NUMPAD8         ,
	DIK_NUMPAD9         ,
	DIK_SUBTRACT        ,
	DIK_NUMPAD4         ,
	DIK_NUMPAD5         ,
	DIK_NUMPAD6         ,
	DIK_ADD             ,
	DIK_NUMPAD1         ,
	DIK_NUMPAD2         ,
	DIK_NUMPAD3         ,
	DIK_NUMPAD0         ,
	DIK_DECIMAL         ,
	0,
	0,
	DIK_OEM_102         ,
	DIK_F11             ,
	DIK_F12             ,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	DIK_F13             ,
	DIK_F14             ,
	DIK_F15             ,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	DIK_KANA            ,
	0,
	0,
	DIK_ABNT_C1         ,
	0,
	0,
	0,
	0,
	0,
	DIK_CONVERT         ,
	0,
	DIK_NOCONVERT       ,
	0,
	DIK_YEN             ,
	DIK_ABNT_C2         ,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	DIK_NUMPADEQUALS    ,
	0,
	0,
	DIK_PREVTRACK       ,
	DIK_AT              ,
	DIK_COLON           ,
	DIK_UNDERLINE       ,
	DIK_KANJI           ,
	DIK_STOP            ,
	DIK_AX              ,
	DIK_UNLABELED       ,
	0,
	DIK_NEXTTRACK       ,
	0,
	0,
	DIK_NUMPADENTER     ,
	DIK_RCONTROL        ,
	0,
	0,
	DIK_MUTE            ,
	DIK_CALCULATOR      ,
	DIK_PLAYPAUSE       ,
	0,
	DIK_MEDIASTOP       ,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	DIK_VOLUMEDOWN      ,
	0,
	DIK_VOLUMEUP        ,
	0,
	DIK_WEBHOME         ,
	DIK_NUMPADCOMMA     ,
	0,
	DIK_DIVIDE          ,
	0,
	DIK_SYSRQ           ,
	DIK_RMENU           ,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	DIK_PAUSE           ,
	0,
	DIK_HOME            ,
	DIK_UP              ,
	DIK_PRIOR           ,
	0,
	DIK_LEFT            ,
	0,
	DIK_RIGHT           ,
	0,
	DIK_END             ,
	DIK_DOWN            ,
	DIK_NEXT            ,
	DIK_INSERT          ,
	DIK_DELETE          ,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	DIK_LWIN            ,
	DIK_RWIN            ,
	DIK_APPS            ,
	DIK_POWER           ,
	DIK_SLEEP           ,
	0,
	0,
	0,
	DIK_WAKE            ,
	0,
	DIK_WEBSEARCH       ,
	DIK_WEBFAVORITES    ,
	DIK_WEBREFRESH      ,
	DIK_WEBSTOP         ,
	DIK_WEBFORWARD      ,
	DIK_WEBBACK         ,
	DIK_MYCOMPUTER      ,
	DIK_MAIL            ,
	DIK_MEDIASELECT     ,	/*0xED*/
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0					,
	0						/*0xFF*/
};

///////////////////////////////////////////////////////////////////////////////////////////////////

