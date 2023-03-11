// -----------------------------------------------------------------------
// Declarations
// -----------------------------------------------------------------------
#def WNDW	240	// window size width
#def WNDH	315 // window size height

#def SCRWB	256	// game screen with borders
#def SCRHB	192	// game screen with borders

// change this if you want other name
#def GAMEEXE	"dizzy.exe"		// game exe file
#def GAMEINI	"dizzy.ini"		// game ini file
#def GAMEPAK	"dizzy.pak"		// game pak file
#def GAMELOG	"dizzy.log"		// game log file

// Game info
str g_game_title;	
str g_game_version;	
str g_game_author;	
str g_game_website;
str g_game_readme;
str g_game_about;
str g_game_shortcut;
tab g_game_languages;	// table with languages names

str g_param;		// command line param
int g_readonly;		// true for read only medium like cd; false for hard disk
int g_install_mode;	// install mode (0=normal, 1=external)
str g_installer;	// path to external installer
str g_uninstaller;	// path to external uninstaller

// -----------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------
int g_resfnt1,g_resfnt2,g_resfnt3;
int g_resbak, g_resage, g_resbrowse;
tab g_resicon;

func LoadResources()
{
	g_reschk = -1; //@!!!
	
	g_resbak = ResBitmap("background.png");
	g_resage = ResBitmap("dizzyage.png");
	g_resbrowse = ResBitmap("browse.png");
	szicon = {"pc","disk","video","audio","input","advanced"};
	g_resicon = tab(sizeof(szicon));
	for(i=0;i<sizeof(szicon);i++)
		g_resicon[i] = ResBitmap("icon_"+szicon[i]+".png");

	g_resfnt1 = ResFont("Times New Roman", 0, 20, QUALITY);
	g_resfnt2 = ResFont("Arial", 5, 14, QUALITY);
	g_resfnt3 = ResFont("Arial", 4, 12, QUALITY);
}



// -----------------------------------------------------------------------
// Utils
// -----------------------------------------------------------------------
func MIN(a,b) { return (a<b)?a:b; }
func MAX(a,b) { return (a>b)?a:b; }

func button_img( x,y,w,h,res,cmd,tooltip )
{
	i = ItemNew();
	ItemSet(IITYPE, IITYPE_BUTTON);
	ItemSet(IIINTERACTIVE, 1);
	ItemSet(IIRECT, x,y,x+w,y+h);
	ItemSet(IIIMGRES, res);
	ItemSet(IIIMGMAP+2, w);
	ItemSet(IIIMGMAP+3, h);
	ItemSet(IIBUTCMDTEXT, cmd);
	if(?tooltip)
		ItemSet(IITOOLTIP, tooltip);
	return i;
}

func button_txt( text, x,y,w,h,res,cmd,tooltip )
{
	i = ItemNew();
	ItemSet(IITYPE, IITYPE_BUTTON);
	ItemSet(IIINTERACTIVE, 1);
	ItemSet(IIRECT, x,y,x+w,y+h);
	ItemSet(IITEXT, text);
	ItemSet(IITXTRES, res);
	ItemSet(IICOLOR+0, 0x000000);
	ItemSet(IICOLOR+1, 0x0000ff);
	ItemSet(IIBUTCMDTEXT, cmd);
	if(?tooltip)
		ItemSet(IITOOLTIP, tooltip);
	return i;
}

func textbox( text, x,y,w,h,res )
{
	i = ItemNew();
	ItemSet(IITYPE, IITYPE_TEXT);
	ItemSet(IIRECT, x,y,x+w,y+h);
	ItemSet(IITEXT, text);
	ItemSet(IITXTRES, res);
	ItemSet(IICOLOR+0, 0x000000);
	ItemSet(IITXTSINGLELINE,0);
	return i;
}

func background_img( icon )
{
	// Background
	ItemNew();
	ItemSet(IITYPE, IITYPE_IMG);
	ItemSet(IIX2, WNDW);
	ItemSet(IIY2, WNDH);
	ItemSet(IIIMGRES, g_resbak);
	ItemSet(IIIMGMAP+2, WNDW);
	ItemSet(IIIMGMAP+3, WNDH);

	// DizzyAGE
	ItemNew();
	ItemSet(IITYPE, IITYPE_IMG);
	ItemSet(IIRECT, 174, 206, 174+35, 206+58);
	ItemSet(IIIMGRES, g_resage);
	ItemSet(IIIMGMAP+2, 35);
	ItemSet(IIIMGMAP+3, 58);
	ItemSet(IIIMGBLEND, IIBLEND_COLORKEY);
	ItemSet(IICOLOR+3, 0xff00ff);

	ItemNew();
	ItemSet(IITYPE, IITYPE_BUTTON);
	ItemSet(IIINTERACTIVE, 1);
	ItemSet(IIRECT, 174, 206, 174+35, 206+58);
	ItemSet(IIBUTCMDTEXT, "gs_shell(`http://www.yolkfolk.com/dizzyage`);");
	
	// Icon
	if(!?icon) return;
	ItemNew();
	ItemSet(IITYPE, IITYPE_IMG);
	ItemSet(IIRECT, 30, 55, 30+34, 55+34);
	ItemSet(IIIMGRES, g_resicon[icon]);
	ItemSet(IIIMGMAP+2, 34);
	ItemSet(IIIMGMAP+3, 34);
	ItemSet(IIIMGBLEND, IIBLEND_COLORKEY);
	ItemSet(IICOLOR+3, 0xff00ff);
}

func button_main( text, x, y, w, cmd, tooltip )
{
	h=20;
	i = ItemNew();
	ItemSet(IITYPE, IITYPE_BUTTON);
	ItemSet(IIINTERACTIVE, 1);
	ItemSet(IIRECT, x,y,x+w,y+h);
	ItemSet(IITEXT, text);
	ItemSet(IITXTALIGN, IIALIGN_CENTERX | IIALIGN_CENTERY );
	ItemSet(IITXTRES, g_resfnt1);
	ItemSet(IICOLOR+0, 0x000000);
	ItemSet(IICOLOR+1, 0x000000);
	ItemSet(IIBUTCMDTEXT, cmd);
	if(?tooltip)
		ItemSet(IITOOLTIP, tooltip);
	return i;
}

func button_disable()
{
	ItemSet(IIINTERACTIVE,0);
	ItemSet(IICOLOR+0, 0x487AAE);
}

func CfgUpdate( group, key, step, min, max, noroll )
{
	val=0;
	if(!gs_inigetint(GAMEINI,group,key,&val)) return;
	val+=step;
	if(?noroll)
	{
		if(val>max) val=max;
		if(val<min) val=min;
	}
	else
	{
		if(val>max) val=min;
		if(val<min) val=max;
	}
	gs_inisetint( GAMEINI,group,key,val );
}

func PageMessage( message, returncmd, timesec )
{
	DlgClear();
	background_img();
	textbox(message,(WNDW-200)/2,120,200,100,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_CENTERX);
	DlgRepaint();
	if(timesec>0)
	{
		timestart = gs_time();
		while(gs_time()-timestart<timesec*1000); // wait seconds
	}
	if(returncmd!="") gs_dostring(returncmd);
}	

func FileExists( filepath )
{
	if(filepath=="") return false;
	f = gs_fileopen(filepath,0);
	if(!f) return false;
	gs_fileclose(f);
	return true;
}

// -----------------------------------------------------------------------
// Page main
// -----------------------------------------------------------------------
func PageMain()
{
	DlgClear();
	icon = g_readonly?1:0;
	background_img(icon);
	
 	h=22; w=100;
	x=(WNDW-w)/2; y=100;
	
	button_main( "run", 		x, y, w, "gs_launch(`"+GAMEEXE+"`);DlgClose();", "play the game" );
	if(!FileExists(GAMEEXE)) button_disable();
	y+=h;
	
	button_main( "config", 		x, y, w, "PageConfig();", "open config page" );
	if(g_readonly) button_disable();
	y+=h;
	
	// external installers
	if(g_install_mode==1)
	{
		button_main( "install", 	x, y, w, "RunInstaller();", "install the game" );
		if(!FileExists(g_installer)) button_disable();
		y+=h;
	
		button_main( "uninstall", 	x, y, w, "RunUninstaller();" , "uninstall the game" ); 
		if(!FileExists(g_uninstaller)) button_disable();
		y+=h;
	}
	else // standard
	{
		button_main( "install", 	x, y, w, "PageInstallPre();", "install game's shortcuts" );
		if(g_readonly) button_disable();
		y+=h;
		
		button_main( "uninstall", 	x, y, w, "PageUninstallPre();", "uninstall the game" );
		if(g_readonly) button_disable();
		y+=h;
	}
	
	button_main( "about", 		x, y, w, "PageAbout();", "open about page" ); y+=h;
	
	y=100+h*6;
	button_main( "exit", 		x, y, w, "DlgClose();", "exit setup" ); y+=h;
	
	DlgRepaint();
}

// -----------------------------------------------------------------------
// Page config
// -----------------------------------------------------------------------
func PageConfig()
{
	DlgClear();
	background_img();

 	h=22; w=100;
	x=(WNDW-w)/2; y=100;
	button_main( "video", 		x, y, w, "PageConfigVideo();", "set video options");  y+=h;
	button_main( "audio", 		x, y, w, "PageConfigAudio();", "set audio options" ); y+=h;
	button_main( "input", 		x, y, w, "PageConfigInput();", "set input options" ); y+=h;
	button_main( "advanced", 	x, y, w, "PageConfigAdvanced();", "set advanced options" ); y+=h;

	// multilanguage support
	val=0;
	if(!gs_inigetint(GAMEINI,"LOCALIZATION","language",&val)) val=0;
	if( val>=0 && val<sizeof(g_game_languages) && sizeof(g_game_languages)>0 )
	{
		button_main( g_game_languages[val], x, y, w, "ConfigSet(0);", "choose game's language.");  y+=h;
		if(sizeof(g_game_languages)<2) button_disable();
		y+=h;
	}
	else
		y+=h*2;

	button_main( "back", 		x, y, w, "PageMain();", "back to main page" ); y+=h;
	
	DlgRepaint();
}

func ConfigSet( cmd )
{
	if(cmd==0)	CfgUpdate("LOCALIZATION","language",1,0,sizeof(g_game_languages)-1);
	PageConfig(); // recreate page
}

// -----------------------------------------------------------------------
// Page config video
// -----------------------------------------------------------------------
func PageConfigVideo()
{
	DlgClear();
	background_img(2);	
	
	val=0; val2=0;
	h=22; w=100;
	x=(WNDW-w)/2; y=100;
	
	// read
	api=1;		gs_inigetint(GAMEINI,"VIDEO","api",&api);
	windowed=1;	gs_inigetint(GAMEINI,"VIDEO","windowed",&windowed);
	width=640;	gs_inigetint(GAMEINI,"VIDEO","width",&width);
	height=480;	gs_inigetint(GAMEINI,"VIDEO","height",&height);
	scale=2;	gs_inigetint(GAMEINI,"VIDEO","scale",&scale);
	desktopres = (width==DesktopW() && height==DesktopH());
	
	button_main( api?"OpenGL":"DirectX", x, y, w, "ConfigVideoSet(0);", "select video api" ); y+=h;
	button_main( (!windowed) ? "full screen" : "windowed", x, y, w, "ConfigVideoSet(1);", "select video mode:\nif you are using a LCD monitor,\nselect the native resolution \nif going in full-screen." ); y+=h;
	button_main( (scale==0) ? "full scale" : ("scale x"+(str)scale), x, y, w, "ConfigVideoSet(3);", "select game scale:\nchoose full scale for resolution fit.");  y+=h;
	button_main( (str)width+" x "+(str)height, WNDW/2-100, y, 200, "ConfigVideoSet(2);", "select video resolution:\nchoose a smaller resolution on slower computers."+(desktopres?"\n( desktop resolution selected )":"")); y+=h;
	
	y+=h*1;
	// advanced
	button_main( "advanced", 	x, y-5, w, "PageConfigVideoAdvanced();", "set advanced video options" ); y+=h;
	// if(windowed!=0) button_disable();
	
	button_main( "back", 		x, y, w, "PageConfig();", "back to config page" ); y+=h;
	
	DlgRepaint();
}

func ConfigVideoSet( cmd )
{
	val=0; w=0; h=0;
	restab = { {320,240}, {640,480}, {800,600}, {1024,768} };
	rescnt = sizeof(restab);
	
	// add default resolution
	desktopw = DesktopW();
	desktoph = DesktopH();
	for(i=0;i<rescnt;i++)
	{
		if(desktopw==restab[i][0] && desktoph==restab[i][1]) break; // already exists
		else
		if( (desktopw<restab[i][0]) || (desktopw==restab[i][0] && desktoph<restab[i][1]) ) // insert
		{	
			tabins(&restab,i,1);
			restab[i] = {desktopw,desktoph};
			break;
		}
	}
	if(i==rescnt) // add if not found
	{
		tabadd(&restab,1);
		restab[rescnt] = {desktopw,desktoph};
	}
	rescnt = sizeof(restab);

	if(cmd==0) // api
	{
		CfgUpdate("VIDEO","api",1,0,1);
	}
	else
	if(cmd==1) // windowed
	{
		CfgUpdate("VIDEO","windowed",1,0,1);
		if(!gs_inigetint(GAMEINI,"VIDEO","windowed",&val)) return;
	}
	else
	if(cmd==2) // resolution
	{
		w=0; h=0;
		if(!gs_inigetint(GAMEINI,"VIDEO","width",&w)) return;
		if(!gs_inigetint(GAMEINI,"VIDEO","height",&h)) return;
		val=0;
		for(i=0;i<rescnt;i++)
			if(restab[i][0]==w && restab[i][1]==h) break;
		val=i+1;
		if(val>=rescnt) val=0;
		gs_inisetint(GAMEINI,"VIDEO","width",restab[val][0]);
		gs_inisetint(GAMEINI,"VIDEO","height",restab[val][1]);
		// ajust scale
		w = restab[val][0];
		h = restab[val][1];
		if(!gs_inigetint(GAMEINI,"VIDEO","scale",&val)) return;
		scale = MIN(w/SCRWB,h/SCRHB);
		maxscale = MIN(restab[rescnt-1][0]/SCRWB,restab[rescnt-1][1]/SCRHB);
		if(maxscale<4) maxscale=4;
		if(scale>maxscale) scale = maxscale;
		if(val<scale) scale = val;
		gs_inisetint(GAMEINI,"VIDEO","scale",scale);
	}
	else
	if(cmd==3) // scale
	{
		maxscale = MIN(restab[rescnt-1][0]/SCRWB,restab[rescnt-1][1]/SCRHB);
		if(maxscale<4) maxscale=4;

		CfgUpdate("VIDEO","scale",1,0,maxscale);
		if(!gs_inigetint(GAMEINI,"VIDEO","scale",&val)) return;
		
		// ajust resolution
		if(val==0)
		{
			w = DesktopW();
			h = DesktopH();
		}
		else
		{
			w = SCRWB*val;
			h = SCRHB*val;
			val=0;
			for(i=rescnt-1;i>=0;i--)
			{
				if( w>restab[i][0] || h>restab[i][1] ) 
				  { val=i+1; break; }
			}
			if(val>=rescnt) val=rescnt-1;
			w = restab[val][0];
			h = restab[val][1];
		}
		
		gs_inisetint(GAMEINI,"VIDEO","width",w);
		gs_inisetint(GAMEINI,"VIDEO","height",h);
	}
	
	PageConfigVideo(); // recreate page
}

func PageConfigVideoAdvanced()
{
	DlgClear();
	background_img(2);	
	
	val=0; 
	h=22; w=120;
	x=(WNDW-w)/2; y=100;
	// bpp
	if(!gs_inigetint(GAMEINI,"VIDEO","bpp",&val)) val=32;
	button_main( (val==16)?"quality low":"quality high", x, y, w, "AdvancedConfigVideoSet(0);", (val==16)?"select quality: 16 bits per pixel":"select quality: 32 bits per pixel");  y+=h; 
	// refresh
	if(!gs_inigetint(GAMEINI,"VIDEO","refresh",&val)) val=0;
	button_main( (val==0)?"refresh default":("refresh  "+(str)val+"Hz"), x, y, w, "AdvancedConfigVideoSet(1);", "select monitor refresh rate");  y+=h; 
	// vsync
	if(!gs_inigetint(GAMEINI,"VIDEO","vsync",&val)) val=0;
	button_main( "vsync "+(val?"on":"off"), x, y, w, "AdvancedConfigVideoSet(2);", "select vertical synchronization");  y+=h; 

	y+=h*3;	
	button_main( "back", 		x, y, w, "PageConfigVideo();", "back to video options" ); y+=h;
	DlgRepaint();
}

func AdvancedConfigVideoSet( cmd )
{
	val=0; w=0; h=0;
	refreshtab = { 0,60,75,85,100,120 };
	refreshcnt = sizeof(refreshtab);
	
	if(cmd==0) // bpp
	{
		if(!gs_inigetint(GAMEINI,"VIDEO","bpp",&val)) return;
		if(val==32) val = 16; else val = 32;
		gs_inisetint(GAMEINI,"VIDEO","bpp",val);
	}
	else
	if(cmd==1) // refresh
	{
		refresh = 0;
		if(!gs_inigetint(GAMEINI,"VIDEO","refresh",&refresh)) return;
		val=0;
		for(i=0;i<refreshcnt;i++)
			if(refreshtab[i]==refresh) break;
		val=i+1;
		if(val>=refreshcnt) val=0;
		gs_inisetint(GAMEINI,"VIDEO","refresh",refreshtab[val]);
	}
	else
	if(cmd==2) // vsync
	{
		if(!gs_inigetint(GAMEINI,"VIDEO","vsync",&val)) return;
		val = !val;
		gs_inisetint(GAMEINI,"VIDEO","vsync",val);
	}
	
	PageConfigVideoAdvanced(); // recreate page
}


// -----------------------------------------------------------------------
// Page config audio
// -----------------------------------------------------------------------
func PageConfigAudio()
{
	DlgClear();
	background_img(3);	
	
	val=0;
	h=22; w=100;
	x=(WNDW-w)/2; y=100;

	// enabled
	if(!gs_inigetint(GAMEINI,"AUDIO","enabled",&val)) val=1;
	button_main( val?"enabled":"disabled", x, y, w, "ConfigAudioSet(0);","select audio support:\nchoose disabled if you have problems\nwith your sound drivers.");  y+=h;

	// volmusic	
	if(!gs_inigetint(GAMEINI,"AUDIO","volmusic",&val)) val=100;
	textbox("music "+(str)val,x,y,w,20,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_CENTERX | IIALIGN_CENTERY );
	button_main( "-",x-20, y, 20, "ConfigAudioSet(3);", "decrease music volume" );
	button_main( "+",x+w, y, 20, "ConfigAudioSet(4);", "increase music volume" );
	y+=h;

	// volfx	
	if(!gs_inigetint(GAMEINI,"AUDIO","volfx",&val)) val=100;
	textbox("fx "+(str)val,x,y,w,20,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_CENTERX | IIALIGN_CENTERY );
	button_main( "-",x-20, y, 20, "ConfigAudioSet(1);", "decrease sound effects volume" );
	button_main( "+",x+w, y, 20, "ConfigAudioSet(2);", "increase sound effects volume" );

	y+=h*4;
	button_main( "back", 		x, y, w, "PageConfig();", "back to config page"); y+=h;

	DlgRepaint();
}

func ConfigAudioSet( cmd )
{
	if(cmd==0)	CfgUpdate("AUDIO","enabled",1,0,1); else
	if(cmd==1)	CfgUpdate("AUDIO","volfx",-10,0,100,1); else
	if(cmd==2)	CfgUpdate("AUDIO","volfx",10,0,100,1); else
	if(cmd==3)	CfgUpdate("AUDIO","volmusic",-10,0,100,1); else
	if(cmd==4)	CfgUpdate("AUDIO","volmusic",10,0,100,1);
	PageConfigAudio(); // recreate page
}

// -----------------------------------------------------------------------
// Page config input
// -----------------------------------------------------------------------
func PageConfigInput()
{
	DlgClear();
	background_img(4);	
	
	val=0;
	h=22; w=140;
	x=(WNDW-w)/2; y=100;

	if(!gs_inigetint(GAMEINI,"INPUT","keyboard",&val)) val=1;
	button_main( val?"keyboard enabled":"keyboard disabled", x, y, w, "ConfigInputSet(0);", "select keyboard support");  y+=h;
	if(!gs_inigetint(GAMEINI,"INPUT","joystick",&val)) val=1;
	button_main( val?"gamepad enabled":"gamepad disabled", x, y, w, "ConfigInputSet(1);", "select gamepad support");  y+=h;
	if(!gs_inigetint(GAMEINI,"INPUT","rumble",&val)) val=1;
	button_main( val?"rumble on":"rumble off", x, y, w, "ConfigInputSet(2);", "select gamepad force feedback support");  y+=h;
	y+=h*3;
	button_main( "back", 		x, y, w, "PageConfig();", "back to config page" ); y+=h;

	DlgRepaint();
}

func ConfigInputSet( cmd )
{
	if(cmd==0)	CfgUpdate("INPUT","keyboard",1,0,1);
	if(cmd==1)	CfgUpdate("INPUT","joystick",1,0,1);
	if(cmd==2)	CfgUpdate("INPUT","rumble",1,0,1);
	enable = 0; val = 0;
	if(!gs_inigetint(GAMEINI,"INPUT","keyboard",&val)) return;
	if(val) enable = 1;
	if(!gs_inigetint(GAMEINI,"INPUT","joystick",&val)) return;
	if(val) enable = 1;
	gs_inisetint(GAMEINI,"INPUT","enabled",enable);
	
	PageConfigInput(); // recreate page
}

// -----------------------------------------------------------------------
// Page config advanced
// -----------------------------------------------------------------------
func PageConfigAdvanced()
{
	DlgClear();
	background_img(5);	
	
	val=0;
	h=22; w=180;
	x=(WNDW-w)/2; y=100;

	if(!gs_inigetint(GAMEINI,"ADVANCED","cool",&val)) val=1;
	button_main( val?"cool cpu":"don't cool cpu", x, y, w, "ConfigAdvancedSet(0);", "selecting this option will allow your computer\nsome time to cool off during the game,\nwith expense of game speed.\ndisable it on slower computers.");  y+=h;
	if(!gs_inigetint(GAMEINI,"ADVANCED","log",&val)) val=1;
	button_main( val?"log messages":"don't log messages", x, y, w, "ConfigAdvancedSet(1);", "if you have problems running the game,\nselect this option to log debug messages and\nsend the resulted .log file to the author.");  y+=h;
	button_main( "edit config file", x, y, w, "gs_shell(`"+GAMEINI+"`);PageConfig();", "edit the config file manually");  y+=h;
	y+=h*3;
	button_main( "back", 		x, y, w, "PageConfig();", "back to config page" ); y+=h;

	DlgRepaint();
}

func ConfigAdvancedSet( cmd )
{
	if(cmd==0)	CfgUpdate("ADVANCED","cool",1,0,1);
	if(cmd==1)	CfgUpdate("ADVANCED","log",1,0,1);
	PageConfigAdvanced(); // recreate page
}

// -----------------------------------------------------------------------
// Page install
// -----------------------------------------------------------------------
int g_installshortcut;
int g_installgroup;
str g_path;

func PageInstallPre()
{
	g_installshortcut = 1;
	g_installgroup = 1;
	PageInstall();
}

func PageInstall()
{
	DlgClear();
	icon = g_readonly?1:0;
	background_img(icon);
	
	h=22; w=170;
	x=(WNDW-w)/2; y=100;

	button_main( "desktop shortcut", x, y, w, "ConfigInstallSet(0);", "create desktop shortcut");
	ItemSet(IITXTALIGN, IIALIGN_LEFT);
	textbox(g_installshortcut?"yes":"no",x,y,w,20,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_RIGHT);
	y+=h;
	button_main( "program group", x, y, w, "ConfigInstallSet(1);", "create program group in the START menu");
	ItemSet(IITXTALIGN, IIALIGN_LEFT);
	textbox(g_installgroup?"yes":"no",x,y,w,20,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_RIGHT);
	
	y+=h*4;
	button_main( "proceed", x, y-5, w, "Install();", "create the selected components"); y+=h;
	if(!g_installshortcut && !g_installgroup) button_disable();
	
	button_main( "back", 		x, y, w, "PageMain();", "back to main page" ); 

	DlgRepaint();
}

func ConfigInstallSet( cmd )
{
	if(cmd==0) g_installshortcut=!g_installshortcut;
	if(cmd==1) g_installgroup=!g_installgroup;
	PageInstall(); // recreate page
}

func InstallShortcut()
{
	return AppAddLink(0,"",g_game_shortcut+".lnk",g_game_title,g_path+"\\setup.exe",g_path,"");
}

func InstallGroup()
{
	ok = true;
		
	ok |= AppAddLink(1,"DizzyAGE",	"DizzyAGE website.lnk",		"Visit the DizzyAGE official website",	"http://www.yolkfolk.com/dizzyage/", "", "" );
	ok |= AppAddLink(1,"DizzyAGE",	"Yolkfolk website.lnk",		"Visit the Yolkfolk Community website",	"http://www.yolkfolk.com", "", "" );
	
	ok |= AppAddLink(1,"DizzyAGE\\"+g_game_shortcut,	g_game_shortcut+".lnk",	"Play "+g_game_title,			g_path+"\\"+GAMEEXE,		g_path, "");
	ok |= AppAddLink(1,"DizzyAGE\\"+g_game_shortcut,	"Setup.lnk",			"Set game options",				g_path+"\\setup.exe",		g_path, "");
	ok |= AppAddLink(1,"DizzyAGE\\"+g_game_shortcut,	"Readme.lnk",			"Read game info file",			g_path+"\\"+g_game_readme,	g_path, "");
	if(g_game_website!="")
	ok |= AppAddLink(1,"DizzyAGE\\"+g_game_shortcut,	"Website.lnk",			"Visit the game's website",		g_game_website,				g_path, "");
	ok |= AppAddLink(1,"DizzyAGE\\"+g_game_shortcut,	"Uninstall.lnk",		"Uninstall game",				g_path+"\\setup.exe",		g_path, "uninstall");

	return ok;
}

func Install()
{
	// shortcut
	if( g_installshortcut )
		if(!InstallShortcut())
		{ 
			PageMessage("failed to create\ndesktop shortcut", "InstallDone();",3);
			return;
		}
	// group
	if( g_installgroup )
		if(!InstallGroup())
		{ 
			PageMessage("failed to create\nprogram group", "InstallDone();",3);
			return;
		}
		
	// success
	PageMessage("install successful!", "InstallDone();", 2);
}

func InstallDone()
{
	PageMain();
}

// -----------------------------------------------------------------------
// Page uninstall
// -----------------------------------------------------------------------
tab g_uninstalloptions;
func PageUninstallPre( options )
{
	if(!?options) options = {1,1,0,0};
	g_uninstalloptions = options;
	PageUninstall();
}

func PageUninstall()
{
	DlgClear();
	background_img();
	
	h=22; w=170;
	x=(WNDW-w)/2; y=100;

	button_main( "delete shortcut", x, y, w, "ConfigUninstallSet(0);", "delete desktop shortcut");
	ItemSet(IITXTALIGN, IIALIGN_LEFT);	
	textbox(g_uninstalloptions[0]?"yes":"no",x,y,w,20,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_RIGHT);
	y+=h;

	button_main( "delete group", x, y, w, "ConfigUninstallSet(1);", "delete program group from the START menu");
	ItemSet(IITXTALIGN, IIALIGN_LEFT);	
	textbox(g_uninstalloptions[1]?"yes":"no",x,y,w,20,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_RIGHT);
	y+=h;

	button_main( "delete game files", x, y, w, "ConfigUninstallSet(2);", "delete game files from the hard disk");
	ItemSet(IITXTALIGN, IIALIGN_LEFT);	
	textbox(g_uninstalloptions[2]?"yes":"no",x,y,w,20,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_RIGHT);
	y+=h;

	button_main( "delete saved games", x, y, w, "ConfigUninstallSet(3);", "delete saved games from the hard disk");
	ItemSet(IITXTALIGN, IIALIGN_LEFT);	
	textbox(g_uninstalloptions[3]?"yes":"no",x,y,w,20,g_resfnt1);
	ItemSet(IITXTALIGN, IIALIGN_RIGHT);

	y+=h*2;
	button_main( "proceed", x, y-5, w, "Uninstall();", "uninstall the selected components"); y+=h;
	if(!(g_uninstalloptions[0] || g_uninstalloptions[1] || g_uninstalloptions[2] || g_uninstalloptions[3] )) button_disable();

	button_main( "back", 		x, y, w, "PageMain();", "back to main page" ); 

	DlgRepaint();
}

func ConfigUninstallSet( cmd )
{
	g_uninstalloptions[cmd] = !g_uninstalloptions[cmd];
	PageUninstall(); // recreate page
}

func Uninstall()
{
	cnt=0;
	if(g_uninstalloptions[0]) // shortcut
	{
		cnt += AppDel(1, g_game_shortcut+".lnk", 0);
	}
	if(g_uninstalloptions[1]) // group
	{
		cnt += AppDel(2, "DizzyAGE\\"+g_game_shortcut+"\\"+g_game_shortcut+".lnk", 0);
		cnt += AppDel(2, "DizzyAGE\\"+g_game_shortcut+"\\Setup.lnk", 0);
		cnt += AppDel(2, "DizzyAGE\\"+g_game_shortcut+"\\Readme.lnk", 0);
		cnt += AppDel(2, "DizzyAGE\\"+g_game_shortcut+"\\Website.lnk", 0);
		cnt += AppDel(2, "DizzyAGE\\"+g_game_shortcut+"\\Uninstall.lnk", 0);
		cnt += AppDel(2, "DizzyAGE\\"+g_game_shortcut, 1);
	}
	if(g_uninstalloptions[2]) // game files
	{
		cnt += AppDel(0, GAMEEXE, 0);
		cnt += AppDel(0, GAMEPAK, 0);
		cnt += AppDel(0, GAMEINI, 0);
		cnt += AppDel(0, g_game_readme, 0);

		cnt += AppDel(0, GAMELOG, 0);
		cnt += AppDel(0, "setup.log", 0); // delete log last in case of messages like unable to delete
	}
	if(g_uninstalloptions[3]) // saved games
	{
		cnt += AppDel(0, "save1.gam", 0);
		cnt += AppDel(0, "save2.gam", 0);
		cnt += AppDel(0, "save3.gam", 0);
		cnt += AppDel(0, "save4.gam", 0);
		cnt += AppDel(0, "save5.gam", 0);
		cnt += AppDel(0, "save6.gam", 0);
		cnt += AppDel(0, "save7.gam", 0);
		cnt += AppDel(0, "save8.gam", 0);
	}
	
	if(g_uninstalloptions[2])	returncmd = "AppDelExe();DlgClose();";
	else						returncmd = "PageMain();";
	PageMessage("uninstall completed!",returncmd,2);
}

// -----------------------------------------------------------------------
// Page about
// -----------------------------------------------------------------------
func PageAbout()
{
	DlgClear();
	background_img();

	h=22; w=140;
	x=(WNDW-w)/2; y=100;
	button_main( "Read me",	x, y, w, "gs_shell(g_game_readme);", "open game's info file"); 
	if(!FileExists(g_game_readme)) button_disable();
	y+=h;
	if(g_game_website!="")
		button_main( "Game website",		x, y, w, "gs_shell(`"+g_game_website+"`);", "visit "+g_game_website );
	else
		button_main( "DizzyAGE website",	x, y, w, "gs_shell(`http://www.yolkfolk.com/dizzyage`);", "visit http://www.yolkfolk.com/dizzyage" );
	y+=h;
	y+=h*4;
	button_main( "back", 		x, y, w, "PageMain();", "back to main page" ); y+=h;

	w=200; h=60;
	x=(WNDW-w)/2; y=150;
	textbox(g_game_about, x, y, w, h, g_resfnt3 );
	ItemSet(IITXTALIGN, IIALIGN_CENTERX);
	
	DlgRepaint();
}

// -----------------------------------------------------------------------
// Make Default Config
// -----------------------------------------------------------------------
func MakeConfigInt( group, list )
{
	val=1;
	for(i=0;i<sizeof(*list);i++)
	{
		if(!gs_inigetint(GAMEINI,group,(*list)[i][0],&val)) val=(*list)[i][1];
		gs_inisetint(GAMEINI,group,(*list)[i][0],val);
	}
}

func MakeConfigStr( group, list )
{
	val="";
	for(i=0;i<sizeof(*list);i++)
	{
		if(!gs_inigetstr(GAMEINI,group,(*list)[i][0],&val)) val=(*list)[i][1];
		gs_inisetstr(GAMEINI,group,(*list)[i][0],val);
	}
}

func MakeConfigDefault()
{
	cfgvideo = {{"api",			0},
				{"windowed",	1},
				{"width",		640},
				{"height",		480},
				{"scale",		2},
				{"bpp",			32},
				{"refresh",		0},
				{"vsync",		0}};
	
	cfgaudio = {{"enabled",		1},
				{"volfx",		100},
				{"volmusic",	100}};
				
	cfginput = {{"enabled",		1},
				{"keyboard",	1},
				{"joystick",	1},
				{"joystick_dx",	500},
				{"joystick_dy",	500},
				{"rumble",		1},
				{"key1_left",	203},
				{"key1_right",	205},
				{"key1_up",		200},
				{"key1_down",	208},
				{"key1_jump",	57},
				{"key1_action",	28},
				{"key1_menu",	1},
				{"key2_left",	44},
				{"key2_right",	45},
				{"key2_up",		37},
				{"key2_down",	50},
				{"key2_jump",	57},
				{"key2_action",	28},
				{"key2_menu",	16}};
				
	cfglanguage={{"language",	0}};

	cfgengine ={{"cool",		1},
				{"log",			0}};
	
	// video
	MakeConfigInt("VIDEO",&cfgvideo);
	MakeConfigInt("AUDIO",&cfgaudio);
	MakeConfigInt("INPUT",&cfginput);
	MakeConfigInt("LOCALIZATION",&cfglanguage);
	MakeConfigInt("ADVANCED",&cfgengine);
}

// -----------------------------------------------------------------------
// Read game info from data/dizzy.inf (pak supported)
// -----------------------------------------------------------------------
func ReadGameInfo()
{
	g_game_title	= AppGetGameInfo("game_title");
	g_game_version	= AppGetGameInfo("game_version");
	g_game_author	= AppGetGameInfo("game_author");
	g_game_website	= AppGetGameInfo("game_website");
	g_game_readme	= AppGetGameInfo("game_readme");
	g_game_about	= AppGetGameInfo("game_about");
	g_game_shortcut	= AppGetGameInfo("game_shortcut");
	game_languages	= AppGetGameInfo("game_languages");

	g_game_languages = strexplode(game_languages,',');
	for(i=0;i<sizeof(g_game_languages);i++)
		g_game_languages[i] = strtrim(g_game_languages[i]," \t");
	
	if(g_game_title=="")		g_game_title	= "Dizzy Game";
	if(g_game_shortcut=="")		g_game_shortcut	= "Dizzy";
	if(g_game_about=="")		g_game_about	= "DizzyAGE created by\nAlexandru and Cristina Simion\nhttp://www.yolkfolk.com/dizzyage";
	
	// audtodetect readme file
	if(g_game_readme=="")
	{
		g_game_readme = "readme.txt";
		if(FileExists("readme.html"))	g_game_readme = "readme.html";
		if(FileExists("readme.doc"))	g_game_readme = "readme.doc";
		if(FileExists("readme.pdf"))	g_game_readme = "readme.pdf";
	}
}

// -----------------------------------------------------------------------
// Run external installer and uninstaller
// -----------------------------------------------------------------------

func DetectExternalInstallersCallback(filepath,dir)
{
	if(dir) return;
	if(strpos(filepath,".exe")==-1) return;
	if(g_installer=="")
	{
		// must start with "install"
		if(strpos(filepath,"install")==0)
			g_installer = filepath;
	}
	if(g_uninstaller=="")
	{
		// must start with "unins"
		if(strpos(filepath,"unins")==0)
			g_uninstaller = filepath;
	}
}

func DetectExternalInstallers()
{
	g_installer="";
	g_uninstaller="";
	gs_filefind("","DetectExternalInstallersCallback",false);
}

func RunInstaller()
{
	if(!gs_launch(g_installer))
	{
		PageMessage("failed to launch\ngame uninstaller", "PageMain();",3);
		return;
	}
	DlgClose();
	return;
}

func RunUninstaller()
{
	if(!gs_launch(g_uninstaller))
	{
		PageMessage("failed to launch\ngame uninstaller", "PageMain();",3);
		return;
	}
	DlgClose();
	return;
}

// -----------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------
func main( param )
{
	// globals
	g_param = param;
	g_path = gs_directoryget()+"\\";

	// test readonly
	g_readonly = true;
	f = gs_fileopen("dummy",1);
	if(!!f)
	{
		g_readonly=false;
		gs_fileclose(f);
		AppDel(0,"dummy",0);
	}

	// @DEBUG
	// g_readonly = 1;
	
	// open pak to read from dizzy.inf
	AppOpenPak(GAMEPAK);
	
	// load
	LoadResources();
	MakeConfigDefault();
	ReadGameInfo();

	// open dialog
	g_dlg = DlgOpen("DizzyAGE Setup", WS_POPUP|WS_CLIPCHILDREN, -1,-1,WNDW,WNDH );
	DlgSetRgn( g_resbak, 0xff00ff );
	DlgVisible(1);
	
	// detect external installer or uninstaller
	DetectExternalInstallers();
	if(g_installer!="" || g_uninstaller!="")
		g_install_mode = 1;
	
	if(g_install_mode==0 && g_param=="uninstall")
		PageUninstallPre( {1,1,0,0} );	// uninstall page
	else
		PageMain();	// normal
}
