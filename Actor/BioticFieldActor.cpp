// Fill out your copyright notice in the Description page of Project Settings.


#include "BioticFieldActor.h"
#include "NiagaraComponent.h"
#include "../State/DemoPlayerState.h"
#include "../Character/Player/PlayerCharacter.h"

const float ABioticFieldActor::HEALING_HP_TICK = 0.05f;

// Sets default values
ABioticFieldActor::ABioticFieldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	BioticFieldNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BioticField"));
	BioticFieldNiagara->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_BIOTIC_FILED(
		TEXT("/Game/Demo/Effects/E_Biotic_Field"));
	if(NS_BIOTIC_FILED.Succeeded())
		BioticFieldNiagara->SetAsset(NS_BIOTIC_FILED.Object);

	InitialLifeSpan = 10.f;
}

// Called when the game starts or when spawned
void ABioticFieldActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABioticFieldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();
	FVector Center = GetActorLocation();
	float DetectRadius = 350.f;

	TArray<FOverlapResult> OverlapResults;
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius)
	);

	if (bResult)
	{
		for (int i = 0; i < OverlapResults.Num(); i++)
		{
			FOverlapResult OverlapResult = OverlapResults[i];
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OverlapResult.GetActor());
			if (PlayerCharacter)
			{
				if (PlayerCharacter->GetController()->IsPlayerController())
				{
					ADemoPlayerState* DemoPlayerState = 
						PlayerCharacter->GetController()->GetPlayerState<ADemoPlayerState>();
					if (DemoPlayerState)
					{
						DemoPlayerState->UpdateHpState(
							DemoPlayerState->CurrentHp + HEALING_HP_TICK);
					}
				}
			}
		}
	}
}

