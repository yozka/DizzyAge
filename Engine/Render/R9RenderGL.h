///////////////////////////////////////////////////////////////////////////////////////////////////
// R9RenderGL.h
// Render platform OpenGL implementation
// Control:
// R9_ENABLE_GLDLL
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __R9RENDERGL_H__
#define __R9RENDERGL_H__

#include <gl\gl.h>
#include "R9Render.h"

#define R9_BATCHSIZE_GL		(3*64)	// number of vertices in the batch buffer

///////////////////////////////////////////////////////////////////////////////////////////////////
// Extensions defines
#define GL_COMBINE                                     0x8570
#define GL_COMBINE_RGB                                 0x8571
#define GL_SOURCE0_RGB                                 0x8580
#define GL_SOURCE1_RGB                                 0x8581
#define GL_OPERAND0_RGB                                0x8590
#define GL_OPERAND1_RGB                                0x8591
#define GL_PRIMARY_COLOR                               0x8577

///////////////////////////////////////////////////////////////////////////////////////////////////

struct r9CapsGL
{
	BOOL	m_texture_env_combine;	// extension used for alpharep (1.3)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class r9RenderGL : public r9Render
{
public:
					r9RenderGL();
virtual				~r9RenderGL();

virtual	BOOL		LoadDll();
virtual	void		UnloadDll();
		void		LogAdapterInfo();
virtual	int			GatherDisplayModes( r9DisplayMode* displaymode );

virtual	BOOL		Init( HWND hwnd, r9Cfg* cfg );
virtual	void		Done();
virtual	BOOL		IsReady();

virtual	R9TEXTURE	TextureCreate( r9Img* img );
virtual	R9TEXTURE	TextureCreateTarget( int width, int height );
virtual	void		TextureDestroy( R9TEXTURE tex );

virtual	void		SetTexture( R9TEXTURE tex );
virtual void		SetState( int state, int value );
virtual	void		SetViewport( fRect& rect );
virtual	void		SetView( int x, int y, dword flip );
virtual	void		SetDefaultStates();

virtual	void		Clear( dword color );
virtual	BOOL		BeginScene( R9TEXTURE target=NULL );
virtual	void		EndScene();
virtual	void		Present();
virtual	BOOL		ToggleVideoMode();

virtual	void		Push( r9Vertex* vx, int vxs, int primitive );
virtual	void		Flush();

virtual	BOOL		SaveScreenShot( fRect* rect=NULL, BOOL full=TRUE);
virtual BOOL		TakeScreenShot( r9Img* img, fRect* rect=NULL, BOOL full=TRUE );
virtual BOOL		CopyTargetToImage( R9TEXTURE target, r9Img* img, fRect* rect );

protected:

		BOOL				CreateRenderWindow();			// create child window
		BOOL				DestroyRenderWindow();			// destroy child window
		static	LRESULT CALLBACK	WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
		void				PrepareParentWindow();			// set window style
		BOOL				GL_CreateDevice();				// create a direct3d device
		BOOL				GL_BatchCreate();				// create buffer for batching
inline	void				GL_BindTexture();				// bind texture and apply wrap and filter

		HDC					m_hdc;							// device context
		HGLRC				m_hrc;							// rendering context

		r9CapsGL			m_caps;							// render caps
		
		int					m_batchcount;					// no of vertexes in the batch buffer
		r9Vertex*			m_batchbuffer;					// batch buffer

		r9Texture*			m_textarget;					// current target, or NULL if default target is set

// DLL functions
typedef HGLRC			(__stdcall *twglCreateContext		)(HDC);
typedef BOOL			(__stdcall *twglMakeCurrent			)(HDC, HGLRC);
typedef BOOL			(__stdcall *twglDeleteContext		)(HGLRC);
typedef const GLubyte*	(__stdcall *tglGetString			)(GLenum name);
typedef void			(__stdcall *tglGenTextures			)(GLsizei n, GLuint *textures);
typedef void			(__stdcall *tglBindTexture			)(GLenum target, GLuint texture);
typedef void			(__stdcall *tglTexImage2D			)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void			(__stdcall *tglDeleteTextures		)(GLsizei n, const GLuint *textures);
typedef void			(__stdcall *tglTexParameteri		)(GLenum target, GLenum pname, GLint param);
typedef void			(__stdcall *tglBlendFunc			)(GLenum sfactor, GLenum dfactor);
typedef void			(__stdcall *tglTexEnvi				)(GLenum target, GLenum pname, GLint param);
typedef void			(__stdcall *tglEnable				)(GLenum cap);
typedef void			(__stdcall *tglShadeModel			)(GLenum mode);
typedef void			(__stdcall *tglViewport				)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void			(__stdcall *tglMatrixMode			)(GLenum mode);
typedef void			(__stdcall *tglLoadIdentity			)(void);
typedef void			(__stdcall *tglOrtho				)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef void			(__stdcall *tglClearColor			)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void			(__stdcall *tglClear				)(GLbitfield mask);
typedef void			(__stdcall *tglEnableClientState	)(GLenum array);
typedef void			(__stdcall *tglColorPointer			)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void			(__stdcall *tglVertexPointer		)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void			(__stdcall *tglTexCoordPointer		)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void			(__stdcall *tglDrawArrays			)(GLenum mode, GLint first, GLsizei count);
typedef void			(__stdcall *tglDisableClientState	)(GLenum array);
typedef void			(__stdcall *tglPixelStorei			)(GLenum pname, GLint param);
typedef void			(__stdcall *tglReadPixels			)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
typedef void			(__stdcall *tglCopyTexImage2D		)(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
typedef void			(__stdcall *tglGetTexImage			)(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);

static	twglCreateContext		m_wglCreateContext		;
static	twglMakeCurrent			m_wglMakeCurrent		;		
static	twglDeleteContext		m_wglDeleteContext		;
static	tglGetString			m_glGetString			;
static	tglGenTextures			m_glGenTextures			;
static	tglBindTexture			m_glBindTexture			;
static	tglTexImage2D			m_glTexImage2D			;
static	tglDeleteTextures		m_glDeleteTextures		;
static	tglTexParameteri		m_glTexParameteri		;
static	tglBlendFunc			m_glBlendFunc			;
static	tglTexEnvi				m_glTexEnvi				;
static	tglEnable				m_glEnable				;
static	tglShadeModel			m_glShadeModel			;
static	tglViewport				m_glViewport			;		
static	tglMatrixMode			m_glMatrixMode			;
static	tglLoadIdentity			m_glLoadIdentity		;		
static	tglOrtho				m_glOrtho				;
static	tglClearColor			m_glClearColor			;
static	tglClear				m_glClear				;
static	tglEnableClientState	m_glEnableClientState	;
static	tglColorPointer			m_glColorPointer		;		
static	tglVertexPointer		m_glVertexPointer		;
static	tglTexCoordPointer		m_glTexCoordPointer		;
static	tglDrawArrays			m_glDrawArrays			;
static	tglDisableClientState	m_glDisableClientState	;
static	tglPixelStorei			m_glPixelStorei			;
static	tglReadPixels			m_glReadPixels			;
static	tglCopyTexImage2D		m_glCopyTexImage2D		;
static	tglGetTexImage			m_glGetTexImage			;

};

inline void r9RenderGL::GL_BindTexture()
{
	guardfast(r9RenderGL::GL_BindTexture);
	// texture
	GLint i = 0;
	if(m_texture) i = (GLuint)(intptr)m_texture->m_handler;
	m_glBindTexture(GL_TEXTURE_2D, i);
	// taddress
	i = (GetState(R9_STATE_TADDRESS)==R9_TADDRESS_WRAP) ? GL_REPEAT : GL_CLAMP;
	m_glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,i);
	m_glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,i);
	// filter
	i = GetState(R9_STATE_FILTER) ? GL_LINEAR : GL_NEAREST;
	m_glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,i);
	m_glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,i);
	// alpha replicate
	if(m_caps.m_texture_env_combine)
	{
		if(GetState(R9_STATE_BLEND)==R9_BLEND_ALPHAREP)
		{
			m_glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
			m_glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
			m_glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR );
			m_glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_RGB );
			m_glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE );
			m_glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_ALPHA );
		}
		else
		{
			m_glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
			// m_glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE );
		}
	}	
	unguardfast();
}



#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
