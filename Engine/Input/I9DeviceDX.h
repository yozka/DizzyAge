///////////////////////////////////////////////////////////////////////////////////////////////////
// I9DeviceDX.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __I9DEVICEDX_H__
#define __I9DEVICEDX_H__

#include "dinput.h"
#include "E9System.h"
#include "I9Def.h"
#include "I9FFDX.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// DEVICE
///////////////////////////////////////////////////////////////////////////////////////////////////
class i9DeviceDX
{
public:
					i9DeviceDX();
virtual				~i9DeviceDX();

virtual	BOOL		Init( int idx, const GUID* guid );
virtual void		Done();
virtual void		Update()							{};
virtual void		Clear()								{};
virtual	BOOL		Acquire();							// aquire this device
virtual BOOL		Unacquire();						// unaquire this device

public:
		BOOL		m_acquired;							// acquired status
		int			m_idx;								// device index (for joysticks)
		i9FFDX*		m_ff;								// force feedback

		IDirectInputDevice8*		m_didevice;			// direct input device object

private:
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// KEYBOARD
///////////////////////////////////////////////////////////////////////////////////////////////////
class i9DeviceDXKeyboard : public i9DeviceDX
{
public:
					i9DeviceDXKeyboard();
virtual				~i9DeviceDXKeyboard();

virtual BOOL		Init( int idx, const GUID* guid );
virtual void		Update();
virtual void		Clear();

private:
static	int			m_dxcode[I9_KEYBOARD_KEYS];	// conversion buffer from DirectX key codes to our key codes
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// MOUSE
///////////////////////////////////////////////////////////////////////////////////////////////////
class i9DeviceDXMouse : public i9DeviceDX
{
public:
						i9DeviceDXMouse();
	virtual				~i9DeviceDXMouse();

	virtual BOOL		Init( int idx, const GUID* guid );
	virtual void		Update();
	virtual void		Clear();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// JOYSTICK
///////////////////////////////////////////////////////////////////////////////////////////////////
class i9DeviceDXJoystick : public i9DeviceDX
{
public:
					i9DeviceDXJoystick();
virtual				~i9DeviceDXJoystick();

virtual BOOL		Init( int idx, const GUID* guid );
virtual void		Update();
virtual void		Clear();
		int			Filter( int value );
//virtual cFFFX*	CreateFFFX( int type=-1 );

			int			m_axeavail[I9_JOY_AXES];
			int			m_keyavail[I9_JOY_KEYS];
private:
			int			m_hat;
			int			m_needpolling;
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////

