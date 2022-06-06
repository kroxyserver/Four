// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FourCharacter.generated.h"

UCLASS(config=Game)
class AFourCharacter : public ACharacter
{
	GENERATED_BODY()

	// MagicStaff of the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MagicStaff;

	// Projectile Spawn Point
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ProjectileSpawnPoint;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Health and Mana Widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthManaWidget;

	// Health Comp for character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UFourHealthComponent* HealthComp;

	// Mana Comp for character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UFourManaComponent* ManaComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* TargettingDecal;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystemComponent* HealEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* SkillOneEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* SkillTwoEffect;

public:
	AFourCharacter();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystemComponent* TargettingEffect;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UParticleSystemComponent* TargettingArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UBoxComponent* TargettingCollision;*/

	void IncreaseDamage();

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void FaceEnemy(FVector LookAtTarget);

	UFUNCTION()
	void OnHealthChanged(UFourHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnManaChanged(UFourManaComponent* OwningManaComponent, float Mana, float ManaDelta);

	UFUNCTION()
	void UpdateAttributes();

	FTimerHandle UpdateAttributeTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float UpdateAttributeRate;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	AActor* AttackClosestEnemyInRangeWithTag(FName Tag);

	// Functionality for player
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SpawnTargettingDecalOnEnemy();

	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void ShowHealthManaWidget(AFourCharacter* TargetActor, bool bValue);


	// Functionality for AI
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	int32 DecideWhichSkillToUse();

	// (Rooted for now) Stun Effect functionality----------------------------
	FTimerDelegate StunDel;

	FTimerHandle StunDelay;

	float StunDuration;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void Stun(AFourCharacter* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void RemoveStun(AFourCharacter* TargetActor);

	// BasicAttack-----------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|BasicAttack")
	TSubclassOf<class AFourProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|BasicAttack")
	float AttackRate;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|BasicAttack")
	void StartAttacking();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|BasicAttack")
	void StopAttacking();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|BasicAttack")
	void SpawnProjectile(AFourCharacter* TargetActor);

	float AttackRange;

	float DamageValue;

	bool bIsAttacking;

	FTimerHandle AttackingTimer;

	FTimerDelegate SpawnProj;

	FTimerHandle SpawnProjDelay;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayAttackAnimation();


	// SkillOne--------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Gameplay|SkillOne")
	void StartSkillOne();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|SkillOne")
	void StopSkillOne();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|SkillOne")
	float SkillOneDuration;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|SkillOne")
	void SpawnSkillOneFX(AFourCharacter* TargetActor);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|SkillOne")
	float SkillOneManaCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|SkillOne")
	float SkillOneCoolDown;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle SkillOneCoolDownTimer;

	FTimerHandle SkillOneTimer;

	FTimerDelegate SpawnSkillOneEffect;

	FTimerHandle SpawnSkillOneEffectDelay;

	bool bIsUsingSkillOne;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillOneAnimation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|SkillOne")
	TSubclassOf<class UDamageType> SkillOneDamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|SkillOne")
	float SkillOneDamage;


	// SkillTwo--------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Gameplay|SkillTwo")
	void StartSkillTwo();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|SkillTwo")
	void StopSkillTwo();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|SkillTwo")
	float SkillTwoDuration;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|SkillTwo")
	void SpawnSkillTwoFX(AFourCharacter* TargetActor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|SkillTwo")
	float SkillTwoManaCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|SkillTwo")
	float SkillTwoCoolDown;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle SkillTwoCoolDownTimer;

	FTimerHandle SkillTwoTimer;

	FTimerDelegate SpawnSkillTwoEffect;

	FTimerHandle SpawnSkillTwoEffectDelay;

	bool bIsUsingSkillTwo;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillTwoAnimation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|SkillTwo")
	TSubclassOf<class UDamageType> SkillTwoDamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|SkillTwo")
	float SkillTwoMaxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|SkillTwo")
	float SkillTwoMinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|SkillTwo")
	float SkillTwoDamageInnerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay|SkillTwo")
	float SkillTwoDamageOuterRadius;

	// TODO---------------------------------------------------------------------
	UFUNCTION(BlueprintImplementableEvent)
	void ShowDeathScreen();

protected:
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	bool bDied;

	TArray<AActor*> IgnoredActors;

	// UPROPERTY(EditDefaultsOnly, Category = "Spectating")
	// TSubclassOf<AActor> SpectatingViewPointClass;

public:
	//virtual void Destroyed() override;

	/** Returns MagicStaff subobject **/
	FORCEINLINE class UStaticMeshComponent* GetMagicStaff() const { return MagicStaff; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	FName Enemy;
};