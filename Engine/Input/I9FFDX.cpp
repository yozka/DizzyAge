//////////////////////////////////////////////////////////////////////////////////////////////////
// I9FFDX.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D9Debug.h"
#include "I9FFDX.h"

#define I9_FF_MAXFORCE		DI_FFNOMINALMAX // 10000

i9FFDX::i9FFDX()
{
	guard(i9FFDX::i9FFDX);
	m_dieffect = NULL;
	unguard();
}

i9FFDX::~i9FFDX()
{
	guard(i9FFDX::~i9FFDX);
	unguard();
}

BOOL i9FFDX::Init( IDirectInputDevice8* didevice, int type )
{
	guard(i9FFDX::Init);
	sassert(didevice);
	HRESULT hr;
    DWORD	rgdwAxes[2]			= { DIJOFS_X, DIJOFS_Y };
    LONG	rglDirection[2]		= { 0, 0 };
	DIPERIODIC diPeriodic;      // type-specific parameters

	DWORD axes = 0;
	hr = didevice->EnumObjects( EnumAxesCallback, (VOID*)&axes, DIDFT_AXIS );
	if(FAILED(hr)) goto error;
	if(axes==0) goto error;
	if(axes>2) axes=2;

	GUID guidEffect;
	if(0<=type && type<I9_FFTYPES) // specific
	{
		const GUID* guids[I9_FFTYPES] = { &GUID_RampForce, &GUID_Square, &GUID_Sine, &GUID_Triangle, &GUID_SawtoothUp, &GUID_SawtoothDown };
		guidEffect = *guids[type];
	}
	else // autodetect
	{
		hr = didevice->EnumEffects( (LPDIENUMEFFECTSCALLBACK)EnumEffectsProc, &guidEffect, DIEFT_PERIODIC );
		if(FAILED(hr)) goto error;
	}

	diPeriodic.dwMagnitude		= I9_FF_MAXFORCE;
	diPeriodic.lOffset			= 0; 
	diPeriodic.dwPhase			= 0; 
	diPeriodic.dwPeriod			= (DWORD)(0.1 * DI_SECONDS); 

    DIEFFECT eff;
    ZeroMemory( &eff, sizeof(eff) );
    eff.dwSize                  = sizeof(DIEFFECT);
    eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.dwDuration              = INFINITE;
    eff.dwSamplePeriod          = 0;
    eff.dwGain                  = DI_FFNOMINALMAX;
    eff.dwTriggerButton         = DIEB_NOTRIGGER;
    eff.dwTriggerRepeatInterval = 0;
    eff.cAxes                   = axes;
    eff.rgdwAxes                = rgdwAxes;
    eff.rglDirection            = rglDirection;
    eff.lpEnvelope              = 0;
    eff.cbTypeSpecificParams    = sizeof(DIPERIODIC);
    eff.lpvTypeSpecificParams   = &diPeriodic;
    eff.dwStartDelay            = 0;

    // Create the prepared effect
    hr = didevice->CreateEffect( guidEffect, &eff, &m_dieffect, NULL );
	if(FAILED(hr)) goto error;
	return TRUE;

	error:
	dlog(LOGINP,"INPUT: FFX not supported.");
	return FALSE;
	unguard();
}

void i9FFDX::Done()
{
	guard(i9FFDX::Done);
	if(!m_dieffect) return;
	m_dieffect->Stop();
	m_dieffect->Release();
	m_dieffect = NULL;
	unguard();
}

void i9FFDX::Play()
{
	guard(i9FFDX::Play);
	if(!m_dieffect) return;
	HRESULT hr = m_dieffect->Start(1, 0);
	unguard();
}

void i9FFDX::Stop()
{
	guard(i9FFDX::Stop);
	if(!m_dieffect) return;
	HRESULT hr = m_dieffect->Stop();
	unguard();
}

void i9FFDX::Set( int magnitude, int period )
{
	guard(i9FFDX::Set);
	if(!m_dieffect) return;

	DIPERIODIC diPeriodic;      // type-specific parameters
	diPeriodic.dwMagnitude		= magnitude;
	diPeriodic.lOffset			= 0; 
	diPeriodic.dwPhase			= 0; 
	diPeriodic.dwPeriod			= (DWORD)((float)period/1000.0 * DI_SECONDS); 

    DIEFFECT eff;
    ZeroMemory( &eff, sizeof(eff) );
    eff.dwSize                  = sizeof(DIEFFECT);
    eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.dwDuration              = INFINITE;
    eff.dwSamplePeriod          = 0;
    eff.dwGain                  = DI_FFNOMINALMAX;
    eff.dwTriggerButton         = DIEB_NOTRIGGER;
    eff.dwTriggerRepeatInterval = 0;
    eff.cAxes                   = 2;
    eff.rgdwAxes                = NULL;
    eff.rglDirection            = NULL;
    eff.lpEnvelope              = 0;
    eff.cbTypeSpecificParams    = sizeof(DIPERIODIC);
    eff.lpvTypeSpecificParams   = &diPeriodic;
    eff.dwStartDelay            = 0;

	HRESULT hr = m_dieffect->SetParameters(&eff,DIEP_TYPESPECIFICPARAMS);
	unguard();
}

BOOL i9FFDX::IsPlaying()
{
	guard(i9FFDX::IsPlaying);
	if(!m_dieffect) return FALSE;
	DWORD flags;
	HRESULT hr = m_dieffect->GetEffectStatus( &flags );
	if(FAILED(hr)) return FALSE;
	return (flags & DIEGES_PLAYING);
	unguard();
}

BOOL CALLBACK i9FFDX::EnumEffectsProc( LPCDIEFFECTINFO pei, LPVOID pv )
{
    *((GUID *)pv) = pei->guid;
    return DIENUM_STOP;
}

BOOL CALLBACK i9FFDX::EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
    DWORD* pdwNumForceFeedbackAxis = (DWORD*) pContext;
    if( (pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0 ) (*pdwNumForceFeedbackAxis)++;
    return DIENUM_CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
