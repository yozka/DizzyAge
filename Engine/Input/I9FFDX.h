///////////////////////////////////////////////////////////////////////////////////////////////////
// I9FFDX.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __I9FFDX_H__
#define __I9FFDX_H__

#include "dinput.h"
#include "E9System.h"

#define I9_FFTYPE_AUTO			-1
#define I9_FFTYPE_RAMPFORCE		0
#define I9_FFTYPE_SQUARE		1
#define I9_FFTYPE_SINE			2
#define I9_FFTYPE_TRIANGLE 		3
#define I9_FFTYPE_SawtoothUp	4
#define I9_FFTYPE_SawtoothDown 	5
#define I9_FFTYPES				6				// supported ff types

class i9FFDX
{
public:
					i9FFDX	();
virtual				~i9FFDX	();

virtual	BOOL		Init			( IDirectInputDevice8* didevice, int type=-1 );
virtual void		Done			();
virtual void		Play			();
virtual void		Stop			();
virtual	void		Set				( int magnitude, int period ); // magnitude=[0,FFFX_MAXFORCE], period=miliseconds
virtual BOOL		IsPlaying		();

static	BOOL CALLBACK EnumEffectsProc( LPCDIEFFECTINFO pei, LPVOID pv );
static	BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

		IDirectInputEffect*			m_dieffect;
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
