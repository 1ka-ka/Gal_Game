#include "Core/VNGameInstance.h"
#include "Core/VNStoryEngine.h"
#include "Core/VNSaveManager.h"
#include "Core/VNAssetLoader.h"
#include "Kismet/GameplayStatics.h"

UVNGameInstance::UVNGameInstance()
{
	StoryEngine = nullptr;
	SaveManager = nullptr;
	AssetLoader = nullptr;
}

void UVNGameInstance::Init()
{
	Super::Init();

	StoryEngine = NewObject<UVNStoryEngine>(this, FName(TEXT("StoryEngine")));
	SaveManager = NewObject<UVNSaveManager>(this, FName(TEXT("SaveManager")));
	AssetLoader = NewObject<UVNAssetLoader>(this, FName(TEXT("AssetLoader")));

	GameSettings = SaveManager->LoadSettings();

	UE_LOG(LogTemp, Log, TEXT("VNGameInstance: Initialized"));
}

void UVNGameInstance::StartNewGame()
{
	if (!StoryEngine->LoadStory(DefaultStoryId))
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameInstance: Failed to load story: %s"), *DefaultStoryId);
		return;
	}

	StoryEngine->StartNewGame();
	CurrentGameMode = EVNGameMode::Gameplay;
	bGameInProgress = true;
	bIsFromSave = false;
	ActiveSaveSlot = -1;

	UE_LOG(LogTemp, Log, TEXT("VNGameInstance: New game started"));
}

void UVNGameInstance::LoadGameFromSave(int32 SlotIndex)
{
	FVNSaveData SaveData;
	if (!SaveManager->LoadGame(SlotIndex, SaveData))
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameInstance: Failed to load save from slot %d"), SlotIndex);
		return;
	}

	if (!StoryEngine->LoadStory(SaveData.StoryId))
	{
		UE_LOG(LogTemp, Error, TEXT("VNGameInstance: Failed to load story: %s"), *SaveData.StoryId);
		return;
	}

	SaveData.bIsFromSave = true;
	SaveData.OriginalSaveSlot = SlotIndex;
	StoryEngine->ResumeFromSave(SaveData);
	CurrentGameMode = EVNGameMode::Gameplay;
	bGameInProgress = true;
	bIsFromSave = true;
	ActiveSaveSlot = SlotIndex;

	UE_LOG(LogTemp, Log, TEXT("VNGameInstance: Game loaded from slot %d"), SlotIndex);
}

void UVNGameInstance::SaveCurrentGame(int32 SlotIndex)
{
	if (!bGameInProgress || !StoryEngine) return;

	FVNSaveData SaveData = StoryEngine->CreateSaveData(SlotIndex);
	SaveData.bIsFromSave = bIsFromSave;
	SaveData.OriginalSaveSlot = ActiveSaveSlot;

	SaveManager->SaveGame(SlotIndex, SaveData);

	if (!bIsFromSave)
	{
		ActiveSaveSlot = SlotIndex;
		bIsFromSave = true;
	}

	UE_LOG(LogTemp, Log, TEXT("VNGameInstance: Game saved to slot %d"), SlotIndex);
}

void UVNGameInstance::EndCurrentGame(bool bSave)
{
	if (bSave && bGameInProgress)
	{
		if (bIsFromSave && ActiveSaveSlot >= 0)
		{
			SaveCurrentGame(ActiveSaveSlot);
		}
		else if (!bIsFromSave)
		{
			for (int32 i = 0; i < SaveManager->GetMaxSaveSlots(); i++)
			{
				if (!SaveManager->HasSave(i))
				{
					SaveCurrentGame(i);
					break;
				}
			}
		}
	}

	bGameInProgress = false;
	CurrentGameMode = EVNGameMode::MainMenu;
	ActiveSaveSlot = -1;
	bIsFromSave = false;

	UE_LOG(LogTemp, Log, TEXT("VNGameInstance: Game ended (saved: %s)"), bSave ? TEXT("true") : TEXT("false"));
}

void UVNGameInstance::ReturnToMainMenu()
{
	CurrentGameMode = EVNGameMode::MainMenu;
}

void UVNGameInstance::UpdateGameSettings(const FVNGameSettings& NewSettings)
{
	GameSettings = NewSettings;
	if (SaveManager)
	{
		SaveManager->SaveSettings(GameSettings);
	}
}
