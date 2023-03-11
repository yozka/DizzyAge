//////////////////////////////////////////////////////////////////////////////////////////////////
// EdiPaint.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __EDIPAINT_H__
#define __EDIPAINT_H__

#include "R9Font.h"
#include "EdiDef.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////////////////////////////

// shaders
#define SHADER_OPAQUE	0		// BLEND_OPAQUE
#define	SHADER_BLEND	1		// BLEND_ALPHA
#define SHADER_ADD		2		// BLEND_ADD
#define SHADER_MOD		3		// BLEND_MOD
#define SHADER_MOD2		4		// BLEND_MOD2
#define SHADER_ALPHAREP	5		// BLEND_ALPHAREP
#define SHADER_MAX		6

//////////////////////////////////////////////////////////////////////////////////////////////////
// TILE
//////////////////////////////////////////////////////////////////////////////////////////////////
class cTile
{
public:
		int			m_id;						// unique id >=0
		int			m_frames;					// number of frames
		R9TEXTURE	m_tex;						// texture
		char*		m_name;						// tile name
						
				cTile		()					{ m_id=0; m_frames=1; m_tex=NULL; m_name=NULL; }
inline	int		GetWidth	()					{ return (m_frames>0) ? (R9_TextureGetWidth(m_tex)/m_frames) : R9_TextureGetWidth(m_tex); }
inline	int		GetHeight	()					{ return R9_TextureGetHeight(m_tex); }
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// BRUSH
//////////////////////////////////////////////////////////////////////////////////////////////////
#define BRUSH_LAYER		0			// layer idx
#define BRUSH_X			1			// position x in world
#define BRUSH_Y			2			// position y in world
#define BRUSH_W			3			// width of the object (usually the sprite width)
#define BRUSH_H			4			// height of the object (usually the sprite height)
#define BRUSH_TILE		5			// tile id
#define BRUSH_FRAME		6			// current tile animation frame (starts with 0)
#define BRUSH_MAP		7			// 4 values: x1,y1,x2,y2
#define BRUSH_FLIP		11			// flip: 0=none, 1=x, 2=y, 3=xy, 4=rot, ...
#define BRUSH_COLOR		12			// color
#define BRUSH_SHADER	13			// shader 0..SHADER_MAX
#define BRUSH_SCALE		14			// scale value (0=1, 1, 2, ...)
#define BRUSH_SELECT	15			// editor internal use reserved
#define BRUSH_CUSTOM	16			// game custom 
#define BRUSH_MAX		48			// dummy

struct tBrush
{
	int	m_data[BRUSH_MAX];
				tBrush()											{}
inline	int		Get( int idx )										{ return m_data[idx]; }
inline	void	Set( int idx, int val )								{ m_data[idx] = val; }
inline	void	MakeBBW	( int &x1, int &y1, int &x2, int &y2 )		{ x1 = m_data[BRUSH_X]; x2 = m_data[BRUSH_X]+m_data[BRUSH_W]; y1 = m_data[BRUSH_Y]; y2 = m_data[BRUSH_Y]+m_data[BRUSH_H]; }
};

// macros to access brush width and height with respect to flip rotation and scale
#define GET_BRUSH_MAPSCALE( brush )		( (brush).m_data[BRUSH_SCALE]>0 ? (float)(brush).m_data[BRUSH_SCALE]/100.0f : 1.0f )
#define GET_BRUSH_MAPWITH( brush )		( (float)( ((brush).m_data[BRUSH_FLIP] & R9_FLIPR) ? ((brush).m_data[BRUSH_MAP+3]-(brush).m_data[BRUSH_MAP+1]) : ((brush).m_data[BRUSH_MAP+2]-(brush).m_data[BRUSH_MAP+0]) ) * GET_BRUSH_MAPSCALE(brush) )
#define GET_BRUSH_MAPHEIGHT( brush )	( (float)( ((brush).m_data[BRUSH_FLIP] & R9_FLIPR) ? ((brush).m_data[BRUSH_MAP+2]-(brush).m_data[BRUSH_MAP+0]) : ((brush).m_data[BRUSH_MAP+3]-(brush).m_data[BRUSH_MAP+1]) ) * GET_BRUSH_MAPSCALE(brush) )

//////////////////////////////////////////////////////////////////////////////////////////////////
// EDIPAINT
//////////////////////////////////////////////////////////////////////////////////////////////////
class cEdiPaint
{
public:
						cEdiPaint		();
						~cEdiPaint		();
	
		BOOL			Init			();
		void			Done			();

		// Tiles
inline	int				TileCount		()					{ return m_tile.Size(); }
inline	cTile*			TileGet			( int idx )			{ if(0<=idx && idx<m_tile.Size()) return m_tile.Get(idx); else return NULL; }
		int				TileAdd			( int id );			// add a new empty tile; id must be unique
		void			TileDel			( int idx );		// delete tile by index
inline	int				TileFind		( int id )			{ int data; if( m_hash.Find(id, data) ) return data; else return -1; }
		BOOL			TileLoad		( char* path );		// load tiles from a path
		BOOL			TileLoadFile	( char* filepath );	// load a tile file
		void			TileUnload		();					// unload load tiles (destroy)

		// Draw scaled
		void			DrawTile		( int idx, int x, int y, iRect& map, dword color=0xffffffff, int flip=0, int frame=0, int blend=R9_BLEND_ALPHA, float scale=1.0f );
		void			DrawTile		( int idx, int x, int y, dword color=0xffffffff, int flip=0, int frame=0, int blend=R9_BLEND_ALPHA, float scale=1.0f );
		void			DrawBrushAt		( tBrush* brush, int x, int y, float zoom=1.0f, BOOL anim=FALSE );
		void			DrawBrushFlashAt( tBrush* brush, int x, int y, float zoom=1.0f, BOOL anim=FALSE );

		// utils
		dword			GetFlashingColorBW();

		// tiles
		cPList<cTile>	m_tile;			// tiles list
		cHash			m_hash;			// hash for tiles (id,idx)
		char			m_tilepath[256];// path to tiles (obtained from the tilefile at load)

		// Others
		int				m_brushrect;						// draw brush rectangles, vor blind visibility debug
		int				m_shadersel;						// blend for selection shader
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// UTILS
//////////////////////////////////////////////////////////////////////////////////////////////////
extern	cEdiPaint	g_paint;

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
