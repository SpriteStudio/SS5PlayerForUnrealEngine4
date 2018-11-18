#include "SpriteStudio5PrivatePCH.h"
#include "SpriteStudio5.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/AssertionMacros.h"

DEFINE_LOG_CATEGORY(LogSpriteStudio);


FCustomVersionRegistration GRegisterSspjCustomVersion(
	SSPJ_GUID,
	SSPJ_VERSION,
	TEXT("SsProjectVersion")
	);


class FSpriteStudio5 : public ISpriteStudio5
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FSpriteStudio5, SpriteStudio5)



void FSpriteStudio5::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("SpriteStudio5"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/SpriteStudio5"), PluginShaderDir);
}


void FSpriteStudio5::ShutdownModule()
{
}



