///////////////////////////////////////////////////////////////////////////////////////////////////
// I9Input.h
// Generic input class. Platform input is derived from this.
// Interface:
// I9_Init, I9_Done, I9_IsReady, I9_Update, I9_Clear, I9_Aquire, I9_Unacquire
// I9_DeviceInit, I9_DeviceDone, I9_DeviceIsPresent, ...
// I9_IsKeyPressed, I9_GetKeyValue, I9_GetKeyDown, I9_GetKeyUp
// I9_GetKeyQCount, I9_GetKeyQCode, I9_GetKeyQValue, I9_ClearKeyQ
// I9_GetKeyAscii, I9_GetKeyShifted, I9_GetKeyName, I9_FindKeyByAscii, I9_FindKeyByName
// I9_GetAxeValue, I9_GetAxeDelta, I9_SetAxeClip, I9_SetAxeSpeed
// ...
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __I9INPUT_H__
#define __I9INPUT_H__

#include "E9System.h"
#include "E9Engine.h"
#include "D9Debug.h"
#include "I9Def.h"

#define I9_API_DEFAULT			0				// default api

// devices
#define I9_DEVICE_KEYBOARD		0				// id for keyboard device
#define I9_DEVICE_MOUSE			1				// id for mouse device
#define I9_DEVICE_JOYSTICK1		2				// id for joystick device
#define I9_DEVICE_JOYSTICK2		3				// id for joystick device
#define I9_DEVICES				4				// supported devices count

#define	I9_QUEUESIZE			64				// size of key queue
#define	I9_KEYMASK_VALUE		0xf000			// key queue mask for value state
#define	I9_KEYMASK_CODE			0x0fff			// key queue mask for key code
#define	I9_KEYUNKNOWN			"unknown"		// unknwn name
#define I9_TRESHHOLD			0.18f			// axes bogus

struct i9Key
{
	int	m_value;			// value of the key 0=off, 1=on
	int	m_frm;				// frame ( refresh ) count
};								

struct i9Axe
{
	int	m_value;			// value of the axe
	int	m_delta;			// delta value of a key ( axe ) from the last refresh
	int	m_min;				// min clipping value
	int	m_max;				// max clipping value
	int	m_speed;			// speed multiplier factor
};

struct i9KeyName
{
	char		m_ascii;	// ascii character
	char		m_shifted;	// ascii character when shift is pressed
	const char*	m_name;		// key name
};

class i9Input
{
public:

// initialize
						i9Input();
virtual					~i9Input();

virtual	BOOL			Init( HWND hwnd, HINSTANCE hinstance );			// init input
virtual	void			Done();											// done input
virtual void			Update( float dtime );							// refresh keys for installed devices
virtual void			Clear();										// clear keys and axes for installed devices
virtual	void			Acquire();										// aquire input; call to aquire when input is needed again
virtual	void			Unacquire();										// unaquire input; call to unaquire when input is not needed for a while

// devices
virtual	BOOL			DeviceInit( int device );						// install device
virtual	void			DeviceDone( int device );						// uninstall device
virtual	BOOL			DeviceIsPresent( int device );					// if device is installed
virtual	void			DeviceUpdate( int device );						// update device to refresh it's keys and axes
virtual	void			DeviceClear( int device );						// clear device to reset it's keys and axes
virtual	BOOL			DeviceFFInit( int device );						// init force feedback support on a device
virtual	void			DeviceFFSet( int device, int mag, int per );	// set force feedback magnitude and period properties on a device
virtual	void			DeviceFFPlay( int device );						// play force feedback on a device
virtual	void			DeviceFFStop( int device );						// stop force feedback on a device
virtual	BOOL			DeviceFFIsPlaying( int device );				// test if force feedback is playing on a device
		
// keys management
		void			PushKey( int key, BOOL value );					// Push a key in the stack (set key too)
		dword			PopKey();										// Pop a key from the stack
		dword			PeekKey();										// return key from the stack - do not pop

// keys access
inline	i9Key&			GetKey( int key )								{ return m_key[key];		}
inline	BOOL			GetKeyValue( int key )							{ return m_key[key].m_value;	}
inline	BOOL			GetKeyDown( int key )							{ return (  m_key[key].m_value && m_key[key].m_frm == m_frm ); }
inline	BOOL			GetKeyUp( int key )								{ return ( !m_key[key].m_value && m_key[key].m_frm == m_frm ); }
																
inline	int				GetKeyQCount()									{ return m_nkq; }
inline	int				GetKeyQCode( int e=0 )							{ return m_keyq[e] & I9_KEYMASK_CODE; }
inline	BOOL			GetKeyQValue( int e=0 )							{ return (m_keyq[e] & I9_KEYMASK_VALUE)?1:0; }
inline	void			ClearKeyQ()										{ m_nkq = 0; }
																
inline	char			GetKeyAscii( int key )							{ if( key<0 || key>=I9_KEYS ) return 0; else return m_keyname[ key ].m_ascii; }
inline	char			GetKeyShifted( int key )						{ if( key<0 || key>=I9_KEYS ) return 0; else return m_keyname[ key ].m_shifted; }
inline	const char*		GetKeyName( int key )							{ if( key<0 || key>=I9_KEYS ) return m_keyname[0].m_name; else return m_keyname[ key ].m_name; }
		int				FindKeyByAscii( char ascii );					// find key (0=unknown if not found)
		int				FindKeyByName( char* name );					// find key (0=unknown if not found)

// axes access
inline	i9Axe&			GetAxe( int axe )								{ return m_axe[axe]; }
inline	int				GetAxeValue( int axe )							{ return m_axe[axe].m_value; }
inline	int				GetAxeDelta( int axe )							{ return m_axe[axe].m_delta; }
inline	void			SetAxeClip( int axe, int min, int max )			{ m_axe[axe].m_min = min; m_axe[axe].m_max = max; }
inline	void			SetAxeSpeed( int axe, int speed )				{ m_axe[axe].m_speed = speed; }

public:
		HWND		m_hwnd;											// hwnd
		HINSTANCE	m_hinstance;									// hinstance
		int			m_frm;											// crt input frame (refresh count)
		float		m_time;											// crt input time
		i9Key*		m_key;											// keys map
		i9Axe*		m_axe;											// axes map
		dword*		m_keyq;											// que buffer

private:
static	i9KeyName	m_keyname[ I9_KEYS ];							// key names
		int			m_nkq;											// key que entries
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
extern	i9Input*	i9_input;		// global instance, created by platform

		BOOL	I9_Init( HWND hwnd, HINSTANCE hinstance, int api=I9_API_DEFAULT );					// init depending on the platform api
		void	I9_Done();																			// done
inline	BOOL	I9_IsReady()									{ return (i9_input!=NULL); }							
inline	void	I9_Update( float dtime )						{ guardfast(I9_Update);				sassert(i9_input); i9_input->Update(dtime); unguardfast(); }
inline	void	I9_Clear()										{ guardfast(I9_Clear);				sassert(i9_input); i9_input->Clear(); unguardfast(); }
inline	void	I9_Acquire()									{ guardfast(I9_Acquire);			sassert(i9_input); i9_input->Acquire(); unguardfast(); }
inline	void	I9_Unacquire()									{ guardfast(I9_Uacquire);			sassert(i9_input); i9_input->Unacquire(); unguardfast(); }
inline	BOOL	I9_DeviceInit( int device )						{ guardfast(I9_DeviceInit);			sassert(i9_input); return i9_input->DeviceInit(device); unguardfast(); }
inline	void	I9_DeviceDone( int device )						{ guardfast(I9_DeviceDone);			sassert(i9_input); i9_input->DeviceDone(device); unguardfast(); }
inline	BOOL	I9_DeviceIsPresent( int device )				{ guardfast(I9_DeviceIsPresent);	sassert(i9_input); return i9_input->DeviceIsPresent(device); unguardfast(); }
inline	BOOL	I9_DeviceFFInit( int device	)					{ guardfast(I9_DeviceFFInit);		sassert(i9_input); return i9_input->DeviceFFInit(device); unguardfast(); }
inline	void	I9_DeviceFFSet( int device, int mag, int per )	{ guardfast(I9_DeviceFFSet);		sassert(i9_input); i9_input->DeviceFFSet(device,mag,per); unguardfast(); }
inline	void	I9_DeviceFFPlay( int device )					{ guardfast(I9_DeviceFFPlay);		sassert(i9_input); i9_input->DeviceFFPlay(device); unguardfast(); }
inline	void	I9_DeviceFFStop( int device )					{ guardfast(I9_DeviceFFStop);		sassert(i9_input); i9_input->DeviceFFStop(device); unguardfast(); }
inline	BOOL	I9_DeviceFFIsPlaying( int device )				{ guardfast(I9_DeviceFFIsPlaying);	sassert(i9_input); return i9_input->DeviceFFIsPlaying(device); unguardfast(); }

inline	BOOL	I9_IsKeyPressed()								{ guardfast(I9_IsKeyPressed);		sassert(i9_input); for(int i=I9_KEYBOARD_FIRSTKEY; i<I9_KEYBOARD_FIRSTKEY+I9_KEYBOARD_KEYS; i++) if( i9_input->GetKeyDown(i) ) return i; return 0; unguardfast(); }
inline	BOOL	I9_GetKeyValue( int key )						{ guardfast(I9_GetKeyValue);		sassert(i9_input); return i9_input->GetKeyValue(key); unguardfast(); }
inline	BOOL	I9_GetKeyDown( int key )						{ guardfast(I9_GetKeyDown);			sassert(i9_input); return i9_input->GetKeyDown(key); unguardfast(); }
inline	BOOL	I9_GetKeyUp( int key )							{ guardfast(I9_GetKeyUp);			sassert(i9_input); return i9_input->GetKeyUp(key); unguardfast(); }

inline	int		I9_GetKeyQCount()								{ guardfast(I9_GetKeyQCount);		sassert(i9_input); return i9_input->GetKeyQCount(); unguardfast(); }
inline	int		I9_GetKeyQCode( int e=0 )						{ guardfast(I9_GetKeyQCode);		sassert(i9_input); return i9_input->GetKeyQCode(e); unguardfast(); }
inline	BOOL	I9_GetKeyQValue( int e=0 )						{ guardfast(I9_GetKeyQValue);		sassert(i9_input); return i9_input->GetKeyQValue(e); unguardfast(); }
inline	void	I9_ClearKeyQ()									{ guardfast(I9_ClearKeyQ);			sassert(i9_input); i9_input->ClearKeyQ(); unguardfast(); }

inline	char		I9_GetKeyAscii( int key )					{ guardfast(I9_GetKeyAscii);		sassert(i9_input); return i9_input->GetKeyAscii(key); unguardfast(); }
inline	char		I9_GetKeyShifted( int key )					{ guardfast(I9_GetKeyShifted);		sassert(i9_input); return i9_input->GetKeyShifted(key); unguardfast(); }
inline	const char*	I9_GetKeyName( int key )					{ guardfast(I9_GetKeyName);			sassert(i9_input); return i9_input->GetKeyName(key); unguardfast(); }
inline	int			I9_FindKeyByAscii( char ascii )				{ guardfast(I9_FindKeyByAscii);		sassert(i9_input); return i9_input->FindKeyByAscii(ascii); unguardfast(); }
inline	int			I9_FindKeyByName( char* name )				{ guardfast(I9_FindKeyByName);		sassert(i9_input); return i9_input->FindKeyByName(name); unguardfast(); }

inline	int		I9_GetAxeValue( int axe )						{ guardfast(I9_GetAxeValue);		sassert(i9_input); return i9_input->GetAxeValue(axe); unguardfast(); }
inline	int		I9_GetAxeDelta( int axe )						{ guardfast(I9_GetAxeDelta);		sassert(i9_input); return i9_input->GetAxeDelta(axe); unguardfast(); }
inline	void	I9_SetAxeClip( int axe, int min, int max )		{ guardfast(I9_SetAxeClip);			sassert(i9_input); i9_input->SetAxeClip(axe,min,max); unguardfast(); }
inline	void	I9_SetAxeSpeed( int axe, int speed )			{ guardfast(I9_SetAxeSpeed);		sassert(i9_input); i9_input->SetAxeSpeed(axe,speed); unguardfast(); }

inline	int		I9_GetMouseX()									{ guardfast(I9_GetMouseX);			sassert(i9_input); return i9_input->GetAxeValue( I9_MOUSE_X ); unguardfast(); }
inline	int		I9_GetMouseY()									{ guardfast(I9_GetMouseY);			sassert(i9_input); return i9_input->GetAxeValue( I9_MOUSE_Y ); unguardfast(); }
inline	int		I9_GetMouseDX()									{ guardfast(I9_GetMouseDX);			sassert(i9_input); return i9_input->GetAxeDelta( I9_MOUSE_X ); unguardfast(); }
inline	int		I9_GetMouseDY()									{ guardfast(I9_GetMouseDY);			sassert(i9_input); return i9_input->GetAxeDelta( I9_MOUSE_Y ); unguardfast(); }
inline	int		I9_GetMouseLB()									{ guardfast(I9_GetMouseLB);			sassert(i9_input); return i9_input->GetKeyValue( I9_MOUSE_B1 ); unguardfast(); }
inline	int		I9_GetMouseRB()									{ guardfast(I9_GetMouseRB);			sassert(i9_input); return i9_input->GetKeyValue( I9_MOUSE_B2 ); unguardfast(); }
inline	int		I9_GetMouseClick()								{ guardfast(I9_GetMouseClick);		sassert(i9_input); return i9_input->GetKeyDown( I9_MOUSE_B1 ); unguardfast(); }
inline	void	I9_SetMouseX( int x )							{ guardfast(I9_SetMouseX);			sassert(i9_input); i9_input->GetAxe( I9_MOUSE_X ).m_value = x; unguardfast(); }
inline	void	I9_SetMouseY( int y )							{ guardfast(I9_SetMouseY);			sassert(i9_input); i9_input->GetAxe( I9_MOUSE_Y ).m_value = y; unguardfast(); }
inline	void	I9_ClipMouse( int x0, int y0, int x1, int y1 )	{ guardfast(I9_ClipMouse);			sassert(i9_input); i9_input->SetAxeClip( I9_MOUSE_X, x0, x1 ); i9_input->SetAxeClip( I9_MOUSE_Y, y0, y1 ); unguardfast(); }
inline	BOOL	I9_MouseMoved()									{ guardfast(I9_MouseMoved);			sassert(i9_input); return i9_input->GetAxeDelta( I9_MOUSE_X ) || i9_input->GetAxeDelta( I9_MOUSE_Y ); unguardfast(); }
inline	BOOL	I9_MousePressed()								{ guardfast(I9_MousePressed);		sassert(i9_input); for(int i=I9_MOUSE_B1; i<I9_MOUSE_B8; i++) if( i9_input->GetKeyDown(i) ) return i; return 0; unguardfast(); }

inline	int		I9_GetJoystickX( int joy )						{ guardfast(I9_GetJoystickX);		sassert(i9_input); return i9_input->GetAxeValue( I9_JOY_X(joy) ); unguardfast(); }
inline	int		I9_GetJoystickY( int joy )						{ guardfast(I9_GetJoystickY);		sassert(i9_input); return i9_input->GetAxeValue( I9_JOY_Y(joy) ); unguardfast(); }
inline	int		I9_GetJoystickB1( int joy )						{ guardfast(I9_GetJoystickB1);		sassert(i9_input); return i9_input->GetKeyValue( I9_JOY_B1(joy) ); unguardfast(); }
inline	int		I9_GetJoystickB2( int joy )						{ guardfast(I9_GetJoystickB2);		sassert(i9_input); return i9_input->GetKeyValue( I9_JOY_B2(joy) ); unguardfast(); }
inline	int		I9_GetJoystickB3( int joy )						{ guardfast(I9_GetJoystickB3);		sassert(i9_input); return i9_input->GetKeyValue( I9_JOY_B3(joy) ); unguardfast(); }
inline	int		I9_GetJoystickB4( int joy )						{ guardfast(I9_GetJoystickB4);		sassert(i9_input); return i9_input->GetKeyValue( I9_JOY_B4(joy) ); unguardfast(); }
inline	int		I9_GetJoystickHAT( int joy, int hatdir )		{ guardfast(I9_GetJoystickHAT);		sassert(i9_input); return i9_input->GetKeyValue( I9_JOY_H1(joy)+hatdir ); unguardfast(); }

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
