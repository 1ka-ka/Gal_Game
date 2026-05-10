#include "UI/VNSettingsWidget.h"
#include "Game/VNGameMode.h"
#include "Core/VNGameInstance.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Border.h"
#include "Components/Spacer.h"
#include "Blueprint/WidgetTree.h"
#include "Brushes/SlateColorBrush.h"

void UVNSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BuildWidgetTree();
}

void UVNSettingsWidget::NativeConstruct()
{
	if (!bWidgetTreeBuilt)
	{
		BuildWidgetTree();
	}

	Super::NativeConstruct();

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (GI)
	{
		CurrentSettings = GI->GetGameSettings();
	}

	if (TextSpeedUpButton)
	{
		TextSpeedUpButton->OnClicked.Clear();
		TextSpeedUpButton->OnClicked.AddDynamic(this, &UVNSettingsWidget::OnTextSpeedUp);
	}
	if (TextSpeedDownButton)
	{
		TextSpeedDownButton->OnClicked.Clear();
		TextSpeedDownButton->OnClicked.AddDynamic(this, &UVNSettingsWidget::OnTextSpeedDown);
	}
	if (AutoDelayUpButton)
	{
		AutoDelayUpButton->OnClicked.Clear();
		AutoDelayUpButton->OnClicked.AddDynamic(this, &UVNSettingsWidget::OnAutoDelayUp);
	}
	if (AutoDelayDownButton)
	{
		AutoDelayDownButton->OnClicked.Clear();
		AutoDelayDownButton->OnClicked.AddDynamic(this, &UVNSettingsWidget::OnAutoDelayDown);
	}
	if (BGMVolumeUpButton)
	{
		BGMVolumeUpButton->OnClicked.Clear();
		BGMVolumeUpButton->OnClicked.AddDynamic(this, &UVNSettingsWidget::OnBGMVolumeUp);
	}
	if (BGMVolumeDownButton)
	{
		BGMVolumeDownButton->OnClicked.Clear();
		BGMVolumeDownButton->OnClicked.AddDynamic(this, &UVNSettingsWidget::OnBGMVolumeDown);
	}
	if (ApplyButton)
	{
		ApplyButton->OnClicked.Clear();
		ApplyButton->OnClicked.AddDynamic(this, &UVNSettingsWidget::OnApplyClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.Clear();
		BackButton->OnClicked.AddDynamic(this, &UVNSettingsWidget::OnBackClicked);
	}

	UpdateDisplayValues();
}

void UVNSettingsWidget::BuildWidgetTree()
{
	if (bWidgetTreeBuilt) return;
	bWidgetTreeBuilt = true;

	if (!WidgetTree) return;

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = RootCanvas;

	UImage* BgImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UCanvasPanelSlot* BgSlot = RootCanvas->AddChildToCanvas(BgImage);
	BgSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	BgSlot->SetOffsets(FMargin(0));
	BgSlot->SetZOrder(0);
	BgImage->SetBrush(FSlateColorBrush(FLinearColor(0.03f, 0.03f, 0.08f, 1.0f)));

	UBorder* PanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(PanelBorder);
	PanelSlot->SetAnchors(FAnchors(0.2f, 0.1f, 0.8f, 0.9f));
	PanelSlot->SetOffsets(FMargin(0));
	PanelSlot->SetZOrder(1);
	PanelBorder->SetBrush(FSlateColorBrush(FLinearColor(0.05f, 0.05f, 0.1f, 0.95f)));
	PanelBorder->SetPadding(FMargin(30, 20, 30, 20));

	UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	PanelBorder->SetContent(Content);

	UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TitleText->SetText(FText::FromString(TEXT("Settings")));
	FSlateFontInfo TitleFont = TitleText->GetFont();
	TitleFont.Size = 28;
	TitleFont.TypefaceFontName = FName(TEXT("Bold"));
	TitleText->SetFont(TitleFont);
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	TitleText->SetJustification(ETextJustify::Center);
	UVerticalBoxSlot* TitleSlot = Content->AddChildToVerticalBox(TitleText);
	TitleSlot->SetPadding(FMargin(0, 0, 0, 30));
	TitleSlot->SetHorizontalAlignment(HAlign_Center);

	auto CreateSettingRow = [&](const FString& Label, UTextBlock*& ValueText, UButton*& UpBtn, UButton*& DownBtn)
	{
		UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		UVerticalBoxSlot* RowSlot = Content->AddChildToVerticalBox(Row);
		RowSlot->SetPadding(FMargin(0, 8));

		UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		LabelText->SetText(FText::FromString(Label));
		FSlateFontInfo LabelFont = LabelText->GetFont();
		LabelFont.Size = 18;
		LabelText->SetFont(LabelFont);
		LabelText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.85f, 1.0f)));
		UHorizontalBoxSlot* LabelSlot = Row->AddChildToHorizontalBox(LabelText);
		LabelSlot->SetPadding(FMargin(0, 5, 20, 5));

		DownBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		FButtonStyle DownBtnStyle = DownBtn->GetStyle();
		DownBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.2f, 0.8f));
		DownBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.2f, 0.4f, 0.9f));
		DownBtn->SetStyle(DownBtnStyle);
		UTextBlock* DownText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		DownText->SetText(FText::FromString(TEXT(" - ")));
		DownText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		DownBtn->AddChild(DownText);
		UHorizontalBoxSlot* DownSlot = Row->AddChildToHorizontalBox(DownBtn);
		DownSlot->SetPadding(FMargin(5));

		ValueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		ValueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo ValueFont = ValueText->GetFont();
		ValueFont.Size = 18;
		ValueText->SetFont(ValueFont);
		ValueText->SetJustification(ETextJustify::Center);
		UHorizontalBoxSlot* ValueSlot = Row->AddChildToHorizontalBox(ValueText);
		ValueSlot->SetPadding(FMargin(10, 5));
		FSlateChildSize ValueSlotSize;
		ValueSlotSize.SizeRule = ESlateSizeRule::Automatic;
		ValueSlotSize.Value = 80.0f;
		ValueSlot->SetSize(ValueSlotSize);

		UpBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		FButtonStyle UpBtnStyle = UpBtn->GetStyle();
		UpBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.2f, 0.8f));
		UpBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.2f, 0.4f, 0.9f));
		UpBtn->SetStyle(UpBtnStyle);
		UTextBlock* UpText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		UpText->SetText(FText::FromString(TEXT(" + ")));
		UpText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UpBtn->AddChild(UpText);
		UHorizontalBoxSlot* UpSlot = Row->AddChildToHorizontalBox(UpBtn);
		UpSlot->SetPadding(FMargin(5));
	};

	CreateSettingRow(TEXT("Text Speed"), TextSpeedValue, TextSpeedUpButton, TextSpeedDownButton);
	CreateSettingRow(TEXT("Auto Delay"), AutoDelayValue, AutoDelayUpButton, AutoDelayDownButton);
	CreateSettingRow(TEXT("BGM Volume"), BGMVolumeValue, BGMVolumeUpButton, BGMVolumeDownButton);

	USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
	UVerticalBoxSlot* SpacerSlot = Content->AddChildToVerticalBox(Spacer);
	FSlateChildSize SpacerSlotSize;
	SpacerSlotSize.SizeRule = ESlateSizeRule::Automatic;
	SpacerSlotSize.Value = 40.0f;
	SpacerSlot->SetSize(SpacerSlotSize);

	UHorizontalBox* BtnRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UVerticalBoxSlot* BtnRowSlot = Content->AddChildToVerticalBox(BtnRow);
	BtnRowSlot->SetHorizontalAlignment(HAlign_Center);

	ApplyButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle ApplyBtnStyle = ApplyButton->GetStyle();
	ApplyBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.2f, 0.1f, 0.9f));
	ApplyBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.4f, 0.2f, 1.0f));
	ApplyButton->SetStyle(ApplyBtnStyle);
	UTextBlock* ApplyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ApplyText->SetText(FText::FromString(TEXT("Apply")));
	ApplyText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ApplyButton->AddChild(ApplyText);
	UHorizontalBoxSlot* ApplySlot = BtnRow->AddChildToHorizontalBox(ApplyButton);
	ApplySlot->SetPadding(FMargin(20, 5));

	BackButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle BackBtnStyle = BackButton->GetStyle();
	BackBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.15f, 0.1f, 0.1f, 0.8f));
	BackBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.3f, 0.15f, 0.15f, 0.9f));
	BackButton->SetStyle(BackBtnStyle);
	UTextBlock* BackText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	BackText->SetText(FText::FromString(TEXT("Back")));
	BackText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	BackButton->AddChild(BackText);
	UHorizontalBoxSlot* BackSlot = BtnRow->AddChildToHorizontalBox(BackButton);
	BackSlot->SetPadding(FMargin(20, 5));
}

void UVNSettingsWidget::SetSettingsCallbacks(AVNGameMode* InGameMode)
{
	GameModePtr = InGameMode;
}

void UVNSettingsWidget::UpdateDisplayValues()
{
	if (TextSpeedValue)
		TextSpeedValue->SetText(FText::FromString(FString::SanitizeFloat(CurrentSettings.TextSpeed, 1)));
	if (AutoDelayValue)
		AutoDelayValue->SetText(FText::FromString(FString::SanitizeFloat(CurrentSettings.AutoAdvanceDelay, 1)));
	if (BGMVolumeValue)
		BGMVolumeValue->SetText(FText::FromString(FString::SanitizeFloat(CurrentSettings.BGMVolume * 100, 0) + TEXT("%")));
}

void UVNSettingsWidget::ApplySettings()
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (GI)
	{
		GI->UpdateGameSettings(CurrentSettings);
	}
}

void UVNSettingsWidget::OnTextSpeedUp()
{
	CurrentSettings.TextSpeed = FMath::Min(CurrentSettings.TextSpeed + 5.0f, 100.0f);
	UpdateDisplayValues();
}

void UVNSettingsWidget::OnTextSpeedDown()
{
	CurrentSettings.TextSpeed = FMath::Max(CurrentSettings.TextSpeed - 5.0f, 5.0f);
	UpdateDisplayValues();
}

void UVNSettingsWidget::OnAutoDelayUp()
{
	CurrentSettings.AutoAdvanceDelay = FMath::Min(CurrentSettings.AutoAdvanceDelay + 0.5f, 10.0f);
	UpdateDisplayValues();
}

void UVNSettingsWidget::OnAutoDelayDown()
{
	CurrentSettings.AutoAdvanceDelay = FMath::Max(CurrentSettings.AutoAdvanceDelay - 0.5f, 0.5f);
	UpdateDisplayValues();
}

void UVNSettingsWidget::OnBGMVolumeUp()
{
	CurrentSettings.BGMVolume = FMath::Min(CurrentSettings.BGMVolume + 0.1f, 1.0f);
	UpdateDisplayValues();
}

void UVNSettingsWidget::OnBGMVolumeDown()
{
	CurrentSettings.BGMVolume = FMath::Max(CurrentSettings.BGMVolume - 0.1f, 0.0f);
	UpdateDisplayValues();
}

void UVNSettingsWidget::OnApplyClicked()
{
	ApplySettings();
}

void UVNSettingsWidget::OnBackClicked()
{
	RemoveFromParent();
	if (GameModePtr.IsValid())
	{
		GameModePtr->ShowMainMenu();
	}
}
