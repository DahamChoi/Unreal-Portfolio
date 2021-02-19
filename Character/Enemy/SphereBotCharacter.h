// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../CharacterBase.h"
#include "SphereBotCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnTransformationEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDeadDelegate);

UCLASS()
class DEMO_API ASphereBotCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASphereBotCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnDeadFunction() override;

	UFUNCTION(BlueprintCallable)
	void Transformation();

	UFUNCTION(BlueprintCallable)
	void TransformationOn();

	UFUNCTION(BlueprintCallable)
	void TransformationOff();

	UFUNCTION(BlueprintCallable)
	void StartShooting();

	UFUNCTION(BlueprintCallable)
	void StopShooting();

public:
	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	bool TransformationAnimation;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	bool TransformationEnable;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	bool TransformationInProgress;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	bool TransformationRotation;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	bool LaserShot;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	bool LaserShotStart;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	bool IsDamage;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	float LaserRange;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	float ShotTimer;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	float DamageEffect;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* PS_SourceLaser;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* PS_SourceLaserSparks;

	FOnTransformationEndDelegate OnTransformationEnd;
	FOnDeadDelegate OnDead;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystem* PS_LaserRed_Template;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystem* PS_ImpactLaserRed_Template;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystem* PS_SparksRed_Template;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* PS_LaserRed;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* PS_ImpactLaserRed;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* PS_SparksRed;

	static const float CAPSULE_RADIUS_BOT;
	static const float CAPSULE_RADIUS_SPHERE;
	static const float WALK_SPEED;
	static const float SPHERE_SPEED;
	static const float WALK_ACCELERATION;
	static const float SPHERE_ACCELERATION;
	static const float BRAKING_FRICTION_WALK;
	static const float BRAKING_FRICTION_SPHERE;
	static const float LASER_MAX_RANGE;
	static const float LASER_ACCELERATION;
	static const float MAX_HEALTH;
	static const float SHOT_DELAY;
	static const float IMPULSE;
	static const float DAMAGE;
	
	static const FRotator SPHERE_ROTATION_RATE;
	static const FRotator WALK_ROTATION_RATE;
};
