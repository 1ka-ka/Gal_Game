#include "Game/VNGameMode.h"
#include "Game/VNPlayerController.h"
#include "Core/VNGameInstance.h"
#include "Core/VNStoryEngine.h"
#include "Core/VNSaveManager.h"
#include "Core/VNAssetLoader.h"
#include "UI/VNMainMenuWidget.h"
#include "UI/VNGameWidget.h"
#include "UI/VNSaveMenuWidget.h"
#include "UI/VNSettingsWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

AVNGameMode::AVNGameMode()
{
	PlayerControllerClass = AVNPlayerController::StaticClass();
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AVNGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("VNGameMode::BeginPlay - START"));

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: GameInstance is NULL!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("VNGameMode: GameInstance OK, calling BindStoryEvents"));

	BindStoryEvents();

	UE_LOG(LogTemp, Warning, TEXT("VNGameMode: calling ShowMainMenu"));

	ShowMainMenu();

	UE_LOG(LogTemp, Warning, TEXT("VNGameMode::BeginPlay - END"));
}

void AVNGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsAutoMode && !bIsPaused)
	{
		UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
		if (GI && GI->IsGameInProgress())
		{
			AutoTimer += DeltaSeconds;
			FVNGameSettings Settings = GI->GetGameSettings();
			if (AutoTimer >= Settings.AutoAdvanceDelay)
			{
				AutoTimer = 0.0f;
				OnAdvanceDialogue();
			}
		}
	}
}

void AVNGameMode::BindStoryEvents()
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	UVNStoryEngine* Engine = GI->GetStoryEngine();
	if (!Engine) return;

	Engine->OnDialogueStarted.Clear();
	Engine->OnChoicePresented.Clear();
	Engine->OnBackgroundChanged.Clear();
	Engine->OnCharactersChanged.Clear();
	Engine->OnStoryEnd.Clear();

	Engine->OnDialogueStarted.AddDynamic(this, &AVNGameMode::OnDialogueStarted);
	Engine->OnChoicePresented.AddDynamic(this, &AVNGameMode::OnChoicePresented);
	Engine->OnBackgroundChanged.AddDynamic(this, &AVNGameMode::OnBackgroundChanged);
	Engine->OnCharactersChanged.AddDynamic(this, &AVNGameMode::OnCharactersChanged);
	Engine->OnStoryEnd.AddDynamic(this, &AVNGameMode::OnStoryEnd);
}

void AVNGameMode::ShowMainMenu()
{
	HideAllUI();

	UE_LOG(LogTemp, Warning, TEXT("VNGameMode::ShowMainMenu - creating widget"));

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: PlayerController is NULL!"));
		return;
	}

	if (!MainMenuWidget)
	{
		MainMenuWidget = CreateWidget<UVNMainMenuWidget>(PC, UVNMainMenuWidget::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("VNGameMode: MainMenuWidget created via PlayerController"));
	}

	if (MainMenuWidget)
	{
		MainMenuWidget->AddToViewport(100);
		MainMenuWidget->SetMainMenuCallbacks(this);
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

		UE_LOG(LogTemp, Warning, TEXT("VNGameMode: MainMenuWidget added to viewport Z=100"));
		UE_LOG(LogTemp, Warning, TEXT("VNGameMode: Widget IsInViewport=%d, Visibility=%d"),
			MainMenuWidget->IsInViewport() ? 1 : 0,
			(int32)MainMenuWidget->GetVisibility());

		if (MainMenuWidget->WidgetTree && MainMenuWidget->WidgetTree->RootWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("VNGameMode: WidgetTree RootWidget is VALID"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("VNGameMode: WidgetTree RootWidget is NULL!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: Failed to create MainMenuWidget!"));
	}

	PC->SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
	UE_LOG(LogTemp, Warning, TEXT("VNGameMode: Mouse cursor enabled, input set to UIOnly"));
}

void AVNGameMode::ShowGameUI()
{
	HideAllUI();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	if (!GameWidget)
	{
		GameWidget = CreateWidget<UVNGameWidget>(PC, UVNGameWidget::StaticClass());
	}

	if (GameWidget)
	{
		GameWidget->AddToViewport(50);
		GameWidget->SetGameCallbacks(this);
		GameWidget->SetVisibility(ESlateVisibility::Visible);
	}

	PC->SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
}

void AVNGameMode::ShowSaveMenu()
{
	HideAllUI();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	if (!SaveMenuWidget)
	{
		SaveMenuWidget = CreateWidget<UVNSaveMenuWidget>(PC, UVNSaveMenuWidget::StaticClass());
	}

	if (SaveMenuWidget)
	{
		SaveMenuWidget->AddToViewport(100);
		SaveMenuWidget->SetSaveMenuCallbacks(this);
		SaveMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}

	PC->SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
}

void AVNGameMode::ShowSettings()
{
	HideAllUI();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	if (!SettingsWidget)
	{
		SettingsWidget = CreateWidget<UVNSettingsWidget>(PC, UVNSettingsWidget::StaticClass());
	}

	if (SettingsWidget)
	{
		SettingsWidget->AddToViewport(100);
		SettingsWidget->SetSettingsCallbacks(this);
		SettingsWidget->SetVisibility(ESlateVisibility::Visible);
	}

	PC->SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
}

void AVNGameMode::HideAllUI()
{
	if (MainMenuWidget)
	{
		if (MainMenuWidget->IsInViewport())
			MainMenuWidget->RemoveFromParent();
		MainMenuWidget = nullptr;
	}
	if (GameWidget)
	{
		if (GameWidget->IsInViewport())
			GameWidget->RemoveFromParent();
		GameWidget = nullptr;
	}
	if (SaveMenuWidget)
	{
		if (SaveMenuWidget->IsInViewport())
			SaveMenuWidget->RemoveFromParent();
		SaveMenuWidget = nullptr;
	}
	if (SettingsWidget)
	{
		if (SettingsWidget->IsInViewport())
			SettingsWidget->RemoveFromParent();
		SettingsWidget = nullptr;
	}
}

void AVNGameMode::OnStartNewGame()
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	ShowGameUI();
	GI->StartNewGame();

	if (!GI->IsGameInProgress())
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: Failed to start new game"));
		HideAllUI();
		ShowMainMenu();
	}
}

void AVNGameMode::OnLoadGame(int32 SlotIndex)
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	ShowGameUI();
	GI->LoadGameFromSave(SlotIndex);

	if (!GI->IsGameInProgress())
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: Failed to load game from slot %d"), SlotIndex);
		HideAllUI();
		ShowMainMenu();
	}
}

void AVNGameMode::OnSaveGame(int32 SlotIndex)
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	GI->SaveCurrentGame(SlotIndex);
}

void AVNGameMode::OnEndGame(bool bSave)
{
	bIsPaused = false;
	bIsAutoMode = false;
	AutoTimer = 0.0f;

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI)
	{
		ShowMainMenu();
		return;
	}

	if (bSave)
	{
		int32 SaveSlotIdx = -1;

		if (GI->IsFromSave() && GI->GetActiveSaveSlot() >= 0)
		{
			SaveSlotIdx = GI->GetActiveSaveSlot();
		}
		else
		{
			UVNSaveManager* SM = GI->GetSaveManager();
			if (SM)
			{
				for (int32 i = 0; i < SM->GetMaxSaveSlots(); i++)
				{
					if (!SM->HasSave(i))
					{
						SaveSlotIdx = i;
						break;
					}
				}
			}
		}

		if (SaveSlotIdx >= 0)
		{
			GI->SaveCurrentGame(SaveSlotIdx);
			UE_LOG(LogTemp, Warning, TEXT("VNGameMode: Game saved to slot %d on end"), SaveSlotIdx);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("VNGameMode: No save slot available on end"));
		}
	}

	GI->EndCurrentGame(bSave);
	ShowMainMenu();
}

void AVNGameMode::OnAdvanceDialogue()
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI || !GI->IsGameInProgress()) return;

	UVNStoryEngine* Engine = GI->GetStoryEngine();
	if (Engine && !Engine->IsAtChoice() && !Engine->IsStoryEnd())
	{
		Engine->AdvanceStory();
	}
}

void AVNGameMode::OnMakeChoice(const FString& ChoiceId)
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	UVNStoryEngine* Engine = GI->GetStoryEngine();
	if (Engine)
	{
		Engine->MakeChoice(ChoiceId);
	}
}

void AVNGameMode::OnTogglePause()
{
	bIsPaused = !bIsPaused;
	if (GameWidget)
	{
		GameWidget->SetPauseState(bIsPaused);
	}
}

void AVNGameMode::OnToggleAuto()
{
	bIsAutoMode = !bIsAutoMode;
	AutoTimer = 0.0f;
	if (GameWidget)
	{
		GameWidget->SetAutoState(bIsAutoMode);
	}
}

void AVNGameMode::OnReturnToMainMenu()
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	
	if (GameWidget)
	{
		GameWidget->HideStoryEnd();
		GameWidget->HideSaveConfirmDialog();
		GameWidget->SetPauseState(false);
		GameWidget->ClearCharacters();
		GameWidget->ClearChoices();
		GameWidget->RemoveFromParent();
		GameWidget = nullptr;
	}
	
	if (SaveMenuWidget)
	{
		if (SaveMenuWidget->IsInViewport())
		{
			SaveMenuWidget->RemoveFromParent();
		}
		SaveMenuWidget = nullptr;
	}
	
	if (SettingsWidget)
	{
		if (SettingsWidget->IsInViewport())
		{
			SettingsWidget->RemoveFromParent();
		}
		SettingsWidget = nullptr;
	}
	
	bIsPaused = false;
	bIsAutoMode = false;
	AutoTimer = 0.0f;
	
	if (GI)
	{
		GI->ReturnToMainMenu();
		
		UVNStoryEngine* Engine = GI->GetStoryEngine();
		if (Engine)
		{
			Engine->Reset();
		}
	}
	
	if (MainMenuWidget)
	{
		if (MainMenuWidget->IsInViewport())
		{
			MainMenuWidget->RemoveFromParent();
		}
		MainMenuWidget = nullptr;
	}
	
	ShowMainMenu();
}

void AVNGameMode::OnDialogueStarted(FString Speaker, FString Text)
{
	UE_LOG(LogTemp, Log, TEXT("VNGameMode: OnDialogueStarted - Speaker=%s, Text=%s"), *Speaker, *Text.Left(50));
	if (GameWidget)
	{
		GameWidget->DisplayDialogue(Speaker, Text);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: OnDialogueStarted - GameWidget is NULL!"));
	}
}

void AVNGameMode::OnChoicePresented(const TArray<FVNChoice>& Choices)
{
	if (GameWidget)
	{
		GameWidget->DisplayChoices(Choices);
	}
}

void AVNGameMode::OnBackgroundChanged(FString BackgroundId)
{
	UE_LOG(LogTemp, Log, TEXT("VNGameMode: OnBackgroundChanged - BackgroundId=%s"), *BackgroundId);
	
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	UVNAssetLoader* Loader = GI->GetAssetLoader();
	if (Loader && GameWidget)
	{
		UTexture2D* BgTexture = Loader->LoadBackground(BackgroundId);
		if (BgTexture)
		{
			UE_LOG(LogTemp, Log, TEXT("VNGameMode: Background loaded successfully"));
			GameWidget->SetBackground(BgTexture);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("VNGameMode: Failed to load background: %s"), *BackgroundId);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameMode: OnBackgroundChanged - Loader=%s, GameWidget=%s"), 
			Loader ? TEXT("VALID") : TEXT("NULL"), GameWidget ? TEXT("VALID") : TEXT("NULL"));
	}
}

void AVNGameMode::OnCharactersChanged(const TArray<FVNCharacterDisplay>& Characters)
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	UVNAssetLoader* Loader = GI->GetAssetLoader();
	if (Loader && GameWidget)
	{
		TArray<UTexture2D*> CharacterTextures;
		TArray<EVNCharacterPosition> Positions;

		for (const FVNCharacterDisplay& Char : Characters)
		{
			UTexture2D* Tex = Loader->LoadCharacterImage(Char.CharacterId, Char.Expression);
			CharacterTextures.Add(Tex);
			Positions.Add(Char.Position);
		}

		GameWidget->SetCharacters(CharacterTextures, Positions);
	}
}

void AVNGameMode::OnStoryEnd()
{
	if (GameWidget)
	{
		GameWidget->ShowStoryEnd();
	}
}
