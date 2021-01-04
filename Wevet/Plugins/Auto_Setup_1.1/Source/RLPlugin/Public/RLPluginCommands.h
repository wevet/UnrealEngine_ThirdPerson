// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "RLPluginStyle.h"

class FRLPluginCommands : public TCommands<FRLPluginCommands>
{
public:

    FRLPluginCommands()
        : TCommands<FRLPluginCommands>( TEXT( "RLPlugin" ),
                                        NSLOCTEXT( "Contexts", "RLPlugin", "RLPlugin Plugin" ),
                                        NAME_None,
                                        FRLPluginStyle::GetStyleSetName() )
    {
    }

    // TCommands<> interface
    virtual void RegisterCommands() override;

public:
    TSharedPtr< FUICommandInfo > PluginAction;
};
