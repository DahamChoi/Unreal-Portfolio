// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TeamHG_Combat.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../Character/Player/PlayerCharacterHandGunForm.h"
#include "../../Controller/BaseAIController.h"
#include "../Common/BTService_Common_TargetDetecter.h"
#include "../../Actor/HandGunActor.h"
#include "../../State/DemoPlayerState.h"

const FName UBTTaskNode_TeamHG_Combat::FIRE_TIMER_KEY = TEXT("FireTimer");

UBTTaskNode_TeamHG_Combat::UBTTaskNode_TeamHG_Combat()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_TeamHG_Combat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, 0.f);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn)
	{
		ControllingPawn->bUseControllerRotationYaw = false;
	}

	// Ethernal Progress Task
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_TeamHG_Combat::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Get Pawn As PlayerCharacterRifleFormEnemy
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerCharacterHandGunForm* CharacterHandGunForm =
		Cast<APlayerCharacterHandGunForm>(ControllingPawn);

	float FireTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(FIRE_TIMER_KEY);

	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(
		UBTService_Common_TargetDetecter::TARGET_KEY);
	AActor* Target = Cast<AActor>(TargetObject);
	if (CharacterHandGunForm)
	{
		if (Target)
		{
			ACharacterBase* TargetCharacter = Cast<ACharacterBase>(Target);
			if (TargetCharacter)
			{
				if (TargetCharacter->IsDead)
				{
					OwnerComp.GetBlackboardComponent()->ClearValue(UBTService_Common_TargetDetecter::TARGET_KEY);
				}
			}

			FVector TargetLocation = Target->GetActorLocation();
			// FireHole Location
			FVector OriginLocation = CharacterHandGunForm->GetMesh()->GetSocketLocation("WeaponSocket") + FTransform(
				CharacterHandGunForm->Weapon->GetActorRotation()).TransformPosition(FVector(100.f, 0.f, 8.f));

			FVector LookVector = TargetLocation - OriginLocation;
			LookVector.Z = 0.f;
			FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

			// Rotate To Target
			ControllingPawn->SetActorRotation(TargetRot);

			// Line Trace
			FVector Start = OriginLocation;
			FVector End = Start + (CharacterHandGunForm->GetCapsuleComponent()->GetForwardVector() * 8000.f);

			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;

			bool bCanTrace = GetWorld()->LineTraceSingleByChannel(
				HitResult, Start, End, ECollisionChannel::ECC_WorldDynamic, CollisionParams);
			bCanTrace = (HitResult.Actor == Target) && bCanTrace;

			// Not Ultimate
			if (!CharacterHandGunForm->PCActionIsUltimate && !CharacterHandGunForm->PCActionIsDodge)
			{
				// Fire
				if (bCanTrace && CharacterHandGunForm->CurrentPlayerState->CurrentBulletCount > 0 
					&& FireTimer > 1.f)
				{
					CharacterHandGunForm->PCActionFire();
					OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, 0.f);
				}

				// Reload
				if (CharacterHandGunForm->CurrentPlayerState->CurrentBulletCount <= 0)
				{
					CharacterHandGunForm->PCActionReload();
				}

				// Skill
				if (FMath::IsNearlyEqual(CharacterHandGunForm->CurrentPlayerState->ESkillTimeRemaining, 0.f))
				{
					CharacterHandGunForm->PCActionDodge();
				}

				// Ultimate
				if (FMath::IsNearlyEqual(
					CharacterHandGunForm->CurrentPlayerState->QSkillPercent, 100.f))
				{
					CharacterHandGunForm->PCActionUltimate();

					FTimerHandle TimerHandle;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
						{
							CharacterHandGunForm->PCActionFire();
						}), 2.f, false);
				}
			}
		}
	}

	FireTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(FIRE_TIMER_KEY);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, FireTimer + DeltaSeconds);
}
