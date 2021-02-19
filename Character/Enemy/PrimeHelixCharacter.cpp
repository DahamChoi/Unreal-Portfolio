// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimeHelixCharacter.h"
#include "../../Controller/PrimeHelixAIController.h"
#include "../../State/DemoPlayerState.h"
#include "../../Controller/BaseAIController.h"
#include "../../AI/Common/BTService_Common_TargetDetecter.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

const float APrimeHelixCharacter::MAX_HP = 400.f;

APrimeHelixCharacter::APrimeHelixCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// AIController Set
	AIControllerClass = APrimeHelixAIController::StaticClass();

	// Capsule Init
	GetCapsuleComponent()->InitCapsuleSize(200.f, 100.0f);

	// User Widget
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(
		TEXT("/Game/Demo/UI/HPBar_PrimeHelix"));
	if (UI_HUD.Succeeded())
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 260.f));
	HPBarWidget->SetDrawSize(FVector2D(300.f, 30.f));

	// Init Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_PRIME_HELIX(
		TEXT("/Game/ParagonMinions/Characters/Minions/Prime_Helix/Meshes/Prime_Helix"));
	if (SM_PRIME_HELIX.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM_PRIME_HELIX.Object);
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		
		FString AnimClassStringTest = 
			"Class'/Game/ParagonMinions/Characters/Minions/Prime_Helix/Animations/Prime_Helix_AnimBlueprint.Prime_Helix_AnimBlueprint_C'";
		UClass* AnimationClass = LoadObject<UClass>(NULL, *AnimClassStringTest);
		if (AnimationClass)
		{
			GetMesh()->SetAnimInstanceClass(AnimationClass);
		}

		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -200.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetCollisionProfileName("BlockAllDynamic");
	}

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_DIE(
		TEXT("/Game/Demo/Sounds/Prime_Helix_Dead_Cue"));
	DieSoundCue = CUE_DIE.Object;

	DieAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DieAudioComponent"));
	DieAudioComponent->bAutoActivate = false;
	DieAudioComponent->SetupAttachment(RootComponent);
	DieAudioComponent->bOverrideAttenuation = true;

	// Lerp Movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
}

void APrimeHelixCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set HP State
	this->SetDefaultCharacterState(MAX_HP, TEXT("PRIME HELIX"));
}

void APrimeHelixCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APrimeHelixCharacter::AttackHeadOn()
{
	if (!IsDead)
	{
		IsAttack = true;
		
		ABaseAIController* AIController = GetController<ABaseAIController>();
		float TargetLength = AIController->GetBlackboardComponent()->GetValueAsFloat(
			UBTService_Common_TargetDetecter::TARGET_LENGTH_KEY);

		// Far Attack
		if (TargetLength > 700.f)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					FHitResult HitResult;
					FCollisionQueryParams Params(NAME_None, false, this);
					bool bResult = GetWorld()->SweepSingleByChannel(
						HitResult,
						GetActorLocation(),
						GetActorLocation() + GetActorForwardVector() * 1000.f,
						FQuat::Identity,
						ECollisionChannel::ECC_GameTraceChannel1,
						FCollisionShape::MakeSphere(1000.f),
						Params
					);

					/*
					DrawDebugSphere(
						GetWorld(), GetActorLocation(), 1000.f, 30, FColor::Red, true);
					*/

					if (bResult)
					{
						if (HitResult.Actor.IsValid())
						{
							FDamageEvent DamageEvent;
							HitResult.Actor->TakeDamage(75.f, DamageEvent, GetController(), this);
						}
					}
				}), 1.5f, false);
		}
		// Near Attack
		else
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					FHitResult HitResult;
					FCollisionQueryParams Params(NAME_None, false, this);
					bool bResult = GetWorld()->SweepSingleByChannel(
						HitResult,
						GetActorLocation() + GetActorForwardVector() * 100.f,
						GetActorLocation() + GetActorForwardVector() * 500.f,
						FQuat::Identity,
						ECollisionChannel::ECC_GameTraceChannel1,
						FCollisionShape::MakeSphere(200.f),
						Params
					);
					
					/*
					DrawDebugSphere(
						GetWorld(), GetActorLocation() + GetActorForwardVector() * 300.f, 200.f, 30, FColor::Red, true);
					*/

					if (bResult)
					{
						if (HitResult.Actor.IsValid())
						{
							FDamageEvent DamageEvent;
							HitResult.Actor->TakeDamage(75.f, DamageEvent, GetController(), this);
						}
					}
				}), 1.f, false);
		}
	}
}

void APrimeHelixCharacter::OnEndAttackFunction()
{
	IsAttack = false;
	OnEndAttack.Broadcast();
}
