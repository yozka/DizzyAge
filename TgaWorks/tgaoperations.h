//////////////////////////////////////////////////////////////////////////////////////////////////
// tgaoperations.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __TGAOPERATIONS_H__
#define __TGAOPERATIONS_H__

//////////////////////////////////////////////////////////////////////////////////////////////////
// defines
//////////////////////////////////////////////////////////////////////////////////////////////////
#define COLOR_MAX		9
#define ALPHA_MAX		5

//////////////////////////////////////////////////////////////////////////////////////////////////
// operations
//////////////////////////////////////////////////////////////////////////////////////////////////
void	OpExplode(char *srcfile,char *file1, char *file2);
void	OpImplode(char *file1, char *file2,char *destfile);
void	OpBuildAlpha(char *file,dword alphacolor);
void	OpMultiBuildAlpha(char *file, BOOL dir);
void	OpCheckColor(char *file);
void	OpMultiCheckColor(char *file, BOOL dir);

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////