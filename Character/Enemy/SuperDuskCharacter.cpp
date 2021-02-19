// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperDuskCharacter.h"
#include "../../Controller/SuperDuskAIController.h"
#include "../../State/DemoPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "DrawDebugHelpers.h"

const float ASuperDuskCharacter::MAX_HP = 200.f;

ASuperDuskCharacter::ASuperDuskCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// AIController Set
	AIControllerClass = ASuperDuskAIController::StaticClass();

	// Capsule Init
	GetCapsuleComponent()->InitCapsuleSize(150.f, 60.0f);

	// User Widget
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(
		TEXT("/Game/Demo/UI/HPBar_SuperDusk"));
	if (UI_HUD.Succeeded())
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 225.f));

	// Init Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_SUPER_DUSK(
		TEXT("/Game/ParagonMinions/Characters/Minions/Dusk_Minions/Meshes/Minion_Lane_Melee_Core_Dusk"));
	if (SM_SUPER_DUSK.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM_SUPER_DUSK.Object);
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		FString AnimClassStringTest =
			"Class'/Game/ParagonMinions/Characters/Minions/Down_Minions/Animations/Melee/SuperDuskAnimBlueprint.SuperDuskAnimBlueprint_C'";
		UClass* AnimationClass = LoadObject<UClass>(NULL, *AnimClassStringTest);
		if (AnimationClass)
		{
			GetMesh()->SetAnimInstanceClass(AnimationClass);
		}

		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -150.f));
		GetMesh()->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetCollisionProfileName("BlockAllDynamic");
	}

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_DIE(
		TEXT("/Game/Demo/Sounds/SuperDuskDie_Cue"));
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

void ASuperDuskCharacter::AttackHeadOn()
{
	if (!IsDead)
	{
		IsAttack = true;
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				AttackTakeDamage();
			}), 0.5f, false);

		FTimerHandle TimerHandle2;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle2, FTimerDelegate::CreateLambda([&]()
			{
				AttackTakeDamage();
			}), 1.5f, false);
	}
}

void ASuperDuskCharacter::OnEndAttackFunction()
{
	IsAttack = false;
	OnEndAttack.Broadcast();
}

void ASuperDuskCharacter::AttackTakeDamage()
{
	if (IsAttack)
	{
		FHitResult HitResult;
		FCollisionQueryParams Params(NAME_None, false, this);
		bool bResult = GetWorld()->SweepSingleByChannel(
			HitResult,
			GetActorLocation() + GetActorForwardVector() * 200.f,
			GetActorLocation() + GetActorForwardVector() * 300.f,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(100.f),
			Params
		);

		if (bResult)
		{
			if (HitResult.Actor.IsValid())
			{
				FDamageEvent DamageEvent;
				HitResult.Actor->TakeDamage(25.f, DamageEvent, GetController(), this);
			}
		}
	}
}

void ASuperDuskCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Player State
	this->SetDefaultCharacterState(MAX_HP, TEXT("SUPER DUSK"));
}

void ASuperDuskCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
