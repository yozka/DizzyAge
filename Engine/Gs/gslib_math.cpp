#include "stdafx.h"
#include "gslib_math.h"
#include "gs.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////

int gsSgn( gsVM* vm )
{
	guard(gsSgn)
	if(!gs_ckparams(vm,1)) GS_RETURNINT(vm,0);
	int type = gs_type(vm,0);
	if(type==GS_INT) 
	{ 
		int a=gs_toint(vm,0); 
		gs_pushint(vm, (a==0) ? 0 : (a>0) ? 1 : -1);
		return 1;
	}
	else
	if(type==GS_FLT) 
	{
		float a=gs_toflt(vm,0);
		gs_pushflt(vm, (a==0.0f) ? 0.0f : (a>0.0f) ? 1.0f : -1.0f);
		return 1;
	}
	gs_error(vm, GSE_BADTYPE, GS_INT, type);
	GS_RETURNINT(vm,0);
	unguard()
}

int gsAbs( gsVM* vm )
{
	guard(gsAbs)
	if(!gs_ckparams(vm,1)) GS_RETURNINT(vm,0);
	int type = gs_type(vm,0);
	if(type==GS_INT) 
	{
		gs_pushint(vm, abs(gs_toint(vm,0))); 
		return 1;
	}
	else
	if(type==GS_FLT) 
	{
		gs_pushflt(vm, (float)fabs((double)gs_toflt(vm,0))); 
		return 1;
	}
	gs_error(vm, GSE_BADTYPE, GS_INT, type);
	GS_RETURNINT(vm,0);
	unguard()
}

int gsFloor( gsVM* vm )
{
	guard(gsFloor)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)floor((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsCeil( gsVM* vm )
{
	guard(gsCeil)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)ceil((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsRound( gsVM* vm )
{
	guard(gsRound)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	float a = gs_toflt(vm,0);
	float b = (float)floor( (double)a );
	gs_pushflt(vm,(a-b<0.5f) ? b : b+1.0f);
	return 1;
	unguard()
}

int gsTrunc( gsVM* vm )
{
	guard(gsTrunc)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	int p = 5;
	if(gs_params(vm)==2 && gs_isint(vm,1)) p = gs_toint(vm,1);
	int a = (int)pow(10.0,(double)p);
	float b = (float)((int)(gs_toflt(vm,0) * a)) / a;
	gs_pushflt(vm,b);
	return 1;
	unguard()
}

int gsExp( gsVM* vm )
{
	guard(gsExp)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)exp((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsLog( gsVM* vm )
{
	guard(gsLog)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)log((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsPow( gsVM* vm )
{
	guard(gsPow)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	if(!gs_cktype(vm,1,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)pow((double)gs_toflt(vm,0),(double)gs_toflt(vm,1)));
	return 1;
	unguard()
}

int gsSqrt( gsVM* vm )
{
	guard(gsSqrt)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)sqrt((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsSin( gsVM* vm )
{
	guard(gsSin)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)sin((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsCos( gsVM* vm )
{
	guard(gsCos)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)cos((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsTan( gsVM* vm )
{
	guard(gsTan)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)tan((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsAcos( gsVM* vm )
{
	guard(gsAcos)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)acos((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsAsin( gsVM* vm )
{
	guard(gsAsin)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)asin((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

int gsAtan( gsVM* vm )
{
	guard(gsAtan)
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNFLT(vm,0.0f);
	gs_pushflt(vm,(float)atan((double)gs_toflt(vm,0)));
	return 1;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// register lib
/////////////////////////////////////////////////////////////////////////////////////////
int gslib_math( gsVM* vm )
{
	guard(gslib_math)

	gs_regfn( vm, "gs_sgn",				gsSgn );			// int|flt > int|flt(sign=-1,0,1)
	gs_regfn( vm, "gs_abs",				gsAbs );			// int|flt > int|flt
	gs_regfn( vm, "gs_floor",			gsFloor );			// flt > flt
	gs_regfn( vm, "gs_ceil",			gsCeil );			// flt > flt
	gs_regfn( vm, "gs_round",			gsRound );			// flt > flt
	gs_regfn( vm, "gs_trunc",			gsTrunc );			// flt(a) [,flt(b=5)] > flt(a with b decimals)

	gs_regfn( vm, "gs_exp",				gsExp );			// flt > flt
	gs_regfn( vm, "gs_log",				gsLog );			// flt > flt
	gs_regfn( vm, "gs_pow",				gsPow );			// flt(a), flt(b) > flt(a raised to the power of b)
	gs_regfn( vm, "gs_sqrt",			gsSqrt );			// flt > flt
	
	gs_regfn( vm, "gs_sin",				gsSin );			// flt > flt; 7 exact decimals
	gs_regfn( vm, "gs_cos",				gsCos );			// flt > flt; 7 exact decimals
	gs_regfn( vm, "gs_tan",				gsTan );			// flt > flt; 7 exact decimals
	gs_regfn( vm, "gs_asin",			gsAsin );			// flt > flt
	gs_regfn( vm, "gs_acos",			gsAcos );			// flt > flt
	gs_regfn( vm, "gs_atan",			gsAtan );			// flt > flt

	gs_regflt( vm, "PI",				3.1415926535897932385f );
	gs_regflt( vm, "PI2",				6.283185307179586477f );
	gs_regflt( vm, "DEG2RAD",			3.1415926535897932385f / 180.0f );
	gs_regflt( vm, "RAD2DEG",			180.0f / 3.1415926535897932385f );

	return 0;	

	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
