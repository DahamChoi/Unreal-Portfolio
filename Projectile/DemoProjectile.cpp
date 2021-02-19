// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADemoProjectile::ADemoProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	AssaultTrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AssaulTrailParticle"));
	AssaultTrailParticle->SetupAttachment(DummyRoot);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ASSAULT_TRAIL(
		TEXT("/Game/Effects/ParticleSystems/Weapons/AssaultRifle/Muzzle/P_AssaultRifle_Trail"));
	if (PS_ASSAULT_TRAIL.Succeeded())
		AssaultTrailParticle->SetTemplate(PS_ASSAULT_TRAIL.Object);

	AssaultMFParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AssultMFParticle"));
	AssaultMFParticle->SetupAttachment(DummyRoot);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ASSAULT_MF(
		TEXT("/Game/Effects/ParticleSystems/Weapons/AssaultRifle/Impacts/P_AssaultRifle_IH"));
	if (PS_ASSAULT_MF.Succeeded())
		AssaultMFParticle->SetTemplate(PS_ASSAULT_MF.Object);

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_IMPACT_STATIC(
		TEXT("/Game/Demo/Sounds/Impacts/SCue_Impt_Bullet_Metal"));
	ImpactStaticSoundCue = CUE_IMPACT_STATIC.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_IMPACT_DYNAMIC(
		TEXT("/Game/Demo/Sounds/Impacts/SCue_Impt_Bullet_Default"));
	ImpactDynamicSoundCue = CUE_IMPACT_DYNAMIC.Object;

	ImpactStaticAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactStaticAudioComponent"));
	ImpactStaticAudioComponent->bAutoActivate = false;
	ImpactStaticAudioComponent->bOverrideAttenuation = true;

	ImpactDynamicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactDynamicAudioComponent"));
	ImpactDynamicAudioComponent->bAutoActivate = false;
	ImpactDynamicAudioComponent->bOverrideAttenuation = true;

	// Default State
	ProjectileDamageAmount = 10.f;
	InitialLifeSpan = 3.f;
}

// Called when the game starts or when spawned
void ADemoProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (ImpactStaticSoundCue->IsValidLowLevelFast())
		ImpactStaticAudioComponent->SetSound(ImpactStaticSoundCue);
	if (ImpactDynamicSoundCue->IsValidLowLevelFast())
		ImpactDynamicAudioComponent->SetSound(ImpactDynamicSoundCue);
}

// Called every frame
void ADemoProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ADemoProjectile::FireInDirection(const FVector& ShootStart, const FRotator& ShootRotation)
{	
	FVector End = ShootStart + (ShootRotation.Vector() * 8000.f);
	FHitResult HitResult;

	FRotator AssaultTrailRotation = ShootRotation;
	AssaultTrailParticle->SetWorldRotation(AssaultTrailRotation);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, ShootStart, End, ECollisionChannel::ECC_WorldDynamic))
	{
		FTransform HitTransform = FTransform(
			UKismetMathLibrary::FindLookAtRotation((HitResult.ImpactNormal + HitResult.ImpactPoint), HitResult.ImpactPoint),
			HitResult.ImpactPoint);
		AssaultMFParticle->SetWorldTransform(HitTransform);
		
		ACharacter* HitCharacter = Cast<ACharacter>(HitResult.Actor);
		if (HitCharacter)
		{
			ImpactDynamicAudioComponent->SetWorldLocation(HitResult.ImpactPoint);
			ImpactDynamicAudioComponent->Play();

			FDamageEvent DamageEvent;
			if (HitCharacter->TakeDamage(ProjectileDamageAmount, DamageEvent, GetOwner()->GetInstigatorController(), GetOwner()) > 0.01f)
				return true;
		}
		else
		{
			ImpactStaticAudioComponent->SetWorldLocation(HitResult.ImpactPoint);
			ImpactStaticAudioComponent->Play();
		}
	}

	return false;
}