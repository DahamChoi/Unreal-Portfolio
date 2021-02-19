// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelStreamingActor.h"
#include "Engine/TargetPoint.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "../Character/Enemy/SphereBotCharacter.h"
#include "../Character/Enemy/PlayerCharacterHandGunFormEnemy.h"
#include "../Character/Enemy/PlayerCharacterRifleFormEnemy.h"
#include "../Character/Enemy/SuperDuskCharacter.h"
#include "../Actor/PayloadActor.h"

// Sets default values
ALevelStreamingActor::ALevelStreamingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &ALevelStreamingActor::OnOverlapBegin);

	LevelSpawnEnemyNubmer = 50;
	SpawnDealy = 1.f;
	SpawnSphereRobot = true;

	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Music
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_BATTLE_MUSIC(
		TEXT("/Game/Demo/Sounds/BattleBackGroundMusic_Cue"));
	BattleMusicSoundCue = CUE_BATTLE_MUSIC.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_ANNOUNCE(
		TEXT("/Game/Demo/Sounds/Attack_commencing_Cue"));
	AnnounceSoundCue = CUE_ANNOUNCE.Object;

	BattleMusicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BattleMusicAudioComponent"));
	BattleMusicAudioComponent->bAutoActivate = false;
	BattleMusicAudioComponent->SetupAttachment(RootComponent);

	AnnounceAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AnnounceAudioComponent"));
	AnnounceAudioComponent->bAutoActivate = false;
	AnnounceAudioComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALevelStreamingActor::BeginPlay()
{
	Super::BeginPlay();

	if (BattleMusicSoundCue->IsValidLowLevelFast())
	{
		BattleMusicAudioComponent->SetSound(BattleMusicSoundCue);
	}

	if (AnnounceSoundCue->IsValidLowLevelFast())
	{
		AnnounceAudioComponent->SetSound(AnnounceSoundCue);
	}
	
	if (SpawnSphereRobot)
	{
		FFunctionStruct FunctionContainer;
		FunctionContainer.pFunc = &ALevelStreamingActor::SpawnSphereRoBotFunction;
		SpawnFunctionArray.Add(FunctionContainer);
	}

	if (SpawnSciFiSoldierTypeA)
	{
		FFunctionStruct FunctionContainer;
		FunctionContainer.pFunc = &ALevelStreamingActor::SpawnSciFiSoldierTypeAFunction;
		SpawnFunctionArray.Add(FunctionContainer);
	}

	if (SpawnSciFiSoldierTypeB)
	{
		FFunctionStruct FunctionContainer;
		FunctionContainer.pFunc = &ALevelStreamingActor::SpawnSciFiSoldierTypeBFunction;
		SpawnFunctionArray.Add(FunctionContainer);
	}

	if (SpawnSuperDusk)
	{
		FFunctionStruct FunctionContainer;
		FunctionContainer.pFunc = &ALevelStreamingActor::SpawnSuperDuskFunction;
		SpawnFunctionArray.Add(FunctionContainer);
	}
}

// Called every frame
void ALevelStreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (LevelTriggerOn)
	{
		DelayTimer += DeltaTime;
		if (DelayTimer > SpawnDealy)
		{
			DelayTimer = 0.f;

			if (SpawnCounter++ < LevelSpawnEnemyNubmer)
			{
				int RandomIndex = FMath::FRandRange(0, SpawnFunctionArray.Num());
				(this->*SpawnFunctionArray[RandomIndex].pFunc)();
			}
			else
			{
				BattleMusicAudioComponent->Stop();
				this->Destroy();
			}
		}
	}
}

void ALevelStreamingActor::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	APayloadActor* PayloadActor = Cast<APayloadActor>(OtherActor);
	if (PayloadActor)
	{
		LevelTriggerOn = true;
		AnnounceAudioComponent->Play();
		BattleMusicAudioComponent->Play();
	}
}

const FTransform& ALevelStreamingActor::GetSpawnTransfrom()
{
	int RandomSpawnIndex = FMath::RandRange(0, TargetArray.Num() - 1);
	return TargetArray[RandomSpawnIndex]->GetActorTransform();
}

void ALevelStreamingActor::SpawnSphereRoBotFunction()
{
	GetWorld()->SpawnActor<ASphereBotCharacter>(
		ASphereBotCharacter::StaticClass(), GetSpawnTransfrom(), SpawnParameters);
}

void ALevelStreamingActor::SpawnSciFiSoldierTypeAFunction()
{
	GetWorld()->SpawnActor<APlayerCharacterRifleFormEnemy>(
		APlayerCharacterRifleFormEnemy::StaticClass(), GetSpawnTransfrom(), SpawnParameters);
}

void ALevelStreamingActor::SpawnSciFiSoldierTypeBFunction()
{
	GetWorld()->SpawnActor<APlayerCharacterHandGunFormEnemy>(
		APlayerCharacterHandGunFormEnemy::StaticClass(), GetSpawnTransfrom(), SpawnParameters);
}

void ALevelStreamingActor::SpawnSuperDuskFunction()
{
	GetWorld()->SpawnActor<ASuperDuskCharacter>(
		ASuperDuskCharacter::StaticClass(), GetSpawnTransfrom(), SpawnParameters);
}

