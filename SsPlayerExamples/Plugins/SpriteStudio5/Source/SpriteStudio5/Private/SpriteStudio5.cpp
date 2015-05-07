#include "SpriteStudio5PrivatePCH.h"
#include "SpriteStudio5.h"

#include "Misc/AssertionMacros.h"

DEFINE_LOG_CATEGORY(LogSpriteStudio);


class FSpriteStudio5 : public ISpriteStudio5
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FSpriteStudio5, SpriteStudio5)



void FSpriteStudio5::StartupModule()
{
}


void FSpriteStudio5::ShutdownModule()
{
}



