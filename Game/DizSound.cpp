//////////////////////////////////////////////////////////////////////////////////////////////////
// DizSound.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DizSound.h"
#include "DizCfg.h"
#include "DizDef.h"
#include "DizScript.h"
#include "DizDebug.h"
#include "E9App.h"
#include "A9Codec.h"

cDizSound g_sound;

//////////////////////////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////////////////////////
cDizSound::cDizSound()
{
	guard(cDizSound::cDizSound)
	// samples
	for(int i=0;i<SOUND_VOICES;i++)	m_voice[i] = NULL;
	m_voicecount = 0;

	m_sample_total = 0;		
	m_sample_fail = 0;		
	m_sample_duplicates = 0;
	m_sample_group = 0;

	// music
	m_music			= NULL;
	m_musicidx		= -1;
	m_musicnext		= -1;
	m_musicstart	= 0;
	m_musicfadein	= 0;
	m_musicfadeout	= 0;
	m_musicpos		= 0;
	m_musicpaused	= 0;
	m_musicvol		= 0.0f;

	m_music_total = 0;
	m_music_fail = 0;	
	m_music_duplicates = 0;
	m_music_group = 0;

	unguard()
}

cDizSound::~cDizSound()
{
	guard(cDizSound::cDizSound)
	unguard()
}

BOOL cDizSound::Init()
{
	guard(cDizSound::Init)
	m_sampleproto.Init(32,16);
	m_musicproto.Init(8,8);
	return TRUE;
	unguard()
}

void cDizSound::Done()
{
	guard(cDizSound::Done)
	SampleStopAll();
	MusicStop();
	m_sampleproto.Done();
	m_musicproto.Done();
	unguard()
}

void cDizSound::Update()
{
	guard(cDizSound::Update)
	if(!A9_IsReady()) return;
	
	// samples - stop (remove) those that have finished playing
	int i;
	m_voicecount = 0;
	for(i=0;i<SOUND_VOICES;i++)
	{
		if(SamplePlaying(i)!=-1) m_voicecount++;
	}
	
	// music
	float dtime = (float)E9_AppGetInt(E9_APP_DELTATIME) / 1000.f;
	MusicUpdate( dtime );

	A9_Update();

	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// SAMPLES
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cDizSound::SampleLoadFile( char* filepath, int group )
{
	guard(cDizSound::SampleLoadFile)

	// check name and extension
	char* name = file_path2file(filepath); if(!name) return FALSE;
	char* ext = file_path2ext(filepath); if(!ext) return FALSE;
	char supported[] = "wav,ogg,ym,mod,it,xm,s3m";
	if(strstr(supported,ext)==NULL) return FALSE; // ignore files with unsupported extensions

	m_sample_total++;

	// check name format
	char szt[128];
	int id = -1;
	int instances = 1;
	int ret = sscanf(name,"%i %s %i",&id,szt,&instances);
	if(ret==0 || id==-1) 
	{ 
		m_sample_fail++; 
		dlog(LOGAPP, "! %s (bad name)\n", filepath); 
		return FALSE; 
	}
	if(instances<1) instances=1;

	// check unique id
	int idx = SampleFind(id);
	if( idx!=-1 )
	{
		m_sample_fail++;
		m_sample_duplicates++;
		dlog(LOGSYS, "! %s (duplicate id)\n", filepath, id);
		return FALSE;
	}

	// load and decompress
	A9BUFFERPROTO bufferproto = A9_BufferPrecache(filepath);
	if(!bufferproto)
	{
		m_sample_fail++;
		dlog(LOGSYS, "! %s (failed to load)\n", filepath);
		return FALSE;
	}

	// add to list
	tSoundProto* proto = snew tSoundProto();
	proto->m_id = id;
	proto->m_group = group;
	proto->m_instances = instances;
	proto->m_bufferproto = bufferproto;
	m_sampleproto.Add(proto);

	if(IS_DEVELOPER()) // log for developers
		dlog(LOGAPP, "  %s [%i]\n", filepath, instances );

	return TRUE;
	unguard()
}

void FFCallback_Sample( char* filepath, BOOL dir )
{
	guard(FFCallback_Sample)
	if(dir) return;
	BOOL ret = g_sound.SampleLoadFile(filepath,g_sound.m_sample_group);
	unguard()
}

BOOL cDizSound::SampleLoad( char* path, int group )
{
	guard(cDizSound::SampleLoad)
	if(!A9_IsReady()) { dlog(LOGAPP,"Sound disabled - no samples are loaded.\n"); return FALSE; }

	if(!path || !path[0]) return FALSE; // invalid path
	int szlen = (int)strlen(path);
	if(path[szlen-1]!='\\') strcat(path,"\\");
	_strlwr(path);
	dlog(LOGAPP,"Loading samples from \"%s\" (group=%i)\n", path, group);

	// init
	m_sample_total		= 0;
	m_sample_fail		= 0;
	m_sample_duplicates	= 0;
	m_sample_group		= group;

	// find files on disk
	int archivefiles = F9_ArchiveGetFileCount(0);
	if(archivefiles==0) // if no archive found then open from disk
	{
		file_findfiles( path, FFCallback_Sample, FILE_FINDREC );
	}
	else // if archive opened, load from it
	{
		for(int i=0;i<archivefiles;i++)
		{
			char* filename = F9_ArchiveGetFileName(0,i);
			if(strstr(filename,path)==filename)
			{
				FFCallback_Sample(filename,FALSE);
			}
		}
	}

	// report
	dlog(LOGAPP, "Samples report: total=%i, failed=%i (duplicates=%i)\n\n", m_sample_total, m_sample_fail, m_sample_duplicates );

	return TRUE;
	unguard()
}

void cDizSound::SampleUnload( int group )
{
	guard(cDizSound::SampleUnload)
	int i;
	for(i=0;i<m_sampleproto.Size();i++)
	{
		if(m_sampleproto[i]->m_group == group)
		{
			SampleStopAll(m_sampleproto[i]->m_id);
			m_sampleproto.Del(i);
			i--;
		}
	}
	unguard()
}

int	cDizSound::SamplePlay( int id, int loop )
{
	guard(cDizSound::SamplePlay)
	int i;
	int protoidx = SampleFind(id);
	if(protoidx==-1) return -1; // invalid id
	tSoundProto* proto = m_sampleproto.Get(protoidx); sassert(proto);
	if( proto->m_bufferproto == NULL ) return -1; // invalid proto

	// find instances and free voice
	int count = 0;
	int voiceidx=-1;
	for(i=0;i<SOUND_VOICES;i++)
	{
		if(SamplePlaying(i)==id) count++; // destroy if stopped
		if(m_voice[i]==NULL) voiceidx = i;
	}
	if(count>=proto->m_instances) return -1; // too many instances playing
	if(voiceidx==-1) return -1; // no free voices
	
	// add new voice
	A9BUFFER buffer = A9_BufferCreateFromProto(proto->m_bufferproto);
	if(!buffer) return -1; // wrong format
	A9_BufferSet(buffer,A9_USER,id);
	A9_BufferSet(buffer,A9_VOLUME,A9_VolumePercentToDecibel(g_cfg.m_volfx));
	A9_BufferPlay(buffer,loop);
	m_voice[voiceidx] = buffer;
	
	return voiceidx;
	unguard()
}

int	cDizSound::SamplePlaying( int voiceidx )
{
	guard(cDizSound::SamplePlaying)
	sassert(0<=voiceidx && voiceidx<SOUND_VOICES);
	if(m_voice[voiceidx]==NULL) return -1;
	if(!A9_BufferIsPlaying(m_voice[voiceidx]))
	{
		A9_BufferDestroy(m_voice[voiceidx]);
		m_voice[voiceidx] = NULL;
		return -1;
	}
	return A9_BufferGet(m_voice[voiceidx],A9_USER);
	unguard()
}

void cDizSound::SampleStop( int voiceidx )
{
	guard(cDizSound::SampleStop)
	sassert(0<=voiceidx && voiceidx<SOUND_VOICES);
	if(m_voice[voiceidx]==NULL) return;
	A9_BufferStop(m_voice[voiceidx]);
	A9_BufferDestroy(m_voice[voiceidx]);
	m_voice[voiceidx] = NULL;
	unguard()
}

void cDizSound::SampleStopAll( int id )
{
	guard(cDizSound::SampleStopAll)
	for(int i=0;i<SOUND_VOICES;i++)
	{
		A9BUFFER buffer = m_voice[i];
		if( buffer && (id==-1 || A9_BufferGet(buffer,A9_USER)==id) )
			SampleStop(i);
	}
	unguard()
}

void cDizSound::SampleVolume( int volume )
{
	guard(cDizSound::SampleVolume)
	if(volume<0) volume=0;
	if(volume>100) volume=100;
	if(volume==g_cfg.m_volfx) return; // same volume
	g_cfg.m_volfx = volume;

	int volumedb = A9_VolumePercentToDecibel(volume);
	for(int i=0;i<SOUND_VOICES;i++)
	{
		A9BUFFER buffer = m_voice[i];
		if(!buffer) continue;
		A9_BufferSet(buffer,A9_VOLUME,volumedb);
	}

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// MUSIC
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cDizSound::MusicLoadFile( char* filepath, int group )
{
	guard(cDizSound::MusicLoadFile)
	// check name and extension
	char* name = file_path2file(filepath); if(!name) return FALSE;
	char* ext = file_path2ext(filepath); if(!ext) return FALSE;
	char supported[] = "wav,ogg,ym,mod,it,xm,s3m";
	if(strstr(supported,ext)==NULL) return FALSE; // ignore files with unsupported extensions

	m_music_total++;

	// check name format
	char szt[128];
	int id = -1;
	int ret = sscanf(name,"%i %s",&id,szt);
	if(ret==0 || id==-1) 
	{ 
		m_music_fail++; 
		dlog(LOGAPP, "! %s (bad name)\n", filepath ); 
		return FALSE; 
	}

	// check unique id
	int idx = MusicFind(id);
	if( idx!=-1 )
	{
		m_music_fail++;
		m_music_duplicates++;
		dlog(LOGSYS, "! %s (duplicate id)\n", filepath, id);
		return FALSE;
	}

	// load and decompress
	A9STREAM stream = A9_StreamCreate(filepath);
	if(!stream)
	{
		m_music_fail++;
		dlog(LOGSYS, "! %s (failed to load)\n", filepath);
		return FALSE;
	}

	// add to list
	tMusicProto* proto = snew tMusicProto();
	proto->m_id = id;
	proto->m_group = group;
	proto->m_stream = stream;
	m_musicproto.Add(proto);

	if(IS_DEVELOPER()) // log for developers
		dlog(LOGAPP, "  %s\n", filepath );

	return TRUE;
	unguard()
}

void FFCallback_Music( char* filepath, BOOL dir )
{
	guard(FFCallback_Music)
	if(dir) return;
	BOOL ret = g_sound.MusicLoadFile(filepath,g_sound.m_music_group);
	unguard()
}

BOOL cDizSound::MusicLoad( char* path, int group )
{
	guard(cDizSound::MusicLoad)
	if(!A9_IsReady()) { dlog(LOGAPP,"Sound disabled - no musics are loaded.\n"); return FALSE; }

	if(!path || !path[0]) return FALSE; // invalid path
	int szlen = (int)strlen(path);
	if(path[szlen-1]!='\\') strcat(path,"\\");
	_strlwr(path);
	dlog(LOGAPP,"Loading musics from \"%s\" (group=%i)\n", path, group);

	// init
	m_music_total		= 0;
	m_music_fail		= 0;
	m_music_duplicates	= 0;
	m_music_group		= group;

	// find files on disk
	int archivefiles = F9_ArchiveGetFileCount(0);
	if(archivefiles==0) // if no archive found then open from disk
	{
		file_findfiles( path, FFCallback_Music, FILE_FINDREC );
	}
	else // if archive opened, load from it
	{
		for(int i=0;i<archivefiles;i++)
		{
			char* filename = F9_ArchiveGetFileName(0,i);
			if(strstr(filename,path)==filename)
			{
				FFCallback_Music(filename,FALSE);
			}
		}
	}

	// report
	dlog(LOGAPP, "Music report: total=%i, failed=%i (duplicates=%i)\n\n", m_music_total, m_music_fail, m_music_duplicates );

	return TRUE;
	unguard()
}

void cDizSound::MusicUnload( int group )
{
	guard(cDizSound::MusicUnload)
	int i;
	for(i=0;i<m_musicproto.Size();i++)
	{
		if(m_musicproto[i]->m_group == group)
		{
			if(i==m_musicidx) MusicStop(); // stop if current
			if(m_musicnext==i) m_musicnext=-1; // clear if next
			m_musicproto.Del(i);
			i--;
		}
	}
	unguard()
}

void cDizSound::MusicFade( int out, int in )
{
	guard(cDizSound::MusicFade);
	m_musicfadeout = out;
	m_musicfadein = in;
	unguard();
}

int cDizSound::MusicPlay( int id, int start )
{
	guard(cDizSound::MusicPlay)

	int protoidx = MusicFind(id);
//@	if(protoidx==-1) return -1; // invalid id >> allow -1 for next music, to stop with fade
	m_musicnext = protoidx; // store for later play
	m_musicstart = start>=0 ? start : 0; // store valid for later start
	if(m_musicfadeout==0)  // @HM
	{
		if(m_musicnext!=m_musicidx)
			m_musicvol = 0.0f; // update will stop current music immediately, if any
	}
	return 0; // ok
	unguard()
}

int cDizSound::MusicPlaying()
{
	guard(cDizSound::MusicPlaying)
	// we are interested in what is the scheduled song, ignoring the short fadeout of the current one
	if(m_musicnext==-1) return -1;
	return m_musicproto[m_musicnext]->m_id;
	unguard()
}

int	cDizSound::MusicPosition()
{
	guard(cDizSound::MusicPosition);
	if(m_musicnext==-1) return -1; // no next music
	if(m_musicidx!=m_musicnext) return m_musicstart; // current is fading, next is about to start from m_musicstart
	// we are playing the next music, report curent position
	if(!m_music) return m_musicstart; // should not happend
	int musicpos = (int)( (float)A9_StreamGetPosition(m_music) / (float)A9_StreamGet(m_music,A9_FREQUENCY) * 1000.0f );
	return musicpos; // in ms
	unguard();
}

void cDizSound::MusicStop()
{
	guard(cDizSound::MusicStop)
	if(m_musicidx==-1) return;
	if(!m_music) { m_musicidx=-1; return; } // should not happened
	A9_StreamSet(m_music,A9_VOLUME,A9_VOLUMEMIN); // silence
	A9_StreamStop(m_music);
	m_music = NULL;
	m_musicidx = -1;
	m_musicpaused = FALSE;
	unguard()
}

void cDizSound::MusicPause( BOOL pause )
{
	guard(cDizSound::MusicPause);
	if( m_musicidx==-1) return;
	if(!m_music) { m_musicidx=-1; return; } // should not happened
	if(pause)
	{
		if(m_musicpaused) return; // already paused
		A9_StreamStop(m_music); // stop
	}
	else
	{
		if(!m_musicpaused) return; // not paused
		A9_StreamPlay(m_music,TRUE); // play
	}
	m_musicpaused = pause;
	unguard();
}

void cDizSound::MusicUpdate( float dtime )
{
	guard(cDizSound::MusicUpdate)

	if(m_musicidx==-1) // nothing is playing
	{
		if(m_musicnext!=-1) // music scheduled
		{
			// set current stream
			m_music = m_musicproto[m_musicnext]->m_stream;
			if(!m_music) { m_musicnext=-1; return; } // cration failure (invalid music file)

			// prepare volume
			if(m_musicfadein>0)
			{
				A9_StreamSet(m_music,A9_VOLUME,A9_VOLUMEMIN); //start from silence
				m_musicvol = 0.0f;
			}
			else
			{
				A9_StreamSet(m_music,A9_VOLUME,A9_VolumePercentToDecibel(g_cfg.m_volmusic));
				m_musicvol = 1.0f;
			}

			// play stream (always looping)
			int start = (int)( (float)m_musicstart / 1000.0f * (float)A9_StreamGet(m_music,A9_FREQUENCY) );
			if(start<0 || start>=A9_StreamGet(m_music,A9_SIZE)) start=0;
			A9_StreamSetPosition(m_music,start);
			A9_StreamPlay(m_music,TRUE);
			m_musicidx = m_musicnext;
			m_musicpos = start;
		}
	}
	else // music is playing
	{
		if(!m_music) { m_musicidx=-1; return; } // should not happened
		if(m_musicpaused) return; // don't update volumes if paused
		if(!A9_StreamIsPlaying(m_music)) return; // don't update volumes if it's not yet playing

		float fadeoutstep	= (m_musicfadeout>0) ? (dtime / (float)m_musicfadeout) : 1.0f;
		float fadeinstep	= (m_musicfadein>0)  ? (dtime / (float)m_musicfadein)  : 1.0f;

		if( m_musicidx == m_musicnext ) // we are keeping this song
		{
			if(m_musicvol<1.0f) // have not reached full volume yet so we have fadein
			{
				m_musicvol+=fadeinstep;
				if(m_musicvol>1.0f) m_musicvol=1.0f;
				A9_StreamSet(m_music, A9_VOLUME, A9_VolumePercentToDecibel( (int)(m_musicvol * g_cfg.m_volmusic) ));
			}
			// else we are very happy

			// check music end
			int musicpos = A9_StreamGetPosition(m_music);
			int musicsize = A9_StreamGet(m_music,A9_SIZE);
			if( musicpos < m_musicpos ) // reached the end of loop and it just started from the begining
				g_script.CallHandler(HANDLER_MUSICLOOP); // notify scripts; user can stop or play other music in the handler
			m_musicpos = musicpos;
		}
		else // we must fade this one out, because a another is waiting next
		{
			if(m_musicvol>0.0f) // have not reached silence yet, but we have fade out
			{
				m_musicvol-=fadeoutstep;
				if(m_musicvol<0.0f) m_musicvol=0.0f;
				A9_StreamSet(m_music, A9_VOLUME, A9_VolumePercentToDecibel( (int)(m_musicvol * g_cfg.m_volmusic) ));
			}
			else
			{
				MusicStop(); // stop and destroy current music
				// in the next update the next song will be made current and played
			}
		}
	}

	unguard()
}

void cDizSound::MusicVolume( int volume )
{
	guard(cDizSound::MusicVolume)
	if(volume<0) volume=0;
	if(volume>100) volume=100;
	if(volume==g_cfg.m_volmusic) return; // same volume
	g_cfg.m_volmusic = volume;

	int volumedb = A9_VolumePercentToDecibel( (int)(m_musicvol * g_cfg.m_volmusic) );
	if(m_musicidx!=-1 && m_music)
	{
		A9_StreamSet(m_music,A9_VOLUME,volumedb);
	}
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
