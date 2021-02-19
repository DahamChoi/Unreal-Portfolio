// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DemoProjectile.generated.h"

UCLASS()
class DEMO_API ADemoProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADemoProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	// Particle
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* AssaultTrailParticle;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* AssaultMFParticle;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* ImpactStaticSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* ImpactDynamicSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* ImpactStaticAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* ImpactDynamicAudioComponent;

	UPROPERTY(VisibleAnywhere)
	float ProjectileDamageAmount;

	bool FireInDirection(const FVector& ShootStart, const FRotator& ShootRotation);
};
