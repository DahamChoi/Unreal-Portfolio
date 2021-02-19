// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

// Sets default values
ARocketProjectile::ARocketProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCompionent"));
	CollisionComponent->InitSphereRadius(5.f);
	CollisionComponent->SetCollisionProfileName("BlockAllDynamic");
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ARocketProjectile::OnHit);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->MaxSpeed = 2000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.1f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	RocketTrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RocketTrailParticle"));
	RocketTrailParticle->SetupAttachment(CollisionComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ROCKET_TRAIL(
		TEXT("/Game/Effects/ParticleSystems/Weapons/RocketLauncher/Muzzle/P_Launcher_proj"));
	if (PS_ROCKET_TRAIL.Succeeded())
		RocketTrailParticle->SetTemplate(PS_ROCKET_TRAIL.Object);

	RocketImpactParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RocketImpactParticle"));
	RocketImpactParticle->SetupAttachment(DummyRoot);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ROCKET_IMPACT(
		TEXT("/Game/Effects/ParticleSystems/Weapons/RocketLauncher/Impact/P_Launcher_IH"));
	if (PS_ROCKET_IMPACT.Succeeded())
		RocketImpactParticle->SetTemplate(PS_ROCKET_IMPACT.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_ROCKET_FIRE(
		TEXT("/Game/Demo/Sounds/Weapons/GrenadeLauncher/SCue_Launcher_Shoot"));
	RocketFireSoundCue = CUE_ROCKET_FIRE.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_ROCKET_IMPACT(
		TEXT("/Game/Demo/Sounds/Impacts/SCue_Impt_Launcher"));
	RocketImpactSoundCue = CUE_ROCKET_IMPACT.Object;

	RocketFireAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RocketFireAudioComponent"));
	RocketFireAudioComponent->bAutoActivate = false;
	RocketFireAudioComponent->SetupAttachment(RootComponent);
	RocketFireAudioComponent->SetRelativeLocation(FVector(80.f, 0.f, 8.f));
	RocketFireAudioComponent->bOverrideAttenuation = true;

	RocketImpactAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("RocketImpactAudioComponent"));
	RocketImpactAudioComponent->bAutoActivate = false;
	RocketImpactAudioComponent->SetupAttachment(CollisionComponent);
	RocketImpactAudioComponent->SetRelativeLocation(FVector(80.f, 0.f, 8.f));
	RocketImpactAudioComponent->bOverrideAttenuation = true;

	InitialLifeSpan = 10.f;
}

// Called when the game starts or when spawned
void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	RocketImpactParticle->Deactivate();

	if (RocketFireAudioComponent->IsValidLowLevelFast())
		RocketFireAudioComponent->SetSound(RocketFireSoundCue);
	if (RocketImpactAudioComponent->IsValidLowLevelFast())
		RocketImpactAudioComponent->SetSound(RocketImpactSoundCue);

	RocketFireAudioComponent->Play();
}

// Called every frame
void ARocketProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARocketProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	FTransform HitTransform = FTransform(
		UKismetMathLibrary::FindLookAtRotation((Hit.ImpactNormal + Hit.ImpactPoint), Hit.ImpactPoint),
		Hit.ImpactPoint);
	RocketImpactParticle->SetWorldTransform(HitTransform);
	RocketImpactParticle->SetActive(true);
	RocketTrailParticle->Deactivate();
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketImpactAudioComponent->Play();

	if (OtherActor != this)
	{
		if (OtherComponent->IsSimulatingPhysics())
		{
			OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
		}

		TArray<AActor*> ignoredActors;
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			50.f,
			Hit.ImpactPoint,
			400.f,
			UDamageType::StaticClass(),
			ignoredActors,
			this->GetOwner(),
			false,
			ECollisionChannel::ECC_WorldDynamic);
	}
}

