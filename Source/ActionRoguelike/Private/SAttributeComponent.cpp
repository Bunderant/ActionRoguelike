﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"


// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100.0f;
}

bool USAttributeComponent::ApplyHealthChange(const float Delta)
{
	Health += Delta;

	OnAttributeChanged.Broadcast(nullptr, this, Health, Delta);
	
	return true;
}

