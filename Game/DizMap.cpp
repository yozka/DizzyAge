//////////////////////////////////////////////////////////////////////////////////////////////////
// DizMap.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "gs.h"
#include "DizMap.h"
#include "DizGame.h"
#include "DizApp.h"

cDizMap	g_map;

//////////////////////////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////////////////////////
cDizMap::cDizMap()
{
	guard(cDizMap::cDizMap)
	m_filename[0]=0;
	m_mapw = 0;
	m_maph = 0;
	m_roomw = GAME_ROOMW;
	m_roomh = GAME_ROOMH;
	m_roomprop = NULL;
	m_roomname = NULL;
	m_objname = NULL;
	m_partition = NULL;
	unguard()
}

cDizMap::~cDizMap()
{
	guard(cDizMap::~cDizMap)
	unguard()
}

BOOL cDizMap::Init()
{
	guard(cDizMap::Init)
	m_brush.Init(1024,512);
	m_brushhash.Init(64,0);
	m_obj.Init(128,64);
	m_objhash.Init(64,0);
	return TRUE;
	unguard()
}

void cDizMap::Done()
{
	guard(cDizMap::Done)
	Reset();
	m_brush.Done();
	m_brushhash.Done();
	m_obj.Done();
	m_objhash.Done();
	unguard()
}

void cDizMap::Reset()
{
	guard(cDizMap::Reset)
	PartitionDone();
	ResetNames();
	m_mapw = 0;
	m_maph = 0;
	m_brush.Empty();
	m_brushhash.Done();
	m_brushhash.Init(64,0);
	m_obj.Empty();
	m_objhash.Done();
	m_objhash.Init(64,0);
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// LOAD
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cDizMap::Load( char* filename )
{
	guard(cDizMap::Load)
	if(!filename || !filename[0]) return FALSE; // invalid file
	_strlwr(filename);
	dlog(LOGAPP,"Loading map \"%s\"\n",filename);
	strcpy(m_filename,filename);

	BOOL ok = LoadMap();
	if(!ok) { dlog(LOGERR,"Loading map FAILED!\n\n"); return FALSE; }
	dlog(LOGAPP,"Loading map SUCCESSFUL!\n\n");

	PartitionMake();

	g_game.MatMapAlloc();
	g_game.Set(G_MAPW, m_mapw);
	g_game.Set(G_MAPH, m_maph);
	g_game.Set(G_ROOMW, m_roomw);
	g_game.Set(G_ROOMH, m_roomh);
	g_game.SetRoom(g_game.Get(G_ROOMX),g_game.Get(G_ROOMY)); // updates materialmap and re-gather objects

	return ok;
	unguard()
}

#define ERROR_CHUNK( info )	{ F9_FileClose(file); dlog(LOGAPP,"brocken chunk (%s)\n", info); return FALSE; }
BOOL cDizMap::LoadMap()
{
	guard(cDizMap::LoadMap)

	// open file
	F9FILE file = F9_FileOpen(m_filename);
	if(!file) { dlog(LOGAPP, "  map file not found.\n"); return FALSE; }
	F9_FileSeek(file,0,2);
	int filesize = F9_FileTell(file);
	F9_FileSeek(file,0,0);

	// read chunks
	int idx;
	int size;
	int chunkid=0;
	int chunksize=0;
	int chunkcount=0;
	int count_brush = 0;
	int count_obj = 0;
	int dummy;
	char* buffer;

	while(true)
	{
		if( F9_FileRead(&chunkid,4,file)!=4 )		{ ERROR_CHUNK("header"); }
		if( F9_FileRead(&chunksize,4,file)!=4 )	{ ERROR_CHUNK("header"); }
		
		switch(chunkid)
		{
			case MAP_CHUNKID:
			{
				if( chunksize!=strlen(MAP_ID) )	{ ERROR_CHUNK("size"); }
				buffer = (char*)smalloc(chunksize);
				size = 0;
				size += F9_FileRead(buffer, chunksize, file);
				if(size!=chunksize) { sfree(buffer);  ERROR_CHUNK("size"); }

				if(memcmp(buffer,MAP_ID,chunksize)!=0) { dlog(LOGAPP,"invalid map id: '%s' (current version: '%s')\n", buffer, MAP_ID); sfree(buffer); F9_FileClose(file); return FALSE; }
				sfree(buffer);
				break;
			}

			case MAP_CHUNKINFO2:
			{
				if( chunksize!= 6*4 ) { ERROR_CHUNK("size"); }
				size = 0;
				size += F9_FileRead(&m_mapw, 4, file);
				size += F9_FileRead(&m_maph, 4, file);
				size += F9_FileRead(&m_roomw, 4, file);
				size += F9_FileRead(&m_roomh, 4, file);
				size += F9_FileRead(&dummy, 4, file);
				size += F9_FileRead(&dummy, 4, file);
				if( size!=chunksize ) { ERROR_CHUNK("size"); }
				
				if(m_roomw<32 || m_roomw>1024) { ERROR_CHUNK("bad room size"); }
				if(m_roomh<32 || m_roomh>1024) { ERROR_CHUNK("bad room size"); }
				m_mapw = m_mapw / m_roomw;
				m_maph = m_maph / m_roomh;
				break;
			}

			case MAP_CHUNKMARKERS2:
			{
				F9_FileSeek(file,chunksize,1); // skip
				break;
			}

			case MAP_CHUNKBRUSHES2:
			{
				if(chunksize%(BRUSH_MAX*4)!=0) { ERROR_CHUNK("size"); }
				int data[BRUSH_MAX];
				size = 0;
				while(size<chunksize)
				{
					int ret = F9_FileRead(data, BRUSH_MAX*4, file);
					if(ret!=BRUSH_MAX*4) { ERROR_CHUNK("size"); }
					size += ret;

					int type = data[BRUSH_TYPE];
					if(type==0) 
					{
						// new brush
						tBrush* brush = snew tBrush();
						idx = m_brush.Add(brush); sassert(idx!=-1);
						memcpy( brush->m_data, data, BRUSH_MAX*4 );
						count_brush++;

						// add tracker to hash
						int id = brush->Get(BRUSH_ID);
						if(id!=0)
							m_brushhash.Add(id,idx);
					}
					else
					if(type==1) 
					{
						// new object
						tBrush* brush = snew tBrush();
						idx = m_obj.Add(brush); sassert(idx!=-1);
						memcpy( brush->m_data, data, BRUSH_MAX*4 );
						brush->Set(BRUSH_COLLISION,0);
						count_obj++;

						// add tracker to hash
						int id = brush->Get(BRUSH_ID);
						if(id!=0)
							m_objhash.Add(id,idx);
					}
				}
				break;
			}
			
			default:
			{
				dlog(LOGAPP, "  unknown chunk: id=%x size=%i\n", chunkid, chunksize);
				if(chunksize>0) F9_FileSeek(file,chunksize,1);
			}
		}
		if( F9_FileTell(file)>=filesize) break;

	}

	F9_FileClose(file);
	dlog(LOGAPP,"  map=%ix%i, room=%ix%i, brushes=%i, objects=%i\n", m_mapw, m_maph, m_roomw, m_roomh, count_brush, count_obj );

	int count = m_mapw * m_maph;
	if(count<=0) return FALSE; // validate size

	// init roomprops
	m_roomprop = (int*)smalloc(count*ROOM_PROPS*sizeof(int));
	memset(m_roomprop, 0, count*ROOM_PROPS*sizeof(int));

	// init roomnames (if not debug reloading)
	m_roomname = (char**)smalloc(count*sizeof(char*)); sassert(m_roomname);
	memset(m_roomname, 0, count*sizeof(char*));

	// init objnames
	if(count_obj>0)
	{
		m_objname = (char**)smalloc(count_obj*sizeof(char*)); sassert(m_objname);
		memset(m_objname, 0, sizeof(char*)*count_obj);
	}

	return TRUE;
	unguard()
}


BOOL cDizMap::Reload()
{
	guard(cDizMap::Reload)
	Reset();
	if(!Load(m_filename)) return FALSE;
	g_script.CallHandler( HANDLER_RELOADMAP );
	return TRUE;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// DRAW ROOM
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizMap::DrawRoom( int rx, int ry, int layer, int mode, int ofsx, int ofsy )
{
	guard(cDizMap::DrawRoom)
	int i;
	int color, shader;
	if(rx<0 || rx>=m_mapw) return; // safe
	if(ry<0 || ry>=m_maph) return; // safe

	int pi = ry * m_mapw + rx;
	if(pi<0 || pi>=PartitionCount()) return; // safe

	// viewport clipping test
	if( !g_paint.m_drawtilesoft )
	{
		iRect viewport;
		viewport.x1 = g_game.Get(G_ROOMX) * ROOM_W - g_game.Get(G_VIEWPORTX);
		viewport.y1 = g_game.Get(G_ROOMY) * ROOM_H - g_game.Get(G_VIEWPORTY);
		viewport.x2 = viewport.x1 + ROOM_W;
		viewport.y2 = viewport.y1 + ROOM_H;
		if( rx*ROOM_W>=viewport.x2 || ry*ROOM_H>=viewport.y2 || (rx+1)*ROOM_W<=viewport.x1 || (ry+1)*ROOM_H<=viewport.y1 )
			return;
	}

	for(i=0;i<m_partition[pi].m_count;i++)
	{
		int brushidx = m_partition[pi].m_data[i];

		tBrush* brush = BrushGet(brushidx);

		if( mode==DRAWMODE_NORMAL	&& (brush->Get(BRUSH_DRAW)&1)==0 ) continue; // don't draw
		if( mode==DRAWMODE_MATERIAL && (brush->Get(BRUSH_DRAW)&2)==0 ) continue; // don't write material
		if( mode==DRAWMODE_DENSITY  && (brush->Get(BRUSH_DRAW)&2)==0 ) continue; // don't write material
		
		if( brush->Get(BRUSH_LAYER) != layer ) continue; // filter layer

		int x = brush->Get(BRUSH_X) - rx * m_roomw + ofsx;
		int y = brush->Get(BRUSH_Y) - ry * m_roomh + ofsy;
		int frame = brush->Get(BRUSH_FRAME);

		if(mode==DRAWMODE_MATERIAL)
		{
			// use special color and shader
			color	= brush->Get(BRUSH_COLOR);
			shader	= brush->Get(BRUSH_SHADER);
			brush->Set(BRUSH_COLOR, MATCOLOR(brush->Get(BRUSH_MATERIAL)) | 0xff000000);
			brush->Set(BRUSH_SHADER, SHADER_ALPHAREP);
			g_paint.m_drawtilemat = brush->Get(BRUSH_MATERIAL); // software use this
			g_paint.DrawBrush( brush, x, y, frame );
			brush->Set(BRUSH_COLOR, color);
			brush->Set(BRUSH_SHADER, shader);
		}
		else
		if(mode==DRAWMODE_DENSITY)
		{
			// use special color and shader
			dword matd_color[MATD_MAX] = {0xff000000,0xff606060,0xffa0a0a0,0xffffffff};
			color	= brush->Get(BRUSH_COLOR);
			shader	= brush->Get(BRUSH_SHADER);
			brush->Set(BRUSH_COLOR, matd_color[MATDENSITY(brush->Get(BRUSH_MATERIAL))]);
			brush->Set(BRUSH_SHADER, SHADER_ALPHAREP);
			g_paint.DrawBrush( brush, x, y, frame );
			brush->Set(BRUSH_COLOR, color);
			brush->Set(BRUSH_SHADER, shader);
		}
		else
		{
			if(brush->Get(BRUSH_ANIM)==2) // only if looping
			{
				int gameframe = g_game.m_gameframe;
				if(brush->Get(BRUSH_DELAY)>0) gameframe /= brush->Get(BRUSH_DELAY);
				frame += gameframe;
			}
			g_paint.DrawBrush( brush, x, y, frame );
		}
	}

	R9_Flush(); // be sure!

	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// DATA
//////////////////////////////////////////////////////////////////////////////////////////////////
int cDizMap::GetRoomProp( int rx, int ry, int idx )
{
	guard(cDizMap::GetRoomProp)
	if(!m_roomprop) return 0;
	int r=rx+ry*m_mapw; 
	if(r<0 || r>=m_mapw*m_maph) return 0;
	if(idx<0 || idx>ROOM_PROPS) return 0;
	return m_roomprop[r*ROOM_PROPS+idx];
	unguard()
}
				
void cDizMap::SetRoomProp( int rx, int ry, int idx, int value )	
{
	guard(cDizMap::SetRoomProp)
	if(!m_roomprop) return;
	int r=rx+ry*m_mapw; 
	if(r<0 || r>=m_mapw*m_maph) return;
	if(idx<0 || idx>ROOM_PROPS) return;
	m_roomprop[r*ROOM_PROPS+idx] = value;
	unguard()
}

char* cDizMap::GetRoomName( int rx, int ry )
{
	guard(cDizMap::GetRoomName)
	if(!m_roomname) return NULL;
	int r=rx+ry*m_mapw; 
	if(r<0 || r>=m_mapw*m_maph) return NULL;
	return m_roomname[r];
	unguard()
}
				
void cDizMap::SetRoomName( int rx, int ry, char* name )	
{
	guard(cDizMap::SetRoomName)
	if(!m_roomname) return;
	int r=rx+ry*m_mapw; 
	if(r<0 || r>=m_mapw*m_maph) return;
	if(m_roomname[r]) { sfree(m_roomname[r]); m_roomname[r]=NULL; }
	if(name) { m_roomname[r] = sstrdup(name); _strupr(m_roomname[r]); }
	unguard()
}

char* cDizMap::GetObjName( int idx )
{
	guard(cDizMap::GetObjName)
	if(!m_objname) return NULL;
	if(idx<0 || idx>=ObjCount()) return NULL;
	return m_objname[idx];
	unguard()
}
				
void cDizMap::SetObjName( int idx, char* name )	
{
	guard(cDizMap::SetObjName)
	if(!m_objname) return;
	if(idx<0 || idx>=ObjCount()) return;
	if(m_objname[idx]) { sfree(m_objname[idx]); m_objname[idx]=NULL; }
	if(name) { m_objname[idx] = sstrdup(name); _strupr(m_objname[idx]); }
	unguard()
}

void cDizMap::ResetNames()
{
	guard(cDizMap::ResetRoomNames)
	int i;
	if(m_roomprop)
	{
		sfree(m_roomprop); m_roomprop = NULL;
	}
	if(m_roomname)
	{
		for(i=0;i<m_mapw*m_maph;i++)
			if(m_roomname[i]) { sfree(m_roomname[i]); m_roomname[i]=NULL; }
		sfree(m_roomname); m_roomname = NULL;
	}
	if(m_objname)
	{
		for(i=0;i<ObjCount();i++)
			if(m_objname[i]) { sfree(m_objname[i]); m_objname[i]=NULL; }
		sfree(m_objname); m_objname = NULL;
	}
	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// PARTITIONS
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizMap::PartitionInit()
{
	guard(cDizMap::PartitionInit)
	if(m_partition!=NULL) return;
	if(PartitionCount()==0) return;
	m_partition = (tBrushPartition*)smalloc( PartitionCount()*sizeof(tBrushPartition) );
	for(int i=0;i<PartitionCount(); i++)
	{
		m_partition[i].m_count = 0;
		m_partition[i].m_size = 0;
		m_partition[i].m_data = NULL;
	}
	unguard()
}

void cDizMap::PartitionDone()
{
	guard(cDizMap::PartitionDone)
	if(PartitionCount()==0) return;
	if(m_partition==NULL) return;
	for(int i=0;i<PartitionCount(); i++)
	{
		if(m_partition[i].m_data) sfree(m_partition[i].m_data)
		m_partition[i].m_data = NULL;
		m_partition[i].m_count = 0;
		m_partition[i].m_size = 0;
	}
	sfree(m_partition);
	m_partition = NULL;
	unguard()
}

void cDizMap::PartitionAdd( int brushidx )
{
	guard(cDizMap::PartitionAdd)
	if(m_partition==NULL) return;
	tBrush* brush = BrushGet(brushidx); sassert(brush!=NULL);
	iRect rbrush;
	brush->MakeBBW(rbrush.x1,rbrush.y1,rbrush.x2,rbrush.y2);
	/* @TODO find a way to optimize and get the partitions
	int brx = rbrush.x1 % m_mapw; // roomx for top-left brush corner
	int bry = rbrush.y1 / m_maph; // roomy for top-left brush corner
	int rooms[4][2] = { {brx,bry}, {brx+1,bry}, {brx,bry+1}, br
	*/
	for(int i=0;i<PartitionCount();i++)
	{
		iRect rpartition;
		int rx = i%m_mapw;
		int ry = i/m_mapw; // bigoldbug
		MakeRoomBBW( rx, ry, rpartition.x1, rpartition.y1, rpartition.x2, rpartition.y2, ROOM_B );
		if( RECT2RECT(rbrush,rpartition) )
		{
			tBrushPartition& partition = m_partition[i];
			if(partition.m_count==partition.m_size)
			{
				partition.m_size += 64;
				partition.m_data = (int*)srealloc( partition.m_data, partition.m_size*sizeof(int) );
			}
			partition.m_data[partition.m_count] = brushidx;
			partition.m_count++;
		}
	}
	unguard()
}

void cDizMap::PartitionMake()
{
	guard(cDizMap::PartitionMake)
	PartitionInit(); // init
	for(int i=0;i<BrushCount();i++)
	{
		PartitionAdd(i);
	}
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
