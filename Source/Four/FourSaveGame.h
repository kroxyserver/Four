// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FourSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class FOUR_API UFourSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UFourSaveGame();

    UPROPERTY(BlueprintReadWrite, Category = "Data To Save")
    FString PlayerName;

    UPROPERTY(BlueprintReadWrite, Category = "Data To Save")
    int32 Highscore;

    UPROPERTY(BlueprintReadWrite, Category = "Data To Save")
    int32 UserIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Save Game ID Vars")
    FString SaveSlotName;
};