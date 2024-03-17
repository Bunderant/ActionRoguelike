// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "SWorldCommonUserWidget.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USWorldCommonUserWidget : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox;

	UPROPERTY(EditAnywhere, Category="UI")
	FVector WorldOffset;

public:
	UPROPERTY(BlueprintReadOnly, Category="UI", meta=(ExposeOnSpawn=true))
	TObjectPtr<AActor> AttachedActor;
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
