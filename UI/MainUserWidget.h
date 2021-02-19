// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUserWidget.generated.h"

USTRUCT(BlueprintType)
struct DEMO_API FBorderArray
{
	GENERATED_USTRUCT_BODY()

public:
	TArray<class UBorder*> BorderArrayData;
};

/**
 * 
 */
UCLASS()
class DEMO_API UMainUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Contructor
	UMainUserWidget(const FObjectInitializer& ObjectInitializer);

	// Natvie Contructor
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	// Bind Delegate
	void BindPlayerState(class ADemoPlayerState* PlayerState);
	void BindGameState(class ADemoGameStateBase* GameState);
	void BindCharacterState(class APlayerCharacter* PlayerCharacter);

	void AddBindingTeamPlayerState(class ADemoPlayerState* PlayerState);

	// Update UI Value
	void UpdatePlayerState();
	void UpdateTeamPlayerState();
	void UpdateGameState();
	void UpdateKillLog();
	void UpdateKillState(FString KillerName, FString KilledName);
	void UpdateCrossHairPosition(FVector2D Vector);
	void ActiveUltimateAnimation();
	void ActiveTakeDamageAnimation();
	void ChangePlayerBattleForm();

	// Animation Functions										//
	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;
	bool PlayAnimationByName(FName AnimationName,
		float StartAtTime,
		int32 NumLoopsToPlay,
		EUMGSequencePlayMode::Type PlayMode,
		float PlaybackSpeed);
	void FillAnimationsMap();
	// ======================================================== //

private:
	// Animation Map
	UPROPERTY()
	TMap<FName, UWidgetAnimation*> AnimationsMap;

	// Bind State
	TWeakObjectPtr<class ADemoPlayerState> CurrentPlayerState;
	TWeakObjectPtr<class ADemoGameStateBase> CurrentGameState;
	TArray<TWeakObjectPtr<class ADemoPlayerState>> CurrentTeamPlayerStates;

	// HP Text Widget
	UPROPERTY()
	class UTextBlock* CurrentHpText;

	// HP Border Widget
	UPROPERTY()
	TArray<class UBorder*> HpBorderArray;

	// CurrentBullet Widget
	UPROPERTY()
	class UTextBlock* CurrentBulletCountText;

	// EIcon Widget
	UPROPERTY()
	class UProgressBar* EIconProgressBar;

	UPROPERTY()
	class UTextBlock* ETimeText;

	// FIcon Widget
	UPROPERTY()
	class UProgressBar* FIconProgressBar;

	UPROPERTY()
	class UTextBlock* FTimeText;

	// Shift Icon Widget
	UPROPERTY()
	class UImage* ShiftIconImg;

	// Q Icon Widget
	UPROPERTY()
	class UProgressBar* QProgressBar;

	UPROPERTY()
	class UTextBlock* QPercentText;

	// Fight Panel
	UPROPERTY()
	class UCanvasPanel* FightPanel;

	// PayLoad Widget
	UPROPERTY()
	class UTextBlock* ActivePayLoadText;

	UPROPERTY()
	class UProgressBar* PayLoadInfoProgressBar;

	UPROPERTY()
	class UCanvasPanel* PlayerInfoPanel;

	// Kill Message Widget
	UPROPERTY()
	class UTextBlock* EliminatedText;

	// Kill Log Widget
	UPROPERTY()
	class UVerticalBox* KillLogBox;

	UPROPERTY()
	TArray<class UTextBlock*> KillLogTextArray;

	UPROPERTY()
	FSlateFontInfo FontInfo;
	
	// Team Player Widget
	UPROPERTY()
	TArray<FBorderArray> HpBorderArray_Team;

	UPROPERTY()
	TArray<class UTextBlock*> PlayerNameText_Team;

	// Native Constructor Checker
	bool IsCallNativeConstructor;
	
	// Victory
	class UTextBlock* VictoryText;

	// Cross Hair
	UPROPERTY()
	class UImage* CrossHairImg;

	// Skill Icon
	UPROPERTY()
	class UImage* FIconImg;

	UPROPERTY()
	class UImage* EIconImg;

	UPROPERTY()
	class UImage* WeaponImg;

	UPROPERTY()
	class UTextBlock* MaxBulletCountText;

	UPROPERTY()
	class UTexture2D* HandGunFormEIcon;

	UPROPERTY()
	class UTexture2D* HandGunFormFIcon;

	UPROPERTY()
	class UTexture2D* HandGunFormWeaponIcon;

	UPROPERTY()
	class UTexture2D* RifleFormEIcon;

	UPROPERTY()
	class UTexture2D* RifleFormFIcon;

	UPROPERTY()
	class UTexture2D* RifleFormWeaponIcon;

private:
	static const FLinearColor ACTIVE_COLOR;
	static const FLinearColor ORIGIN_COLOR;

	static const FName ELIMINATED_ANIMATION_NAME;
	static const FName ULTIMATE_ANIMATION_NAME;
	static const FName TAKE_DAMAGE_ANIMATION_NAME;
	static const FName CHANGE_FORM_ANIMATION_NAME;
};
