# Unreal Portfolio
포토폴리오 목적으로 제작된 오버워치 시스템을 모작한 간단한 데모 게임입니다.
엔진 : Unreal Engine 4 4.24.1
에디터 : Microsoft Visual Studio Community 2019 16.8.5
제작기간 : 2021.01.25~2021.02.24 (약 한달)
개발규모 : 1인개발

# 데모 동영상

촬영중

# 기술 설명서


## Controller - Character 구조

![enter image description here](https://user-images.githubusercontent.com/7011030/108332989-c598c280-7213-11eb-9595-251ebd576913.png)

## Main User Widget

Delegate를 활용해 UI가 반응해야 하는 특정 이벤트가 발생한 상황에 Broadcast하는 방식으로 Main UI 구현

### DemoPlayerController.cpp

    MainUserWidget = CreateWidget<UMainUserWidget>(this, MainUserWidgetClass);
	MainUserWidget->AddToViewport();

	ADemoGameStateBase* DemoGameState = GetWorld()->GetGameState<ADemoGameStateBase>();
	MainUserWidget->BindGameState(DemoGameState);

	ADemoPlayerState* DemoPlayerState = GetPlayerState<ADemoPlayerState>();
	MainUserWidget->BindPlayerState(DemoPlayerState);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter());
	if (PlayerCharacter)
	{
		MainUserWidget->BindCharacterState(PlayerCharacter);
	}
    
    
 ### MainUserWidget.cpp
 

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

  

![enter image description here](https://user-images.githubusercontent.com/7011030/108333064-dea17380-7213-11eb-8358-e57d2dcf40f4.png)

## Behavior Tree
 - **AI Perception**을 활용하여 적을 감지하는 것을 기본베이스로 Behavior Tree를 제작
 - Sight를 Domination Sense로 지정, Damage Sense를 서브로 활용
 - 이 게임은 Team AI와 Enemy AI가 분리되어 있으므로 **SetGenericTeamId**를 활용하여 Team으로 설정된 Character는 타겟으로 탐지하지 않도록 설계
 - PlayerCharacter를 사용하는 캐릭터 같은 경우, EQS 쿼리를 사용해 엄폐물을 찾아 이동하고 지속적으로 움직여 맞추기 어렵게 제작
 - 모든 AI의 기본액션
 - 1. 화물로 이동한다.
 - 2. 화물 주변을 탐색한다. (EQS쿼리 활용)
 - 3. 만약 적을 발견할 경우 AI특성에 맞는 특정 액션을 취한다.
### Sphere Bot
[Temp/AI/SphereBot at main · DahamChoi/Temp (github.com)](https://github.com/DahamChoi/Temp/tree/main/AI/SphereBot)
![enter image description here](https://user-images.githubusercontent.com/7011030/108334000-db5ab780-7214-11eb-9068-24d46d04e7a6.PNG)
### Super Dusk
[Temp/AI/SuperDusk at main · DahamChoi/Temp (github.com)](https://github.com/DahamChoi/Temp/tree/main/AI/SuperDusk)
![enter image description here](https://user-images.githubusercontent.com/7011030/108334712-8f5c4280-7215-11eb-87da-f8b8cbcabe23.PNG)
### Prime Helix
[Temp/AI/PrimeHelix at main · DahamChoi/Temp (github.com)](https://github.com/DahamChoi/Temp/tree/main/AI/PrimeHelix)
![enter image description here](https://user-images.githubusercontent.com/7011030/108335700-946dc180-7216-11eb-9f36-ebb813089dde.PNG)
### SciFiSolider, ScifiSoliderHandGun
[Temp/AI/ScifiSolider at main · DahamChoi/Temp (github.com)](https://github.com/DahamChoi/Temp/tree/main/AI/ScifiSolider)
![enter image description here](https://user-images.githubusercontent.com/7011030/108335837-b6ffda80-7216-11eb-99ce-76dc9d873ff3.PNG)
### TeamPlayer
[Temp/AI/TeamPlayer at main · DahamChoi/Temp (github.com)](https://github.com/DahamChoi/Temp/tree/main/AI/TeamPlayer)
 Rifle
![TeamPlayer Rifle](https://user-images.githubusercontent.com/7011030/108336103-0940fb80-7217-11eb-9e85-6db364974f43.PNG)
HandGun
![enter image description here](https://user-images.githubusercontent.com/7011030/108336277-34c3e600-7217-11eb-8e8f-e73888c4d564.PNG)
