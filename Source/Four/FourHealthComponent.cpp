// Fill out your copyright notice in the Description page of Project Settings.


#include "FourHealthComponent.h"
#include "FourGameMode.h"
#include "Particles/ParticleSystem.h"

// Sets default values for this component's properties
UFourHealthComponent::UFourHealthComponent()
{
	DefaultHealth = 100.f;
	bIsDead = false;

	TeamNum = 255;
}

// Called when the game starts
void UFourHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UFourHealthComponent::HandleTakeAnyDamage);
	}

	Health = DefaultHealth;
}

void UFourHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsDead)
	{
		return;
	}

	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}

	// Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Clamped : %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.f;

	if (bIsDead)
	{
		AFourGameMode* GM = Cast<AFourGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->ActorDied(DamagedActor);
		}
	}

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

float UFourHealthComponent::GetHealth() const
{
	return Health;
}

float UFourHealthComponent::GetMaxHealth() const
{
	return DefaultHealth;
}

bool UFourHealthComponent::IsDead() const
{
	return bIsDead;
}

void UFourHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

bool UFourHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}

	UFourHealthComponent* HealthCompA = Cast<UFourHealthComponent>(ActorA->GetComponentByClass(UFourHealthComponent::StaticClass()));
	UFourHealthComponent* HealthCompB = Cast<UFourHealthComponent>(ActorB->GetComponentByClass(UFourHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}