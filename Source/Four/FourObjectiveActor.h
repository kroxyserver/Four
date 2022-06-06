// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FourObjectiveActor.generated.h"

UCLASS()
class FOUR_API AFourObjectiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFourObjectiveActor();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereCollisionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* ObjectiveFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* PickUpFX;

	void PlayEffects();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
