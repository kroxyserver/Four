// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FourManaComponent.generated.h"

// OnManaChaged Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnManaChangedSignature, UFourManaComponent*, ManaComp, float, Mana, float, ManaDelta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOUR_API UFourManaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFourManaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "ManaComponent")
	float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ManaComponent")
	float DefaultMana;

public:
	float GetMana() const;

	float GetMaxMana() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnManaChangedSignature OnManaChanged;

	UFUNCTION(BlueprintCallable, Category = "ManaComponent")
	void RestoreMana(float ManaAmount);

	UFUNCTION(BlueprintCallable, Category = "ManaComponent")
	void ConsumeMana(float ManaAmount);
};
