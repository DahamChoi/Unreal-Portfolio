// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SphereBot_Detect.h"
#include "../../Controller/SphereBotAIController.h"
#include "../../Character/Player/PlayerCharacter.h"
#include "../../Character/Enemy/SphereBotCharacter.h"
#include "../Common/BTService_Common_TargetDetecter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"

const FName UBTService_SphereBot_Detect::IS_ATTACK_RANGE_KEY = TEXT("IsAttackRange");
const float UBTService_SphereBot_Detect::ATTACK_RANGE = 1000.f;

UBTService_SphereBot_Detect::UBTService_SphereBot_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 0.1f;
}

void UBTService_SphereBot_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetCharacter();
	if (nullptr == ControllingPawn)	return;

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 5000.f;

	if (nullptr == World)	return;
	
	UObject* TargetObject =
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(UBTService_Common_TargetDetecter::TARGET_KEY);
	if (TargetObject)
	{
		AActor* Target = Cast<AActor>(TargetObject);
		if (Target)
		{
			float TargetLength = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(
				UBTService_Common_TargetDetecter::TARGET_LENGTH_KEY);
			ASphereBotCharacter* SphereBotCharacter = Cast<ASphereBotCharacter>(ControllingPawn);
			if (SphereBotCharacter)
			{
				if (!SphereBotCharacter->TransformationEnable)
				{
					FTransform Transform = SphereBotCharacter->GetMesh()->GetSocketTransform("B_SB_LaserSocket");
					FVector Start = Transform.GetLocation() + SphereBotCharacter->GetCapsuleComponent()->GetForwardVector() * 200.f;
					FVector End = Start + (SphereBotCharacter->GetCapsuleComponent()->GetForwardVector() * 3000.f);

					FHitResult HitResult;
					FCollisionQueryParams CollisionParams;
					if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldDynamic, CollisionParams))
					{
						if (HitResult.Actor == Target && TargetLength <= ATTACK_RANGE)
							OwnerComp.GetBlackboardComponent()->SetValueAsBool(IS_ATTACK_RANGE_KEY, true);
						else
							OwnerComp.GetBlackboardComponent()->SetValueAsBool(IS_ATTACK_RANGE_KEY, false);
					}
				}
			}
		}
	}
}
