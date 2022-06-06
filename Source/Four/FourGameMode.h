// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FourGameMode.generated.h"

enum class EWaveState : uint8;

UCLASS(minimalapi)
class AFourGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFourGameMode();

	void ActorDied(AActor* DeadActor);

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveObjectiveFromObjectivesArray(FVector ObjSpawnLoc);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FVector> ObjectiveSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, bool> Objectives;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool bWonGame);

	UPROPERTY(BlueprintReadWrite)
	int32 WaveNum = 1;

	UPROPERTY(BlueprintReadWrite)
	int32 HighScore = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 Score = 0;

	float StartDelay = 3.f;

	// AI
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnAIEnemy();

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> AISpawnLocations;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle SpawnAITimer;

	UPROPERTY(BlueprintReadWrite)
	float AISpawnRate = 10.f;

	// Objective
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnObjective();

	/*UPROPERTY(BlueprintReadWrite)
	TArray<int32> Objectives;*/

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle SpawnObjectiveTimer;

	UPROPERTY(BlueprintReadWrite)
	float ObjectiveSpawnRate = 10.f;

private:
	class AFourCharacter* Player;
	class APlayerController* PlayerController;

	void HandleGameStart();
	int32 GetNumOfEnemys();
};