// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_ScifiSoldierHG_Combat.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "../../Actor/HandGunActor.h"
#include "../../Controller/BaseAIController.h"
#include "../../Character/Enemy/PlayerCharacterHandGunFormEnemy.h"
#include "../Common/BTService_Common_TargetDetecter.h"
#include "../../State/DemoPlayerState.h"

const FName UBTTaskNode_ScifiSoldierHG_Combat::FIRE_TIMER_KEY = TEXT("FireTimer");

UBTTaskNode_ScifiSoldierHG_Combat::UBTTaskNode_ScifiSoldierHG_Combat()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_ScifiSoldierHG_Combat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// Set Fire Timer Default
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, 0.f);

	// Ethernal Progress Task
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_ScifiSoldierHG_Combat::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Get Pawn As PlayerCharacterRifleFormEnemy
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerCharacterHandGunFormEnemy* HandGunEnemy =
		Cast<APlayerCharacterHandGunFormEnemy>(ControllingPawn);

	// Get Current FireTimer Value
	float FireTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(FIRE_TIMER_KEY);

	// Turn To Target
	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(
		UBTService_Common_TargetDetecter::TARGET_KEY);
	AActor* Target = Cast<AActor>(TargetObject);
	if (HandGunEnemy)
	{
		if (Target)
		{
			FVector TargetLocation = Target->GetActorLocation();
			// FireHole Location
			FVector OriginLocation = HandGunEnemy->GetMesh()->GetSocketLocation("WeaponSocket") + FTransform(
				HandGunEnemy->Weapon->GetActorRotation()).TransformPosition(FVector(100.f, 0.f, 8.f));

			FVector LookVector = TargetLocation - OriginLocation;
			LookVector.Z = 0.f;
			FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

			// Rotate To Target
			ControllingPawn->SetActorRotation(TargetRot);

			// Line Trace
			FVector Start = OriginLocation;
			FVector End = Start + (HandGunEnemy->GetCapsuleComponent()->GetForwardVector() * 3000.f);

			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;

			bool bCanTrace = GetWorld()->LineTraceSingleByChannel(
				HitResult, Start, End, ECollisionChannel::ECC_WorldDynamic, CollisionParams);

			// Fire
			if (HandGunEnemy->CurrentPlayerState->CurrentBulletCount > 0 && FireTimer > 3.f && bCanTrace &&
				!HandGunEnemy->PCActionIsFire)
			{
				HandGunEnemy->PCActionFire();
				OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, 0.f);
			}

			// Reload
			if (HandGunEnemy->CurrentPlayerState->CurrentBulletCount <= 0)
			{
				HandGunEnemy->PCActionReload();
			}

			// Dodge SKill
			if (FMath::IsNearlyEqual(HandGunEnemy->CurrentPlayerState->ESkillTimeRemaining, 0.f))
			{
				HandGunEnemy->PCActionDodge();
			}
		}
	}

	// Update Fire Timer
	FireTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(FIRE_TIMER_KEY);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, FireTimer + DeltaSeconds);

	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
