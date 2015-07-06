#include "SpriteStudio5EdPrivatePCH.h"

#include "MessageLogModule.h"
#include "AssetTypeActions_SsProject.h"

DEFINE_LOG_CATEGORY(LogSpriteStudioEd);
#define LOCTEXT_NAMESPACE ""

class FSpriteStudio5Ed : public ISpriteStudio5Ed
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FAssetTypeActions_SsProject> SspjAssetTypeActions;
};

IMPLEMENT_MODULE(FSpriteStudio5Ed, SpriteStudio5Ed)



void FSpriteStudio5Ed::StartupModule()
{
	SspjAssetTypeActions = MakeShareable(new FAssetTypeActions_SsProject);
	FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get().RegisterAssetTypeActions(SspjAssetTypeActions.ToSharedRef());

	Style = MakeShareable(new FSpriteStudio5EdStyle());

	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	MessageLogModule.RegisterLogListing("SSPJ Import Log", LOCTEXT("SspjImportLog", "SSPJ Import Log"));
}


void FSpriteStudio5Ed::ShutdownModule()
{
	if (SspjAssetTypeActions.IsValid())
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get().UnregisterAssetTypeActions(SspjAssetTypeActions.ToSharedRef());
		}
		SspjAssetTypeActions.Reset();
	}

	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	MessageLogModule.UnregisterLogListing("SSPJ Import Log");
}

#undef LOCTEXT_NAMESPACE
