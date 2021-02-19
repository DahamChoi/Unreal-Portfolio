// Fill out your copyright notice in the Description page of Project Settings.


#include "ScifiWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PointLightComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "../Character/Player/PlayerCharacter.h"
#include "../State/DemoPlayerState.h"

// Sets default values
AScifiWeapon::AScifiWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Dummy Root
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// WeaponSkeletalMesh
	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
	WeaponStaticMesh->SetupAttachment(DummyRoot);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_RIFLE(
		TEXT("/Game/SciFiSoldier/Meshes/SM_AssaultRifle"));
	if (SM_RIFLE.Succeeded())
		WeaponStaticMesh->SetStaticMesh(SM_RIFLE.Object);

	AssaultGunHoleParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AssaultGunHoleParticle"));
	AssaultGunHoleParticle->SetupAttachment(WeaponStaticMesh);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ASSAULT_GUNHOLE(
		TEXT("/Game/Effects/ParticleSystems/Weapons/AssaultRifle/Muzzle/P_AssaultRifle_MF.P_AssaultRifle_MF"));
	if (PS_ASSAULT_GUNHOLE.Succeeded())
		AssaultGunHoleParticle->SetTemplate(PS_ASSAULT_GUNHOLE.Object);
	AssaultGunHoleParticle->SetRelativeLocation(FVector(80.f, 0.f, 8.f));

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_WEAPON_FIRE(
		TEXT("/Game/Demo/Sounds/Weapons/AssultRifle/SCue_AR_Shoot_LP"));
	WeaponFireSoundCue = CUE_WEAPON_FIRE.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_WEAPON_DRY_FIRE(
		TEXT("/Game/Demo/Sounds/Weapons/AssultRifle/SCue_AR_Dryfire"));
	WeaponDryFireSoundCue = CUE_WEAPON_DRY_FIRE.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_WEAPON_RELOAD(
		TEXT("/Game/Demo/Sounds/Weapons/AssultRifle/SCue_AR_Reload"));
	WeaponReloadSoundCue = CUE_WEAPON_RELOAD.Object;

	WeaponAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponAudioComponent"));
	WeaponAudioComponent->bAutoActivate = false;
	WeaponAudioComponent->SetupAttachment(WeaponStaticMesh);
	WeaponAudioComponent->SetRelativeLocation(FVector(80.f, 0.f, 8.f));
	WeaponAudioComponent->bOverrideAttenuation = true;

	WeaponDryAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponDryAudioComponent"));
	WeaponDryAudioComponent->bAutoActivate = false;
	WeaponDryAudioComponent->SetupAttachment(WeaponStaticMesh);
	WeaponDryAudioComponent->SetRelativeLocation(FVector(80.f, 0.f, 8.f));
	WeaponAudioComponent->bOverrideAttenuation = true;

	WeaponReloadAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponReloadAudioComponent"));
	WeaponReloadAudioComponent->bAutoActivate = false;
	WeaponReloadAudioComponent->SetupAttachment(WeaponStaticMesh);
	WeaponReloadAudioComponent->SetRelativeLocation(FVector(80.f, 0.f, 8.f));
	WeaponAudioComponent->bOverrideAttenuation = true;
}

// Called when the game starts or when spawned
void AScifiWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponFireSoundCue->IsValidLowLevelFast())
		WeaponAudioComponent->SetSound(WeaponFireSoundCue);
	if (WeaponDryFireSoundCue->IsValidLowLevelFast())
		WeaponDryAudioComponent->SetSound(WeaponDryFireSoundCue);
	if (WeaponReloadSoundCue->IsValidLowLevelFast())
		WeaponReloadAudioComponent->SetSound(WeaponReloadSoundCue);

	AssaultGunHoleParticle->SetActive(false);
}

// Called every frame
void AScifiWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AScifiWeapon::WeaponFire(bool CanFire)
{
	if (CanFire)
	{
		AssaultGunHoleParticle->SetActive(true);

		if (!WeaponAudioComponent->IsPlaying())
			WeaponAudioComponent->Play();
	}
	else
	{
		WeaponAudioComponent->Stop();
		AssaultGunHoleParticle->SetActive(false);

		if (!WeaponDryAudioComponent->IsPlaying())
			WeaponDryAudioComponent->Play();
	}
}

void AScifiWeapon::WeaponStopFire()
{
	WeaponAudioComponent->Stop();
	AssaultGunHoleParticle->SetActive(false);
}

void AScifiWeapon::WeaponReload()
{
	WeaponReloadAudioComponent->Play();
}