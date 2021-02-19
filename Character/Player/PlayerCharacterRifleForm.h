// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.h"
#include "Components/TimelineComponent.h"
#include "../../UI/MainUserWidget.h"
#include "PlayerCharacterRifleForm.generated.h"

UCLASS()
class DEMO_API APlayerCharacterRifleForm : public APlayerCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacterRifleForm();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Character State
	virtual void InitCharacterState() override;

	/* PlayerCharacter Action Function	*/

	// Crouch
	UFUNCTION()
	void PCActionCrouch();

	UFUNCTION()
	void PCActionStopCrouch();

	// Fire
	UFUNCTION()
	void PCActionFire();

	UFUNCTION()
	void PCActionStopFire();

	// Reload
	UFUNCTION()
	void PCActionReload();

	// Ultimate
	UFUNCTION()
	void PCActionUltimate();

	// BioticFiled
	UFUNCTION()
	void PCActionBioticFiled();

	// RocketLauncher
	UFUNCTION()
	void PCActionRocketLauncher();

	// Sprint
	virtual void PCActionSprint() override;

	// Jump
	virtual void PCActionJump() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsReload;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsUltimate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsCrouch;

	// Reload Animation EndTime Checker Parameter
	UPROPERTY()
	float LastReloadTime;

	// Check Fire Cool Time
	UPROPERTY()
	float LastFireBulletTime;

	// Projectile Class
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ARocketProjectile> RocketProjectileClass;

	// Ultimate Paramater
	UPROPERTY()
	class ACharacter* CurrentPendingTarget;

	UPROPERTY()
	float UltimateLerpRotatorAlpha;

	UPROPERTY()
	class AScifiWeapon* ScifiWeapon;

	// Sound
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* BioticFieldSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* BioticFieldAudioComponent;

	// WALK SPEED
	static const float CROUCH_WALK_SPEED;

	// PLAYER STATE
	static const float MAX_HP;
	static const float MAX_E_SKILL_TIME_REMAINING;
	static const float MAX_F_SKILL_TIME_REMAINING;
	static const int MAX_BULLET_COUNT;
};
