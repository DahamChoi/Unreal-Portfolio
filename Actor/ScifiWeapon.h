// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBaseActor.h"
#include "ScifiWeapon.generated.h"

UCLASS()
class DEMO_API AScifiWeapon : public AWeaponBaseActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScifiWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void WeaponFire(bool CanFire);

	UFUNCTION(BlueprintCallable)
	void WeaponStopFire();

	UFUNCTION(BlueprintCallable)
	void WeaponReload();

private:
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* AssaultGunHoleParticle;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* WeaponFireSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* WeaponDryFireSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* WeaponReloadSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* WeaponAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* WeaponDryAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* WeaponReloadAudioComponent;
};
