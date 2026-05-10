#include "UI/VNGameWidget.h"
#include "Game/VNGameMode.h"
#include "Core/VNGameInstance.h"
#include "Core/VNSaveManager.h"
#include "Core/VNStoryEngine.h"
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
#include "Components/BorderSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Spacer.h"
#include "Blueprint/WidgetTree.h"
#include "Brushes/SlateColorBrush.h"

UVNGameWidget::UVNGameWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVNGameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BuildWidgetTree();
}

void UVNGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PauseButton)
		PauseButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnPauseClicked);
	if (SaveButton)
		SaveButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnSaveClicked);
	if (AutoButton)
		AutoButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnAutoClicked);
	if (EndButton)
		EndButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnEndClicked);
	if (DialogueClickButton)
		DialogueClickButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnDialogueBoxClicked);
	if (ConfirmSaveYesButton)
		ConfirmSaveYesButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnSaveConfirmYes);
	if (ConfirmSaveNoButton)
		ConfirmSaveNoButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnSaveConfirmNo);
	if (StoryEndButton)
		StoryEndButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnStoryEndClicked);
	if (PauseClickButton)
		PauseClickButton->OnClicked.AddDynamic(this, &UVNGameWidget::OnPauseClicked);
}

void UVNGameWidget::BuildWidgetTree()
{
	if (bWidgetTreeBuilt) return;
	bWidgetTreeBuilt = true;

	if (!WidgetTree) return;

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = RootCanvas;

	BackgroundImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UCanvasPanelSlot* BgSlot = RootCanvas->AddChildToCanvas(BackgroundImage);
	BgSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	BgSlot->SetOffsets(FMargin(0));
	BgSlot->SetZOrder(0);
	BackgroundImage->SetBrush(FSlateColorBrush(FLinearColor::Black));

	CharacterContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UCanvasPanelSlot* CharSlot = RootCanvas->AddChildToCanvas(CharacterContainer);
	CharSlot->SetAnchors(FAnchors(0, 0, 1, 0.7f));
	CharSlot->SetOffsets(FMargin(0));
	CharSlot->SetZOrder(1);

	TopMenuBar = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UCanvasPanelSlot* TopSlot = RootCanvas->AddChildToCanvas(TopMenuBar);
	TopSlot->SetAnchors(FAnchors(0, 0, 1, 0));
	TopSlot->SetOffsets(FMargin(10, 10, -10, 40));
	TopSlot->SetZOrder(10);

	auto CreateTopButton = [&](const FString& Label) -> UButton*
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		FButtonStyle BtnStyle = Btn->GetStyle();
		BtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.15f, 0.7f));
		BtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.2f, 0.3f, 0.9f));
		BtnStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.3f, 0.3f, 0.4f, 1.0f));
		Btn->SetStyle(BtnStyle);

		UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Txt->SetText(FText::FromString(Label));
		FSlateFontInfo Font = Txt->GetFont();
		Font.Size = 14;
		Txt->SetFont(Font);
		Txt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Txt->SetJustification(ETextJustify::Center);
		Btn->AddChild(Txt);

		UHorizontalBoxSlot* BtnSlot = TopMenuBar->AddChildToHorizontalBox(Btn);
		BtnSlot->SetPadding(FMargin(5, 2, 5, 2));

		return Btn;
	};

	PauseButton = CreateTopButton(TEXT("Pause"));
	SaveButton = CreateTopButton(TEXT("Save"));
	AutoButton = CreateTopButton(TEXT("Auto"));
	EndButton = CreateTopButton(TEXT("End"));

	PauseButtonText = Cast<UTextBlock>(PauseButton->GetChildAt(0));
	AutoButtonText = Cast<UTextBlock>(AutoButton->GetChildAt(0));

	DialogueBoxBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UCanvasPanelSlot* DlgSlot = RootCanvas->AddChildToCanvas(DialogueBoxBorder);
	DlgSlot->SetAnchors(FAnchors(0.05f, 0.7f, 0.95f, 0.95f));
	DlgSlot->SetOffsets(FMargin(0));
	DlgSlot->SetZOrder(5);
	DialogueBoxBorder->SetBrush(FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.05f, 0.85f)));
	DialogueBoxBorder->SetPadding(FMargin(20, 15, 20, 15));

	UCanvasPanel* DlgPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	DialogueBoxBorder->SetContent(DlgPanel);

	SpeakerText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	SpeakerText->SetText(FText::GetEmpty());
	FSlateFontInfo SpeakerFont = SpeakerText->GetFont();
	SpeakerFont.Size = 18;
	SpeakerFont.TypefaceFontName = FName(TEXT("Bold"));
	SpeakerText->SetFont(SpeakerFont);
	SpeakerText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.85f, 1.0f, 1.0f)));
	UCanvasPanelSlot* SpeakerSlot = DlgPanel->AddChildToCanvas(SpeakerText);
	SpeakerSlot->SetAnchors(FAnchors(0, 0, 1, 0));
	SpeakerSlot->SetOffsets(FMargin(0, 0, 0, 30));
	SpeakerSlot->SetPosition(FVector2D(0, 5));

	DialogueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	DialogueText->SetText(FText::GetEmpty());
	FSlateFontInfo DlgFont = DialogueText->GetFont();
	DlgFont.Size = 16;
	DialogueText->SetFont(DlgFont);
	DialogueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	DialogueText->SetAutoWrapText(true);
	UCanvasPanelSlot* DlgTextSlot = DlgPanel->AddChildToCanvas(DialogueText);
	DlgTextSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	DlgTextSlot->SetOffsets(FMargin(0, 35, 0, 0));

	DialogueClickButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle ClickBtnStyle = DialogueClickButton->GetStyle();
	ClickBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	ClickBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	ClickBtnStyle.Pressed.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	DialogueClickButton->SetStyle(ClickBtnStyle);
	DialogueClickButton->SetBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
	DialogueClickButton->SetClickMethod(EButtonClickMethod::MouseDown);

	UTextBlock* ClickHintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ClickHintText->SetText(FText::FromString(TEXT("▼")));
	FSlateFontInfo ClickFont = ClickHintText->GetFont();
	ClickFont.Size = 16;
	ClickHintText->SetFont(ClickFont);
	ClickHintText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.6f)));
	ClickHintText->SetJustification(ETextJustify::Center);
	DialogueClickButton->AddChild(ClickHintText);

	UCanvasPanelSlot* ClickBtnSlot = DlgPanel->AddChildToCanvas(DialogueClickButton);
	ClickBtnSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	ClickBtnSlot->SetOffsets(FMargin(0));
	ClickBtnSlot->SetZOrder(10);

	ChoiceContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UCanvasPanelSlot* ChoiceSlot = RootCanvas->AddChildToCanvas(ChoiceContainer);
	ChoiceSlot->SetAnchors(FAnchors(0.2f, 0.3f, 0.8f, 0.65f));
	ChoiceSlot->SetOffsets(FMargin(0));
	ChoiceSlot->SetZOrder(6);

	PauseOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UCanvasPanelSlot* PauseSlot = RootCanvas->AddChildToCanvas(PauseOverlay);
	PauseSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	PauseSlot->SetOffsets(FMargin(0));
	PauseSlot->SetZOrder(20);
	PauseOverlay->SetBrush(FSlateColorBrush(FLinearColor(0, 0, 0, 0.6f)));
	PauseOverlay->SetVisibility(ESlateVisibility::Hidden);

	UVerticalBox* PauseContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	PauseOverlay->SetContent(PauseContent);

	USpacer* PauseTopSpacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
	PauseContent->AddChildToVerticalBox(PauseTopSpacer)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	UTextBlock* PauseText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	PauseText->SetText(FText::FromString(TEXT("PAUSED")));
	PauseText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo PauseFont = PauseText->GetFont();
	PauseFont.Size = 36;
	PauseText->SetFont(PauseFont);
	PauseText->SetJustification(ETextJustify::Center);
	UVerticalBoxSlot* PauseTextSlot = PauseContent->AddChildToVerticalBox(PauseText);
	PauseTextSlot->SetHorizontalAlignment(HAlign_Center);
	PauseTextSlot->SetPadding(FMargin(0, 0, 0, 20));

	PauseClickButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle ResumeBtnStyle = PauseClickButton->GetStyle();
	ResumeBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.2f, 0.3f, 0.8f));
	ResumeBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.3f, 0.5f, 0.9f));
	PauseClickButton->SetStyle(ResumeBtnStyle);
	UTextBlock* ResumeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ResumeText->SetText(FText::FromString(TEXT("Click to Resume")));
	ResumeText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo ResumeFont = ResumeText->GetFont();
	ResumeFont.Size = 20;
	ResumeText->SetFont(ResumeFont);
	ResumeText->SetJustification(ETextJustify::Center);
	PauseClickButton->AddChild(ResumeText);
	UVerticalBoxSlot* ResumeSlot = PauseContent->AddChildToVerticalBox(PauseClickButton);
	ResumeSlot->SetHorizontalAlignment(HAlign_Center);
	ResumeSlot->SetPadding(FMargin(0, 10));

	USpacer* PauseBottomSpacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
	PauseContent->AddChildToVerticalBox(PauseBottomSpacer)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	SaveConfirmDialog = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UCanvasPanelSlot* SaveSlot = RootCanvas->AddChildToCanvas(SaveConfirmDialog);
	SaveSlot->SetAnchors(FAnchors(0.3f, 0.35f, 0.7f, 0.65f));
	SaveSlot->SetOffsets(FMargin(0));
	SaveSlot->SetZOrder(25);
	SaveConfirmDialog->SetBrush(FSlateColorBrush(FLinearColor(0.05f, 0.05f, 0.1f, 0.95f)));
	SaveConfirmDialog->SetVisibility(ESlateVisibility::Hidden);

	UVerticalBox* SaveContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	SaveConfirmDialog->SetContent(SaveContent);

	UTextBlock* SavePrompt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	SavePrompt->SetText(FText::FromString(TEXT("Save progress?")));
	SavePrompt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo SaveFont = SavePrompt->GetFont();
	SaveFont.Size = 20;
	SavePrompt->SetFont(SaveFont);
	SavePrompt->SetJustification(ETextJustify::Center);
	UVerticalBoxSlot* PromptSlot = SaveContent->AddChildToVerticalBox(SavePrompt);
	PromptSlot->SetPadding(FMargin(0, 20, 0, 30));
	PromptSlot->SetHorizontalAlignment(HAlign_Center);

	UHorizontalBox* SaveBtnRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UVerticalBoxSlot* BtnRowSlot = SaveContent->AddChildToVerticalBox(SaveBtnRow);
	BtnRowSlot->SetHorizontalAlignment(HAlign_Center);

	ConfirmSaveYesButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle YesBtnStyle = ConfirmSaveYesButton->GetStyle();
	YesBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.3f, 0.1f, 0.9f));
	YesBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.5f, 0.2f, 1.0f));
	ConfirmSaveYesButton->SetStyle(YesBtnStyle);
	UTextBlock* YesText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	YesText->SetText(FText::FromString(TEXT("Save")));
	YesText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ConfirmSaveYesButton->AddChild(YesText);
	UHorizontalBoxSlot* YesSlot = SaveBtnRow->AddChildToHorizontalBox(ConfirmSaveYesButton);
	YesSlot->SetPadding(FMargin(20, 5));

	ConfirmSaveNoButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle NoBtnStyle = ConfirmSaveNoButton->GetStyle();
	NoBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.3f, 0.1f, 0.1f, 0.9f));
	NoBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.5f, 0.2f, 0.2f, 1.0f));
	ConfirmSaveNoButton->SetStyle(NoBtnStyle);
	UTextBlock* NoText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NoText->SetText(FText::FromString(TEXT("Don't Save")));
	NoText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ConfirmSaveNoButton->AddChild(NoText);
	UHorizontalBoxSlot* NoSlot = SaveBtnRow->AddChildToHorizontalBox(ConfirmSaveNoButton);
	NoSlot->SetPadding(FMargin(20, 5));

	StoryEndOverlay = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UCanvasPanelSlot* EndSlot = RootCanvas->AddChildToCanvas(StoryEndOverlay);
	EndSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	EndSlot->SetOffsets(FMargin(0));
	EndSlot->SetZOrder(30);
	StoryEndOverlay->SetBrush(FSlateColorBrush(FLinearColor(0, 0, 0, 0.8f)));
	StoryEndOverlay->SetVisibility(ESlateVisibility::Hidden);

	UVerticalBox* EndContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	StoryEndOverlay->SetContent(EndContent);

	UTextBlock* EndText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	EndText->SetText(FText::FromString(TEXT("Simulation Ended")));
	EndText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo EndFont = EndText->GetFont();
	EndFont.Size = 36;
	EndText->SetFont(EndFont);
	EndText->SetJustification(ETextJustify::Center);
	UVerticalBoxSlot* EndTextSlot = EndContent->AddChildToVerticalBox(EndText);
	EndTextSlot->SetPadding(FMargin(0, 80, 0, 40));
	EndTextSlot->SetHorizontalAlignment(HAlign_Center);

	StoryEndButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle EndBtnStyle = StoryEndButton->GetStyle();
	EndBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.2f, 0.9f));
	EndBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.2f, 0.4f, 1.0f));
	StoryEndButton->SetStyle(EndBtnStyle);
	UTextBlock* ReturnText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ReturnText->SetText(FText::FromString(TEXT("Return to Main Menu")));
	ReturnText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	StoryEndButton->AddChild(ReturnText);
	UVerticalBoxSlot* ReturnSlot = EndContent->AddChildToVerticalBox(StoryEndButton);
	ReturnSlot->SetHorizontalAlignment(HAlign_Center);
}

void UVNGameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsTyping)
	{
		UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
		float TextSpeed = GI ? GI->GetGameSettings().TextSpeed : 30.0f;

		TypewriterTimer += InDeltaTime;
		float CharInterval = 1.0f / TextSpeed;

		while (TypewriterTimer >= CharInterval && CurrentCharIndex < FullDialogueText.Len())
		{
			CurrentCharIndex++;
			TypewriterTimer -= CharInterval;
		}

		FString DisplayText = FullDialogueText.Left(CurrentCharIndex);
		DialogueText->SetText(FText::FromString(DisplayText));

		if (CurrentCharIndex >= FullDialogueText.Len())
		{
			bIsTyping = false;
		}
	}
}

void UVNGameWidget::SetGameCallbacks(AVNGameMode* InGameMode)
{
	GameModePtr = InGameMode;
}

void UVNGameWidget::DisplayDialogue(const FString& Speaker, const FString& Text)
{
	SpeakerText->SetText(FText::FromString(Speaker));
	FullDialogueText = Text;
	CurrentCharIndex = 0;
	TypewriterTimer = 0.0f;
	bIsTyping = true;
	DialogueText->SetText(FText::GetEmpty());
	ClearChoices();
	if (DialogueClickButton)
	{
		DialogueClickButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UVNGameWidget::DisplayChoices(const TArray<FVNChoice>& Choices)
{
	ClearChoices();

	if (DialogueClickButton)
	{
		DialogueClickButton->SetVisibility(ESlateVisibility::Hidden);
	}

	for (int32 i = 0; i < Choices.Num(); i++)
	{
		UButton* ChoiceBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		FButtonStyle ChoiceBtnStyle = ChoiceBtn->GetStyle();
		ChoiceBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.15f, 0.15f, 0.25f, 0.9f));
		ChoiceBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.45f, 1.0f));
		ChoiceBtnStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.35f, 0.35f, 0.55f, 1.0f));
		ChoiceBtn->SetStyle(ChoiceBtnStyle);

		UTextBlock* ChoiceText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		ChoiceText->SetText(FText::FromString(Choices[i].Text));
		FSlateFontInfo Font = ChoiceText->GetFont();
		Font.Size = 18;
		ChoiceText->SetFont(Font);
		ChoiceText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		ChoiceText->SetJustification(ETextJustify::Center);
		ChoiceBtn->AddChild(ChoiceText);

		UVerticalBoxSlot* BtnSlot = ChoiceContainer->AddChildToVerticalBox(ChoiceBtn);
		BtnSlot->SetPadding(FMargin(10, 5));
		BtnSlot->SetHorizontalAlignment(HAlign_Center);

		ChoiceBtn->OnClicked.AddDynamic(this, &UVNGameWidget::OnChoiceButtonClicked);

		ChoiceButtons.Add(ChoiceBtn);
		ChoiceButtonIndexMap.Add(i);
	}
}

void UVNGameWidget::ClearChoices()
{
	if (DialogueClickButton)
	{
		DialogueClickButton->SetVisibility(ESlateVisibility::Visible);
	}

	for (UButton* Btn : ChoiceButtons)
	{
		if (Btn)
		{
			Btn->OnClicked.Clear();
			if (Btn->IsInViewport())
			{
				Btn->RemoveFromParent();
			}
		}
	}
	ChoiceButtons.Empty();
	ChoiceButtonIndexMap.Empty();

	if (ChoiceContainer && ChoiceContainer->GetChildrenCount() > 0)
	{
		ChoiceContainer->ClearChildren();
	}
}

void UVNGameWidget::SetBackground(UTexture2D* Texture)
{
	if (BackgroundImage)
	{
		if (Texture)
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(Texture);
			Brush.DrawAs = ESlateBrushDrawType::Image;
			BackgroundImage->SetBrush(Brush);
		}
		else
		{
			BackgroundImage->SetBrush(FSlateColorBrush(FLinearColor::Black));
		}
	}
}

void UVNGameWidget::SetCharacters(const TArray<UTexture2D*>& Textures, const TArray<EVNCharacterPosition>& Positions)
{
	ClearCharacters();

	UCanvasPanel* ParentCanvas = Cast<UCanvasPanel>(GetRootWidget());
	if (!ParentCanvas) return;

	for (int32 i = 0; i < Textures.Num(); i++)
	{
		if (!Textures[i]) continue;

		UImage* CharImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		FSlateBrush Brush;
		Brush.SetResourceObject(Textures[i]);
		Brush.DrawAs = ESlateBrushDrawType::Image;
		CharImage->SetBrush(Brush);

		UCanvasPanelSlot* ImgSlot = ParentCanvas->AddChildToCanvas(CharImage);
		ImgSlot->SetZOrder(2);

		float XCenter = 0.5f;
		if (Positions.IsValidIndex(i))
		{
			switch (Positions[i])
			{
			case EVNCharacterPosition::Left: XCenter = 0.12f; break;
			case EVNCharacterPosition::Center: XCenter = 0.5f; break;
			case EVNCharacterPosition::Right: XCenter = 0.88f; break;
			case EVNCharacterPosition::CenterLeft: XCenter = 0.30f; break;
			case EVNCharacterPosition::CenterRight: XCenter = 0.70f; break;
			}
		}

		float CharWidth = 0.10f;
		float CharHeight = 0.55f;
		ImgSlot->SetAnchors(FAnchors(XCenter - CharWidth / 2, 0.08f, XCenter + CharWidth / 2, 0.08f + CharHeight));
		ImgSlot->SetOffsets(FMargin(0));

		CharacterImages.Add(CharImage);
	}
}

void UVNGameWidget::ClearCharacters()
{
	for (UImage* Img : CharacterImages)
	{
		if (Img && Img->IsInViewport())
		{
			Img->RemoveFromParent();
		}
	}
	CharacterImages.Empty();
}

void UVNGameWidget::SetPauseState(bool bPaused)
{
	if (PauseOverlay)
	{
		PauseOverlay->SetVisibility(bPaused ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (PauseButtonText)
	{
		PauseButtonText->SetText(FText::FromString(bPaused ? TEXT("Resume") : TEXT("Pause")));
	}
}

void UVNGameWidget::SetAutoState(bool bAuto)
{
	if (AutoButtonText)
	{
		AutoButtonText->SetText(FText::FromString(bAuto ? TEXT("Auto(ON)") : TEXT("Auto")));
	}
}

void UVNGameWidget::ShowStoryEnd()
{
	if (StoryEndOverlay)
	{
		StoryEndOverlay->SetVisibility(ESlateVisibility::Visible);
	}
	ClearCharacters();
	ClearChoices();
}

void UVNGameWidget::HideStoryEnd()
{
	if (StoryEndOverlay)
	{
		StoryEndOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UVNGameWidget::ShowSaveConfirmDialog()
{
	if (SaveConfirmDialog)
	{
		SaveConfirmDialog->SetVisibility(ESlateVisibility::Visible);
	}
}

void UVNGameWidget::HideSaveConfirmDialog()
{
	if (SaveConfirmDialog)
	{
		SaveConfirmDialog->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UVNGameWidget::OnDialogueBoxClicked()
{
	if (bIsTyping)
	{
		CurrentCharIndex = FullDialogueText.Len();
		DialogueText->SetText(FText::FromString(FullDialogueText));
		bIsTyping = false;
	}
	else
	{
		if (GameModePtr.IsValid() && !GameModePtr->IsPaused())
		{
			GameModePtr->OnAdvanceDialogue();
		}
	}
}

void UVNGameWidget::OnPauseClicked()
{
	if (GameModePtr.IsValid())
	{
		GameModePtr->OnTogglePause();
	}
}

void UVNGameWidget::OnSaveClicked()
{
	if (!GameModePtr.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNGameWidget: OnSaveClicked - GameModePtr is invalid"));
		return;
	}

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNGameWidget: OnSaveClicked - GameInstance is NULL"));
		return;
	}

	if (!GI->IsGameInProgress())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNGameWidget: OnSaveClicked - No game in progress"));
		return;
	}

	UVNSaveManager* SM = GI->GetSaveManager();
	if (!SM) return;

	int32 SaveSlotIdx = -1;

	if (GI->IsFromSave() && GI->GetActiveSaveSlot() >= 0)
	{
		SaveSlotIdx = GI->GetActiveSaveSlot();
		UE_LOG(LogTemp, Warning, TEXT("VNGameWidget: Save to active slot %d"), SaveSlotIdx);
	}
	else
	{
		for (int32 i = 0; i < SM->GetMaxSaveSlots(); i++)
		{
			if (!SM->HasSave(i))
			{
				SaveSlotIdx = i;
				UE_LOG(LogTemp, Warning, TEXT("VNGameWidget: Found empty slot %d"), SaveSlotIdx);
				break;
			}
		}

		if (SaveSlotIdx < 0)
		{
			SaveSlotIdx = 0;
			UE_LOG(LogTemp, Warning, TEXT("VNGameWidget: All slots full, overwriting slot 0"));
		}
	}

	if (SaveSlotIdx >= 0)
	{
		GameModePtr->OnSaveGame(SaveSlotIdx);
		UE_LOG(LogTemp, Warning, TEXT("VNGameWidget: Save completed to slot %d"), SaveSlotIdx);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VNGameWidget: Failed to find save slot"));
	}
}

void UVNGameWidget::OnAutoClicked()
{
	if (GameModePtr.IsValid())
	{
		GameModePtr->OnToggleAuto();
	}
}

void UVNGameWidget::OnEndClicked()
{
	ShowSaveConfirmDialog();
}

void UVNGameWidget::OnChoiceButtonClicked()
{
	for (int32 i = 0; i < ChoiceButtons.Num(); i++)
	{
		if (ChoiceButtons[i] && ChoiceButtons[i]->IsHovered())
		{
			OnChoiceClicked(ChoiceButtonIndexMap[i]);
			return;
		}
	}
}

void UVNGameWidget::OnChoiceClicked(int32 ChoiceIndex)
{
	if (GameModePtr.IsValid())
	{
		UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
		if (GI)
		{
			UVNStoryEngine* Engine = GI->GetStoryEngine();
			if (Engine)
			{
				const FVNStoryNode* Node = Engine->GetCurrentNode();
				if (Node && Node->Choices.IsValidIndex(ChoiceIndex))
				{
					ClearChoices();
					GameModePtr->OnMakeChoice(Node->Choices[ChoiceIndex].ChoiceId);
				}
			}
		}
	}
}

void UVNGameWidget::OnSaveConfirmYes()
{
	HideSaveConfirmDialog();
	if (GameModePtr.IsValid())
	{
		GameModePtr->OnEndGame(true);
	}
}

void UVNGameWidget::OnSaveConfirmNo()
{
	HideSaveConfirmDialog();
	if (GameModePtr.IsValid())
	{
		GameModePtr->OnEndGame(false);
	}
}

void UVNGameWidget::OnStoryEndClicked()
{
	if (GameModePtr.IsValid())
	{
		GameModePtr->OnReturnToMainMenu();
	}
}
