#include "SpriteStudio5PrivatePCH.h"
#include "SsAnimePack.h"


void FSsPartAnime::Serialize(FArchive& Ar)
{
	for(int32 i = 0; i < Attributes.Num(); ++i)
	{
		Attributes[i].Serialize(Ar);
	}
}

void FSsAnimation::Serialize(FArchive& Ar)
{
	for(int32 i = 0; i < PartAnimes.Num(); ++i)
	{
		PartAnimes[i].Serialize(Ar);
	}
}

void FSsAnimePack::Serialize(FArchive& Ar)
{
	if(Ar.IsLoading() || Ar.IsSaving())
	{
		for(int32 i = 0; i < AnimeList.Num(); ++i)
		{
			AnimeList[i].Serialize(Ar);
		}
	}
}

int32 FSsAnimePack::FindAnimationIndex(const FName& Name)
{
	for(int32 i = 0; i < AnimeList.Num(); ++i)
	{
		if(Name == AnimeList[i].AnimationName)
		{
			return i;
		}
	}
	return -1;
}
