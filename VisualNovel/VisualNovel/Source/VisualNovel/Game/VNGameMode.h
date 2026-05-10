#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Data/VNStoryData.h"
#include "VNGameMode.generated.h"

class UVNGameWidget;
class UVNMainMenuWidget;
class UVNSaveMenuWidget;
class UVNSettingsWidget;

UCLASS()
class VISUALNOVEL_API AVNGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVNGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void ShowGameUI();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void ShowSaveMenu();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void ShowSettings();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void HideAllUI();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnStartNewGame();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnLoadGame(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnSaveGame(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnEndGame(bool bSave);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnAdvanceDialogue();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnMakeChoice(const FString& ChoiceId);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnTogglePause();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnToggleAuto();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	void OnReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	bool IsPaused() const { return bIsPaused; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel")
	bool IsAutoMode() const { return bIsAutoMode; }

private:
	UPROPERTY()
	UVNMainMenuWidget* MainMenuWidget;

	UPROPERTY()
	UVNGameWidget* GameWidget;

	UPROPERTY()
	UVNSaveMenuWidget* SaveMenuWidget;

	UPROPERTY()
	UVNSettingsWidget* SettingsWidget;

	bool bIsPaused = false;
	bool bIsAutoMode = false;
	float AutoTimer = 0.0f;

	void BindStoryEvents();

	UFUNCTION()
	void OnDialogueStarted(FString Speaker, FString Text);

	UFUNCTION()
	void OnChoicePresented(const TArray<FVNChoice>& Choices);

	UFUNCTION()
	void OnBackgroundChanged(FString BackgroundId);

	UFUNCTION()
	void OnCharactersChanged(const TArray<FVNCharacterDisplay>& Characters);

	UFUNCTION()
	void OnStoryEnd();
};
