//////////////////////////////////////////////////////////////////////////////////////////////////
// DizSound.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __DIZSOUND_H__
#define __DIZSOUND_H__

#include "E9System.h"
#include "E9List.h"
#include "D9Debug.h"
#include "A9Audio.h"

#define	SOUND_VOICES		16		// max number of samples playing at once

//////////////////////////////////////////////////////////////////////////////////////////////////
// structs
//////////////////////////////////////////////////////////////////////////////////////////////////
struct tSoundProto
{
	tSoundProto()			{ m_id=0; m_group=0; m_instances=0; m_bufferproto=NULL; }
	~tSoundProto()			{ if(m_bufferproto) A9_BufferDeprecache(m_bufferproto); }
	int						m_id;			// sample id
	int						m_group;		// resource group
	int						m_instances;	// how many instances of this proto are allowed to play simultaneous
	A9BUFFERPROTO			m_bufferproto;	// bufferproto
};

struct tMusicProto
{
	tMusicProto()			{ m_id=0; m_group=0; m_stream = NULL; }
	~tMusicProto()			{ if(m_stream) A9_StreamDestroy(m_stream); }
	int						m_id;			// music id
	int						m_group;		// resource group
	A9STREAM				m_stream;		// stream proto
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// Sound manager
//////////////////////////////////////////////////////////////////////////////////////////////////
class cDizSound
{
public:
							cDizSound			();
							~cDizSound			();
						
		// init			
		BOOL				Init				();								// init
		void				Done				();								// done
		void				Update				();								// update called every frame
						
		// samples		
		BOOL				SampleLoad			( char* path, int group=0 );	// load samples from a path (protos)
		BOOL				SampleLoadFile		( char* filepath, int group=0 );// load a sample file (proto)
		void				SampleUnload		( int group=0 );				// destroy all samples (proto)
		int					SamplePlay			( int id, int loop=0 );			// play a proto sample; return voiceidx or -1 if failed
		int					SamplePlaying		( int voiceidx );				// return sample id if playing or -1 if not playing
		void				SampleStop			( int voiceidx );				// stop a voice
		void				SampleStopAll		( int id=-1 );					// stop all voices of a sample id, or all voices if id is -1
		void				SampleVolume		( int volume );					// set samples volume
inline	int					SampleFind			( int id )						{ for(int i=0;i<m_sampleproto.Size();i++) if(m_sampleproto[i]->m_id==id) return i; return -1; }

		A9BUFFER			m_voice[SOUND_VOICES];								// list with voices buffers
		cPList<tSoundProto>	m_sampleproto;										// list with loaded samples
		int					m_voicecount;										// playing voices (debug info)

		int					m_sample_total;										// status report on total samples declared (load+failed)
		int					m_sample_fail;										// status report on samples failed to load
		int					m_sample_duplicates;								// status report on id duplicates
		int					m_sample_group;										// current loading group

		// music
		BOOL				MusicLoad			( char* path, int group=0 );	// load all musics from a path (protos)
		BOOL				MusicLoadFile		( char* filename, int group=0 );// load a music file (proto)
		void				MusicUnload			( int group=0 );				// destroy all musics (proto)
		void				MusicFade			( int out, int in );			// set fade values
		int					MusicPlay			( int id, int start=0 );		// set a music to play next from a specified position; returns -1 if failed; don't necessarily plays at once
		int					MusicPlaying		();								// return id of next music (-1 if none) = m_musicnext
		int					MusicPosition		();								// return position of next music in miliseconds; -1 if no next music programmed
		void				MusicStop			();								// music stop immediately
		void				MusicPause			( BOOL pause );					// music pause; stop, but remember where it was
		void				MusicUpdate			( float dtime );				// deals with the play, stop and volume management
		void				MusicVolume			( int volume );					// set music volume
inline	int					MusicFind			( int id )						{ for(int i=0;i<m_musicproto.Size();i++) if(m_musicproto[i]->m_id==id) return i; return -1; }

		A9STREAM			m_music;											// music stream
		int					m_musicidx;											// current playing music index in proto list (-1 if none)
		int					m_musicnext;										// next music programmed to play (-1 if none); this is considered the current music by the logic
		int					m_musicstart;										// position where the next music will start in miliseconds; 0=begining of music
		int					m_musicfadein;										// music fade in (seconds)
		int					m_musicfadeout;										// music fade out (seconds)
		int					m_musicpos;											// current music position (used to know when music ends) in samples
		BOOL				m_musicpaused;										// music paused
		float				m_musicvol;											// current playing music volume factor [0..1] (used for fades)
		cPList<tMusicProto>	m_musicproto;										// list with loaded musics

		int					m_music_total;										// status report on total musics declared (load+failed)
		int					m_music_fail;										// status report on musics failed to load
		int					m_music_duplicates;									// status report on id duplicates
		int					m_music_group;										// current loading group
};

extern cDizSound g_sound;

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
