// Copyright Epic Games, Inc. All Rights Reserved.

#include "FourGameMode.h"
#include "FourCharacter.h"
#include "FourPlayerState.h"
#include "FourGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AFourGameMode::AFourGameMode()
{
	GameStateClass = AFourGameStateBase::StaticClass();
	PlayerStateClass = AFourPlayerState::StaticClass();
}

void AFourGameMode::ActorDied(AActor* DeadActor)
{
	if (DeadActor == Player)
	{
		if (PlayerController)
		{
			DisableInput(PlayerController);
		}
		GameOver(false); // Save game in BP
	}
	else if (DeadActor->ActorHasTag("Bot"))
	{
		AFourPlayerState* PS = Cast<AFourPlayerState>(Player->GetPlayerState());
		if (PS)
		{
			PS->AddScore(1.f);
			Score++;
		}

		if (!GetWorld()->GetTimerManager().IsTimerActive(SpawnAITimer) && GetNumOfEnemys() == 1)
		{
			GameOver(true); // Save game in BP
		}
	}
}

void AFourGameMode::BeginPlay()
{
	Super::BeginPlay();

	HandleGameStart();

	GetWorld()->GetTimerManager().SetTimer(SpawnAITimer, this, &AFourGameMode::SpawnAIEnemy, AISpawnRate, true, StartDelay);

	GetWorld()->GetTimerManager().SetTimer(SpawnObjectiveTimer, this, &AFourGameMode::SpawnObjective, ObjectiveSpawnRate, true, StartDelay);
}

void AFourGameMode::HandleGameStart()
{
	Player = Cast<AFourCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	StartGame();
}

int32 AFourGameMode::GetNumOfEnemys()
{
	TArray<AActor*> Enemys;
	UGameplayStatics::GetAllActorsWithTag(this, "Bot", Enemys);

	return Enemys.Num();
}