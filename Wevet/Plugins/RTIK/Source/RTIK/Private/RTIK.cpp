#include "RTIK.h"

DEFINE_LOG_CATEGORY(LogNIK)

class FRTIKPlugin : public IRTIKPlugin
{
	virtual void StartupModule() override
	{
		UE_LOG(LogNIK, Log, TEXT("FullBodyIK Plugin : StartupModule"));
	}

	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogNIK, Log, TEXT("FullBodyIK Plugin : ShutdownModule"));
	}
};

IMPLEMENT_MODULE(FRTIKPlugin, RTIK)
