//////////////////////////////////////////////////////////////////////////////////////////////////
// DizMap.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DIZMAP_H__
#define __DIZMAP_H__

#include "E9System.h"
#include "D9Debug.h"
#include "DizPaint.h"

#define	MAP_W							( g_map.m_mapw )		// map width macro (in rooms)
#define	MAP_H							( g_map.m_maph )		// map height macro (in rooms)
#define	ROOM_W							( g_map.m_roomw )		// room width macro
#define	ROOM_H							( g_map.m_roomh )		// room height macro
#define	ROOM_B							16						// room border (on every side)
#define ROOM_WB							(ROOM_W+2*ROOM_B)		// room width bordered
#define ROOM_HB							(ROOM_H+2*ROOM_B)		// room height bordered
#define ROOM_PROPS						8						// number of room properties
#define	MAP_ROOMXY2ROOMN( rx, ry )		( rx + ry * MAP_W )		// convert from room rx,ry to a room index
#define	MAP_POSX2ROOMX( x )				( (x>=0) ? (x/g_map.m_roomw) : ((x+1)/g_map.m_roomw-1) )
#define	MAP_POSY2ROOMY( y )				( (y>=0) ? (y/g_map.m_roomh) : ((y+1)/g_map.m_roomh-1) )

#define MAP_ID							"dizzymap"
#define MAP_CHUNKID						0x11111111
#define MAP_CHUNKINFO					0x22222222	// obsolete
#define MAP_CHUNKINFO2					0x22222223
#define MAP_CHUNKMARKERS				0x33333333	// obsolete
#define MAP_CHUNKMARKERS2				0x33333334
#define MAP_CHUNKBRUSHES				0x88888888	// obsolete
#define MAP_CHUNKBRUSHES2				0x88888889


//////////////////////////////////////////////////////////////////////////////////////////////////
// PARTITIONS
//////////////////////////////////////////////////////////////////////////////////////////////////
struct tBrushPartition
{
	int		m_count;	// count of elements in data list
	int		m_size;		// data list size
	int*	m_data;		// data list
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// cDizMap
//////////////////////////////////////////////////////////////////////////////////////////////////
class cDizMap
{
public:
						cDizMap				();
						~cDizMap			();
					
		// init		
		BOOL			Init				();								// init (first time)
		void			Done				();								// done (last time)
		void			Reset				();								// reset when start game; clears map brushes

		// load
		BOOL			Load				( char* filename );				// load map data; map and objects should be empty before load map
		BOOL			LoadMap				();								// load brushes and stuff
		BOOL			Reload				();								// reload map for debug purposes

		// draw
		void			DrawRoom			( int rx, int ry, int layer, int mode, int ofsx=0, int ofsy=0 );	// rx=roomx, ry=roomy layer=0..8; mode: 0=normal, 1=material, 2=density; ofsx=ofsetx, ofs=ofsety

		// data
		char			m_filename[128];	// map file name (kept for reloads)
		int				m_mapw;				// map width in rooms
		int				m_maph;				// map height in rooms
		int				m_roomw;			// room width
		int				m_roomh;			// room height
		int*			m_roomprop;			// matric with room props [mapw x maph x ROOM_PROPS]
		char**			m_roomname;			// matrix with room names [mapw x maph]
		char**			m_objname;			// objects names
		int				GetRoomProp			( int rx, int ry, int idx );
		void			SetRoomProp			( int rx, int ry, int idx, int value );
		char*			GetRoomName			( int rx, int ry );
		void			SetRoomName			( int rx, int ry, char* name );
		char*			GetObjName			( int idx );
		void			SetObjName			( int idx, char* name );
		void			ResetNames			();					// free room names, room props and object names

inline	void			MakeRoomBBW			( int rx, int ry, int &x1, int &y1, int &x2, int &y2, int border=0 )	{ x1=rx*m_roomw-border; y1=ry*m_roomh-border; x2=(rx+1)*m_roomw+border; y2=(ry+1)*m_roomh+border; }


		// brushes
inline	int					BrushCount		()					{ return m_brush.Size(); }
inline	tBrush*				BrushGet		( int idx )			{ return m_brush.Get(idx); }
inline	int					BrushFind		( int id )			{ int data; if( m_brushhash.Find(id, data) ) return data; else return -1; }
		cPList<tBrush>		m_brush;							// brushes list (static brushes)
		cHash				m_brushhash;						// brushes hash


		// objects
inline	int					ObjCount		()					{ return m_obj.Size(); }
inline	tBrush*				ObjGet			( int idx )			{ return m_obj.Get(idx); }
inline	int					ObjFind			( int id )			{ int data; if( m_objhash.Find(id, data) ) return data; else return -1; }
		cPList<tBrush>		m_obj;								// objects list (dynamic brushes)
		cHash				m_objhash;							// objects hash


		// partition
inline	int					PartitionCount	()					{ return m_mapw*m_maph; }
		void				PartitionInit	();
		void				PartitionDone	();
		void				PartitionAdd	( int brushidx );	// add a brush to partitioning
		void				PartitionMake	();					// init and partition brushes
		tBrushPartition*	m_partition;						// partitions list

};

extern cDizMap	g_map;

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
