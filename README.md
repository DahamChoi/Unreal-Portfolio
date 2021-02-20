# Unreal Portfolio
  포토폴리오 목적으로 제작된 오버워치 시스템을 모작한 간단한 데모 게임입니다.
 - 엔진 : Unreal Engine 4 4.26.1
 - 에디터 : Microsoft Visual Studio Community 2019 16.8.5
 - 제작기간 : 2021.01.25~2021.02.19 (약 한달)
 - 개발규모 : 1인개발

# 데모 동영상

> Youtube

[![이미지 텍스트](https://img.youtube.com/vi/dzeQ4YHV19c0.jpg)](https://www.youtube.com/watch?v=dzeQ4YHV19c)

# 기술 설명서


## Controller - Character 구조

![enter image description here](https://user-images.githubusercontent.com/7011030/108332989-c598c280-7213-11eb-9595-251ebd576913.png)

## CharacterBase
 - HP Bar Widget 관리
 - TakeDamage -> Dead 이벤트 처리
 -  **AIStimuliSource** 설정
 - PlayerState 기본값 설정
 > Take Damage

    float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
    {
    	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    	// Update Hp
    	CurrentPlayerState->UpdateHpState(
    		CurrentPlayerState->CurrentHp - DamageAmount);
    
    	// Update Hp Widget
    	if (HPBarWidgetObject)
    		HPBarWidgetObject->UpdateHPWidget(CurrentPlayerState->CurrentHp);
    
    	// Is Dead?
    	if (CurrentPlayerState->CurrentHp <= 0.001f)
    	{
    		if (!IsDead)
    		{
    			// Dead Event
    			IsDead = true;
    
    			RagDollDead();
    			OnDead.Broadcast();
    
    			// Disable Hp Widget
    			GetWorld()->GetTimerManager().SetTimer(DisableHpWidgetTimer, FTimerDelegate::CreateLambda([&]()
    				{
    					UHPBarWidget* Widget = Cast<UHPBarWidget>(HPBarWidget->GetUserWidgetObject());
    					if (Widget)
    						Widget->DisableHPBarWidget();
    				}), 2.5f, false);
    
    			// Destory Actor
    			GetWorld()->GetTimerManager().SetTimer(DestoryActorTimer, FTimerDelegate::CreateLambda([&]()
    				{
    					this->Destroy();
    				}), 7.f, false);
    
    			// Dead Event Delegate To Main Widget
    			if (DamageCauser)
    			{
    				ACharacterBase* CharacterBase = Cast<ACharacterBase>(DamageCauser);
    				if (CharacterBase)
    				{
    					APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(CharacterBase);
    
    					ADemoPlayerState* DemoPlayerState = CharacterBase->GetPlayerState<ADemoPlayerState>();
    					ADemoGameStateBase* DemoGameState = GetWorld()->GetGameState<ADemoGameStateBase>();
    
    					FString KillerName = DemoPlayerState->PlayerName;
    					FString KilledName = CurrentPlayerState->PlayerName;
    
    					// Update Player Widget
    					if (PlayerCharacter)
    					{
    						if (PlayerCharacter->GetController()->IsPlayerController())
    						{
    							PlayerCharacter->OnKillEnemy.Broadcast(KillerName, KilledName);
    						
    							// Sound
    							TakeDownAudioComponent->Play();
    						}
    					}
    
    					// Update Game State
    					DemoGameState->AddKillLog(KillerName, KilledName);
    				}
    			}
    			
    		}
    
    		return 0.f;
    	}
    
    	// AI Perception
    	if (!GetController()->IsPlayerController() && DamageCauser)
    	{
    		IGenericTeamAgentInterface* MyInterface = Cast<IGenericTeamAgentInterface>(DamageCauser->GetInstigatorController());
    		IGenericTeamAgentInterface* CauserInterface = Cast<IGenericTeamAgentInterface>(GetController());
    
    		if (MyInterface && CauserInterface)
    		{
    			if (MyInterface->GetGenericTeamId() != CauserInterface->GetGenericTeamId())
    			{
    				UAISense_Damage::ReportDamageEvent(
    					this, this, EventInstigator, FinalDamage, DamageCauser->GetActorLocation(), this->GetActorLocation());
    			}
    		}
    	}
    
    	return FinalDamage;
    }

## Player Character
 총을 쏘며 걸어다니는 캐릭터의 부모 클래스
 - Aim Zoom (Float Curve, Timeline 활용)
 - InputComponent
 - Form 전환 (HandGun <-> Rifle)
 - Sprint, Aim 액션
 - Skill Timer (PlayerState와 연동)
 - Demo Projectile 발사 기능
 - MainUserWidget Delegate Broadcast

## Player Character Rifle Form
 - Player와 Team AI가 활용
 - Crouch, Fire, Reload, Ultimate, BioticFiled, RocketLaunch 액션

> 궁극기 처리 코드 (Tick)

        // If Have Current Target
    	if (CurrentPendingTarget)
    	{
    		// Get TargetRotation
    		FVector TargetLocation = CurrentPendingTarget->GetActorLocation();
    		FVector OriginLocation = this->GetActorLocation();
    		FVector LookVector = TargetLocation - OriginLocation;
    		FRotator TargetRot = LookVector.Rotation();
    		
    	// If IsActionAim Have Less Pitch
    	if (PCActionIsAim)
    		TargetRot.Pitch -= 2.5f;
    
    	// Lerp Rotate Movement
    	UltimateLerpRotatorAlpha = FMath::Min(1.f, DeltaTime + UltimateLerpRotatorAlpha);
    	GetController()->SetControlRotation(FMath::Lerp(GetController()->GetControlRotation(), TargetRot, UltimateLerpRotatorAlpha));
    
    	// Is Target RagDoll Dead
    	if (CurrentPendingTarget->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_None)
    	{
    		CurrentPendingTarget = nullptr;
    	}
       }
       
  > Anim BP
 - Epic Games Animation Starter Pack 활용
 - Blend Space, Aim Offset, Bone Layer Blend 활용
 ![enter image description here](https://user-images.githubusercontent.com/7011030/108585189-6610f300-738a-11eb-83ac-6da419cc3fbb.PNG)
 ![enter image description here](https://user-images.githubusercontent.com/7011030/108585190-67dab680-738a-11eb-887f-d4b605eb85d2.PNG)

> Team AI Beahiovr Tree

![enter image description here](https://user-images.githubusercontent.com/7011030/108585311-2696d680-738b-11eb-8c11-dcd80f0c7510.PNG)

## Player Character HandGun Form
 - Player와 Team AI가 활용
 - Fire, Reload, Ultimate, Dodge, Rampage 액션 처리
 - Behavior Tree와 Anim BP는 위와 유사
 
## Player Character Rifle(HandGun) Form Enemy
 - 적 캐릭터 Behavior Tree 연동과 Mesh등의 기본 에셋 처리를 위해 오버라이드
  > Anim BP (AimOffset 부분만 변경)
 
![enter image description here](https://user-images.githubusercontent.com/7011030/108585368-91e0a880-738b-11eb-8c9f-695e764b75d7.PNG)
 
  > Behavior Tree
  > (EQS 쿼리와 Simple Pannel을 활용해 공격을 하면서 지속적으로 움직이고 엄폐물을 찾도록 구현)
  
![enter image description here](https://user-images.githubusercontent.com/7011030/108585444-fa2f8a00-738b-11eb-81de-5e1c3643b411.PNG)  

## Enemy Character 처리 방식 (예시 : Super Dusk Character)
 - Mesh, CapsuleComponent, HPBarWidget, CharacterMovement 값 설정
 - AnimBP와 BehaviorTree(BlackBoard)와 연결된 Attack등의 이벤트 처리
 > AnimBP
 
![enter image description here](https://user-images.githubusercontent.com/7011030/108585557-6611f280-738c-11eb-8620-f4406f28441c.PNG)

![enter image description here](https://user-images.githubusercontent.com/7011030/108585562-71651e00-738c-11eb-8371-d503d54b5126.PNG)

 > BehaviorTree
 
![enter image description here](https://user-images.githubusercontent.com/7011030/108585586-95286400-738c-11eb-981a-a5a866f88813.PNG)

## Base AI Controller
 - **AI Perception** 설정 (Sight를 Domination Sense로 설정, Damage를 서브로 활용)
 - **GetTeamAttiudeToward** 를 Override하고 SetGenericTeamId를 활용하여 적과 팀원을 구별할 수 있게 설정
 - **OnBaseTargetPerceptionUpdate** 를 통해 BlackBoard Target값 Update
 - Behavior Tree와 BlackBoard를 관리 구동

## Demo Player Controller
 - *Player Controller*
 - **IGenericTeamAgentInterface** 를 상속하여 AI Sense가 TeamGenericId를 활용할 수 있게 설정
 - Main User Widget을 로드, Add To Viewport

## Main User Widget

Delegate를 활용해 UI가 반응해야 하는 특정 이벤트가 발생한 상황에 Broadcast하는 방식으로 Main UI 구현

> DemoPlayerController.cpp

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
    
    
 > MainUserWidget.cpp
 

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

## Payload Actor
 - **Spline Component**를 활용하여 일정한 트랙을 따라서 움직이고, 오버워치와 같이 팀원이 화물에 가까이 붙으면 화물이 이동하고 적이 가로막고 있으면 움직이지 못하게 구현
 - 화물 상태를 GameState에 지속적으로 알려줌
 
 > 화물 이동 코드

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


## Level Streaming Actor
 -  **(OnBeginOverlap)** 됐을 때 **(특정시간)** 간격으로 **(특정횟수)** 동안 **(특정한)** 몬스터가 나타나도록 관리하는 Actor
 > 몬스터 스폰 코드 (함수 포인트 배열 사용)

     int RandomIndex = FMath::FRandRange(0, SpawnFunctionArray.Num());
	 (this->*SpawnFunctionArray[RandomIndex].pFunc)();
