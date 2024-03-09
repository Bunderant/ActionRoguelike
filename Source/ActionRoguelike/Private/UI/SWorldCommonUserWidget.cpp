// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SWorldCommonUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

void USWorldCommonUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attached actor is no longer valid, removing projected world widget."))
		RemoveFromParent();
		return;
	}

	FVector2D ScreenPosition;
	if (!UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetOwningPlayer(),
		AttachedActor->GetActorLocation() + WorldOffset,
		ScreenPosition,
		false))
	{
		return;
	}

	ParentSizeBox->SetRenderTranslation(ScreenPosition);
}
