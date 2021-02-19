// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "../../State/DemoPlayerState.h"
#include "../../UI/HPBarWidget.h"
#include "../../Projectile/DemoProjectile.h"
#include "../../Actor/WeaponBaseActor.h"
#include "../../Controller/DemoPlayerController.h"
#include "../../Controller/TeamAIController.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GameFramework/GameModeBase.h"
#include "DrawDebugHelpers.h"
#include "PlayerCharacterHandGunForm.h"
#include "PlayerCharacterRifleForm.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/GameplayStatics.h"

const FVector APlayerCharacter::HIP_CAMERA_LOCATION = FVector(150.f, 80.f, 70.f);
const FVector APlayerCharacter::AIM_CAMERA_LOCATION = FVector(200.f, 90.f, 70.f);

const float APlayerCharacter::DEFAULT_WALK_SPEED = 600.f;
const float APlayerCharacter::SPRINT_WALK_SPEED = 1200.f;

APlayerCharacter::APlayerCharacter()
{
	// Tick Every Time
	PrimaryActorTick.bCanEverTick = true;

	// TPS Game Setting
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->SetWalkableFloorAngle(70.f);

	// Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->SetRelativeLocation(HIP_CAMERA_LOCATION);
	FollowCamera->bUsePawnControlRotation = false;

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailParticle"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ASSAULT_TRAIL(
		TEXT("/Game/Effects/ParticleSystems/Weapons/AssaultRifle/Muzzle/P_AssaultRifle_Trail_2"));
	if (PS_ASSAULT_TRAIL.Succeeded())
		TrailParticle->SetTemplate(PS_ASSAULT_TRAIL.Object);

	// Hp Bar Widget
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(
		TEXT("/Game/Demo/UI/HPBar_TeamPlayer"));
	if (UI_HUD.Succeeded())
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 105.f));

	// Load Aim Camera Float Curve
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveF(TEXT("/Game/Demo/Curve/CameraCurve"));
	if (CurveF.Succeeded())
		CurveFloat = CurveF.Object;

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_DIE(
		TEXT("/Game/Demo/Sounds/solider_dead_Cue"));
	DieSoundCue = CUE_DIE.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_FOOT_STEP(
		TEXT("/Game/Demo/Sounds/Footsteps/SCue_FS_Metal"));
	FootStepSoundCue = CUE_FOOT_STEP.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_ULTIMATE(
		TEXT("/Game/Demo/Sounds/Ultimate_Cue"));
	UltimateSoundCue = CUE_ULTIMATE.Object;

	DieAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DieAudioComponent"));
	DieAudioComponent->bAutoActivate = false;
	DieAudioComponent->SetupAttachment(RootComponent);
	DieAudioComponent->bOverrideAttenuation = true;

	FootStepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootStepAudioComponent"));
	FootStepAudioComponent->bAutoActivate = false;
	FootStepAudioComponent->SetupAttachment(RootComponent);
	FootStepAudioComponent->bOverrideAttenuation = true;

	UltimateAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("UltimateAudioComponent"));
	UltimateAudioComponent->bAutoActivate = false;
	UltimateAudioComponent->SetupAttachment(RootComponent);
	UltimateAudioComponent->bOverrideAttenuation = true;

	AIStimuliSource->RegisterForSense(UAISense_Damage::StaticClass());

	PlayerName = TEXT("DAHAMIMI");

	// Projectile Class
	ProjectileClass = ADemoProjectile::StaticClass();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	TrailParticle->SetActive(false);

	// Sound
	if (FootStepSoundCue->IsValidLowLevelFast())
	{
		FootStepAudioComponent->SetSound(FootStepSoundCue);
	}

	if (UltimateSoundCue->IsValidLowLevelFast())
	{
		UltimateAudioComponent->SetSound(UltimateSoundCue);
	}

	// Aiming Timeline Bind Function
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("HandleCurveProgress"));
	AimingTimeline = FTimeline();
	AimingTimeline.AddInterpFloat(CurveFloat, ProgressFunction);

	if (this->GetController())
	{
		if (GetController()->IsPlayerController())
		{
			// If Player Controlled Don't Visible
			HPBarWidget->SetVisibility(false);
		}
	}

	SpawnTransform = GetActorTransform();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->Velocity.Size() > 10.f && !GetCharacterMovement()->IsFalling())
	{
		if(GetGameTimeSinceCreation() - FootStepTimer > (PCActionIsSprint ? 0.2f : 0.4f))
		{
			FootStepAudioComponent->Play();
			FootStepTimer = GetGameTimeSinceCreation();
		}
	}
	else
	{
		if (FootStepAudioComponent->IsPlaying())
		{
			FootStepAudioComponent->Stop();
		}
	}

	// Track
	TrailParticle->SetWorldLocation(Weapon->GetActorLocation() + FTransform(
		Weapon->GetActorRotation()).TransformPosition(FVector(80.f, 0.f, 8.f)));
	TrailParticle->SetWorldRotation(Weapon->GetActorRotation());

	// Tick TimeLine
	AimingTimeline.TickTimeline(DeltaTime);

	if (this->GetController()->IsPlayerController())
	{
		// Update Cross Hair
		OnUpdateCrossHair.Broadcast(Get2DVectorByTraceWeapon());
	}

	// When Sprint, But MoveVelocity Is Small, Not Sprint State 
	if (PCActionIsSprint && GetVelocity().Size() <= 10.f)
		PCActionStopSprint();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	// Bind Jump Action
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::PCActionJump);

	// Bind Aim Action
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::PCActionAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::PCActionStopAim);

	// Bind Sprint Action
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::PCActionSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::PCActionStopSprint);

	// Bind Movement Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// Controll Rotations Axis
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Change Battle Form
	PlayerInputComponent->BindAction("ChangeBattleForm", IE_Released, this, &APlayerCharacter::PCActionChangeBattleForm);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentPlayerState->CurrentHp - DamageAmount <= 0.001f)
	{
		if (!IsDead)
		{
			if (GetController()->IsPlayerController() || GetController<ATeamAIController>())
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
					{
						IsDead = false;

						GetMesh()->SetSimulatePhysics(false);

						this->SetActorRelativeTransform(SpawnTransform);

						GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
						GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

						CurrentPlayerState->ResetPlayerState();
						HPBarWidgetObject->UpdateHPWidget(CurrentPlayerState->CurrentHp);
					}), 7.f, false);
			}
		}
	}

	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (GetController()->IsPlayerController() || GetController<ATeamAIController>())
	{
		GetWorld()->GetTimerManager().ClearTimer(DisableHpWidgetTimer);
		GetWorld()->GetTimerManager().ClearTimer(DestoryActorTimer);
	}

	OnTakeDamageActive.Broadcast();

	return FinalDamage;
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !IsDead)
	{
		// IsActionSprint True, But Value Is Less Than 0.f, Set IsActionSprite False
		if (PCActionIsSprint && Value < 0.f)
		{
			PCActionIsSprint = false;
			return;
		}

		// Get Movement Direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !IsDead)
	{
		// IsSprint Ture, Can't Move Right Left
		if (PCActionIsSprint)
			return;

		// Get Movement Direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

FVector2D APlayerCharacter::Get2DVectorByTraceWeapon()
{
	FVector MuzzleLocation = Weapon->GetActorLocation() + FTransform(
		Weapon->GetActorRotation()).TransformPosition(FVector(100.f, 0.f, 8.f));
	FRotator MuzzleRotation = Weapon->GetActorRotation();
	FVector End = MuzzleLocation + MuzzleRotation.Vector() * 8000.f;

	ADemoPlayerController* PlayerController = Cast<ADemoPlayerController>(GetController());
	
	FVector2D ScreenLocation;
	UGameplayStatics::ProjectWorldToScreen(PlayerController, End, ScreenLocation);

	return ScreenLocation;
}

void APlayerCharacter::PCActionChangeBattleForm()
{
	if (GetGameTimeSinceCreation() - LastChangeFormTime < 5.f)
		return;

	LastChangeFormTime = GetGameTimeSinceCreation();

	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	APlayerCharacter* NewCharacter;
	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (CurrentPlayerState->CurrentBattleForm == EBattleFormEnum::BFE_HAND_GUN)
	{
		NewCharacter = 
			GetWorld()->SpawnActor<APlayerCharacterRifleForm>(
				this->GetActorLocation(), this->GetActorRotation(), Parameters);
	}
	else
	{
		NewCharacter =
			GetWorld()->SpawnActor<APlayerCharacterHandGunForm>(
				this->GetActorLocation(), this->GetActorRotation(), Parameters);
	}

	CurrentPlayerState->ChangeBattleForm();

	GetController()->Possess(NewCharacter);
	NewCharacter->CurrentPlayerState = this->CurrentPlayerState;
	NewCharacter->InitCharacterState();
	NewCharacter->HPBarWidget->SetVisibility(false);

	this->UnPossessed();
	this->Weapon->Destroy();
	this->Destroy();
}

void APlayerCharacter::PCActionJump()
{
	// Jump Cool Time - 1.5 sec
	if (!PCActionIsSprint && !GetMovementComponent()->IsFalling() && GetGameTimeSinceCreation() - LastJumpTime > 1.5f
		&& !PCActionIsAim && !IsDead)
	{
		LastJumpTime = GetGameTimeSinceCreation();

		Super::Jump();
	}
}

void APlayerCharacter::PCActionAim()
{
	if (!PCActionIsAim && !GetMovementComponent()->IsFalling() && !IsDead)
	{
		PCActionIsAim = true;
		TrailParticle->SetActive(true);
		AimingTimeline.PlayFromStart();
	}
}

void APlayerCharacter::PCActionStopAim()
{
	if (PCActionIsAim)
	{
		PCActionIsAim = false;
		TrailParticle->SetActive(false);
		AimingTimeline.PlayFromStart();
	}
}

void APlayerCharacter::PCActionSprint()
{
	if (GetVelocity().Size() >= 10.f && !GetMovementComponent()->IsFalling() && !IsDead)
	{
		PCActionIsSprint = true;
		PCActionIsAim = false;	// Can't Aim When Sprint

		CurrentPlayerState->UpdatebSprint(true);
		GetCharacterMovement()->MaxWalkSpeed = SPRINT_WALK_SPEED;
	}
}

void APlayerCharacter::PCActionStopSprint()
{
	PCActionIsSprint = false;
	CurrentPlayerState->UpdatebSprint(false);
	GetCharacterMovement()->MaxWalkSpeed = DEFAULT_WALK_SPEED;
}

void APlayerCharacter::HandleCurveProgress(float value)
{
	FVector SFV = PCActionIsAim ? HIP_CAMERA_LOCATION : AIM_CAMERA_LOCATION;
	FVector EFV = PCActionIsAim ? AIM_CAMERA_LOCATION : HIP_CAMERA_LOCATION;
	FVector NewLocation = FMath::Lerp(SFV, EFV, value);
	FollowCamera->SetRelativeLocation(NewLocation);
}

bool APlayerCharacter::FireDemoProjectile(float DamageAmount)
{
	// Get Fire Location
	FVector MuzzleLocation = Weapon->GetActorLocation() + FTransform(
		Weapon->GetActorRotation()).TransformPosition(FVector(100.f, 0.f, 8.f));
	FRotator MuzzleRotation = Weapon->GetActorRotation();

	// Spawn DemoProjectile
	UWorld* World = GetWorld();
	if (World)
	{
		ADemoProjectile* Projectile = World->SpawnActor<ADemoProjectile>(
			ProjectileClass, MuzzleLocation, MuzzleRotation);
		Projectile->ProjectileDamageAmount = DamageAmount;
		if (Projectile)
		{
			// Set Projectile Owner
			Projectile->SetOwner(this);

			// Fire In Direction
			return (Projectile->FireInDirection(MuzzleLocation, MuzzleRotation));
		}
	}

	return false;
}

void APlayerCharacter::ESkillAdvanceTimer()
{
	if (CurrentPlayerState->ESkillTimeRemaining <= 0.01f)
	{
		GetWorldTimerManager().ClearTimer(ESkillTimerHandle);
	}

	CurrentPlayerState->UpdateSkillTimeRemaining(
		CurrentPlayerState->ESkillTimeRemaining - 0.1f, CurrentPlayerState->FSkillTimeRemaining
	);
}

void APlayerCharacter::FSkillAdvanceTimer()
{
	if (CurrentPlayerState->FSkillTimeRemaining <= 0.01f)
	{
		GetWorldTimerManager().ClearTimer(FSkillTimerHandle);
	}

	CurrentPlayerState->UpdateSkillTimeRemaining(
		CurrentPlayerState->ESkillTimeRemaining, CurrentPlayerState->FSkillTimeRemaining - 0.1f
	);
}

void APlayerCharacter::RunESkillTimer()
{
	GetWorldTimerManager().SetTimer(ESkillTimerHandle, this,
		&APlayerCharacter::ESkillAdvanceTimer, 0.1f, true);
}

void APlayerCharacter::RunFSkillTimer()
{
	GetWorldTimerManager().SetTimer(FSkillTimerHandle, this,
		&APlayerCharacter::FSkillAdvanceTimer, 0.1f, true);
}
