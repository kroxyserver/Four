// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FourGameStateBase.generated.h"

// Wave states
UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	WaitingToComplete, // waiting for players to kill all bots
	WaveComplete,
	GameOver,
};

UCLASS()
class FOUR_API AFourGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;

public:
	void SetWaveState(EWaveState NewState);
};
