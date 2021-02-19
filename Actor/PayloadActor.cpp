// Fill out your copyright notice in the Description page of Project Settings.


#include "PayloadActor.h"
#include "Components/BoxComponent.h"
#include "TrackActor.h"
#include "Components/SplineComponent.h"
#include "Engine/CollisionProfile.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "../Character/Player/PlayerCharacter.h"
#include "../State/DemoGameStateBase.h"
#include "../Character/CharacterBase.h"
#include "../Controller/TeamAIController.h"

// Sets default values
APayloadActor::APayloadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	PayloadSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PayloadSkeletalMesh"));
	PayloadSkeletalMesh->SetupAttachment(DummyRoot);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_PAYLOAD(TEXT("/Game/VTH/MESHES/VTH_SK"));
	if (SM_PAYLOAD.Succeeded())
	{
		// Set Mesh
		PayloadSkeletalMesh->SetSkeletalMesh(SM_PAYLOAD.Object);
		PayloadSkeletalMesh->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
		
		// Set AnimBP
		PayloadSkeletalMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		// get the blueprint class reference from the editor
		FString AnimClassStringTest = "Class'/Game/VTH/Anim/VTH_ABP.VTH_ABP_C'";

		// load the class
		UClass* AnimationClass = LoadObject<UClass>(NULL, *AnimClassStringTest);
		if (AnimationClass)
		{
			PayloadSkeletalMesh->SetAnimClass(AnimationClass);
		}
	}

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetBoxExtent(FVector(700.f, 700.f, 400.f));
	
	FScriptDelegate DelegateBegin;
	DelegateBegin.BindUFunction(this, "OnOverlapBegin");
	BoxCollision->OnComponentBeginOverlap.Add(DelegateBegin);

	FScriptDelegate DelegateEnd;
	DelegateEnd.BindUFunction(this, "OnOverlapEnd");
	BoxCollision->OnComponentEndOverlap.Add(DelegateEnd);

	BoxCollision->SetupAttachment(DummyRoot);

	Alpha = 0.f;
	Duration = 180.f;
	Forward = 0.f;

	// sound
	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_ANNOUNCE_1(
		TEXT("/Game/Demo/Sounds/Victory_Cue"));
	AnnounceCue1 = CUE_ANNOUNCE_1.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CUE_ANNOUNCE_2(
		TEXT("/Game/Demo/Sounds/The_payload_is_approaching_it_s_destination_Cue"));
	AnnounceCue2 = CUE_ANNOUNCE_2.Object;

	AnnounceAudioComponent1 = CreateDefaultSubobject<UAudioComponent>(TEXT("AnnounceAudioComponent1"));
	AnnounceAudioComponent1->bAutoActivate = false;
	AnnounceAudioComponent1->SetupAttachment(RootComponent);

	AnnounceAudioComponent2 = CreateDefaultSubobject<UAudioComponent>(TEXT("AnnounceAudioComponent2"));
	AnnounceAudioComponent2->bAutoActivate = false;
	AnnounceAudioComponent2->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APayloadActor::BeginPlay()
{
	Super::BeginPlay();

	if (AnnounceCue1->IsValidLowLevelFast())
	{
		AnnounceAudioComponent1->SetSound(AnnounceCue1);
	}

	if (AnnounceCue2->IsValidLowLevelFast())
	{
		AnnounceAudioComponent2->SetSound(AnnounceCue2);
	}

	if (Track)
	{
		FVector NewLocation =
			Track->UsefulTrack->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
		FRotator NewRotator =
			Track->UsefulTrack->GetRotationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
		NewRotator.Yaw -= 90.f;
		this->SetActorLocationAndRotation(NewLocation, FRotator(0.f,NewRotator.Yaw,0.f));
	}
}

// Called every frame
void APayloadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Alpha > 0.999f && !AnnounceChecker1)
	{
		AnnounceAudioComponent1->Play();
		AnnounceChecker1 = true;
	}

	if (Alpha > 0.9f && !AnnounceChecker2)
	{
		AnnounceAudioComponent2->Play();
		AnnounceChecker2 = true;
	}

	if (CanMove && Track)
	{
		// Set New Location And Roatation
		float SplineLength = Track->UsefulTrack->GetSplineLength();
		float LerpFloat = FMath::Lerp(0.f, SplineLength, Alpha);
		FVector NewLocation =
			Track->UsefulTrack->GetLocationAtDistanceAlongSpline(LerpFloat, ESplineCoordinateSpace::World);
		FRotator NewRotator =
			Track->UsefulTrack->GetRotationAtDistanceAlongSpline(LerpFloat, ESplineCoordinateSpace::World);
		NewRotator.Yaw -= 90.f;
		this->SetActorLocationAndRotation(NewLocation, FRotator(0.f, NewRotator.Yaw, 0.f));

		// Update Alpha
		Alpha += (DeltaTime / Duration);
		Forward = FMath::Min(1.f, Forward + DeltaTime);
	}

	if (!CanMove)
	{
		Forward = FMath::Max(0.f, Forward - DeltaTime);
	}

	for (int i = 0; i < InEnemyArray.Num(); i++)
	{
		if (InEnemyArray[i]->IsDead)
		{
			InEnemyArray.RemoveSingle(InEnemyArray[i]);
			--EnemyInCount;
			break;
		}
	}

	SetCanMovePayload();
}

void APayloadActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* overlap = Cast<ACharacter>(OtherActor);
	if (overlap)
	{
		ATeamAIController* TeamAIController = overlap->GetController<ATeamAIController>();
		if (overlap->IsPlayerControlled() || TeamAIController)
		{
			++OurPlayerInCount;
		}
		else
		{
			ACharacterBase* CharacterBase = Cast<ACharacterBase>(OtherActor);
			if (CharacterBase)
			{
				InEnemyArray.Add(CharacterBase);
				++EnemyInCount;
			}
		}
	}
}

void APayloadActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* overlap = Cast<ACharacter>(OtherActor);
	if (overlap)
	{
		ATeamAIController* TeamAIController = overlap->GetController<ATeamAIController>();
		if (overlap->IsPlayerControlled() || TeamAIController)
		{
			++OurPlayerInCount;
		}
		else
		{
			ACharacterBase* CharacterBase = Cast<ACharacterBase>(OtherActor);
			if (CharacterBase)
			{
				if (!CharacterBase->IsDead)
				{
					--EnemyInCount;
					InEnemyArray.Remove(CharacterBase);
				}
			}
		}
	}
}

void APayloadActor::SetCanMovePayload()
{
	if (OurPlayerInCount >= 1 && EnemyInCount <= 0)
		this->CanMove = true;
	else
		this->CanMove = false;

	ADemoGameStateBase* gameState = GetWorld()->GetGameState<ADemoGameStateBase>();
	if (this->CanMove)
		gameState->UpdatePayloadState(EPayloadStateEnum::PSE_MOVE, Alpha);
	else if (EnemyInCount > 0)
		gameState->UpdatePayloadState(EPayloadStateEnum::PSE_FIGHTING, Alpha);
	else
		gameState->UpdatePayloadState(EPayloadStateEnum::PSE_STOP, Alpha);
}

