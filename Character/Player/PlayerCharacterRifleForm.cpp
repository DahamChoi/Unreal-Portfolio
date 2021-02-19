// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterRifleForm.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "../../Actor/ScifiWeapon.h"
#include "../../State/DemoPlayerState.h"
#include "../../Actor/BioticFieldActor.h"
#include "../../Projectile/RocketProjectile.h"
#include "../../Projectile/DemoProjectile.h"
#include "../../Controller/TeamAIController.h"

const float APlayerCharacterRifleForm::CROUCH_WALK_SPEED = 300.f;
const float APlayerCharacterRifleForm::MAX_HP = 200.f;
const float APlayerCharacterRifleForm::MAX_E_SKILL_TIME_REMAINING = 20.f;
const float APlayerCharacterRifleForm::MAX_F_SKILL_TIME_REMAINING = 8.f;
const int APlayerCharacterRifleForm::MAX_BULLET_COUNT = 30;

// Sets default values
APlayerCharacterRifleForm::APlayerCharacterRifleForm()
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
			"Class'/Game/SciFiSoldier/Animation/UE4ASP_HeroTPP_AnimBlueprint.UE4ASP_HeroTPP_AnimBlueprint_C'";
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
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_BIOTIC_FILED(
		TEXT("/Game/Demo/Sounds/BioticFiled_Cue"));
	BioticFieldSoundCue = CUE_BIOTIC_FILED.Object;

	BioticFieldAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BioticFieldAudioComponent"));
	BioticFieldAudioComponent->bAutoActivate = false;
	BioticFieldAudioComponent->SetupAttachment(RootComponent);
	BioticFieldAudioComponent->bOverrideAttenuation = true;

	// Projectile Class
	RocketProjectileClass = ARocketProjectile::StaticClass();
}

// Called when the game starts or when spawned
void APlayerCharacterRifleForm::BeginPlay()
{
	Super::BeginPlay();

	// Sound
	if (BioticFieldSoundCue->IsValidLowLevelFast())
		BioticFieldAudioComponent->SetSound(BioticFieldSoundCue);

	// Set Player State
	this->SetDefaultCharacterState(
		MAX_HP, PlayerName, MAX_BULLET_COUNT, MAX_E_SKILL_TIME_REMAINING, MAX_F_SKILL_TIME_REMAINING);
	
	// Spawn Weapon Actor And Attach To Mesh-WeponSocket
	Weapon = GetWorld()->SpawnActor<AScifiWeapon>();
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(GetMesh(), Rules, FName("WeaponSocket"));

	ScifiWeapon = Cast<AScifiWeapon>(Weapon);
}

// Called every frame
void APlayerCharacterRifleForm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// When Crouch + Move + Aim, Can't Fire
	if (GetVelocity().Size() >= 10.f && PCActionIsAim && PCActionIsCrouch)
		PCActionStopFire();

	// When Sprint, But MoveVelocity Is Small, Not Sprint State 
	if (PCActionIsSprint && GetVelocity().Size() <= 10.f)
		PCActionStopSprint();

	// Reload Animation End
	if (GetGameTimeSinceCreation() - LastReloadTime >= 1.05f)
		PCActionIsReload = false;

	// When Last Fire After 0.1sec And PCActionIsFire
	if ((GetGameTimeSinceCreation() - LastFireBulletTime > 0.1f) && PCActionIsFire)
	{
		// Bullet Count Is Bigger Than 0 Or IsUltimate
		int BulletCount = CurrentPlayerState->CurrentBulletCount;
		if (BulletCount > 0 || PCActionIsUltimate)
		{
			// Play Sound Can WeponFire
			ScifiWeapon->WeaponFire(true);

			// Not Ultimate Sub BulletCount In PlayerState
			if (!PCActionIsUltimate)
				CurrentPlayerState->UpdateBulletCount(BulletCount - 1);

			if (this->FireDemoProjectile())
			{
				// If Hit Enemy, Charage Ultimate Percent
				CurrentPlayerState->UpdateQSkillPercent(CurrentPlayerState->QSkillPercent + 1.f);
			}
		}
		else
		{
			// Play Sound Can't WeponFire
			ScifiWeapon->WeaponFire(false);

			// Stop Fire Action
			PCActionStopFire();
		}

		// Update LastFireTime
		LastFireBulletTime = GetGameTimeSinceCreation();
	}

	// Auto Aiming When Ultimate Time
	if (PCActionIsUltimate)
	{
		// Get All Character
		ACharacter* PendingTarget = nullptr;
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
							// Get More Less Length Target
							if (PendingTarget == nullptr) { PendingTarget = *Itr; continue; }
							if (Itr->GetDistanceTo(this) < PendingTarget->GetDistanceTo(this))
							{
								PendingTarget = *Itr;
							}
						}
					}
				}
			}
		}

		// If Find Another Less Length Target
		if (PendingTarget)
		{
			if (PendingTarget != CurrentPendingTarget)
			{
				UltimateLerpRotatorAlpha = 0.f;
				CurrentPendingTarget = PendingTarget;
			}
		}

		// If Have Current Target
		if (CurrentPendingTarget)
		{
			// Get TargetRotation
			FVector TargetLocation = CurrentPendingTarget->GetActorLocation();
			FVector OriginLocation = this->GetActorLocation();
			FVector LookVector = TargetLocation - OriginLocation;
			FRotator TargetRot = LookVector.Rotation();

			// If IsActionAim Have Less Pitch
			if (PCActionIsAim)
				TargetRot.Pitch -= 2.5f;

			// Lerp Rotate Movement
			UltimateLerpRotatorAlpha = FMath::Min(1.f, DeltaTime + UltimateLerpRotatorAlpha);
			GetController()->SetControlRotation(FMath::Lerp(GetController()->GetControlRotation(), TargetRot, UltimateLerpRotatorAlpha));

			// Is Target RagDoll Dead
			if (CurrentPendingTarget->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_None)
			{
				CurrentPendingTarget = nullptr;
			}
		}
	}
}

// Called to bind functionality to input
void APlayerCharacterRifleForm::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind Crouch Action
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacterRifleForm::PCActionCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacterRifleForm::PCActionStopCrouch);

	// Bind Fire Action
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacterRifleForm::PCActionFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacterRifleForm::PCActionStopFire);

	// Bind Reload Action
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacterRifleForm::PCActionReload);

	// Bind Skill Action
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &APlayerCharacterRifleForm::PCActionUltimate);
	PlayerInputComponent->BindAction("ESkill", IE_Pressed, this, &APlayerCharacterRifleForm::PCActionBioticFiled);
	PlayerInputComponent->BindAction("FSkill", IE_Pressed, this, &APlayerCharacterRifleForm::PCActionRocketLauncher);
}

void APlayerCharacterRifleForm::InitCharacterState()
{
	// Set Player State
	this->SetDefaultCharacterState(
		MAX_HP, PlayerName, MAX_BULLET_COUNT, MAX_E_SKILL_TIME_REMAINING, MAX_F_SKILL_TIME_REMAINING);
}

void APlayerCharacterRifleForm::PCActionCrouch()
{
	if (!IsDead)
	{
		PCActionIsCrouch = true;
		GetCharacterMovement()->MaxWalkSpeed = CROUCH_WALK_SPEED;
	}
}

void APlayerCharacterRifleForm::PCActionStopCrouch()
{
	PCActionIsCrouch = false;
	GetCharacterMovement()->MaxWalkSpeed = DEFAULT_WALK_SPEED;
}

void APlayerCharacterRifleForm::PCActionFire()
{
	if (!PCActionIsSprint && !GetMovementComponent()->IsFalling() && !PCActionIsReload && !IsDead)
	{
		if (CurrentPlayerState->CurrentBulletCount > 0 || PCActionIsUltimate)
		{
			ScifiWeapon->WeaponFire(true);
			PCActionIsFire = true;

			LastFireBulletTime = GetGameTimeSinceCreation();
		}
	}
}

void APlayerCharacterRifleForm::PCActionStopFire()
{
	PCActionIsFire = false;
	ScifiWeapon->WeaponStopFire();
}

void APlayerCharacterRifleForm::PCActionReload()
{
	if (!PCActionIsSprint && !GetMovementComponent()->IsFalling() && !PCActionIsReload && !IsDead)
	{
		PCActionIsFire = false;
		PCActionIsReload = true;

		CurrentPlayerState->UpdateBulletCount(MAX_BULLET_COUNT);
		ScifiWeapon->WeaponReload();

		LastReloadTime = GetGameTimeSinceCreation();
	}
}

void APlayerCharacterRifleForm::PCActionUltimate()
{
	if (!PCActionIsSprint && !GetMovementComponent()->IsFalling() && !PCActionIsReload && !IsDead && !PCActionIsUltimate)
	{
		if (FMath::IsNearlyEqual(CurrentPlayerState->QSkillPercent, 100.f))
		{
			UltimateAudioComponent->Play();

			PCActionIsUltimate = true;
			CurrentPlayerState->UpdateBulletCount(MAX_BULLET_COUNT);
			CurrentPendingTarget = nullptr;
			UltimateLerpRotatorAlpha = 0.f;

			OnUltimateActive.Broadcast();

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
				{
					CurrentPlayerState->UpdateQSkillPercent(0.f);
					PCActionIsUltimate = false;
				}), 8.f, false);
		}
	}
}

void APlayerCharacterRifleForm::PCActionBioticFiled()
{
	if (!PCActionIsSprint && !GetMovementComponent()->IsFalling() && !PCActionIsReload && !IsDead)
	{
		if (CurrentPlayerState->ESkillTimeRemaining <= 0.01f)
		{
			// Spawn BioticField
			FVector BioticFieldSpawnLocation = GetActorLocation();
			BioticFieldSpawnLocation.Z -= GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 5.f;
			GetWorld()->SpawnActor<ABioticFieldActor>(BioticFieldSpawnLocation, FRotator(0.f, 0.f, 0.f));

			// Sound
			BioticFieldAudioComponent->Play();

			// Set Remain Time Timer
			CurrentPlayerState->UpdateSkillTimeRemaining(
				MAX_E_SKILL_TIME_REMAINING, CurrentPlayerState->FSkillTimeRemaining
			);

			this->RunESkillTimer();
		}
	}
}

void APlayerCharacterRifleForm::PCActionRocketLauncher()
{
	if (!PCActionIsSprint && !GetMovementComponent()->IsFalling() && !PCActionIsReload && !IsDead)
	{
		if (CurrentPlayerState->FSkillTimeRemaining <= 0.01f)
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);

			FVector MuzzleLocation = Weapon->GetActorLocation() + FTransform(
				Weapon->GetActorRotation()).TransformPosition(FVector(100.f, 0.f, 8.f));
			FRotator MuzzleRotation = Weapon->GetActorRotation();

			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				ARocketProjectile* Projectile = World->SpawnActor<ARocketProjectile>(
					RocketProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				if (Projectile)
				{
					Projectile->SetOwner(this);
					Projectile->FireInDirection(MuzzleRotation.Vector());
				}
			}

			CurrentPlayerState->UpdateSkillTimeRemaining(
				CurrentPlayerState->ESkillTimeRemaining, MAX_F_SKILL_TIME_REMAINING
			);

			this->RunFSkillTimer();
		}
	}
}

void APlayerCharacterRifleForm::PCActionSprint()
{
	if (!PCActionIsReload && !PCActionIsCrouch)
	{
		Super::PCActionSprint();
	}
}

void APlayerCharacterRifleForm::PCActionJump()
{
	if (!PCActionIsReload && !PCActionIsCrouch)
	{
		Super::PCActionJump();
	}
}