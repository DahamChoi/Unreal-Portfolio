// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CharacterBase.h"
#include "Components/TimelineComponent.h"
#include "../../UI/MainUserWidget.h"
#include "PlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnUltimateActiveDelegate);

DECLARE_MULTICAST_DELEGATE(FOnTakeDamageActiveDelegate);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateCrossHairDelegate, FVector2D);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKillEnemyDelegate, FString, FString);

UCLASS()
class DEMO_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

protected:
	// Begin Play
	virtual void BeginPlay() override;

public:
	// Contructor
	APlayerCharacter();

	// Player Character Components //
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* FollowCamera;

	// Not Setup Attach, When Begin Play, Spawn This Actor And AttachTo Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AWeaponBaseActor* Weapon;

	// Particle
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite)
	class UParticleSystemComponent* TrailParticle;

	// Call Every Time
	virtual void Tick(float DeltaTime) override;

	// If This Character Controller Is Player Controller, Setup Player Input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Take Damage
	virtual float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// WASD Movement - Bind Axis
	virtual void MoveForward(float Value);
	
	virtual void MoveRight(float Value);

	// Trace To 2D
	FVector2D Get2DVectorByTraceWeapon();

	// Character State
	virtual void InitCharacterState() {};

	/* PlayerCharacter Action Function	*/

	// Battle Form
	virtual void PCActionChangeBattleForm();

	// Jump
	UFUNCTION()
	virtual void PCActionJump();

	// Aim
	UFUNCTION()
	virtual void PCActionAim();

	UFUNCTION()
	virtual void PCActionStopAim();

	// Sprint
	UFUNCTION()
	virtual void PCActionSprint();

	UFUNCTION()
	virtual void PCActionStopSprint();

	// Aim Camera TimeLine Function
	UFUNCTION()
	void HandleCurveProgress(float value);

	UFUNCTION()
	bool FireDemoProjectile(float DamageAmount = 10.f);

	// For Update Remaing Time, (Timer Function)
	UFUNCTION()
	void ESkillAdvanceTimer();

	UFUNCTION()
	void FSkillAdvanceTimer();

	UFUNCTION()
	void RunESkillTimer();

	UFUNCTION()
	void RunFSkillTimer();

	// Player Name To Edit Unreal Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsAim;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PCActionIsSprint;

	// Aim Camera TimeLine
	UPROPERTY()
	FTimeline AimingTimeline;

	// Aim Camera Movement Float Curve
	UPROPERTY()
	UCurveFloat* CurveFloat;

	// Check Jump Cool Time
	UPROPERTY()
	float LastJumpTime;

	UPROPERTY()
	float LastChangeFormTime;

	UPROPERTY()
	FTransform SpawnTransform;

	// Sound
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* FootStepSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* UltimateSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* FootStepAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* UltimateAudioComponent;

	UPROPERTY()
	float FootStepTimer;

	// Projectile Class
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ADemoProjectile> ProjectileClass;

	// Skill Remain Time Timer
	UPROPERTY()
	FTimerHandle ESkillTimerHandle;

	UPROPERTY()
	FTimerHandle FSkillTimerHandle;

	// Main UI Bind Delegate
	FOnKillEnemyDelegate OnKillEnemy;
	FOnUpdateCrossHairDelegate OnUpdateCrossHair;
	FOnTakeDamageActiveDelegate OnTakeDamageActive;
	FOnUltimateActiveDelegate OnUltimateActive;

	// Aim Camera Location
	static const FVector HIP_CAMERA_LOCATION;
	static const FVector AIM_CAMERA_LOCATION;

	// WALK SPEED
	static const float DEFAULT_WALK_SPEED;
	static const float SPRINT_WALK_SPEED;
};
