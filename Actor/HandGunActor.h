// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBaseActor.h"
#include "HandGunActor.generated.h"

UCLASS()
class DEMO_API AHandGunActor : public AWeaponBaseActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandGunActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HandGunFire();
	
	UFUNCTION()
	void HandGunReload();

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* WeaponFireSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* WeaponReloadSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* WeaponAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* WeaponReloadAudioComponent;
};
