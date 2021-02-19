// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterHandGunForm.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "../../State/DemoPlayerState.h"
#include "../../Actor/HandGunActor.h"
#include "../../Projectile/DemoProjectile.h"
#include "../../Controller/TeamAIController.h"
#include "../../Controller/DemoPlayerController.h"

const float APlayerCharacterHandGunForm::RELOAD_ANIMATION_TIME = 2.3f;
const float APlayerCharacterHandGunForm::FIRE_ANIMATION_TIME = 0.65f;

const float APlayerCharacterHandGunForm::MAX_HP = 200.f;
const float APlayerCharacterHandGunForm::MAX_E_SKILL_TIME_REMAINING = 10.f;
const float APlayerCharacterHandGunForm::MAX_F_SKILL_TIME_REMAINING = 15.f;
const int APlayerCharacterHandGunForm::MAX_BULLET_COUNT = 8;

// Sets default values
APlayerCharacterHandGunForm::APlayerCharacterHandGunForm()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set CapsuleComponent Size
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Init Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_PC(
		TEXT("/Game/SciFiSoldier/Meshes/SK_ScifiSoldierUE4"));

	if (SM_PC.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM_PC.Object);

		FString AnimClassStringTest =
			"Class'/Game/TPS_HandGun/TPS_HandGun/Animation/HandGun_AnimBlueprint.HandGun_AnimBlueprint_C'";
		UClass* AnimationClass = LoadObject<UClass>(NULL, *AnimClassStringTest);
		if (AnimationClass)
		{
			GetMesh()->SetAnimInstanceClass(AnimationClass);
		}
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetCollisionProfileName("BlockAllDynamic");

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_DODGE(
		TEXT("/Game/Demo/Sounds/Dodge_Cue"));
	DodgeSoundCue = CUE_DODGE.Object;

	DodgeAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DodgeAudioComponent"));
	DodgeAudioComponent->bAutoActivate = false;
	DodgeAudioComponent->SetupAttachment(RootComponent);
	DodgeAudioComponent->bOverrideAttenuation = true;

	// Load Dodge Movement Float Curve
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveF(TEXT("/Game/Demo/Curve/DodgeCurve"));
	if (CurveF.Succeeded())
		DodgeCurveFloat = CurveF.Object;
}

// Called when the game starts or when spawned
void APlayerCharacterHandGunForm::BeginPlay()
{
	Super::BeginPlay();

	// Sound
	if (DodgeSoundCue->IsValidLowLevelFast())
		DodgeAudioComponent->SetSound(DodgeSoundCue);

	// Set Player State
	this->SetDefaultCharacterState(MAX_HP, PlayerName, MAX_BULLET_COUNT,
			MAX_E_SKILL_TIME_REMAINING, MAX_F_SKILL_TIME_REMAINING);
	
	// Spawn Weapon Actor And Attach To Mesh-WeponSocket
	Weapon = GetWorld()->SpawnActor<AHandGunActor>();
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(GetMesh(), Rules, FName("WeaponSocket_2"));

	HandGun = Cast<AHandGunActor>(Weapon);

	// Dodge Timeline Bind Function
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleDodgeCurveProgress"));
	DodgeTimeline = FTimeline();
	DodgeTimeline.AddInterpFloat(DodgeCurveFloat, ProgressFunction);
}

// Called every frame
void APlayerCharacterHandGunForm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DodgeTimeline.TickTimeline(DeltaTime);

	if (PCActionIsReload && GetGameTimeSinceCreation() - LastReloadTime > RELOAD_ANIMATION_TIME)
	{
		PCActionIsReload = false;
	}

	if (PCActionIsFire && GetGameTimeSinceCreation() - LastFireTime > FIRE_ANIMATION_TIME)
	{
		PCActionIsFire = false;
		TrailParticle->SetActive(true);
	}
}

// Called to bind functionality to input
void APlayerCharacterHandGunForm::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind Fire Action
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacterHandGunForm::PCActionFire);

	// Bind Reload Action
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacterHandGunForm::PCActionReload);

	// Bind Skill Action
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &APlayerCharacterHandGunForm::PCActionUltimate);
	PlayerInputComponent->BindAction("ESkill", IE_Pressed, this, &APlayerCharacterHandGunForm::PCActionDodge);
	PlayerInputComponent->BindAction("FSkill", IE_Pressed, this, &APlayerCharacterHandGunForm::PCActionRampage);
}

void APlayerCharacterHandGunForm::InitCharacterState()
{
	// Set Player State
	this->SetDefaultCharacterState(
		MAX_HP, PlayerName, MAX_BULLET_COUNT, MAX_E_SKILL_TIME_REMAINING, MAX_F_SKILL_TIME_REMAINING);
}

void APlayerCharacterHandGunForm::PCActionFire()
{
	if (PCActionIsUltimate)
	{
		PCActionIsUltimate = false;

		// Get All Character
		TArray<ACharacter*> TargetArray;
		for (TObjectIterator<ACharacter> Itr; Itr; ++Itr)
		{
			// Is Valid
			if (Itr)
			{
				if (Itr->GetController())
				{
					// Is Player Controlled
					if (!Itr->IsPlayerControlled() && (!Cast<ATeamAIController>(Itr->GetController())))
					{
						// Is RagDoll State
						if (!(Itr->GetCapsuleComponent()->GetCollisionEnabled() == ECollisionEnabled::NoCollision))
						{
							TargetArray.Add(*Itr);
						}
					}
				}
			}
		}

		for (int i = 0; i < TargetArray.Num(); i++)
		{
			// Get Fire Location
			FVector OriginLocation = this->GetActorLocation() + this->GetCapsuleComponent()->GetForwardVector() * 100.f;
			FVector TargetLocation = TargetArray[i]->GetActorLocation();
			FRotator MuzzleRotation = (TargetLocation - OriginLocation).Rotation();

			// Spawn DemoProjectile
			UWorld* World = GetWorld();
			if (World)
			{
				ADemoProjectile* Projectile = World->SpawnActor<ADemoProjectile>(
					ProjectileClass, OriginLocation, MuzzleRotation);
				Projectile->ProjectileDamageAmount = 999.f;
				if (Projectile)
				{
					// Set Projectile Owner
					Projectile->SetOwner(this);

					// Sound
					HandGun->HandGunFire();

					// Fire In Direction
					Projectile->FireInDirection(OriginLocation, MuzzleRotation);
				}
			}
		}
	}
	else if (!GetMovementComponent()->IsFalling() && !PCActionIsSprint && !PCActionIsDodge &&
		!PCActionIsFire && !PCActionIsReload && !PCActionIsRampage && !PCActionIsUltimate && PCActionIsAim &&
		CurrentPlayerState->CurrentBulletCount > 0)
	{
		// Bullet Count Is Bigger Than 0 Or IsUltimate
		int BulletCount = CurrentPlayerState->CurrentBulletCount;
		if (BulletCount > 0)
		{
			CurrentPlayerState->UpdateBulletCount(BulletCount - 1);
			HandGun->HandGunFire();

			if (this->FireDemoProjectile(50.f))
			{
				// If Hit Enemy, Charage Ultimate Percent
				CurrentPlayerState->UpdateQSkillPercent(CurrentPlayerState->QSkillPercent + 5.f);
			}
		}

		PCActionIsFire = true;
		LastFireTime = GetGameTimeSinceCreation();

		TrailParticle->SetActive(false);
	}
}

void APlayerCharacterHandGunForm::PCActionReload()
{
	if (!GetMovementComponent()->IsFalling() && !PCActionIsSprint && !PCActionIsDodge &&
		!PCActionIsFire && !PCActionIsReload && !PCActionIsRampage && !PCActionIsUltimate)
	{
		HandGun->HandGunReload();

		PCActionIsReload = true;
		LastReloadTime = GetGameTimeSinceCreation();

		// Update Player State
		CurrentPlayerState->UpdateBulletCount(CurrentPlayerState->MaxBulletCount);
	}
}

void APlayerCharacterHandGunForm::PCActionUltimate()
{
	if (!GetMovementComponent()->IsFalling() && !PCActionIsSprint && !PCActionIsDodge &&
		!PCActionIsFire && !PCActionIsReload && !PCActionIsAim && !PCActionIsUltimate && !PCActionIsRampage)
	{
		if (FMath::IsNearlyEqual(CurrentPlayerState->QSkillPercent, 100.f))
		{
			CurrentPlayerState->UpdateQSkillPercent(0.f);

			PCActionIsUltimate = true;
			UltimateAudioComponent->Play();
		}
	}
}

void APlayerCharacterHandGunForm::PCActionDodge()
{
	if (!GetMovementComponent()->IsFalling() && !PCActionIsSprint && !PCActionIsDodge &&
		!PCActionIsFire && !PCActionIsReload && !PCActionIsRampage && !PCActionIsUltimate)
	{
		if (CurrentPlayerState->ESkillTimeRemaining <= 0.01f)
		{
			// Sound
			DodgeAudioComponent->Play(0.3f);

			PCActionIsDodge = true;

			if (FMath::IsNearlyEqual(LastForwardValue, 0.f))
			{
				if (FMath::IsNearlyEqual(LastRightValue, 0.f))
				{
					PCActionDodgeDirection = EDodgeDirectionEnum::EDDE_FRONT;
				}
				else if (LastRightValue > 0.f)
				{
					PCActionDodgeDirection = EDodgeDirectionEnum::EDDE_RIGHT;
				}
				else
				{
					PCActionDodgeDirection = EDodgeDirectionEnum::EDDE_LEFT;
				}
			}
			else if (LastForwardValue > 0.f)
			{
				PCActionDodgeDirection = EDodgeDirectionEnum::EDDE_FRONT;
			}
			else
			{
				PCActionDodgeDirection = EDodgeDirectionEnum::EDDE_BACK;
			}

			DodgeTimeline.PlayFromStart();

			// Update Player State
			CurrentPlayerState->UpdateBulletCount(CurrentPlayerState->MaxBulletCount);

			CurrentPlayerState->UpdateSkillTimeRemaining(
				MAX_E_SKILL_TIME_REMAINING, CurrentPlayerState->FSkillTimeRemaining
			);

			this->RunESkillTimer();
		}
	}
}

void APlayerCharacterHandGunForm::PCActionRampage()
{
	if (!GetMovementComponent()->IsFalling() && !PCActionIsSprint && !PCActionIsDodge &&
		!PCActionIsFire && !PCActionIsReload && !PCActionIsRampage && !PCActionIsUltimate && PCActionIsAim &&
		CurrentPlayerState->CurrentBulletCount > 0)
	{
		PCActionIsRampage = true;

		CurrentPlayerState->UpdateSkillTimeRemaining(
			CurrentPlayerState->ESkillTimeRemaining, MAX_F_SKILL_TIME_REMAINING
		);

		this->RunFSkillTimer();

		GetWorld()->GetTimerManager().SetTimer(RampageTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				int BulletCount = CurrentPlayerState->CurrentBulletCount;
				if (BulletCount > 0)
				{
					CurrentPlayerState->UpdateBulletCount(BulletCount - 1);

					HandGun->HandGunFire();

					if (this->FireDemoProjectile(50.f))
					{
						// If Hit Enemy, Charage Ultimate Percent
						CurrentPlayerState->UpdateQSkillPercent(CurrentPlayerState->QSkillPercent + 5.f);
					}
				}
				else
				{
					PCActionIsRampage = false;
					GetWorld()->GetTimerManager().ClearTimer(RampageTimerHandle);
				}
			}), 0.333f, true);
	}
}

void APlayerCharacterHandGunForm::HandleDodgeCurveProgress(float value)
{
	if (PCActionIsDodge)
	{
		// Get Movement Direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		EAxis::Type AxisType = EAxis::Y;
		if (PCActionDodgeDirection == EDodgeDirectionEnum::EDDE_FRONT
			|| PCActionDodgeDirection == EDodgeDirectionEnum::EDDE_BACK)
		{
			AxisType = EAxis::X;
		}

		float InputValue = 20.f * value;
		if (PCActionDodgeDirection == EDodgeDirectionEnum::EDDE_LEFT
			|| PCActionDodgeDirection == EDodgeDirectionEnum::EDDE_BACK)
		{
			InputValue = -InputValue;
		}

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(AxisType);
		AddMovementInput(Direction, InputValue);
	}
}

void APlayerCharacterHandGunForm::MoveForward(float Value)
{
	if (!PCActionIsDodge && !PCActionIsUltimate)
	{
		LastForwardValue = Value;
		Super::MoveForward(Value);
	}
}

void APlayerCharacterHandGunForm::MoveRight(float Value)
{
	if (!PCActionIsDodge && !PCActionIsUltimate)
	{
		LastRightValue = Value;
		Super::MoveRight(Value);
	}
}

