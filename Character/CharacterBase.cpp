// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "../UI/HPBarWidget.h"
#include "Player/PlayerCharacter.h"
#include "../State/DemoPlayerState.h"
#include "../State/DemoGameStateBase.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Auto PossessAI
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyPawn"));

	// User Widget
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetupAttachment(RootComponent);
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarWidget->SetDrawSize(FVector2D(150.f, 30.f));
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	// AI Source
	AIStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionSource"));
	AIStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_TAKE_DOWN(
		TEXT("/Game/Demo/Sounds/EnemyDown_Cue"));
	TakeDownSoundCue = CUE_TAKE_DOWN.Object;

	TakeDownAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TakeDownAudioComponent"));
	TakeDownAudioComponent->bAutoActivate = false;

	// On Dead Delegate
	OnDead.AddUObject(this, &ACharacterBase::OnDeadFunction);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Auto Register
	AIStimuliSource->RegisterWithPerceptionSystem();

	// Get Current Player State
	CurrentPlayerState = GetController()->GetPlayerState<ADemoPlayerState>();

	// Get Widget Object
	HPBarWidgetObject = Cast<UHPBarWidget>(HPBarWidget->GetUserWidgetObject());

	// Sound
	if (TakeDownSoundCue->IsValidLowLevelFast())
		TakeDownAudioComponent->SetSound(TakeDownSoundCue);

	if (DieSoundCue->IsValidLowLevelFast())
		DieAudioComponent->SetSound(DieSoundCue);
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Update Hp
	CurrentPlayerState->UpdateHpState(
		CurrentPlayerState->CurrentHp - DamageAmount);

	// Update Hp Widget
	if (HPBarWidgetObject)
		HPBarWidgetObject->UpdateHPWidget(CurrentPlayerState->CurrentHp);

	// Is Dead?
	if (CurrentPlayerState->CurrentHp <= 0.001f)
	{
		if (!IsDead)
		{
			// Dead Event
			IsDead = true;

			RagDollDead();
			OnDead.Broadcast();

			// Disable Hp Widget
			GetWorld()->GetTimerManager().SetTimer(DisableHpWidgetTimer, FTimerDelegate::CreateLambda([&]()
				{
					UHPBarWidget* Widget = Cast<UHPBarWidget>(HPBarWidget->GetUserWidgetObject());
					if (Widget)
						Widget->DisableHPBarWidget();
				}), 2.5f, false);

			// Destory Actor
			GetWorld()->GetTimerManager().SetTimer(DestoryActorTimer, FTimerDelegate::CreateLambda([&]()
				{
					this->Destroy();
				}), 7.f, false);

			// Dead Event Delegate To Main Widget
			if (DamageCauser)
			{
				ACharacterBase* CharacterBase = Cast<ACharacterBase>(DamageCauser);
				if (CharacterBase)
				{
					APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(CharacterBase);

					ADemoPlayerState* DemoPlayerState = CharacterBase->GetPlayerState<ADemoPlayerState>();
					ADemoGameStateBase* DemoGameState = GetWorld()->GetGameState<ADemoGameStateBase>();

					FString KillerName = DemoPlayerState->PlayerName;
					FString KilledName = CurrentPlayerState->PlayerName;

					// Update Player Widget
					if (PlayerCharacter)
					{
						if (PlayerCharacter->GetController()->IsPlayerController())
						{
							PlayerCharacter->OnKillEnemy.Broadcast(KillerName, KilledName);
						
							// Sound
							TakeDownAudioComponent->Play();
						}
					}

					// Update Game State
					DemoGameState->AddKillLog(KillerName, KilledName);
				}
			}
			
		}

		return 0.f;
	}

	// AI Perception
	if (!GetController()->IsPlayerController() && DamageCauser)
	{
		UAISense_Damage::ReportDamageEvent(
			this, this, EventInstigator, FinalDamage, DamageCauser->GetActorLocation(), this->GetActorLocation());
	}

	return FinalDamage;
}

void ACharacterBase::OnDeadFunction()
{
	if (DieAudioComponent)
	{
		DieAudioComponent->Play();
	}
}

void ACharacterBase::SetDefaultCharacterState(float MaxHp, FString PlayerName, int MaxBulletCount, float MaxESkillTimeReamining, float MaxFSkillTimeRemaining)
{
	// Init HP Bar Widget
	if (CurrentPlayerState)
	{
		CurrentPlayerState->SetDemoPlayerName(PlayerName);
		CurrentPlayerState->SetDefaultState(MaxHp, MaxBulletCount, MaxESkillTimeReamining, MaxFSkillTimeRemaining);
		if (HPBarWidgetObject)
		{
			HPBarWidgetObject->InitHPBarWidget(CurrentPlayerState->MaxHp / 25.f);
			HPBarWidgetObject->SetNameText(PlayerName);
		}
	}
}

void ACharacterBase::RagDollDead()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetCharacterMovement()->DisableMovement();
}

