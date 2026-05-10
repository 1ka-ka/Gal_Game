#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"
#include "Data/VNStoryData.h"
#include "VNGameWidget.generated.h"

class AVNGameMode;

UCLASS()
class VISUALNOVEL_API UVNGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UVNGameWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetGameCallbacks(AVNGameMode* InGameMode);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void DisplayDialogue(const FString& Speaker, const FString& Text);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void DisplayChoices(const TArray<FVNChoice>& Choices);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void ClearChoices();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetBackground(UTexture2D* Texture);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetCharacters(const TArray<UTexture2D*>& Textures, const TArray<EVNCharacterPosition>& Positions);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void ClearCharacters();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetPauseState(bool bPaused);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetAutoState(bool bAuto);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void ShowStoryEnd();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void HideStoryEnd();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void ShowSaveConfirmDialog();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void HideSaveConfirmDialog();

private:
	void BuildWidgetTree();

	UPROPERTY()
	UImage* BackgroundImage;

	UPROPERTY()
	UVerticalBox* CharacterContainer;

	UPROPERTY()
	TArray<UImage*> CharacterImages;

	UPROPERTY()
	UBorder* DialogueBoxBorder;

	UPROPERTY()
	UButton* DialogueClickButton;

	UPROPERTY()
	UTextBlock* SpeakerText;

	UPROPERTY()
	UTextBlock* DialogueText;

	UPROPERTY()
	UVerticalBox* ChoiceContainer;

	UPROPERTY()
	TArray<UButton*> ChoiceButtons;

	UPROPERTY()
	TArray<int32> ChoiceButtonIndexMap;

	UPROPERTY()
	UHorizontalBox* TopMenuBar;

	UPROPERTY()
	UButton* PauseButton;

	UPROPERTY()
	UButton* SaveButton;

	UPROPERTY()
	UButton* AutoButton;

	UPROPERTY()
	UButton* EndButton;

	UPROPERTY()
	UTextBlock* PauseButtonText;

	UPROPERTY()
	UTextBlock* AutoButtonText;

	UPROPERTY()
	UBorder* PauseOverlay;

	UPROPERTY()
	UButton* PauseClickButton;

	UPROPERTY()
	UBorder* SaveConfirmDialog;

	UPROPERTY()
	UButton* ConfirmSaveYesButton;

	UPROPERTY()
	UButton* ConfirmSaveNoButton;

	UPROPERTY()
	UBorder* StoryEndOverlay;

	UPROPERTY()
	UButton* StoryEndButton;

	TWeakObjectPtr<AVNGameMode> GameModePtr;

	FString FullDialogueText;
	int32 CurrentCharIndex = 0;
	float TypewriterTimer = 0.0f;
	bool bIsTyping = false;
	bool bWidgetTreeBuilt = false;

	UFUNCTION()
	void OnDialogueBoxClicked();

	UFUNCTION()
	void OnPauseClicked();

	UFUNCTION()
	void OnSaveClicked();

	UFUNCTION()
	void OnAutoClicked();

	UFUNCTION()
	void OnEndClicked();

	UFUNCTION()
	void OnChoiceButtonClicked();

	UFUNCTION()
	void OnChoiceClicked(int32 ChoiceIndex);

	UFUNCTION()
	void OnSaveConfirmYes();

	UFUNCTION()
	void OnSaveConfirmNo();

	UFUNCTION()
	void OnStoryEndClicked();
};
