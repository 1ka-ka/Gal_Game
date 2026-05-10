#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "VNMainMenuWidget.generated.h"

class AVNGameMode;

UCLASS()
class VISUALNOVEL_API UVNMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UVNMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetMainMenuCallbacks(AVNGameMode* InGameMode);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void SetBackground(UTexture2D* Texture);

private:
	void BuildWidgetTree();

	UPROPERTY()
	UImage* BackgroundImage;

	UPROPERTY()
	UTextBlock* TitleText;

	UPROPERTY()
	UButton* StartButton;

	UPROPERTY()
	UButton* LoadButton;

	UPROPERTY()
	UButton* SettingsButton;

	TWeakObjectPtr<AVNGameMode> GameModePtr;

	bool bWidgetTreeBuilt = false;

	UFUNCTION()
	void OnStartClicked();

	UFUNCTION()
	void OnLoadClicked();

	UFUNCTION()
	void OnSettingsClicked();
};
