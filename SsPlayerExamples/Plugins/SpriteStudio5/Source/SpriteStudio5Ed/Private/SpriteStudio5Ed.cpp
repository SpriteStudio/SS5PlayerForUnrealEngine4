#include "SpriteStudio5EdPrivatePCH.h"

#include "MessageLogModule.h"
#include "ISettingsModule.h"

#include "AssetTypeActions_SsProject.h"
#include "SsImportSettings.h"

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


	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Editor", "Plugins", "SpriteStudio5",
			LOCTEXT("SsImportSettingsName", "SpriteStudio5"),
			LOCTEXT("SsImportSettingsDescription", "SpriteStudio Import Settings"),
			GetMutableDefault<USsImportSettings>()
			);
	}
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


	if(ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "SpriteStudio5");
	}
}

#undef LOCTEXT_NAMESPACE
