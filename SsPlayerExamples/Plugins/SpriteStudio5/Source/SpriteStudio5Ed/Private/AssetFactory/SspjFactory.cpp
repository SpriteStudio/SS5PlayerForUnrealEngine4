#include "SpriteStudio5EdPrivatePCH.h"
#include "SspjFactory.h"

#include "AssetRegistryModule.h"

#include "ReimportSspjFactory.h"
#include "SsProject.h"
#include "SsCellMap.h"
#include "SsAnimePack.h"
#include "SsLoader.h"


namespace
{
	FString GetFilePath(const FString& CurPath, const FString& BaseDir, const FString& FileName)
	{
		if(FPaths::IsRelative(BaseDir))
		{
			return CurPath / BaseDir / FileName;
		}
		return BaseDir / FileName;
	}
}


USspjFactory::USspjFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Formats.Add(TEXT("sspj;SpriteStudio Project"));

	SupportedClass = USsProject::StaticClass();
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;
	bText = false;
}

bool USspjFactory::DoesSupportClass(UClass * Class)
{
	return (Class == USsProject::StaticClass());
}

UClass* USspjFactory::ResolveSupportedClass()
{
	return USsProject::StaticClass();
}

UObject* USspjFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* InBufferEnd, FFeedbackContext* Warn)
{
	bool bReimport = this->IsA(UReimportSspjFactory::StaticClass());
	TMap<FString, UTexture*>* ExistImages = NULL;
	if(bReimport)
	{
		ExistImages = &(Cast<UReimportSspjFactory>(this)->ExistImages);
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

	FString ProjectNameStr = InName.ToString();
	FName ProjectName = InName;

	UPackage* InParentPackage = Cast<UPackage>(InParent);
	if(InParentPackage && !bReimport)
	{
		FString ProjectPackageName;
		FString BasePackageName = FPackageName::GetLongPackagePath(InParent->GetOutermost()->GetName()) / ProjectNameStr;
		AssetToolsModule.Get().CreateUniqueAssetName(BasePackageName, TEXT(""), ProjectPackageName, ProjectNameStr);
		InParentPackage->Rename(*ProjectPackageName);
	}

	// インポート開始
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, ProjectName, Type);

	// sspj
	USsProject* NewProject = FSsLoader::LoadSsProject(InParent, ProjectName, Flags, Buffer, (InBufferEnd - Buffer) + 1);
	NewProject->SetFilepath( GetCurrentFilename() );
	if(NewProject)
	{
		if(NewProject->AssetImportData == nullptr)
		{
			NewProject->AssetImportData = NewObject<UAssetImportData>(NewProject);
		}
		NewProject->AssetImportData->SourceFilePath = FReimportManager::SanitizeImportFilename(CurrentFilename, NewProject);
		NewProject->AssetImportData->SourceFileTimestamp = IFileManager::Get().GetTimeStamp(*CurrentFilename).ToString();
		NewProject->AssetImportData->bDirty = false;

		FString CurPath = FPaths::GetPath(GetCurrentFilename());

		TArray<FString> ImagePaths;

		// ssce
		NewProject->CellmapList.Empty();
		NewProject->CellmapList.AddZeroed(NewProject->CellmapNames.Num());
		for(int i = 0; i < NewProject->CellmapNames.Num(); ++i)
		{
			FString FileName = GetFilePath(CurPath, NewProject->Settings.CellMapBaseDirectory, NewProject->CellmapNames[i].ToString());

			TArray<uint8> Data;
			if(FFileHelper::LoadFileToArray(Data, *FileName))
			{
				const uint8* BufferBegin = Data.GetData();
				const uint8* BufferEnd = BufferBegin + Data.Num() - 1;
				if(FSsLoader::LoadSsCellMap(&(NewProject->CellmapList[i]), BufferBegin, (BufferEnd - BufferBegin) + 1))
				{
					NewProject->CellmapList[i].FileName = NewProject->CellmapNames[i];
					if(0 < NewProject->CellmapList[i].ImagePath.Len())
					{
						ImagePaths.AddUnique(NewProject->CellmapList[i].ImagePath);
					}
				}
			}
		}

		// ssae
		NewProject->AnimeList.Empty();
		NewProject->AnimeList.AddZeroed(NewProject->AnimepackNames.Num());
		for(int i = 0; i < NewProject->AnimepackNames.Num(); ++i)
		{
			FString FileName = GetFilePath(CurPath, NewProject->Settings.AnimeBaseDirectory, NewProject->AnimepackNames[i].ToString());

			TArray<uint8> Data;
			if(FFileHelper::LoadFileToArray(Data, *FileName))
			{
				const uint8* BufferBegin = Data.GetData();
				const uint8* BufferEnd = BufferBegin + Data.Num() - 1;
				FSsLoader::LoadSsAnimePack(&(NewProject->AnimeList[i]), BufferBegin, (BufferEnd - BufferBegin) + 1);
			}
		}

		// texture
		for(int i = 0; i < ImagePaths.Num(); ++i)
		{
			FString FileName = GetFilePath(CurPath, NewProject->Settings.ImageBaseDirectory, ImagePaths[i]);

			UTexture* ImportedTexture = NULL;
			if(ExistImages && ExistImages->Contains(ImagePaths[i]))
			{
				ImportedTexture = ExistImages->FindChecked(ImagePaths[i]);
			}

			TArray<uint8> Data;
			if(FFileHelper::LoadFileToArray(Data, *FileName))
			{

				UTextureFactory* TextureFact = NewObject<UTextureFactory>();
				TextureFact->AddToRoot();

				FString TextureName = FPaths::GetBaseFilename(ImagePaths[i]);

				UPackage* TexturePackage = NULL;
				if(ImportedTexture)
				{
					TexturePackage = ImportedTexture->GetOutermost();
				}
				else
				{
					FString TexturePackageName;
					FString BasePackageName = FPackageName::GetLongPackagePath(InParent->GetOutermost()->GetName()) / TextureName;
					AssetToolsModule.Get().CreateUniqueAssetName(BasePackageName, TEXT(""), TexturePackageName, TextureName);
					TexturePackage = CreatePackage(NULL, *TexturePackageName);
				}

				const uint8* BufferBegin = Data.GetData();
				const uint8* BufferEnd = BufferBegin + Data.Num();
				UTexture2D* NewTexture = (UTexture2D*)TextureFact->FactoryCreateBinary(
					UTexture2D::StaticClass(),
					TexturePackage,
					FName(*TextureName),
					Flags,
					NULL,
					*FPaths::GetExtension(ImagePaths[i]),
					BufferBegin, BufferEnd,
					Warn
					);
				if(NewTexture)
				{
					if(!ImportedTexture)
					{
						NewTexture->MipGenSettings = TMGS_NoMipmaps;
						NewTexture->AddressX = TA_Clamp;
						NewTexture->AddressY = TA_Clamp;
						NewTexture->CompressionSettings = TextureCompressionSettings::TC_EditorIcon;
					}

					FAssetRegistryModule::AssetCreated(NewTexture);
					TexturePackage->SetDirtyFlag(true);

					TextureFact->RemoveFromRoot();

					ImportedTexture = NewTexture;
				}
			}

			if(ImportedTexture)
			{
				for(int ii = 0; ii < NewProject->CellmapList.Num(); ++ii)
				{
					if(NewProject->CellmapList[ii].ImagePath == ImagePaths[i])
					{
						NewProject->CellmapList[ii].Texture = ImportedTexture;
					}
				}
			}
		}
	}

	// インポート終了
	FEditorDelegates::OnAssetPostImport.Broadcast(this, NewProject);
	return NewProject;
}