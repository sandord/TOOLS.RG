/* ###################################################################################
#  INCLUDES
################################################################################### */

#include	"EDITMENU.H"

#include	"TILEMAP.H"

#include	<GODLIB\DEBUGLOG\DEBUGLOG.H>
#include	<GODLIB\FILE\FILE.H>
#include	<GODLIB\GUI\GUI.H>
#include	<GODLIB\GUI\GUIDATA.H>
#include	<GODLIB\GUI\GUIEDIT.H>
#include	<GODLIB\MEMORY\MEMORY.H>
#include	<GODLIB\STRING\STRING.H>


/* ###################################################################################
#  DEFINES
################################################################################### */

#define	dEM_BLOCKSX	32
#define	dEM_BLOCKSY	32

#define	dEM_MAPROW_COUNT	12
#define	dEM_MAPCOL_COUNT	20

/* ###################################################################################
#  ENUMS
################################################################################### */

enum
{
	eEM_VAR_DEL,
	eEM_VAR_INS,
	eEM_VAR_FILENAME,
	eEM_VAR_MAPHEIGHT,
	eEM_VAR_MAPWIDTH,
	eEM_VAR_MAPX,
	eEM_VAR_MAPY,
	eEM_VAR_MAPXMAX,
	eEM_VAR_MAPYMAX,

	eEM_VAR_LIMIT
};

enum
{
	eEM_VARCLIENT_DEL,
	eEM_VARCLIENT_INS,
	eEM_VARCLIENT_MAPX,
	eEM_VARCLIENT_MAPY,

	eEM_VARCLIENT_LIMIT
};

enum
{
	eEM_DIR_NONE,

	eEM_DIR_DOWN,
	eEM_DIR_LEFT,
	eEM_DIR_RIGHT,
	eEM_DIR_UP,

	eEM_DIR_LIMIT
};

/* ###################################################################################
#  STRUCTS
################################################################################### */

typedef	struct	sEditMenuClass
{
	sHashTree *			mpTree;
	sTileMap			mTileMap;
	sString 			mMapFileName;
	sHashTreeVar		mVars[ eEM_VAR_LIMIT ];
	sHashTreeVarClient	mButtMainClient;
	sHashTreeVarClient	mVarClients[ eEM_VARCLIENT_LIMIT ];
	U16					mDel;
	U16					mIns;
	U32					mMapX;
	U32					mMapY;
	U32					mMapXmax;
	U32					mMapYmax;
} sEditMenuClass;


/* ###################################################################################
#  DATA
################################################################################### */

sEditMenuClass	gEditMenuClass;


/* ###################################################################################
#  PROTOTYPES
################################################################################### */

void	EditMenu_MapRePaint( void );

void	EditMenu_Del_OnWrite( sHashTreeVarClient  * apClient );
void	EditMenu_Ins_OnWrite( sHashTreeVarClient  * apClient );

void	EditMenu_MapX_OnWrite( sHashTreeVarClient  * apClient );
void	EditMenu_MapY_OnWrite( sHashTreeVarClient  * apClient );


/* ###################################################################################
#  CODE
################################################################################### */

/*-----------------------------------------------------------------------------------*
* FUNCTION : EditMenu_Init( sHashTree * apTree )
* ACTION   : EditMenu_Init
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	EditMenu_Init( sHashTree * apTree )
{
	Memory_Clear( sizeof(sEditMenuClass), &gEditMenuClass );

	gEditMenuClass.mpTree = apTree;
	TileMap_Init( &gEditMenuClass.mTileMap, dEM_BLOCKSX, dEM_BLOCKSY );

	gEditMenuClass.mMapXmax = gEditMenuClass.mTileMap.mBlocksX - 20;
	gEditMenuClass.mMapYmax = gEditMenuClass.mTileMap.mBlocksX - 12;

	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_FILENAME  ], apTree, "TILEEDIT\\EDITMENU\\FILENAME",	sizeof(sString*),	&gEditMenuClass.mMapFileName );
	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_MAPWIDTH  ], apTree, "TILEEDIT\\EDITMENU\\MAPWIDTH",	sizeof(U16),		&gEditMenuClass.mTileMap.mBlocksX );
	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_MAPHEIGHT ], apTree, "TILEEDIT\\EDITMENU\\MAPHEIGHT",	sizeof(U16),		&gEditMenuClass.mTileMap.mBlocksY );
	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_MAPX      ], apTree, "TILEEDIT\\EDITMENU\\MAPX",		sizeof(U32),		&gEditMenuClass.mMapX );
	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_MAPY      ], apTree, "TILEEDIT\\EDITMENU\\MAPY",		sizeof(U32),		&gEditMenuClass.mMapY );
	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_MAPXMAX   ], apTree, "TILEEDIT\\EDITMENU\\MAPXMAX",	sizeof(U32),		&gEditMenuClass.mMapXmax );
	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_MAPYMAX   ], apTree, "TILEEDIT\\EDITMENU\\MAPYMAX",	sizeof(U32),		&gEditMenuClass.mMapYmax );

	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_DEL  ], apTree, "TILEEDIT\\MAPMENU\\DEL",	sizeof(U16),	&gEditMenuClass.mDel );
	HashTree_Var_Init( &gEditMenuClass.mVars[ eEM_VAR_INS  ], apTree, "TILEEDIT\\MAPMENU\\INS",	sizeof(U16),	&gEditMenuClass.mIns );

	HashTree_VarClient_Init( &gEditMenuClass.mButtMainClient, apTree, "GUI\\BUTTONS\\BUTT_EDIT_MAIN", 0 );

	HashTree_VarClient_Init( &gEditMenuClass.mVarClients[ eEM_VARCLIENT_DEL      ], apTree, "TILEEDIT\\MAPMENU\\DEL",			EditMenu_Del_OnWrite );
	HashTree_VarClient_Init( &gEditMenuClass.mVarClients[ eEM_VARCLIENT_INS      ], apTree, "TILEEDIT\\MAPMENU\\INS",			EditMenu_Ins_OnWrite );
	HashTree_VarClient_Init( &gEditMenuClass.mVarClients[ eEM_VARCLIENT_MAPX     ], apTree, "TILEEDIT\\EDITMENU\\MAPX",			EditMenu_MapX_OnWrite );
	HashTree_VarClient_Init( &gEditMenuClass.mVarClients[ eEM_VARCLIENT_MAPX     ], apTree, "TILEEDIT\\EDITMENU\\MAPY",			EditMenu_MapY_OnWrite );
	String_Init( &gEditMenuClass.mMapFileName, "TEST.MAP" );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : EditMenu_DeInit( void )
* ACTION   : EditMenu_DeInit
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	EditMenu_DeInit( void )
{
	U16	i;

	String_DeInit( &gEditMenuClass.mMapFileName );

	HashTree_VarClient_DeInit( &gEditMenuClass.mButtMainClient, gEditMenuClass.mpTree );

	for( i=0; i<eEM_VARCLIENT_LIMIT; i++ )
	{
		HashTree_VarClient_DeInit( &gEditMenuClass.mVarClients[ i ], gEditMenuClass.mpTree );
	}

	for( i=0; i<eEM_VAR_LIMIT; i++ )
	{
		HashTree_Var_DeInit( &gEditMenuClass.mVars[ i ], gEditMenuClass.mpTree );
	}
	TileMap_DeInit( &gEditMenuClass.mTileMap );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : EditMenu_MapLoad( char * apFileName )
* ACTION   : EditMenu_MapLoad
* CREATION : 03.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	EditMenu_MapLoad( char * apFileName )
{
	sTileMap *	lpTileFile;
	U32			lSize;

	if( apFileName )	
	{
		String_Set( &gEditMenuClass.mMapFileName, apFileName );

		lpTileFile = File_Load( apFileName );
		if( lpTileFile )
		{
			if( TileMap_Validate( lpTileFile ) )
			{
				TileMap_Relocate( lpTileFile );

				gEditMenuClass.mTileMap = *lpTileFile;
				lSize  = lpTileFile->mBlocksY;
				lSize *= lpTileFile->mBlocksY;
				lSize *= 2;
				if( lSize )
				{
					if( gEditMenuClass.mTileMap.mpBlocks )
					{
						mMEMFREE( gEditMenuClass.mTileMap.mpBlocks );
						gEditMenuClass.mTileMap.mpBlocks = 0;
					}

					gEditMenuClass.mTileMap.mpBlocks = mMEMCALLOC( lSize );
					if( gEditMenuClass.mTileMap.mpBlocks )
					{
						Memory_Copy( lSize, lpTileFile->mpBlocks, gEditMenuClass.mTileMap.mpBlocks );
					}
				}
				else
				{
					gEditMenuClass.mTileMap.mpBlocks = 0;
				}

			}
			File_UnLoad( lpTileFile );
		}
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void EditMenu_MapSave(char * apFileName)
* ACTION   : EditMenu_MapSave
* CREATION : 03.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void EditMenu_MapSave(char * apFileName)
{
	sTileMap *	lpMap;
	U32			lSize;

	String_Set( &gEditMenuClass.mMapFileName, apFileName );

	lpMap = TileMap_Serialise( &gEditMenuClass.mTileMap );
	if( lpMap )
	{
		lSize = TileMap_GetSize( lpMap );
		TileMap_Delocate( lpMap );
		File_Save( apFileName, lpMap, lSize );
		mMEMFREE( lpMap );
	}
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : EditMenu_GetpTileMap( void )
* ACTION   : EditMenu_GetpTileMap
* CREATION : 03.03.2004 PNK
*-----------------------------------------------------------------------------------*/

sTileMap *	EditMenu_GetpTileMap( void )
{
	return( &gEditMenuClass.mTileMap );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void EditMenu_MapX_OnWrite(sHashTreeVarClient  * apClient)
* ACTION   : EditMenu_MapX_OnWrite
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void EditMenu_MapX_OnWrite(sHashTreeVarClient  * apClient)
{
	HashTree_VarRead( apClient->mpVar, &gEditMenuClass.mMapX, sizeof(gEditMenuClass.mMapX) );
	EditMenu_MapRePaint();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void EditMenu_MapY_OnWrite(sHashTreeVarClient  * apClient)
* ACTION   : EditMenu_MapY_OnWrite
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void EditMenu_MapY_OnWrite(sHashTreeVarClient  * apClient)
{
	HashTree_VarRead( apClient->mpVar, &gEditMenuClass.mMapY, sizeof(gEditMenuClass.mMapY) );
	EditMenu_MapRePaint();
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : void EditMenu_MapRePaint(void)
* ACTION   : EditMenu_MapRePaint
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void EditMenu_MapRePaint(void)
{
	sGuiEvent *		lpEvent;
	sGuiButton *	lpButton;

	if( gEditMenuClass.mButtMainClient.mpVar )
	{
		lpEvent = (sGuiEvent*)gEditMenuClass.mButtMainClient.mpVar->mpData;
		if( lpEvent )
		{			
			lpButton = (sGuiButton*)lpEvent->mpInfo;
			if( lpButton )
			{
				lpButton->mInfo.mRedrawFlag = 2;
			}
		}
	}		
}



/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 EditMenu_GetMapX(void)
* ACTION   : EditMenu_GetMapX
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U32 EditMenu_GetMapX(void)
{
	return( gEditMenuClass.mMapX );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : U32 EditMenu_GetMapY(void)
* ACTION   : EditMenu_GetMapY
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

U32 EditMenu_GetMapY(void)
{
	return( gEditMenuClass.mMapY );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : const char * EditMenu_GetpMapFileName(void)
* ACTION   : EditMenu_GetpMapFileName
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

const char * EditMenu_GetpMapFileName(void)
{
	return( gEditMenuClass.mMapFileName.mpChars );	
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : EditMenu_Del_OnWrite( sHashTreeVarClient  * apClient )
* ACTION   : EditMenu_Del_OnWrite
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	EditMenu_Del_OnWrite( sHashTreeVarClient  * apClient )
{

	HashTree_VarRead( apClient->mpVar, &gEditMenuClass.mDel, sizeof(U16) );	

	DebugLog_Printf1( "EditMenu_Del_OnWrite() %d", gEditMenuClass.mDel );

	switch( gEditMenuClass.mDel )
	{
	case	eEM_DIR_DOWN:
		TileMap_RowDelete( &gEditMenuClass.mTileMap, (U16)(gEditMenuClass.mMapY + (dEM_MAPROW_COUNT-1)) );
		break;
	case	eEM_DIR_LEFT:
		TileMap_ColDelete( &gEditMenuClass.mTileMap, (U16)gEditMenuClass.mMapX );
		break;
	case	eEM_DIR_RIGHT:
		TileMap_ColDelete( &gEditMenuClass.mTileMap, (U16)(gEditMenuClass.mMapX + (dEM_MAPCOL_COUNT-1)) );
		break;
	case	eEM_DIR_UP:
		TileMap_RowDelete( &gEditMenuClass.mTileMap, (U16)(gEditMenuClass.mMapY) );
		break;
	default:
		break;
	}

	HashTree_VarWrite( &gEditMenuClass.mVars[ eEM_VAR_MAPWIDTH  ], &gEditMenuClass.mTileMap.mBlocksX );
	HashTree_VarWrite( &gEditMenuClass.mVars[ eEM_VAR_MAPHEIGHT ], &gEditMenuClass.mTileMap.mBlocksY );
}


/*-----------------------------------------------------------------------------------*
* FUNCTION : EditMenu_Ins_OnWrite( sHashTreeVarClient  * apClient )
* ACTION   : EditMenu_Ins_OnWrite
* CREATION : 04.03.2004 PNK
*-----------------------------------------------------------------------------------*/

void	EditMenu_Ins_OnWrite( sHashTreeVarClient  * apClient )
{

	HashTree_VarRead( apClient->mpVar, &gEditMenuClass.mIns, sizeof(U16) );		

	DebugLog_Printf1( "EditMenu_Ins_OnWrite() %d", gEditMenuClass.mIns );

	switch( gEditMenuClass.mIns )
	{
	case	eEM_DIR_DOWN:
		TileMap_RowInsert( &gEditMenuClass.mTileMap, (U16)(gEditMenuClass.mMapY + (dEM_MAPROW_COUNT-1)) );
		break;
	case	eEM_DIR_LEFT:
		TileMap_ColInsert( &gEditMenuClass.mTileMap, (U16)(gEditMenuClass.mMapX) );
		break;
	case	eEM_DIR_RIGHT:
		TileMap_ColInsert( &gEditMenuClass.mTileMap, (U16)(gEditMenuClass.mMapX + (dEM_MAPCOL_COUNT-1)) );
		break;
	case	eEM_DIR_UP:
		TileMap_RowInsert( &gEditMenuClass.mTileMap, (U16)(gEditMenuClass.mMapY) );
		break;
	default:
		break;
	}

	HashTree_VarWrite( &gEditMenuClass.mVars[ eEM_VAR_MAPWIDTH  ], &gEditMenuClass.mTileMap.mBlocksX );
	HashTree_VarWrite( &gEditMenuClass.mVars[ eEM_VAR_MAPHEIGHT ], &gEditMenuClass.mTileMap.mBlocksY );
}





/* ################################################################################ */
