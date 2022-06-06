// Copyright Epic Games, Inc. All Rights Reserved.

#include "FourCharacter.h"
#include "FourProjectile.h"
#include "FourGameMode.h"
#include "FourHealthComponent.h"
#include "FourManaComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/Gameplaystatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"


//////////////////////////////////////////////////////////////////////////
// AFourCharacter

AFourCharacter::AFourCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a MagicStaff
	MagicStaff = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagicStaff"));
	MagicStaff->SetupAttachment(GetMesh(), "WeaponSocket");

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(MagicStaff);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create Health Mana Widget
	HealthManaWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health & Mana Widget"));
	HealthManaWidget->SetupAttachment(RootComponent);

	// Health Comp
	HealthComp = CreateDefaultSubobject<UFourHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &AFourCharacter::OnHealthChanged);

	// Mana Comp
	ManaComp = CreateDefaultSubobject<UFourManaComponent>(TEXT("ManaComp"));
	ManaComp->OnManaChanged.AddDynamic(this, &AFourCharacter::OnManaChanged);

	// TargettingEffect
	TargettingEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TargettingEffect"));
	TargettingEffect->SetupAttachment(RootComponent);
	TargettingEffect->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	TargettingEffect->SetVisibility(false);

	//// TargettingArrow
	//TargettingArrow = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TargettingArrow"));
	//TargettingArrow->SetupAttachment(CameraBoom);
	//TargettingArrow->SetRelativeLocation(FVector(1200.f, 0.f, -1290.f));
	//TargettingArrow->SetVisibility(false);

	//// TargettingCollision
	//TargettingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TargettingCollision"));
	//TargettingCollision->SetupAttachment(TargettingArrow);

	// Healing Effect
	HealEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HealEffect"));
	HealEffect->SetupAttachment(RootComponent);
	HealEffect->SetVisibility(false);

	// SkillOneEffect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> SkillOne(TEXT("ParticleSystem'/Game/RPGEffects/Particles/P_Mage_Lightning_Bolt.P_Mage_Lightning_Bolt'"));
	if (SkillOne.Succeeded())
	{
		SkillOneEffect = SkillOne.Object;
	}

	// SkillTwoEffect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> SkillTwo(TEXT("ParticleSystem'/Game/RPGEffects/Particles/P_Warrior_HeavyImpact.P_Warrior_HeavyImpact'"));
	if (SkillTwo.Succeeded())
	{
		SkillTwoEffect = SkillTwo.Object;
	}

	// BasicAttack
	AttackRate = 1.f;
	DamageValue = 0.f;
	bIsAttacking = false;
	
	// SkillOne
	SkillOneDuration = 2.f;
	bIsUsingSkillOne = false;
	SkillOneDamageType = UDamageType::StaticClass();
	SkillOneDamage = 40.0f;
	SkillOneManaCost = 30.f;
	SkillOneCoolDown = 10.f;
	
	// SkillTwo
	SkillTwoDuration = 2.f;
	bIsUsingSkillTwo = false;
	SkillTwoDamageType = UDamageType::StaticClass();
	SkillTwoMaxDamage = 40.0f;
	SkillTwoMinDamage = 10.f;
	SkillTwoDamageOuterRadius = 400.f;
	SkillTwoDamageInnerRadius = 100.f;
	SkillTwoManaCost = 30.f;
	SkillTwoCoolDown = 15.f;
	
	// Stun
	StunDuration = 1.f;

	IgnoredActors.Add(this);
	UpdateAttributeRate = 2.f;
	AttackRange = 1200.f;
	bDied = false;
}

// BasicAttack-------------------------------------------------------------------------------------
void AFourCharacter::StartAttacking()
{
	if (!bIsAttacking && !bIsUsingSkillOne && !bIsUsingSkillTwo)
	{
		AFourCharacter* Target = Cast<AFourCharacter>(AttackClosestEnemyInRangeWithTag(Enemy));
		if (Target)
		{
			//FString Temp = FString::Printf(TEXT("Fire"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Temp);

			bIsAttacking = true;

			// Look At Target
			FaceEnemy(Target->GetActorLocation());

			PlayAttackAnimation();
			
			// BasicAttack Rate Timer
			GetWorld()->GetTimerManager().SetTimer(AttackingTimer, this, &AFourCharacter::StopAttacking, AttackRate, false);

			SpawnProj.BindUFunction(this, FName("SpawnProjectile"), Target);
			GetWorld()->GetTimerManager().SetTimer(SpawnProjDelay, SpawnProj, .4f, false);
		}
	}
}

void AFourCharacter::StopAttacking()
{
	bIsAttacking = false;
}

void AFourCharacter::SpawnProjectile(AFourCharacter* TargetActor)
{
	FaceEnemy(TargetActor->GetActorLocation());

	FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
	FRotator SpawnRotation = ProjectileSpawnPoint->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.Owner = this;

	AFourProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AFourProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	SpawnedProjectile->SetDamageValue(DamageValue);
	SpawnedProjectile->SetHomingTarget(TargetActor);
}

// SkillOne----------------------------------------------------------------------------------------
void AFourCharacter::StartSkillOne()
{
	if (!bIsAttacking && !bIsUsingSkillOne && !bIsUsingSkillTwo)
	{
		bool IsSkillOneAvailable = !GetWorld()->GetTimerManager().IsTimerActive(SkillOneCoolDownTimer);

		if (IsSkillOneAvailable && ManaComp->GetMana() > SkillOneManaCost)
		{
			AFourCharacter* Target = Cast<AFourCharacter>(AttackClosestEnemyInRangeWithTag(Enemy));
			if (Target)
			{
				FString Temp = FString::Printf(TEXT("skill1"));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Temp);

				bIsUsingSkillOne = true;

				// Consume Mana
				ManaComp->ConsumeMana(SkillOneManaCost);

				// Look At Target
				FaceEnemy(Target->GetActorLocation());

				// Put SkillOne on CoolDown
				GetWorld()->GetTimerManager().SetTimer(SkillOneCoolDownTimer, SkillOneCoolDown, false);

				PlaySkillOneAnimation();
				
				// SkillOne Duration Timer
				GetWorld()->GetTimerManager().SetTimer(SkillOneTimer, this, &AFourCharacter::StopSkillOne, SkillOneDuration, false);

				// Call SpawnSkillOneFX Function
				SpawnSkillOneEffect.BindUFunction(this, FName("SpawnSkillOneFX"), Target);
				GetWorld()->GetTimerManager().SetTimer(SpawnSkillOneEffectDelay, SpawnSkillOneEffect, 1.3f, false);
			}
		}
		else
		{
			/*FString Temp = FString::Printf(TEXT("Not enough mana"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Temp);*/
		}
	}
}

void AFourCharacter::StopSkillOne()
{
	bIsUsingSkillOne = false;
}

void AFourCharacter::SpawnSkillOneFX(AFourCharacter* TargetActor)
{
	// Stun TargetActor
	Stun(TargetActor);

	// Remove Stun from TargetActor
	StunDel.BindUFunction(this, FName("RemoveStun"), TargetActor);
	GetWorld()->GetTimerManager().SetTimer(StunDelay, StunDel, StunDuration, false);

	UGameplayStatics::ApplyDamage(TargetActor, SkillOneDamage, GetController(), this, SkillOneDamageType);
	UGameplayStatics::SpawnEmitterAttached(SkillOneEffect, TargetActor->GetRootComponent());
}

// SkillTwo----------------------------------------------------------------------------------------
void AFourCharacter::StartSkillTwo()
{
	if (!bIsAttacking && !bIsUsingSkillTwo && !bIsUsingSkillTwo)
	{
		bool IsSkillTwoAvailable = !GetWorld()->GetTimerManager().IsTimerActive(SkillTwoCoolDownTimer);

		if (IsSkillTwoAvailable && ManaComp->GetMana() > SkillTwoManaCost)
		{
			AFourCharacter* Target = Cast<AFourCharacter>(AttackClosestEnemyInRangeWithTag(Enemy));
			if (Target)
			{
				FString Temp = FString::Printf(TEXT("skill2"));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Temp);

				bIsUsingSkillTwo = true;
		
				// Consume Mana
				ManaComp->ConsumeMana(SkillTwoManaCost);

				// Look At Target
				FaceEnemy(Target->GetActorLocation());
				
				// Put SkillTwo on CoolDown
				GetWorld()->GetTimerManager().SetTimer(SkillTwoCoolDownTimer, SkillTwoCoolDown, false);
				
				PlaySkillTwoAnimation();

				// SkillTwo Duration Timer
				GetWorld()->GetTimerManager().SetTimer(SkillTwoTimer, this, &AFourCharacter::StopSkillTwo, SkillTwoDuration, false);

				// Call SpawnSkillTwoFX Function
				SpawnSkillTwoEffect.BindUFunction(this, FName("SpawnSkillTwoFX"), Target);
				GetWorld()->GetTimerManager().SetTimer(SpawnSkillTwoEffectDelay, SpawnSkillTwoEffect, 1.3f, false);
			}
		}
		else
		{
			/*FString Temp = FString::Printf(TEXT("Not enough mana"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Temp);*/
		}
	}
}

void AFourCharacter::StopSkillTwo()
{
	bIsUsingSkillTwo = false;
}

void AFourCharacter::SpawnSkillTwoFX(AFourCharacter* TargetActor)
{
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		SkillTwoMaxDamage,
		SkillTwoMinDamage,
		TargetActor->GetActorLocation(),
		SkillTwoDamageInnerRadius,
		SkillTwoDamageOuterRadius,
		50.f,
		SkillTwoDamageType,
		IgnoredActors,
		this,
		GetInstigatorController()
	);

	UGameplayStatics::SpawnEmitterAttached(SkillTwoEffect, TargetActor->GetMesh());
}

// ------------------------------------------------------------------------------------------------
void AFourCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(UpdateAttributeTimer, this, &AFourCharacter::UpdateAttributes, UpdateAttributeRate, true);
}

void AFourCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveForward", this, &AFourCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFourCharacter::MoveRight);

	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &AFourCharacter::StartAttacking);
	PlayerInputComponent->BindAction("Skill1", IE_Pressed, this, &AFourCharacter::StartSkillOne);
	PlayerInputComponent->BindAction("Skill2", IE_Pressed, this, &AFourCharacter::StartSkillTwo);
}

//void AFourCharacter::Destroyed()
//{
//	Super::Destroyed();
//
//	if (SpectatingViewPointClass)
//	{
//		TArray<AActor*> ReturnedActors;
//		UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewPointClass, ReturnedActors);
//
//		if (ReturnedActors.Num() > 0)
//		{
//			AActor* NewViewTarget = ReturnedActors[0];
//
//			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
//			{
//				APlayerController* PC = It->Get();
//				if (PC)
//				{
//					PC->SetViewTargetWithBlend(NewViewTarget, 2.f, EViewTargetBlendFunction::VTBlend_Cubic);
//				}
//			}
//		}
//	}
//}

void AFourCharacter::FaceEnemy(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - GetActorLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	SetActorRotation(LookAtRotation);
}

void AFourCharacter::OnHealthChanged(UFourHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bDied)
	{
		bDied = true;

		GetWorld()->GetTimerManager().ClearTimer(UpdateAttributeTimer);
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		ShowDeathScreen();

		SetLifeSpan(5.f);
	}
}

// empty for now
void AFourCharacter::OnManaChanged(UFourManaComponent* OwningManaComponent, float Mana, float ManaDelta)
{
}

void AFourCharacter::UpdateAttributes()
{
	// For Healing
	if (HealthComp->GetHealth() < HealthComp->GetMaxHealth())
	{
		HealEffect->SetVisibility(true);

		HealthComp->Heal(HealthComp->GetMaxHealth() / 50); // heal 2.5% max health every tick
	}
	else
	{
		HealEffect->SetVisibility(false);
	}

	// For Mana Restoration
	if (ManaComp->GetMana() < ManaComp->GetMaxMana())
	{
		ManaComp->RestoreMana(ManaComp->GetMaxMana() / 50); // restore 2.5% max mana every tick
	}
}

AActor* AFourCharacter::AttackClosestEnemyInRangeWithTag(FName Tag)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);

	AActor* ClosestActor = nullptr;
	float ClosestActorDistance = MAX_FLT;

	for (AActor* Temp : FoundActors)
	{
		AFourCharacter* TempFour = Cast<AFourCharacter>(Temp);
		if (TempFour && !TempFour->bDied)
		{
			if (TempFour != this)
			{
				float Dist = FVector::Dist(GetActorLocation(), TempFour->GetActorLocation());
				if (Dist < ClosestActorDistance)
				{
					ClosestActorDistance = Dist;
					ClosestActor = TempFour;
				}
			}
		}
	}

	if (ClosestActorDistance < AttackRange)
	{
		return ClosestActor;
	}

	return nullptr;
}

// For Player
void AFourCharacter::SpawnTargettingDecalOnEnemy()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Enemy, FoundActors);

	AFourCharacter* ClosestActor = nullptr;
	float ClosestActorDistance = MAX_FLT;

	for (AActor* Temp : FoundActors)
	{
		AFourCharacter* TempFour = Cast<AFourCharacter>(Temp);
		if (TempFour && !TempFour->bDied)
		{
			ShowHealthManaWidget(TempFour, false);
			TempFour->TargettingEffect->SetVisibility(false);

			float Dist = FVector::Dist(GetActorLocation(), TempFour->GetActorLocation());
			if (Dist < ClosestActorDistance)
			{
				ClosestActorDistance = Dist;
				ClosestActor = TempFour;
			}
		}
	}

	if (ClosestActor && ClosestActorDistance < AttackRange)
	{
		ShowHealthManaWidget(ClosestActor, true);
		ClosestActor->TargettingEffect->SetVisibility(true);
	}
}

// For AI
int32 AFourCharacter::DecideWhichSkillToUse()
{
	// Prioritize BasicAttack
	int32 SkillPriority = 0;

	// Get Current Mana
	float CurrentMana = ManaComp->GetMana();

	// Are Skills on Cooldown or Not
	bool IsSkillOneAvailable = !GetWorld()->GetTimerManager().IsTimerActive(SkillOneCoolDownTimer);
	bool IsSkillTwoAvailable = !GetWorld()->GetTimerManager().IsTimerActive(SkillTwoCoolDownTimer);

	if (!bIsAttacking && !bIsUsingSkillOne && !bIsUsingSkillTwo)
	{
		if (IsSkillOneAvailable && CurrentMana > SkillOneManaCost)
		{
			// Prioritize Skill 1
			SkillPriority = 1;
		}
		else
		{
			if (!bIsUsingSkillTwo && IsSkillTwoAvailable && CurrentMana > SkillTwoManaCost)
			{
				// Prioritize Skill 2
				SkillPriority = 2;
			}
		}
	}
	return SkillPriority;
}

// Its actually Rooted for now
void AFourCharacter::Stun(AFourCharacter* TargetActor)
{
	TargetActor->GetCharacterMovement()->DisableMovement();
}

// Remove Rooted
void AFourCharacter::RemoveStun(AFourCharacter* TargetActor)
{
	TargetActor->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AFourCharacter::IncreaseDamage()
{
	DamageValue += 5.f;

	/*FString Temp = FString::Printf(TEXT("%f"), DamageValue);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Temp);*/
}

void AFourCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFourCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
