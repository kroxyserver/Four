// Fill out your copyright notice in the Description page of Project Settings.


#include "FourManaComponent.h"

// Sets default values for this component's properties
UFourManaComponent::UFourManaComponent()
{
	DefaultMana = 100.f;
}

// Called when the game starts
void UFourManaComponent::BeginPlay()
{
	Super::BeginPlay();

	Mana = DefaultMana;
}

float UFourManaComponent::GetMana() const
{
	return Mana;
}

float UFourManaComponent::GetMaxMana() const
{
	return DefaultMana;
}

void UFourManaComponent::RestoreMana(float ManaAmount)
{
	Mana = FMath::Clamp(Mana + ManaAmount, 0.0f, DefaultMana);

	//UE_LOG(LogTemp, Log, TEXT("Mana Changed: %s (+%s)"), *FString::SanitizeFloat(Mana), *FString::SanitizeFloat(ManaAmount));

	OnManaChanged.Broadcast(this, Mana, -ManaAmount);
}

void UFourManaComponent::ConsumeMana(float ManaAmount)
{
	Mana = FMath::Clamp(Mana - ManaAmount, 0.0f, DefaultMana);

	//UE_LOG(LogTemp, Log, TEXT("Mana Changed: %s (+%s)"), *FString::SanitizeFloat(Mana), *FString::SanitizeFloat(ManaAmount));

	OnManaChanged.Broadcast(this, Mana, ManaAmount);
}