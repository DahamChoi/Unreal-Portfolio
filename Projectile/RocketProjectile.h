// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketProjectile.generated.h"

UCLASS()
class DEMO_API ARocketProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARocketProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	// Sphere Collision Component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComponent;

	// Projectile Movement Component
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// Particle System
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* RocketTrailParticle;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* RocketImpactParticle;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* RocketFireSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* RocketImpactSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* RocketFireAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* RocketImpactAudioComponent;

	UFUNCTION()
	void FireInDirection(const FVector& ShootDirection);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
