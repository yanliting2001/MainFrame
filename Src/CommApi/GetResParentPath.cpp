#include "GetResParentPath.h"

char *GetDataBasePath(char *cDBPath, int iSize)
{
	if ( !cDBPath || !theBaseApp )
		return NULL;

	SAFE_STRNCPY(cDBPath, theBaseApp->GetDataBaseFolder(), iSize);

	return cDBPath;
}

char *GetMemBasePath(char *cMemPath, int iSize)
{
	if ( !cMemPath || !theBaseApp )
		return NULL;

	SAFE_STRNCPY(cMemPath, theBaseApp->GetResFolder(), iSize);

	return cMemPath;
}

char *GetXmlBasePath(char *cXmlPah, int iSize)
{
	if ( !cXmlPah || !theBaseApp )
		return NULL;

	SAFE_STRNCPY(cXmlPah, theBaseApp->GetXmlFolder(), iSize);

	return cXmlPah;
}

char *GetSongTextureBasePath(char *cSongTexturePath, int iSize)
{
	if ( !cSongTexturePath || !theBaseApp )
		return NULL;

	SAFE_STRNCPY(cSongTexturePath, theBaseApp->GetSongTextureFolder(), iSize);

	return cSongTexturePath;
}

char *GetSingerTextureBasePath(char *cSingerTexturePath, int iSize)
{
	if ( !cSingerTexturePath || !theBaseApp )
		return NULL;

	SAFE_STRNCPY(cSingerTexturePath, theBaseApp->GetSingerTextureFolder(), iSize);

	return cSingerTexturePath;
}
