// Fill out your copyright notice in the Description page of Project Settings.


#include "HandGunActor.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

// Sets default values
AHandGunActor::AHandGunActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Dummy Root
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// WeaponSkeletalMesh
	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
	WeaponStaticMesh->SetupAttachment(DummyRoot);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_FIRE_GUN(
		TEXT("/Game/TPS_HandGun/TPS_HandGun/Weapon/Mesh/SM_FireGun"));
	if (SM_FIRE_GUN.Succeeded())
		WeaponStaticMesh->SetStaticMesh(SM_FIRE_GUN.Object);

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_WEAPON_FIRE(
		TEXT("/Game/Demo/Sounds/ShotGunSound_Cue"));
	WeaponFireSoundCue = CUE_WEAPON_FIRE.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_WEAPON_RELOAD(
		TEXT("/Game/Demo/Sounds/Weapons/GrenadeLauncher/SCue_Launcher_Reload_Start"));
	WeaponReloadSoundCue = CUE_WEAPON_RELOAD.Object;

	WeaponAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponAudioComponent"));
	WeaponAudioComponent->bAutoActivate = false;
	WeaponAudioComponent->SetupAttachment(WeaponStaticMesh);
	WeaponAudioComponent->SetRelativeLocation(FVector(80.f, 0.f, 8.f));
	WeaponAudioComponent->bOverrideAttenuation = true;

	WeaponReloadAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponReloadAudioComponent"));
	WeaponReloadAudioComponent->bAutoActivate = false;
	WeaponReloadAudioComponent->SetupAttachment(WeaponStaticMesh);
	WeaponReloadAudioComponent->SetRelativeLocation(FVector(80.f, 0.f, 8.f));
	WeaponAudioComponent->bOverrideAttenuation = true;
}

// Called when the game starts or when spawned
void AHandGunActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponFireSoundCue->IsValidLowLevelFast())
		WeaponAudioComponent->SetSound(WeaponFireSoundCue);
	if (WeaponReloadSoundCue->IsValidLowLevelFast())
		WeaponReloadAudioComponent->SetSound(WeaponReloadSoundCue);
}

// Called every frame
void AHandGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHandGunActor::HandGunFire()
{
	WeaponAudioComponent->Play();
}

void AHandGunActor::HandGunReload()
{
	WeaponReloadAudioComponent->Play();
}

