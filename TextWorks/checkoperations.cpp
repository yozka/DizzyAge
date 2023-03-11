
//////////////////////////////////////////////////////////////////////////////////////////////////
// checkoperations.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "checkoperations.h"

#include "resource.h"
#include "util.h"

extern HWND	g_hwndlog;
extern char g_outputfile[MAX_PATH];
extern char g_intagprefix[MAX_PATH];
extern char g_intagsufix[MAX_PATH];
extern char g_outtagprefix[MAX_PATH];
extern char g_outtagsufix[MAX_PATH];
extern int  g_tagid;
extern int	g_tagidmax;
extern int	g_tagcount;
extern int  g_errors;
extern int	g_processedfiles;
extern char	g_extensions[3][64];

//////////////////////////////////////////////////////////////////////////////////////////////////
// Process input file, count tags, and remember the bigest id
//////////////////////////////////////////////////////////////////////////////////////////////////
void OpGetLastTag(char* inputfile,BOOL dir)
{
	// export only .gs files
	char* ext = file_path2ext(inputfile);
	if(!ext || stricmp(ext,g_extensions[0])!=0) return;

	char sz[256];

	FILE* file = fopen(inputfile, "rb"); 
	if(!file) {	sprintf(sz,"Can't open %s.",inputfile);LOGLN(sz); return; }
	fseek(file,0,2);
	int filesize = ftell(file);
	fseek(file,0,0);
	char* filebuffer = (char*)smalloc(filesize+1); 
	filebuffer[filesize]=0;
	fread(filebuffer,filesize,1,file);
	fclose(file);

	char* buffer = filebuffer;
	while(*buffer)
	{
		// find tag in inputfile
		int tagid=0;
		int tagpos=0;
		int taglen=0;
		if(!ParserSkipToTag(buffer,g_intagprefix,g_intagsufix,tagid,tagpos,taglen)) break; // done

		g_tagcount++;
		if(tagid>g_tagidmax) g_tagidmax=tagid;
		buffer += tagpos+taglen;
	}

	free(filebuffer);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Process inputfile, finds texts and replace them with tags; add tags and text in output file
//////////////////////////////////////////////////////////////////////////////////////////////////
void OpExportFile(char* inputfile, BOOL dir)
{
	// export only .gs files
	char* ext = file_path2ext(inputfile);
	if(!ext || stricmp(ext,g_extensions[0])!=0) return;

	char sz[256];

	// load input file
	LOGLN("EXPORT FILE:");
	LOGLN(inputfile);
	FILE* file = fopen(inputfile, "rb"); 
	if(!file) {	sprintf(sz,"Can't open %s.",inputfile); LOGLN(sz); return; }
	fseek(file,0,2);
	int filesize = ftell(file);
	fseek(file,0,0);
	char* filebuffer = (char*)smalloc(filesize+1); 
	filebuffer[filesize]=0;
	fread(filebuffer,filesize,1,file);
	fclose(file);
	g_processedfiles++;

	// buffer to store exported text messages
	int	msgbuffersize = 0;		// strlen
	char* msgbuffer = NULL;
	sprintf(sz,"// File: %s\r\n\r\n",inputfile);
	msgbuffersize = strlen(sz);
	msgbuffer = (char*)malloc(msgbuffersize);
	memcpy(msgbuffer,sz,msgbuffersize);

	// buffer to store new modified input file
	int	newbuffersize = 0;		// strlen
	char* newbuffer = NULL;

	int size;
	char intag[256];
	char outtag[256];

	// process messages
	char* buffer = filebuffer;
	while(TRUE)
	{
		int msgstart,msglen=0;
		int ret = ParseMessageText(buffer,msgstart,msglen);
		if(ret==0)	break; // no more texts
		if(ret==-1)	
		{ 
			sprintf(sz,"Broken text at pos %i",msgstart);
			LOGLN(sz);
			g_errors++; 
			break;
		}
		if(msglen>=2) // at least 2 "
		{
			// create tags
			sprintf(intag,"%s%i%s",g_intagprefix,g_tagid,g_intagsufix);
			sprintf(outtag,"%s%i%s",g_outtagprefix,g_tagid,g_outtagsufix);
			int intagsize = strlen(intag);
			int outtagsize = strlen(outtag);

			// copy in msg buffer
			size = msglen + outtagsize + 2;
			msgbuffer = (char*)realloc(msgbuffer,msgbuffersize+size);
			memcpy(msgbuffer+msgbuffersize,outtag,outtagsize); // tag out
			memcpy(msgbuffer+msgbuffersize+outtagsize,buffer+msgstart,msglen); // text
			memcpy(msgbuffer+msgbuffersize+outtagsize+msglen,"\r\n",2); // enter
			msgbuffersize += size;

			// copy in new buffer
			size = msgstart + intagsize;
			newbuffer = (char*)realloc(newbuffer,newbuffersize+size);
			memcpy(newbuffer+newbuffersize,buffer,msgstart); // code until text
			memcpy(newbuffer+newbuffersize+msgstart,intag,intagsize); // tag in
			newbuffersize += size;

			g_tagid++;
			g_tagcount++;
		}

		buffer += msgstart + msglen;
	}

	// copy in new buffer, the rest of the file
	size = filesize - (int)(buffer-filebuffer); // left in buffer
	if(size>0)
	{
		newbuffer = (char*)realloc(newbuffer,newbuffersize+size);
		memcpy(newbuffer+newbuffersize,buffer,size); // rest of code
		newbuffersize += size;
	}

	msgbuffer = (char*)realloc(msgbuffer,msgbuffersize+2);
	memcpy(msgbuffer+msgbuffersize,"\r\n",2); // enter
	msgbuffersize+=2;

	// save input2 file
	char inputfile2[256];
	strcpy(inputfile2,inputfile);
	char* szext = file_path2ext(inputfile2);
	if(szext) *szext=0;
	strcat(inputfile2,g_extensions[1]);
	file = fopen(inputfile2, "wb"); 
	if(file)
	{
		if(newbuffersize)
			fwrite(newbuffer,newbuffersize,1,file);
		fclose(file);
	}

	// append output file
	file = fopen(g_outputfile, "r+b"); 
	if(file) 
	{
		fseek(file,0,2);
		if(msgbuffersize)
			fwrite(msgbuffer,msgbuffersize,1,file);
		fclose(file);
	}

	// free
	if(filebuffer) sfree(filebuffer);
	if(msgbuffer) sfree(msgbuffer);
	if(newbuffer) sfree(newbuffer);
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Process input file for tags, get their text from the output file and replace then in the input2
//////////////////////////////////////////////////////////////////////////////////////////////////
void OpImportFile(char* inputfile, BOOL dir)
{
	// export only .gs files
	char* ext = file_path2ext(inputfile);
	if(!ext || stricmp(ext,g_extensions[1])!=0) return;

	char sz[256];

	// load input file
	LOGLN("IMPORT FILE:");
	LOGLN(inputfile);
	FILE* file = fopen(inputfile, "rb"); 
	if(!file) {	sprintf(sz,"Can't open %s.",inputfile);LOGLN(sz); return; }
	fseek(file,0,2);
	int filesize = ftell(file);
	fseek(file,0,0);
	char* filebuffer = (char*)smalloc(filesize+1); 
	filebuffer[filesize]=0;
	fread(filebuffer,filesize,1,file);
	fclose(file);
	g_processedfiles++;

	// load output file
	file = fopen(g_outputfile, "rb"); 
	if(!file) {	sprintf(sz,"Can't open %s.",g_outputfile);LOGLN(sz); return; }
	fseek(file,0,2);
	int msgbuffersize = ftell(file);
	fseek(file,0,0);
	char* msgbuffer = (char*)smalloc(msgbuffersize+1); 
	msgbuffer[msgbuffersize]=0;
	fread(msgbuffer,msgbuffersize,1,file);
	fclose(file);

	// new input file
	int newbuffersize = 0;
	char* newbuffer = NULL;

	// process tags
	int size;
	char outtag[256];
	char* buffer = filebuffer;
	while(*buffer)
	{
		// find tag in inputfile
		int tagid=0;
		int tagpos=0;
		int taglen=0;
		if(!ParserSkipToTag(buffer,g_intagprefix,g_intagsufix,tagid,tagpos,taglen)) break; // done

		// find tag in the output file
		sprintf(outtag,"%s%i%s",g_outtagprefix,tagid,g_outtagsufix);
		char* sztag = strstr(msgbuffer,outtag);
		if(sztag==NULL)
		{
			sprintf(sz,"Tag %i (%s) not found in outputfile!",tagid,outtag);
			LOGLN(sz);
			buffer += tagpos+taglen; // jump over tag
			g_errors++; 
			continue;
		}

		// find the string comming after the tag, in the output file
		int msgstart,msglen=0;
		int ret = ParseMessageText(sztag,msgstart,msglen);
		if(ret!=1 || msglen<2) // at least 2 quotes
		{
			sprintf(sz,"Broken output tag %i", tagid);
			LOGLN(sz);
			buffer += tagpos+taglen; // jump over tag
			g_errors++; 
			continue;
		}

		// copy in new buffer
		size = tagpos + msglen;
		newbuffer = (char*)realloc(newbuffer,newbuffersize+size);
		if(tagpos>0) memcpy(newbuffer+newbuffersize,buffer,tagpos); // code until tag
		if(msglen>0) memcpy(newbuffer+newbuffersize+tagpos,sztag+msgstart,msglen); // text
		newbuffersize += size;

		g_tagcount++;
		g_tagid = tagid;
		buffer += tagpos+taglen;
	}

	// copy in new buffer, the rest of the file
	size = filesize - (int)(buffer-filebuffer); // left in buffer
	if(size>0)
	{
		newbuffer = (char*)realloc(newbuffer,newbuffersize+size);
		memcpy(newbuffer+newbuffersize,buffer,size); // rest of code
		newbuffersize += size;
	}

	// save input2 file
	char inputfile2[256];
	strcpy(inputfile2,inputfile);
	char* szext = file_path2ext(inputfile2);
	if(szext) *szext=0;
	strcat(inputfile2,g_extensions[2]);
	file = fopen(inputfile2, "wb"); 
	if(file)
	{
		if(newbuffersize)
			fwrite(newbuffer,newbuffersize,1,file);
		fclose(file);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Skip comment (supports both line and multiline)
// Returns TRUE if comment was found, and skipped
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ParseSkipComments( char* buffer, int& parsedlen )
{
	int len;
	parsedlen = 0;

	// skip line comment
	if(buffer[0]=='/' && buffer[1]=='/')
	{
		buffer++;
		parsedlen++;
		parser_skipline(buffer, len);
		parsedlen+=len;
		return TRUE;
	}

	// skip multiline comment
	if(buffer[0]=='/' && buffer[1]=='*') 
	{
		buffer+=2;
		parsedlen+=2;
		parser_skiptotoken(buffer, "*/", len);
		parsedlen+=len;
		if(buffer[len]!=0) parsedlen+=2; // if found valid, jump over it
		// if bad comment, it consumes all the buffer
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Skips string (supports instring quotes (\"), and concatenated strings ("text1" "text2")
// Returns TRUE if string was found and skipped; set parsedlen after the last quote
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ParseSkipString( char* buffer, int& parsedlen )
{
	int len;
	parsedlen = 0;
	if(*buffer!='\"') return FALSE; // no string here
	buffer++;
	parsedlen++;
	
	// smart find ending quote
	while(*buffer)
	{
		// skip to the next quote
		parser_skiptochar(buffer,"\"",len);
		if(buffer[len]==0) return FALSE; // bad string, without ending quote!

		// jump over quote
		buffer += len+1;
		parsedlen += len+1;

		// check jumped quote if it was an in-string quote
		if(len>0)
		{
			// count backslashes before quote
			int cnt=0;
			char* bufferx = buffer-2; // just before quote
			while(*bufferx=='\\' && cnt<len) { bufferx--; cnt++; }
			if(cnt%2) // if not an even number of backslashes, then the last one belongs to the quote
				continue; // so ignore it
		}

		// check quote if concatenate quote
		parser_skipspace(buffer,len); // skip spaces after quote
		if(buffer[len]=='\"') // if after spaces comes another quote
		{
			// jump over second quote
			buffer += len+1;
			parsedlen += len+1;
			continue; // if so, then ignore it
		}

		// found the last quote!
		return TRUE;
	}

	// eos
	return FALSE; // bad string, witohut ending quote!
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Parses buffer to find where a valid text string starts and how long is it
// Buffer ends with \0, text will includes both quotes \"
//////////////////////////////////////////////////////////////////////////////////////////////////
int ParseMessageText( char* buffer, int& msgstart, int& msglen )
{
	int pos = 0;
	int len;
	msgstart = 0;
	msglen = 0;
	char* buffer0=buffer;
	
	// skip to string
	while(*buffer)
	{
		if(*buffer=='\"') break; // found

		// skip comments
		if(ParseSkipComments(buffer,len))
			buffer += len;
		else
			buffer++; // skip normal code character
	}
	if(*buffer==0) return 0; // not found

	msgstart = (int)(buffer-buffer0);
	
	// skip to after string
	if(!ParseSkipString(buffer,len)) return -1; // invalid string (not ending)

	// found a good one
	msglen = len;
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Parse, finds and get token info (pos, id, strlen)
// Returns TRUE if valid token found
// Token is in PREFIX%iSUFIX format.
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ParserSkipToTag(char* buffer, char* tagprefix, char* tagsufix, int& tagid, int& tagpos, int& taglen )
{
	tagid=0;
	tagpos=0;
	taglen=0;

	int len;
	int tagprefix_size = strlen(tagprefix);
	int tagsufix_size = strlen(tagsufix);
	char sz[256];

	char* buffer00 = buffer;

	while(*buffer)
	{
		char* buffer0 = buffer;

		// skip to prefix
		while(*buffer)
		{
			int ss;
			for(ss=0;ss<tagprefix_size;ss++)
				if(buffer[ss]!=tagprefix[ss]) 
					break;
			if(ss==tagprefix_size) break; // found here
			// skip comments
			if(ParseSkipComments(buffer,len))
				buffer += len;
			else
				buffer++; // skip normal code character
		}
		if(*buffer==0) return FALSE; // no prefix found at all
		
		// jump over prefix
		char* buffer1 = buffer;
		buffer += tagprefix_size;

		// check id+sufix
		int ret = sscanf(buffer,"%i",&tagid);
		if(ret==1) // got a number
		{
			sprintf(sz,"%i%s",tagid,tagsufix);
			len = strlen(sz);
			if(strstr(buffer,sz)!=buffer)
			{
				// not matching back
				buffer = buffer0+1;
				continue;
			}

			// found it!
			tagpos = buffer1-buffer00;
			taglen = tagprefix_size + len;
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
