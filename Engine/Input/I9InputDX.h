///////////////////////////////////////////////////////////////////////////////////////////////////
// I9InputDX.h
// Platform implementation
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __I9INPUTDX_H__
#define __I9INPUTDX_H__

#include "dinput.h"
#include "I9Input.h"
#include "I9DeviceDX.h"

class i9InputDX : public i9Input
{
public:
						i9InputDX();
virtual					~i9InputDX();

virtual	BOOL			Init( HWND hwnd, HINSTANCE hinstance );
virtual	void			Done();
virtual	void			Acquire();
virtual	void			Unacquire();

// devices
virtual	BOOL			DeviceInit( int device );
virtual	void			DeviceDone( int device );
virtual	BOOL			DeviceIsPresent( int device );
virtual	void			DeviceUpdate( int device );
virtual	void			DeviceClear( int device );
virtual	BOOL			DeviceFFInit( int device );
virtual	void			DeviceFFSet( int device, int mag, int per );
virtual	void			DeviceFFPlay( int device );
virtual	void			DeviceFFStop( int device );
virtual	BOOL			DeviceFFIsPlaying( int device );

static	BOOL CALLBACK	EnumJoysticksCallback( const DIDEVICEINSTANCE* didevicei, void* user );
		const char*		ErrorDesc	( int err = -1 );	// get error name
		
		IDirectInput8*	m_di;							// direct input object
		i9DeviceDX*		m_device[I9_DEVICES];			// devices list

private:
static	int				m_tjoyidx;						// temporary: requested joy index 0,1...
static	int				m_tjoyskip;						// temporary: how may joys have been skiped
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
