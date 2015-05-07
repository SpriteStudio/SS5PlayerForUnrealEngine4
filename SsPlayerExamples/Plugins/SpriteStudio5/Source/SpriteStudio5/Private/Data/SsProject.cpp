#include "SpriteStudio5PrivatePCH.h"
#include "SsProject.h"

#include "SsAnimePack.h"
#include "SsCellMap.h"
#include "SsString_uty.h"


USsProject::USsProject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USsProject::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if(Ar.IsLoading() || Ar.IsSaving())
	{
		for(int32 i = 0; i < AnimeList.Num(); ++i)
		{
			AnimeList[i].Serialize(Ar);
		}
	}
}

int32 USsProject::FindAnimePackIndex(const FName& AnimePackName) const
{
	for(int32 i = 0; i < AnimeList.Num(); ++i)
	{
		if(AnimePackName == AnimeList[i].AnimePackName)
		{
			return i;
		}
	}
	return -1;
}

int32 USsProject::FindCellMapIndex(const FName& CellmapName) const
{
	for(int32 i = 0; i < CellmapList.Num(); ++i)
	{
		if(CellmapName == CellmapList[i].FileName)
		{
			return i;
		}
	}
	return -1;
}

bool USsProject::FindAnimationIndex(const FName& InAnimPackName, const FName& InAnimationName, int32& OutAnimPackIndex, int32& OutAnimationIndex) const
{
	OutAnimPackIndex = FindAnimePackIndex(InAnimPackName);
	if(OutAnimPackIndex < 0){ return false; }

	OutAnimationIndex = AnimeList[OutAnimPackIndex].FindAnimationIndex(InAnimationName);
	if(OutAnimationIndex < 0){ return false; }

	return true;
}

const FSsAnimation* USsProject::FindAnimation(int32 AnimPackIndex, int32 AnimationIndex) const
{
	if((0 <= AnimPackIndex) && (AnimPackIndex < AnimeList.Num()))
	{
		if((0 <= AnimationIndex) && (AnimationIndex < AnimeList[AnimPackIndex].AnimeList.Num()))
		{
			return &(AnimeList[AnimPackIndex].AnimeList[AnimationIndex]);
		}
	}
	return NULL;
}

FString USsProject::GetSsceBasepath() const
{
	return getFullPath(ProjFilepath, Settings.CellMapBaseDirectory);
}
FString USsProject::GetSsaeBasepath() const
{
	return getFullPath(ProjFilepath, Settings.AnimeBaseDirectory);
}
FString USsProject::GetImageBasepath() const
{
	return getFullPath(ProjFilepath, Settings.ImageBaseDirectory);
}


namespace
{
	uint32 CalcMaxRenderPartsNum_Recursive(const USsProject& Proj, const FSsAnimePack& AnimePack)
	{
		uint32 Result = AnimePack.Model.PartList.Num();
		for(int32 i = 0; i < AnimePack.Model.PartList.Num(); ++i)
		{
			int32 RefAnimePackIndex = Proj.FindAnimePackIndex(AnimePack.Model.PartList[i].RefAnimePack);
			if(0 < RefAnimePackIndex)
			{
				Result += CalcMaxRenderPartsNum_Recursive(Proj, Proj.AnimeList[RefAnimePackIndex]);
			}
		}
		return Result;
	}
}

uint32 USsProject::CalcMaxRenderPartsNum() const
{
	uint32 Result = 0;

	for(int32 i = 0; i < AnimeList.Num(); ++i)
	{
		for(int32 j = 0; j < AnimeList[i].AnimeList.Num(); ++j)
		{
			Result = FMath::Max(Result, CalcMaxRenderPartsNum_Recursive(*this, AnimeList[i]));
		}
	}
	return Result;
}
