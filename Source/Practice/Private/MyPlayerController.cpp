#include "MyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "MyGameState.h"
#include "Components/TextBlock.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MyCharacter.h"

AMyPlayerController::AMyPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr)
{
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapname = GetWorld()->GetMapName();
	if (CurrentMapname.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}


UUserWidget* AMyPlayerController::GetHUDWidget()
{
	return HUDWidgetInstance;
}

//게임화면 (아마)
void AMyPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

			AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
			if (MyGameState)
			{
				MyGameState->UpdateHUD();
			}
		}
	}
}

void AMyPlayerController::ShowWaveText()
{
	if (HUDWidgetInstance)
	{
		int32 CurrentWaveIndex = 0;
		if (GetWorld())
		{
			if (AMyGameState* MyGameState = GetWorld()->GetGameState<AMyGameState>())
			{
				CurrentWaveIndex = MyGameState->CurrentWaveIndex + 1;
			}
		}
		if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("WaveStartText"))))
		{
			WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d Start !"), CurrentWaveIndex)));
		}
		UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName("WaveStartWidget"));
		if (PlayAnimFunc)
		{
			HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}
	}
}

//메뉴화면
void AMyPlayerController::ShowMainMenu(bool IsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (IsRestart)
			{
				ButtonText->SetText(FText::FromString(FString::Printf(TEXT("Restart !"))));
			}
			else
			{
				ButtonText->SetText(FText::FromString(FString::Printf(TEXT("Start !"))));
			}
		}

		if (IsRestart)
		{
			if (UTextBlock* TitleText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("TitleText"))))
			{
				if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TitleText->SetText(FText::FromString(FString::Printf(TEXT("Game Over"))));
				}

			}

			if (UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim")))
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("TotalScore"))))
			{
				if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), MyGameInstance->TotalScore)));
				}
				
			}
		}
	}
}

void AMyPlayerController::StartGame()
{
	if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		MyGameInstance->CurrentLevelIndex = 0;
		MyGameInstance->CurrentWaveIndex = 0;
		MyGameInstance->TotalScore = 0;
	}
	if (APawn* Activator = GetPawn())
	{
		if (AMyCharacter* MyCharacter = Cast<AMyCharacter>(Activator))
		{
			MyCharacter->Health = MyCharacter->MaxHealth;
		}
	}

	SetPause(false);
	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
}