#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Data/VNSaveData.h"
#include "VNSettingsWidget.generated.h"

class AVNGameMode;

UCLASS()
class VISUALNOVEL_API UVNSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetSettingsCallbacks(AVNGameMode* InGameMode);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void ApplySettings();

private:
	void BuildWidgetTree();

	UPROPERTY()
	UTextBlock* TextSpeedValue;

	UPROPERTY()
	UTextBlock* AutoDelayValue;

	UPROPERTY()
	UTextBlock* BGMVolumeValue;

	UPROPERTY()
	UButton* TextSpeedUpButton;

	UPROPERTY()
	UButton* TextSpeedDownButton;

	UPROPERTY()
	UButton* AutoDelayUpButton;

	UPROPERTY()
	UButton* AutoDelayDownButton;

	UPROPERTY()
	UButton* BGMVolumeUpButton;

	UPROPERTY()
	UButton* BGMVolumeDownButton;

	UPROPERTY()
	UButton* ApplyButton;

	UPROPERTY()
	UButton* BackButton;

	TWeakObjectPtr<AVNGameMode> GameModePtr;
	FVNGameSettings CurrentSettings;
	bool bWidgetTreeBuilt = false;

	void UpdateDisplayValues();

	UFUNCTION()
	void OnTextSpeedUp();

	UFUNCTION()
	void OnTextSpeedDown();

	UFUNCTION()
	void OnAutoDelayUp();

	UFUNCTION()
	void OnAutoDelayDown();

	UFUNCTION()
	void OnBGMVolumeUp();

	UFUNCTION()
	void OnBGMVolumeDown();

	UFUNCTION()
	void OnApplyClicked();

	UFUNCTION()
	void OnBackClicked();
};
