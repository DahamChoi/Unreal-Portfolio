// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterHandGunForm.generated.h"

UENUM(BlueprintType)
enum class EDodgeDirectionEnum : uint8
{
	EDDE_FRONT	UMETA(DisplayName = "FRONT"),
	EDDE_BACK	UMETA(DisplayName = "BACK"),
	EDDE_LEFT	UMETA(DisplayName = "LEFT"),
	EDDE_RIGHT	UMETA(DisplayName = "RIGHT")
};

UCLASS()
class DEMO_API APlayerCharacterHandGunForm : public APlayerCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacterHandGunForm();

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

	// Fire
	UFUNCTION()
	void PCActionFire();

	// Reload
	UFUNCTION()
	void PCActionReload();

	// Ultimate
	UFUNCTION()
	void PCActionUltimate();

	// Dodge
	UFUNCTION()
	void PCActionDodge();

	// Rampage
	UFUNCTION()
	void PCActionRampage();

	// Dodge TimeLine Function
	UFUNCTION()
	void HandleDodgeCurveProgress(float value);

	// Move Forward, Right
	virtual void MoveForward(float Value) override;

	virtual void MoveRight(float Value) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsReload;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsUltimate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsDodge;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsRampage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EDodgeDirectionEnum PCActionDodgeDirection;

	UPROPERTY()
	float LastReloadTime;

	UPROPERTY()
	float LastFireTime;

	UPROPERTY()
	float LastForwardValue;

	UPROPERTY()
	float LastRightValue;

	UPROPERTY()
	class AHandGunActor* HandGun;

	// Dodge Movement Float Curve
	UPROPERTY()
	UCurveFloat* DodgeCurveFloat;

	// Dodge TimeLine
	UPROPERTY()
	FTimeline DodgeTimeline;

	// Rampage Timer
	UPROPERTY()
	FTimerHandle RampageTimerHandle;

	// Sound
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* DodgeSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* DodgeAudioComponent;

	// ANIMATION TIME
	static const float RELOAD_ANIMATION_TIME;
	static const float FIRE_ANIMATION_TIME;

	// PLAYER STATE
	static const float MAX_HP;
	static const float MAX_E_SKILL_TIME_REMAINING;
	static const float MAX_F_SKILL_TIME_REMAINING;
	static const int MAX_BULLET_COUNT;
};
