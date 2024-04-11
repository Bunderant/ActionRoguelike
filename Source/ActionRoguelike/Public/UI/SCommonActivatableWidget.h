// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SCommonActivatableWidget.generated.h"

/**
 * Base class for all custom activatable widgets, containing any additional project-wide functionality. 
 */
UCLASS(Blueprintable, Abstract)
class ACTIONROGUELIKE_API USCommonActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
};
