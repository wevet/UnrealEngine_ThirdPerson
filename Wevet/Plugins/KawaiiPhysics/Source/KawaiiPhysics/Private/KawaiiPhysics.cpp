#include "KawaiiPhysics.h"

DEFINE_LOG_CATEGORY(LogKawaiiPhysics)

class FKawaiiPhysicsPlugin : public IKawaiiPhysicsPlugin
{
public:
	virtual void StartupModule() override
	{
		UE_LOG(LogKawaiiPhysics, Log, TEXT("KawaiiPhysics Plugin : StartupModule"));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogKawaiiPhysics, Log, TEXT("KawaiiPhysics Plugin : StartupModule"));
	}
};

IMPLEMENT_MODULE(FKawaiiPhysicsPlugin, KawaiiPhysics)

