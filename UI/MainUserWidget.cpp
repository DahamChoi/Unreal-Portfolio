// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Engine/Font.h"
#include "Engine/Texture2D.h"
#include "../State/DemoGameStateBase.h"
#include "../State/DemoPlayerState.h"
#include "../Character/Player/PlayerCharacter.h"

const FLinearColor UMainUserWidget::ACTIVE_COLOR = FLinearColor(1.f, 0.8740f, 0.44f);
const FLinearColor UMainUserWidget::ORIGIN_COLOR = FLinearColor(1.f, 1.f, 1.f);

const FName UMainUserWidget::ELIMINATED_ANIMATION_NAME = TEXT("EliminatedTextAnimation");
const FName UMainUserWidget::ULTIMATE_ANIMATION_NAME = TEXT("UltimateAnimation");
const FName UMainUserWidget::TAKE_DAMAGE_ANIMATION_NAME = TEXT("TakeDamageAnimation");
const FName UMainUserWidget::CHANGE_FORM_ANIMATION_NAME = TEXT("ChangeFormAnimation");

UMainUserWidget::UMainUserWidget(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	// Font
	static ConstructorHelpers::FObjectFinder<UFont> OverwatchFontObj(
		TEXT("/Game/Demo/UI/koverwatch_Font"));
	FontInfo = FSlateFontInfo(OverwatchFontObj.Object, 20);

	// Img
	static ConstructorHelpers::FObjectFinder<UTexture2D> HANDGUN_E(
		TEXT("/Game/Demo/UI/Mccree_ability_icon_2")
	);
	HandGunFormEIcon = HANDGUN_E.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> HANDGUN_F(
		TEXT("/Game/Demo/UI/Mccree_ability_icon_1")
	);
	HandGunFormFIcon = HANDGUN_F.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> HANDGUN_WEAPON(
		TEXT("/Game/Demo/UI/Mccree_ability_icon_0")
	);
	HandGunFormWeaponIcon = HANDGUN_WEAPON.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> RIFLE_E(
		TEXT("/Game/Demo/UI/icon-ability-56")
	);
	RifleFormEIcon = RIFLE_E.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> RIFLE_F(
		TEXT("/Game/Demo/UI/icon-ability-54")
	);
	RifleFormFIcon = RIFLE_F.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> RIFLE_WEAPON(
		TEXT("/Game/Demo/UI/icon-ability-53")
	);
	RifleFormWeaponIcon = RIFLE_WEAPON.Object;
}

void UMainUserWidget::NativeConstruct()
{
	// Only Call One Time
	if (IsCallNativeConstructor)	return;
	IsCallNativeConstructor = true;

	// HP Text
	CurrentHpText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CURRENT_HP_TEXT")));

	// Bullet Count Text
	CurrentBulletCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("BulletCount")));

	// HP BorderArray
	for (int i = 0; i < 8; i++)
	{
		FName WidgetName =
			FName(*(FString("HP_") + FString::FromInt(i + 1)));
		UBorder* Border = Cast<UBorder>(GetWidgetFromName(WidgetName));
		if (Border)
		{
			HpBorderArray.Add(Border);
		}
	}

	// EIcon Widget
	EIconProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("E_ICON_PROGRESS_BAR")));
	ETimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("E_TIME_TEXT")));

	// FIcon Widget
	FIconProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("F_ICON_PROGRESS_BAR")));
	FTimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("F_TIME_TEXT")));

	// QIcon Widget
	QProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("Q_ProgressBar")));
	QPercentText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Q_Percent_Text")));

	// Shift Widget
	ShiftIconImg = Cast<UImage>(GetWidgetFromName(TEXT("SHIFT_ICON_BACK_IMG")));

	// Fight Panel
	FightPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("FIGHT_PANEL")));

	// Payload Widget
	ActivePayLoadText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ActivePayLoadText")));
	PayLoadInfoProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PayloadInfoProgressBar")));
	PlayerInfoPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PLAYER_INFO_PANEL")));

	// EliminatedText Widget
	EliminatedText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ELIMINATED_TEXT")));

	// Kill Log Widget
	KillLogBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("KILL_LOG_VERTICAL_BOX")));

	// Team Player Widget
	for (int i = 0; i < 3; i++)
	{
		// Init HpBorderArrayTeam
		HpBorderArray_Team.Add(FBorderArray());

		// Team Player Name Text Widget
		FName WidgetName =
			FName(*(FString("TEAM_PLAYER_NAME_TEXT_") + FString::FromInt(i + 1)));
		UTextBlock* TextBlock = Cast<UTextBlock>(GetWidgetFromName(WidgetName));
		PlayerNameText_Team.Add(TextBlock);
	}

	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			FName WidgetName =
				FName(*(FString("TEAM_") + FString::FromInt(j + 1) + TEXT("_HP_") 
					+ FString::FromInt(i + 1)));	//	TEAM_(j+1)_HP_(i+1)
			UBorder* Border = Cast<UBorder>(GetWidgetFromName(WidgetName));
			if (Border)
			{
				HpBorderArray_Team[j].BorderArrayData.Add(Border);
			}
		}
	}
	
	// Skill icon
	FIconImg = Cast<UImage>(GetWidgetFromName(TEXT("F_ICON")));
	EIconImg = Cast<UImage>(GetWidgetFromName(TEXT("E_ICON")));
	WeaponImg = Cast<UImage>(GetWidgetFromName(TEXT("WEAPON_IMG")));

	// MaxBulletCount
	MaxBulletCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MaxBulletCount")));

	// CrossHair Img
	CrossHairImg = Cast<UImage>(GetWidgetFromName(TEXT("CROSS_HAIR_IMG")));

	// Victory
	VictoryText = Cast<UTextBlock>(GetWidgetFromName(TEXT("VICTORY_TEXT")));

	// Animation Setup
	FillAnimationsMap();
}

void UMainUserWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

}

void UMainUserWidget::BindPlayerState(ADemoPlayerState* PlayerState)
{
	CurrentPlayerState = PlayerState;
	PlayerState->OnPlayerStateChange.AddUObject(this, &UMainUserWidget::UpdatePlayerState);
	PlayerState->OnPlayerFormChange.AddUObject(this, &UMainUserWidget::ChangePlayerBattleForm);
}

void UMainUserWidget::BindGameState(ADemoGameStateBase* GameState)
{
	CurrentGameState = GameState;
	GameState->OnGameStateChange.AddUObject(this, &UMainUserWidget::UpdateGameState);
	GameState->OnKillLogUpdate.AddUObject(this, &UMainUserWidget::UpdateKillLog);
}

void UMainUserWidget::BindCharacterState(APlayerCharacter* PlayerCharacter)
{
	PlayerCharacter->OnKillEnemy.AddUObject(this, &UMainUserWidget::UpdateKillState);
	PlayerCharacter->OnUltimateActive.AddUObject(this, &UMainUserWidget::ActiveUltimateAnimation);
	PlayerCharacter->OnTakeDamageActive.AddUObject(this, &UMainUserWidget::ActiveTakeDamageAnimation);
	PlayerCharacter->OnUpdateCrossHair.AddUObject(this, &UMainUserWidget::UpdateCrossHairPosition);
}

void UMainUserWidget::AddBindingTeamPlayerState(ADemoPlayerState* PlayerState)
{
	CurrentTeamPlayerStates.Add(PlayerState);
	PlayerState->OnPlayerStateChange.AddUObject(this, &UMainUserWidget::UpdateTeamPlayerState);
}

void UMainUserWidget::UpdatePlayerState()
{
	NativeConstruct();

	// HP Text
	CurrentHpText->SetText(
		FText::FromString(FString::FromInt((int)CurrentPlayerState->CurrentHp)));

	// HP Border
	for (int i = 0; i < HpBorderArray.Num(); i++)
	{
		float HpAlpha = CurrentPlayerState->CurrentHp - ((HpBorderArray.Num() - i - 1) * (CurrentPlayerState->MaxHp / HpBorderArray.Num()));
		HpAlpha = (HpAlpha < 0) ? 0.2f : (0.2f + (HpAlpha * 0.032f));

		HpBorderArray[i]->SetBrushColor(FLinearColor(1.f, 0.3548f, 0.03125f, HpAlpha));
	}
	
	// Bullet Text
	CurrentBulletCountText->SetText(
		FText::FromString(FString::FromInt(CurrentPlayerState->CurrentBulletCount)));
	MaxBulletCountText->SetText(
		FText::FromString(FString::FromInt(CurrentPlayerState->MaxBulletCount)));

	// Shift Skill
	if (CurrentPlayerState->bSprint)
		ShiftIconImg->SetColorAndOpacity(ACTIVE_COLOR);
	else
		ShiftIconImg->SetColorAndOpacity(ORIGIN_COLOR);

	// Q Skill
	QProgressBar->SetPercent(
		(CurrentPlayerState->QSkillPercent / ADemoPlayerState::Q_SKILL_MAX_PERCENT));
	QPercentText->SetText(FText::FromString(FString::FromInt((int)CurrentPlayerState->QSkillPercent).Append(("%"))));

	// E Skill
	if (FMath::IsNearlyEqual(CurrentPlayerState->ESkillTimeRemaining, 0.f))
	{
		ETimeText->SetVisibility(ESlateVisibility::Hidden);
		EIconProgressBar->SetPercent(1.f);
		EIconProgressBar->SetFillColorAndOpacity(ORIGIN_COLOR);
	}
	else if (CurrentPlayerState->ESkillTimeRemaining <= CurrentPlayerState->MaxESkillTimeRemaining
		&& CurrentPlayerState->ESkillTimeRemaining >= CurrentPlayerState->MaxESkillTimeRemaining - 1.f)
	{
		ETimeText->SetVisibility(ESlateVisibility::Hidden);
		EIconProgressBar->SetPercent(1.f);
		EIconProgressBar->SetFillColorAndOpacity(ACTIVE_COLOR);
	}
	else
	{
		ETimeText->SetVisibility(ESlateVisibility::Visible);
		EIconProgressBar->SetFillColorAndOpacity(ACTIVE_COLOR);
		EIconProgressBar->SetPercent(
			1.f - (CurrentPlayerState->ESkillTimeRemaining / CurrentPlayerState->MaxESkillTimeRemaining)
		);
		ETimeText->SetText(
			FText::FromString(FString::FromInt((int)CurrentPlayerState->ESkillTimeRemaining))
		);
	}

	// F Skill
	if (FMath::IsNearlyEqual(CurrentPlayerState->FSkillTimeRemaining, 0.f))
	{
		FTimeText->SetVisibility(ESlateVisibility::Hidden);

		FIconProgressBar->SetPercent(1.f);
		FIconProgressBar->SetFillColorAndOpacity(ORIGIN_COLOR);
	}
	else if (CurrentPlayerState->FSkillTimeRemaining <= CurrentPlayerState->MaxFSkillTimeRemaining
		&& CurrentPlayerState->FSkillTimeRemaining >= CurrentPlayerState->MaxFSkillTimeRemaining - 1.f)
	{
		FTimeText->SetVisibility(ESlateVisibility::Hidden);

		FIconProgressBar->SetPercent(1.f);
		FIconProgressBar->SetFillColorAndOpacity(ACTIVE_COLOR);
	}
	else
	{
		FTimeText->SetVisibility(ESlateVisibility::Visible);
		FTimeText->SetText(
			FText::FromString(FString::FromInt((int)CurrentPlayerState->FSkillTimeRemaining))
		);

		FIconProgressBar->SetFillColorAndOpacity(ACTIVE_COLOR);
		FIconProgressBar->SetPercent(
			1.f - (CurrentPlayerState->FSkillTimeRemaining / CurrentPlayerState->MaxFSkillTimeRemaining)
		);
	}
}

void UMainUserWidget::UpdateTeamPlayerState()
{
	NativeConstruct();

	// HP Border
	for (int j = 0; j < CurrentTeamPlayerStates.Num(); j++)
	{
		PlayerNameText_Team[j]->SetText(FText::FromString(CurrentTeamPlayerStates[j]->PlayerName));

		for (int i = 0; i < HpBorderArray_Team[j].BorderArrayData.Num(); i++)
		{
			float HpAlpha = CurrentTeamPlayerStates[j]->CurrentHp - (i * (CurrentPlayerState->MaxHp / HpBorderArray.Num()));
			HpAlpha = (HpAlpha < 0) ? 0.2f : (0.2f + (HpAlpha * 0.032f));

			HpBorderArray_Team[j].BorderArrayData[i]->SetBrushColor(FLinearColor(1.f, 0.3548f, 0.03125f, HpAlpha));
		}
	}
}

void UMainUserWidget::UpdateGameState()
{
	NativeConstruct();

	if (FMath::IsNearlyEqual(CurrentGameState->PayloadAlphaDistance, 1.f))
	{
		VictoryText->SetVisibility(ESlateVisibility::Visible);
	}

	PayLoadInfoProgressBar->SetPercent(CurrentGameState->PayloadAlphaDistance);
	PlayerInfoPanel->SetRenderTransform(
		FWidgetTransform(
			FVector2D(-320 * CurrentGameState->PayloadAlphaDistance, 0.f),
			FVector2D(1.f, 1.f),
			FVector2D(0.f, 0.f),
			0.f
		));
	
	switch (CurrentGameState->PayloadState)
	{
	case EPayloadStateEnum::PSE_STOP:
		ActivePayLoadText->SetVisibility(ESlateVisibility::Hidden);
		FightPanel->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EPayloadStateEnum::PSE_MOVE:
		ActivePayLoadText->SetVisibility(ESlateVisibility::Visible);
		FightPanel->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EPayloadStateEnum::PSE_FIGHTING:
		ActivePayLoadText->SetVisibility(ESlateVisibility::Hidden);
		FightPanel->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}

void UMainUserWidget::UpdateKillLog()
{
	FKillLog KillLog = CurrentGameState->KillLogArray.Last();
	
	UTextBlock* NewKillLogText = NewObject<UTextBlock>(UTextBlock::StaticClass());
	NewKillLogText->SetVisibility(ESlateVisibility::Visible);
	NewKillLogText->SetText(FText::FromString(KillLog.KillerName + TEXT(" > ") + KillLog.KilledName));
	NewKillLogText->SetFont(FontInfo);

	KillLogBox->AddChildToVerticalBox(NewKillLogText);
	KillLogTextArray.Add(NewKillLogText);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (KillLogTextArray.Num() > 0)
			{
				UTextBlock* First = *KillLogTextArray.begin();
				KillLogTextArray.Remove(First);
				First->RemoveFromParent();
			}
		}), 5.f, false);
}

void UMainUserWidget::UpdateKillState(FString KillerName, FString KilledName)
{
	NativeConstruct();

	FString EliminatedTextString;
	EliminatedTextString = KilledName + TEXT(" 처치");
	EliminatedText->SetText(FText::FromString(EliminatedTextString));

	PlayAnimationByName(ELIMINATED_ANIMATION_NAME, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UMainUserWidget::UpdateCrossHairPosition(FVector2D Vector)
{
	NativeConstruct();

	CrossHairImg->SetRenderTransform(
		FWidgetTransform(FVector2D(Vector.X * 2.f, Vector.Y * 2.f), FVector2D(1.f, 1.f), FVector2D(0.f, 0.f), 0.f));
}

void UMainUserWidget::ActiveUltimateAnimation()
{
	NativeConstruct();

	PlayAnimationByName(ULTIMATE_ANIMATION_NAME, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UMainUserWidget::ActiveTakeDamageAnimation()
{
	NativeConstruct();

	PlayAnimationByName(TAKE_DAMAGE_ANIMATION_NAME, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UMainUserWidget::ChangePlayerBattleForm()
{
	if(CurrentPlayerState->CurrentBattleForm == EBattleFormEnum::BFE_RIFLE)
	{
		FIconImg->SetBrushFromTexture(RifleFormFIcon);
		EIconImg->SetBrushFromTexture(RifleFormEIcon);
		WeaponImg->SetBrushFromTexture(RifleFormWeaponIcon);
	}
	else
	{
		FIconImg->SetBrushFromTexture(HandGunFormEIcon);
		EIconImg->SetBrushFromTexture(HandGunFormFIcon);
		WeaponImg->SetBrushFromTexture(HandGunFormWeaponIcon);
	}

	PlayAnimationByName(CHANGE_FORM_ANIMATION_NAME, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

UWidgetAnimation* UMainUserWidget::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnim = AnimationsMap.Find(AnimationName);
	if (WidgetAnim)
	{
		return *WidgetAnim;
	}
	return nullptr;
}

bool UMainUserWidget::PlayAnimationByName(FName AnimationName, float StartAtTime, int32 NumLoopsToPlay, EUMGSequencePlayMode::Type PlayMode, float PlaybackSpeed)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(AnimationName);
	if (WidgetAnim)
	{
		PlayAnimation(WidgetAnim, StartAtTime, NumLoopsToPlay, PlayMode, PlaybackSpeed);
		return true;
	}
	return false;
}

void UMainUserWidget::FillAnimationsMap()
{
	AnimationsMap.Empty();

	UProperty* Prop = GetClass()->PropertyLink;

	// Run through all properties of this class to find any widget animations
	while (Prop != nullptr)
	{
		// Only interested in object properties
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty* ObjProp = Cast<UObjectProperty>(Prop);

			// Only want the properties that are widget animations
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);

				UWidgetAnimation* WidgetAnim = Cast<UWidgetAnimation>(Obj);

				if (WidgetAnim != nullptr && WidgetAnim->MovieScene != nullptr)
				{
					FName AnimName = WidgetAnim->MovieScene->GetFName();
					AnimationsMap.Add(AnimName, WidgetAnim);
				}
			}
		}

		Prop = Prop->PropertyLinkNext;
	}
}
