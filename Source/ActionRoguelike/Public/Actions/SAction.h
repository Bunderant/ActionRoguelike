// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SAction.generated.h"

class USActionComponent;
/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class ACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Actions")
	FGameplayTagContainer GrantedTags;

	UPROPERTY(EditDefaultsOnly, Category="Actions")
	FGameplayTagContainer BlockedTags;

public:
	UPROPERTY(EditDefaultsOnly, Category="Actions")
	FName ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Actions")
	bool bAutoStart;
	
	UFUNCTION(BlueprintNativeEvent, Category="Actions")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Actions")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category="Actions")
	bool CanStart() const;

	UFUNCTION(BlueprintCallable, Category="Actions")
	bool IsRunning() const;

	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, Category="Actions")
	USActionComponent* GetOwningComponent() const;

protected:
	bool bIsRunning;
};
