//////////////////////////////////////////////////////////////////////////////////////////////////
// EdiMap.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __EDIMAP_H__
#define __EDIMAP_H__

#include "EdiDef.h"
#include "EdiPaint.h"

#define	MAP_SIZEDEFAULT	2000
#define MAP_SIZEMIN		128
#define MAP_SIZEMAX		100000

#define	VIEWB	(16)
#define	VIEWW	(g_map.m_vieww)
#define	VIEWH	(g_map.m_viewh)
#define	VIEWX	(g_map.m_viewx)
#define	VIEWY	(g_map.m_viewy)

#define	CAMX	(g_map.m_camx)
#define	CAMY	(g_map.m_camy)
#define	CAMW	(VIEWW/CAMZ)
#define	CAMH	(VIEWH/CAMZ)
#define	CAMZ	(g_map.m_camz)
#define CAMX1	(CAMX-CAMW/2)
#define CAMY1	(CAMY-CAMH/2)
#define CAMX2	(CAMX+CAMW/2)
#define CAMY2	(CAMY+CAMH/2)

// util
#define	VIEW2CAM( x, y ) { x = x/CAMZ + CAMX1; y = y/CAMZ + CAMY1; }	// from view (screen space) to camera (world space)
#define	CAM2VIEW( x, y ) { x = CAMZ*(x-CAMX1); y = CAMZ*(y-CAMY1); }	// from camera (world space ) to view (screen space)
#define INVIEW( x, y )	 ((x>=0 && x<VIEWW) && (y>=0 && y<VIEWH))
#define INVIEWB( x, y )	 ((x>=-VIEWB && x<VIEWW+VIEWB) && (y>=-VIEWB && y<VIEWH+VIEWB))

//////////////////////////////////////////////////////////////////////////////////////////////////
// partitioning
//////////////////////////////////////////////////////////////////////////////////////////////////
#define PARTITION_CELSIZE		1024

class cPartitionCel
{
public:
	int	 m_count;	// how many elements
	int	 m_size;	// how big the databuffer
	int* m_data;	// data buffer

	cPartitionCel();
	~cPartitionCel();
	void	Add( int val );		// adds a value
	void	Sub( int val );		// delete a value
	int		Find( int val );	// find a value; ret index or -1
};

struct tMarker
{
	int x;	// marker pos x
	int y;	// marker pos y
	int z;	// marker zoom
	tMarker( int _x, int _y, int _z ) { x=_x; y=_y; z=_z; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// cEdiMap
//////////////////////////////////////////////////////////////////////////////////////////////////
class cEdiMap
{
public:
					cEdiMap				();
virtual				~cEdiMap			();

virtual BOOL		Init				();
virtual void		Done				();

virtual void		Update				( float dtime );			// update
virtual void		Draw				();							// draw current
virtual	void		Refresh				();							// refresh draw

		void		Reset				();
		BOOL		Resize				( int width, int height );	// resize map; return true if no crop occured

public:
		// map
		int			m_mapw;											// map width
		int			m_maph;											// map height
		int			m_roomw;										// room width
		int			m_roomh;										// room height
		int			m_roomgrid;										// room grid visible
				
		// view
		int			m_viewx;										// viewport left
		int			m_viewy;										// viewport top
		int			m_vieww;										// viewport width
		int			m_viewh;										// viewport height

		// camera												
		int			m_camx;											// camera position x (center)
		int			m_camy;											// camera position y (center)
		int			m_camz;											// scale factor >=1
																
		// refresh			
		BOOL		m_hideselected;									// don't draw selected tiles
		BOOL		m_refresh;										// if refresh is necessary
		R9TEXTURE	m_target;										// render target texture

		// brushes
		int			BrushNew			();							// add a new brush to the brushlist
		void		BrushIns			( int idx, tBrush& brush );	// insert a new brush and shift others; selectcount friendly
		void		BrushDel			( int idx );				// delete one brush from brushlist and shift others; selectcount friendly
//		void		BrushDrawOld		( iRect& view );			// draw brushes in view
		void		BrushDrawExtra		( iRect& view );			// draw brushes in view using partitioning
		int			BrushPick			( int x, int y );			// pick brush from a map position; -1 if not found
		void		BrushToFront		( int idx );				// bring brush to front (first visible in layer)
		void		BrushToBack			( int idx );				// bring brush to back (last visible in layer)
		void		BrushClear			();							// free brush buffers and counts; selectcount friendly

		int			m_brushcount;									// brushes count
		int			m_brushsize;									// brush buffer size (how many it can hold)
		tBrush*		m_brush;										// brush buffer list (brushes in map)
		int			m_brushviscount;								// visible brushes count
		int			m_brushvissize;									// visible brushes buffer size
		int*		m_brushvis;										// visible brushes list (brushes to draw; updated on refresh)

		// partitioning
		void		PartitionInit		();							// create partitions cels depending on the map size
		void		PartitionDone		();							// destroy partitions cels
inline	void		PartitionReset		()							{ PartitionDone(); PartitionInit(); }
inline	int			PartitionCountW		()							{ return (m_mapw+PARTITION_CELSIZE-1) / PARTITION_CELSIZE; }
inline	int			PartitionCountH		()							{ return (m_maph+PARTITION_CELSIZE-1) / PARTITION_CELSIZE; }
		BOOL		PartitionAdd		( int brushidx );			// add a brush index to partitioning; true if successful
		void		PartitionDel		( int brushidx );			// del a brush index from partitioning; might need PartitionFix
		int			PartitionGet		( iRect& rect, int* buffer, int buffersize );	// retrive all partitions that intersect a specified area
		void		PartitionFix		( int brushidx1, int brushidx2, int delta ); // adjust all indices in interval (including) by delta
		BOOL		PartitionRepartition();							// remove and reenter all brushes; ret ok (or false if some failures)
		cPList<cPartitionCel> m_partition;							// partitions cels list

		// markers
		void		MarkerToggle		( int x, int y );			// add/remove marker at a given pos
		void		MarkerGoto			( int dir=1 );				// go to nearest/next marker
		int			MarkerClosest		( int x, int y, int &dist );// get index of closest marker to a given pos 
		void		MarkerClear			();							// clear all markers
		void		MarkerResize		();							// remove out markers after map is resized (private)
		BOOL		MarkerTest			( int idx );				// test if a marker is inside map (private)
		cList<tMarker> m_marker;									// markers list

		// selection
		void		SelectionRefresh	();							// recount selection
		void		SelectionGoto		( int dir=1 );				// go to next selected brush
		int			m_selectcount;									// count of selected brushes
		int			m_selectgoto;									// index of brush used in goto selection

		// others
		void		DrawGrid( iRect &view );						// draw grid using EdiApp settings
		void		DrawAxes( int x, int y );						// draw axes
		void		DrawScrollers();								// draw side scrollers
		iRect		GetHScrollRect();
		iRect		GetVScrollRect();
		void		CheckMapView();									// checks viewport if exceedes map sizes and adjust
		int			m_scrolling;									// 0=no, 1=horizontal, 1=vertical
		int			m_scrollofs;									// scroll pick offset (h or v)

		// statistics
		int			m_count_brushdraw;
		int			m_count_brushcheck;

		// save map image
		BOOL		SaveMapImage( char* filename );					// save huge map image, rendering in the target texture each 256x256 cel
};				

extern	cEdiMap		g_map;

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
