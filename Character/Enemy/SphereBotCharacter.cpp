// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereBotCharacter.h"
#include "../Player/PlayerCharacter.h"
#include "../../Controller/SphereBotAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "../../State/DemoPlayerState.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../UI/HPBarWidget.h"

const float ASphereBotCharacter::CAPSULE_RADIUS_BOT = 75.f;
const float ASphereBotCharacter::CAPSULE_RADIUS_SPHERE = 54.f;
const float ASphereBotCharacter::WALK_SPEED = 180.f;
const float ASphereBotCharacter::SPHERE_SPEED = 800.f;
const float ASphereBotCharacter::WALK_ACCELERATION = 1024.f;
const float ASphereBotCharacter::SPHERE_ACCELERATION = 2048.f;
const float ASphereBotCharacter::BRAKING_FRICTION_WALK = 0.3f;
const float ASphereBotCharacter::BRAKING_FRICTION_SPHERE = 0.01f;
const float ASphereBotCharacter::LASER_MAX_RANGE = 5000.f;
const float ASphereBotCharacter::LASER_ACCELERATION = 100.f;
const float ASphereBotCharacter::MAX_HEALTH = 100.f;
const float ASphereBotCharacter::SHOT_DELAY = 1.2f;
const float ASphereBotCharacter::IMPULSE = 5000.f;
const float ASphereBotCharacter::DAMAGE = 0.1f;
const FRotator ASphereBotCharacter::WALK_ROTATION_RATE = FRotator(0.f, 0.f, 300.f);
const FRotator ASphereBotCharacter::SPHERE_ROTATION_RATE = FRotator(0.f, 0.f, 700.f);

// Sets default values
ASphereBotCharacter::ASphereBotCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// AI Controller Set
	AIControllerClass = ASphereBotAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Init Capsule
	GetCapsuleComponent()->InitCapsuleSize(75.f, 75.f);

	// User Widget
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(
		TEXT("/Game/Demo/UI/HPBar"));
	if (UI_HUD.Succeeded())
		HPBarWidget->SetWidgetClass(UI_HUD.Class);

	// Init Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_SPHERE_BOT(
		TEXT("/Game/SphereRobot/Character/SphereRobot/Mesh/SK_SphereRobot"));
	if (SM_SPHERE_BOT.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM_SPHERE_BOT.Object);
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		FString AnimClassStringTest =
			"Class'/Game/SphereRobot/Character/SphereRobot/Animations/AnimBP/A_BP_SphereRobot_1.A_BP_SphereRobot_1_C'";
		UClass* AnimationClass = LoadObject<UClass>(NULL, *AnimClassStringTest);
		if (AnimationClass)
		{
			GetMesh()->SetAnimInstanceClass(AnimationClass);
		}
		
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -77.5));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetCollisionProfileName("BlockAllDynamic");
	}

	// Source Laser
	PS_SourceLaser = CreateDefaultSubobject<UParticleSystemComponent>(
		TEXT("PS_SourceLaser"));
	PS_SourceLaser->SetupAttachment(GetMesh(),TEXT("B_SB_LaserSocket"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_SOURCE_LASER(
		TEXT("/Game/SphereRobot/Particles/Red/PS_ImpactLaser_Red"));
	if (PS_SOURCE_LASER.Succeeded())
		PS_SourceLaser->SetTemplate(PS_SOURCE_LASER.Object);
	PS_SourceLaser->SetRelativeLocation(FVector(5.9771f, 0.5229f, 0.f));
	PS_SourceLaser->SetRelativeRotation(FRotator(0.f, 5.f, 0.f));

	// Source Laser Sparks
	PS_SourceLaserSparks = CreateDefaultSubobject<UParticleSystemComponent>(
		TEXT("PS_SourceLaserSparks"));
	PS_SourceLaserSparks->SetupAttachment(GetMesh(), TEXT("B_SB_LaserSocket"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_SOURCE_LASER_SPARKS(
		TEXT("/Game/SphereRobot/Particles/Red/PS_SourceLaserSparks_Red"));
	if (PS_SOURCE_LASER_SPARKS.Succeeded())
		PS_SourceLaserSparks->SetTemplate(PS_SOURCE_LASER_SPARKS.Object);
	PS_SourceLaserSparks->SetRelativeLocation(FVector(5.f, 0.f, 0.f));

	// Instance Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_LASER_RED(
		TEXT("/Game/SphereRobot/Particles/Red/PS_Laser_Red"));
	PS_LaserRed_Template = PS_LASER_RED.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_IMPACT_LASER_RED(
		TEXT("/Game/SphereRobot/Particles/Red/PS_ImpactLaser_Red"));
	PS_ImpactLaserRed_Template = PS_IMPACT_LASER_RED.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_SPARKS_RED(
		TEXT("/Game/SphereRobot/Particles/Red/PS_Sparks_Red"));
	PS_SparksRed_Template = PS_SPARKS_RED.Object;

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_DIE(
		TEXT("/Game/Demo/Sounds/SphereBot_Dead_Cue"));
	DieSoundCue = CUE_DIE.Object;

	DieAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DieAudioComponent"));
	DieAudioComponent->bAutoActivate = false;
	DieAudioComponent->SetupAttachment(RootComponent);
	DieAudioComponent->bOverrideAttenuation = true;
}

// Called when the game starts or when spawned
void ASphereBotCharacter::BeginPlay()
{
	Super::BeginPlay();

	// HP SET
	this->SetDefaultCharacterState(MAX_HEALTH, TEXT("SPHERE BOT"));

	PS_SourceLaserSparks->Deactivate();
	PS_SourceLaser->SetVisibility(false);
}

// Called every frame
void ASphereBotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsDead && LaserShot)
	{
		LaserRange = FMath::Clamp<float>(LaserRange + LASER_ACCELERATION, 0.f, LASER_MAX_RANGE);
		
		FTransform Transform = GetMesh()->GetSocketTransform("B_SB_LaserSocket");
		FVector Start = Transform.GetLocation();
		FVector End = Start + (GetCapsuleComponent()->GetForwardVector() * LaserRange);
		
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		bool bResult = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldDynamic,CollisionParams);
		FTransform HitTransform = FTransform(
			UKismetMathLibrary::FindLookAtRotation((HitResult.ImpactNormal + HitResult.ImpactPoint), HitResult.ImpactPoint),
			HitResult.ImpactPoint);
		
		PS_LaserRed->SetBeamSourcePoint(0, Transform.GetLocation(), 0);
		PS_ImpactLaserRed->SetWorldTransform(HitTransform);
		PS_SparksRed->SetWorldTransform(HitTransform);

		if (bResult)
		{
			PS_LaserRed->SetBeamTargetPoint(0,HitResult.Location,0);
			PS_ImpactLaserRed->SetVisibility(true);
			PS_SparksRed->SetVisibility(true);

			ACharacter* Character = Cast<ACharacter>(HitResult.Actor);
			if (Character)
			{
				FDamageEvent DamageEvent;
				Character->TakeDamage(DAMAGE, DamageEvent, GetOwner()->GetInstigatorController(), this);
			}
			else
			{
				if(HitResult.Component->IsSimulatingPhysics())
					HitResult.Component->AddImpulseAtLocation(GetCapsuleComponent()->GetForwardVector() * IMPULSE, HitResult.Location);
			}
		}
		else
		{
			PS_LaserRed->SetBeamTargetPoint(0, HitResult.TraceEnd, 0);
			PS_ImpactLaserRed->SetVisibility(false);
			PS_SparksRed->SetVisibility(false);
		}
	}
}

void ASphereBotCharacter::OnDeadFunction()
{
	Super::OnDeadFunction();

	StopShooting();
}

void ASphereBotCharacter::Transformation()
{
	if (!IsDead && !LaserShotStart && !TransformationInProgress &&
		!GetCharacterMovement()->IsFalling())
	{
		if (TransformationEnable)
		{
			TransformationOff();
		}
		else
		{
			TransformationOn();
		}
	}
}

void ASphereBotCharacter::TransformationOn()
{
	TransformationInProgress = true;
	TransformationAnimation = true;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			Super::Jump();
			TransformationRotation = false;

			GetCharacterMovement()->MaxWalkSpeed = SPHERE_SPEED;
			GetCharacterMovement()->MaxAcceleration = SPHERE_ACCELERATION;
			GetCharacterMovement()->BrakingFrictionFactor = BRAKING_FRICTION_SPHERE;
			GetCharacterMovement()->RotationRate = SPHERE_ROTATION_RATE;
			GetCapsuleComponent()->SetCapsuleRadius(CAPSULE_RADIUS_SPHERE);
			GetCapsuleComponent()->SetCapsuleHalfHeight(CAPSULE_RADIUS_SPHERE);

			TransformationEnable = true;
			TransformationInProgress = false;

			OnTransformationEnd.Broadcast();

		}), 0.35f, false);
}

void ASphereBotCharacter::TransformationOff()
{
	TransformationInProgress = true;
	TransformationRotation = false;

	Super::Jump();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			Super::Jump();
			TransformationRotation = false;

			GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;
			GetCharacterMovement()->MaxAcceleration = WALK_ACCELERATION;
			GetCharacterMovement()->BrakingFrictionFactor = BRAKING_FRICTION_WALK;
			GetCharacterMovement()->RotationRate = WALK_ROTATION_RATE;
			GetCapsuleComponent()->SetCapsuleRadius(CAPSULE_RADIUS_BOT);
			GetCapsuleComponent()->SetCapsuleHalfHeight(CAPSULE_RADIUS_BOT);

			TransformationAnimation = false;
			TransformationEnable = false;
			TransformationInProgress = false;

			OnTransformationEnd.Broadcast();

		}), 0.1f, false);
}

void ASphereBotCharacter::StartShooting()
{
	if (!IsDead && !TransformationAnimation && !GetCharacterMovement()->IsFalling())
	{
		if (FMath::IsNearlyEqual(ShotTimer, 0) || 
			ShotTimer + SHOT_DELAY < GetGameTimeSinceCreation())
		{
			ShotTimer = GetGameTimeSinceCreation();

			if (!LaserShot)
			{
				LaserShotStart = true;
				PS_SourceLaserSparks->SetActive(true);
				
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						if (LaserShotStart)
						{
							LaserShot = true;
							LaserRange = 0.f;
							PS_SourceLaser->SetVisibility(true);

							FTransform SpawnTransform =
								GetMesh()->GetSocketTransform("B_SB_LaserSocket");

							PS_LaserRed = UGameplayStatics::SpawnEmitterAtLocation(
								GetWorld(), PS_LaserRed_Template, SpawnTransform);
							PS_ImpactLaserRed = UGameplayStatics::SpawnEmitterAtLocation(
								GetWorld(), PS_ImpactLaserRed_Template, SpawnTransform.GetLocation());
							PS_SparksRed = UGameplayStatics::SpawnEmitterAtLocation(
								GetWorld(), PS_SparksRed_Template, SpawnTransform.GetLocation());
						}
					}), 0.6f, false);
			}
		}
	}
}

void ASphereBotCharacter::StopShooting()
{
	LaserShotStart = false;
	PS_SourceLaserSparks->Deactivate();
	PS_SourceLaser->SetVisibility(false);

	if (PS_LaserRed)
		PS_LaserRed->DestroyComponent();
	if (PS_ImpactLaserRed)
		PS_ImpactLaserRed->DestroyComponent();
	if (PS_SparksRed)
		PS_SparksRed->DestroyComponent();

	LaserShot = false;
}