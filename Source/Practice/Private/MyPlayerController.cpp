#include "MyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "MyGameState.h"
#include "Components/TextBlock.h"
#include "MyGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyCharacter.h"
#include "Components/Image.h"

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
	if (IsValid(HUDWidgetInstance))
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (IsValid(MainMenuWidgetInstance))
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetClass is null"));
		return;
	}

	HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);

	if (!IsValid(HUDWidgetInstance))
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance create failed"));
		return;
	}

	HUDWidgetInstance->AddToViewport();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	AMyGameState* MyGameState = GetWorld() ? GetWorld()->GetGameState<AMyGameState>() : nullptr;
	if (IsValid(MyGameState))
	{
		MyGameState->UpdateHUD();
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

void AMyPlayerController::ShowThornWarning()
{
	if (HUDWidgetInstance)
	{
		UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName("ThornWarning"));
		if (PlayAnimFunc)
		{
			HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}
	}
}

void AMyPlayerController::ShowSlowImage()
{
	if (UImage* SlowImage = Cast<UImage>(HUDWidgetInstance->GetWidgetFromName(TEXT("SlowImage"))))
	{
		SlowImage->SetVisibility(ESlateVisibility::Visible);

		FTimerDelegate SpeedBackDelegate;
		SpeedBackDelegate.BindLambda([SlowImage]()
			{
				SlowImage->SetVisibility(ESlateVisibility::Collapsed);
			});

		GetWorld()->GetTimerManager().SetTimer(
			SpeedBackWidget,
			SpeedBackDelegate,
			8.0f,
			false
		);
	}
}

void AMyPlayerController::ShowReverseImage()
{
	if (UImage* ReverseImage = Cast<UImage>(HUDWidgetInstance->GetWidgetFromName(TEXT("ReverseImage"))))
	{
		ReverseImage->SetVisibility(ESlateVisibility::Visible);

		FTimerDelegate MoveBackDelegate;
		MoveBackDelegate.BindLambda([ReverseImage]()
			{
				ReverseImage->SetVisibility(ESlateVisibility::Collapsed);
			});

		GetWorld()->GetTimerManager().SetTimer(
			MoveBackWidget,
			MoveBackDelegate,
			8.0f,
			false
		);
	}
}

//메뉴화면
void AMyPlayerController::ShowMainMenu(bool IsRestart)
{
	if (IsValid(HUDWidgetInstance))
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (IsValid(MainMenuWidgetInstance))
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (!MainMenuWidgetClass)
	{
		return;
	}

	MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);

	if (!IsValid(MainMenuWidgetInstance))
	{
		return;
	}

	MainMenuWidgetInstance->AddToViewport();

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());

	if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
	{
		if (IsRestart)
		{
			ButtonText->SetText(FText::FromString(FString::Printf(TEXT("Restart"))));
		}
		else
		{
			ButtonText->SetText(FText::FromString(FString::Printf(TEXT("Start"))));
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

		if (UTextBlock* QuitText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("QuitButtonText"))))
		{
			if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this)))
			{
				QuitText->SetText(FText::FromString(FString::Printf(TEXT("Main\nMenu"))));
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

	SetPause(false);
	UGameplayStatics::OpenLevel(this, FName("BasicLevel"));
}

void AMyPlayerController::QuitGame()
{
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);

	if (CurrentLevelName == TEXT("MenuLevel"))
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

		UKismetSystemLibrary::QuitGame(
			this,
			PlayerController,
			EQuitPreference::Quit,
			true
		);

		return;
	}

	if (IsValid(HUDWidgetInstance))
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (IsValid(MainMenuWidgetInstance))
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	SetPause(false);
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	UGameplayStatics::OpenLevel(this, FName("MenuLevel"));
}