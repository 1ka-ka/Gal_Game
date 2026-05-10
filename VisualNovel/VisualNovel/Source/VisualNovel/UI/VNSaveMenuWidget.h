#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Data/VNSaveData.h"
#include "VNSaveMenuWidget.generated.h"

class AVNGameMode;
class UVNBranchViewWidget;
class UVNStoryTextViewWidget;

UCLASS()
class VISUALNOVEL_API UVNSaveMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetSaveMenuCallbacks(AVNGameMode* InGameMode);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void RefreshSaveSlots();

private:
	void BuildWidgetTree();

	UPROPERTY()
	UVerticalBox* SaveSlotContainer;

	UPROPERTY()
	TArray<UButton*> SlotButtons;

	UPROPERTY()
	UButton* BackButton;

	UPROPERTY()
	UBorder* SlotDetailPanel;

	UPROPERTY()
	UButton* ContinueButton;

	UPROPERTY()
	UButton* BranchButton;

	UPROPERTY()
	UButton* StoryTextButton;

	UPROPERTY()
	UButton* DeleteButton;

	UPROPERTY()
	UTextBlock* SlotDetailText;

	UPROPERTY()
	UVNBranchViewWidget* BranchViewWidget;

	UPROPERTY()
	UVNStoryTextViewWidget* StoryTextViewWidget;

	TWeakObjectPtr<AVNGameMode> GameModePtr;
	int32 SelectedSlot = -1;
	TArray<FVNSaveSlotInfo> SlotInfos;
	TArray<int32> SlotButtonIndexMap;
	bool bWidgetTreeBuilt = false;

	UFUNCTION()
	void OnSlotButtonClicked();

	UFUNCTION()
	void OnSlotClicked(int32 SlotIndex);

	UFUNCTION()
	void OnBackClicked();

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnBranchClicked();

	UFUNCTION()
	void OnStoryTextClicked();

	UFUNCTION()
	void OnDeleteClicked();

	void ShowSlotDetail(int32 SlotIndex);
	void HideSlotDetail();
};
