#include "RTIK.h"

DEFINE_LOG_CATEGORY(LogRTIK)

class FRTIKPlugin : public IRTIKPlugin
{
	virtual void StartupModule() override
	{
		UE_LOG(LogRTIK, Log, TEXT("FRTIKPlugin : StartupModule"));
	}

	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogRTIK, Log, TEXT("FRTIKPlugin : ShutdownModule"));
	}
};

IMPLEMENT_MODULE(FRTIKPlugin, RTIK)
