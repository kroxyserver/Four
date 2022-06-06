// Fill out your copyright notice in the Description page of Project Settings.


#include "FourProjectile.h"
#include "FourCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFourProjectile::AFourProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SphereComponent
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(10.f);
	SphereComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = SphereComponent;

	// TrailEffect
	TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);

	// ProjectileMovementComponent
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed = 800.f;
	ProjectileMovementComponent->MaxSpeed = 800.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bIsHomingProjectile = true;
	ProjectileMovementComponent->HomingAccelerationMagnitude = 10000.f;

	// ExplosionEffect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("ParticleSystem'/Game/BattleWizardPolyart/Projectile/P_FireBallExplosion.P_FireBallExplosion'"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	// DamageType and Damage
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;
}

void AFourProjectile::SetDamageValue(float DamageValue)
{
	Damage += DamageValue;
}

// Called when the game starts or when spawned
void AFourProjectile::BeginPlay()
{
	Super::BeginPlay();

	AFourCharacter* MyOwner = Cast<AFourCharacter>(GetOwner());
	if (MyOwner)
	{
		EnemyTag = MyOwner->Enemy;
	}

	SetLifeSpan(10.f);
}

void AFourProjectile::Destroyed()
{
	FVector SpawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, SpawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}

// Called every frame
void AFourProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFourProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AFourCharacter* HitPlayer = Cast<AFourCharacter>(OtherActor);
	if (HitPlayer && HitPlayer->ActorHasTag(EnemyTag))
	{
		UGameplayStatics::ApplyDamage(HitPlayer, Damage, GetInstigatorController(), this->GetOwner(), DamageType);
		Destroy();
	}
}

void AFourProjectile::SetHomingTarget(AFourCharacter* Target)
{
	//TargetActor = Target;
	ProjectileMovementComponent->HomingTargetComponent = Target->GetRootComponent();
}