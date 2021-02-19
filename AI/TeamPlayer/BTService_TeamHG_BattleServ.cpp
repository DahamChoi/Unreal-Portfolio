// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_TeamHG_BattleServ.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Character/Player/PlayerCharacterHandGunForm.h"
#include "../../Controller/BaseAIController.h"
#include "../../Actor/BioticFieldActor.h"

const FName UBTService_TeamHG_BattleServ::IS_ULTIMATE_ON_KEY = TEXT("IsUltimateOn");
const FName UBTService_TeamHG_BattleServ::IS_DODGE_KEY = TEXT("IsDodge");
const FName UBTService_TeamHG_BattleServ::BIOTIC_FIELD_LOCATION_KEY = TEXT("BioticFiledLocation");

UBTService_TeamHG_BattleServ::UBTService_TeamHG_BattleServ()
{

}

void UBTService_TeamHG_BattleServ::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerCharacterHandGunForm* CharacterHandGunForm =
		Cast<APlayerCharacterHandGunForm>(ControllingPawn);

	if (CharacterHandGunForm)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(
			IS_ULTIMATE_ON_KEY, CharacterHandGunForm->PCActionIsUltimate);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(
			IS_DODGE_KEY, CharacterHandGunForm->PCActionIsDodge);

		// BioticField Location
		ABioticFieldActor* PendingBioticField = nullptr;
		for (TObjectIterator<ABioticFieldActor> Itr; Itr; ++Itr)
		{
			// Is Valid
			if (Itr)
			{
				if (PendingBioticField == nullptr)
				{
					PendingBioticField = *Itr; continue;
				}
				if (Itr->GetDistanceTo(CharacterHandGunForm) <
					PendingBioticField->GetDistanceTo(CharacterHandGunForm))
				{
					PendingBioticField = *Itr;
				}
			}
		}

		if (PendingBioticField)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(
				BIOTIC_FIELD_LOCATION_KEY, PendingBioticField->GetActorLocation());
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->ClearValue(BIOTIC_FIELD_LOCATION_KEY);
		}
	}
}