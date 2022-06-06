// Fill out your copyright notice in the Description page of Project Settings.


#include "FourObjectiveActor.h"
#include "FourCharacter.h"
#include "FourGameMode.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFourObjectiveActor::AFourObjectiveActor()
{
	SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = SphereCollisionComp;

	ObjectiveFX = CreateDefaultSubobject<UParticleSystem>(TEXT("ObjectiveFX"));

	PickUpFX = CreateDefaultSubobject<UParticleSystem>(TEXT("PickUpFX"));
}

void AFourObjectiveActor::PlayEffects()
{
	UGameplayStatics::SpawnEmitterAttached(ObjectiveFX, SphereCollisionComp);
}

// Called when the game starts or when spawned
void AFourObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	
	PlayEffects();
}

// Called every frame
void AFourObjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFourObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AFourCharacter* PlayerCharacter = Cast<AFourCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->ActorHasTag("Player"))
	{
		// increase BasicAttack Damage
		PlayerCharacter->IncreaseDamage();

		/*FString Temp = FString::Printf(TEXT("Overlapped and destroyed"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Temp);*/

		UGameplayStatics::SpawnEmitterAtLocation(this, PickUpFX, GetActorLocation(), FRotator::ZeroRotator, FVector(.5f));

		// Making space for new objectives to spawn
		AFourGameMode* GM = Cast<AFourGameMode>(UGameplayStatics::GetGameMode(this));
		if (GM)
		{
			GM->RemoveObjectiveFromObjectivesArray(GetActorLocation());
		}

		Destroy();
	}
}