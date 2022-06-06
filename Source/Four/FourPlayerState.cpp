// Fill out your copyright notice in the Description page of Project Settings.


#include "FourPlayerState.h"

void AFourPlayerState::AddScore(float ScoreDelta)
{
	float MyScore;
	MyScore = GetScore();
	MyScore += ScoreDelta;
	SetScore(MyScore);
}