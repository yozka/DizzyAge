///////////////////////////////////////////////////////////////////////////////////////////////////
// E9Config.h
// Edit this file to configurate the engine
// Include through E9Engine.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __E9CONFIG_H__
#define __E9CONFIG_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG CONTROL
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG
	#define D9_ENABLE_GUARD
#endif
	#define D9_ENABLE_FUNC
	#define D9_ENABLE_ASSERT
//	#define D9_ENABLE_DH
	#define D9_ENABLE_MEMORY
	#define D9_ENABLE_MEMORYTRACK
	#define D9_ENABLE_MEMORYCOUNT
	#define D9_ENABLE_MEMORYCHECK
	#define D9_ENABLE_MEMORYCONTENT

///////////////////////////////////////////////////////////////////////////////////////////////////
// AUDIO CONTROL
///////////////////////////////////////////////////////////////////////////////////////////////////
	#define A9_ENABLE_WAV
	#define	A9_ENABLE_OGG
	#define A9_ENABLE_DUMB
	#define A9_ENABLE_YM

///////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER CONTROL
///////////////////////////////////////////////////////////////////////////////////////////////////
	#define R9_ENABLE_PNG
//	#define R9_ENABLE_JPG
	#define R9_ENABLE_DLLDX
	#define R9_ENABLE_DLLGL

///////////////////////////////////////////////////////////////////////////////////////////////////
// OTHERS CONTROL
///////////////////////////////////////////////////////////////////////////////////////////////////
//	#define E9_ENABLE_ZIP

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
