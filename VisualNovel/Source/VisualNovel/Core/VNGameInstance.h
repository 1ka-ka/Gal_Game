#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Data/VNStoryData.h"
#include "Data/VNSaveData.h"
#include "VNGameInstance.generated.h"

class UVNStoryEngine;
class UVNSaveManager;
class UVNAssetLoader;

UENUM(BlueprintType)
enum class EVNGameMode : uint8
{
	MainMenu UMETA(DisplayName = "MainMenu"),
	Gameplay UMETA(DisplayName = "Gameplay"),
	SaveMenu UMETA(DisplayName = "SaveMenu"),
	Settings UMETA(DisplayName = "Settings"),
	Paused UMETA(DisplayName = "Paused")
};

UCLASS()
class VISUALNOVEL_API UVNGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UVNGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	UVNStoryEngine* GetStoryEngine() const { return StoryEngine; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	UVNSaveManager* GetSaveManager() const { return SaveManager; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	UVNAssetLoader* GetAssetLoader() const { return AssetLoader; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void LoadGameFromSave(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void SaveCurrentGame(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void EndCurrentGame(bool bSave);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	EVNGameMode GetCurrentGameMode() const { return CurrentGameMode; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void SetCurrentGameMode(EVNGameMode Mode) { CurrentGameMode = Mode; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	bool IsGameInProgress() const { return bGameInProgress; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	FVNGameSettings GetGameSettings() const { return GameSettings; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void UpdateGameSettings(const FVNGameSettings& NewSettings);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	int32 GetActiveSaveSlot() const { return ActiveSaveSlot; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	bool IsFromSave() const { return bIsFromSave; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	FString GetDefaultStoryId() const { return DefaultStoryId; }

private:
	UPROPERTY()
	UVNStoryEngine* StoryEngine;

	UPROPERTY()
	UVNSaveManager* SaveManager;

	UPROPERTY()
	UVNAssetLoader* AssetLoader;

	UPROPERTY()
	EVNGameMode CurrentGameMode = EVNGameMode::MainMenu;

	UPROPERTY()
	FVNGameSettings GameSettings;

	UPROPERTY()
	int32 ActiveSaveSlot = -1;

	UPROPERTY()
	bool bGameInProgress = false;

	UPROPERTY()
	bool bIsFromSave = false;

	UPROPERTY(EditDefaultsOnly, Category = "VisualNovel")
	FString DefaultStoryId = TEXT("main_story");
};
