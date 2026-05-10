#include "UI/VNSaveMenuWidget.h"
#include "UI/VNBranchViewWidget.h"
#include "UI/VNStoryTextViewWidget.h"
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
#include "Components/Border.h"
#include "Components/ScrollBox.h"
#include "Components/Spacer.h"
#include "Blueprint/WidgetTree.h"
#include "Brushes/SlateColorBrush.h"

void UVNSaveMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BuildWidgetTree();
}

void UVNSaveMenuWidget::NativeConstruct()
{
	if (!bWidgetTreeBuilt)
	{
		BuildWidgetTree();
	}

	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.Clear();
		ContinueButton->OnClicked.AddDynamic(this, &UVNSaveMenuWidget::OnContinueClicked);
	}
	if (BranchButton)
	{
		BranchButton->OnClicked.Clear();
		BranchButton->OnClicked.AddDynamic(this, &UVNSaveMenuWidget::OnBranchClicked);
	}
	if (StoryTextButton)
	{
		StoryTextButton->OnClicked.Clear();
		StoryTextButton->OnClicked.AddDynamic(this, &UVNSaveMenuWidget::OnStoryTextClicked);
	}
	if (DeleteButton)
	{
		DeleteButton->OnClicked.Clear();
		DeleteButton->OnClicked.AddDynamic(this, &UVNSaveMenuWidget::OnDeleteClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.Clear();
		BackButton->OnClicked.AddDynamic(this, &UVNSaveMenuWidget::OnBackClicked);
	}

	RefreshSaveSlots();
}

void UVNSaveMenuWidget::BuildWidgetTree()
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

	UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TitleText->SetText(FText::FromString(TEXT("选择存档")));
	FSlateFontInfo TitleFont = TitleText->GetFont();
	TitleFont.Size = 32;
	TitleFont.TypefaceFontName = FName(TEXT("Bold"));
	TitleText->SetFont(TitleFont);
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	TitleText->SetJustification(ETextJustify::Center);
	UCanvasPanelSlot* TitleSlot = RootCanvas->AddChildToCanvas(TitleText);
	TitleSlot->SetAnchors(FAnchors(0.3f, 0.02f, 0.7f, 0.08f));
	TitleSlot->SetOffsets(FMargin(0));
	TitleSlot->SetZOrder(1);

	UScrollBox* ScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
	UCanvasPanelSlot* ScrollSlot = RootCanvas->AddChildToCanvas(ScrollBox);
	ScrollSlot->SetAnchors(FAnchors(0.05f, 0.1f, 0.55f, 0.85f));
	ScrollSlot->SetOffsets(FMargin(0));
	ScrollSlot->SetZOrder(1);

	SaveSlotContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	ScrollBox->AddChild(SaveSlotContainer);

	SlotDetailPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UCanvasPanelSlot* DetailSlot = RootCanvas->AddChildToCanvas(SlotDetailPanel);
	DetailSlot->SetAnchors(FAnchors(0.6f, 0.1f, 0.95f, 0.85f));
	DetailSlot->SetOffsets(FMargin(0));
	DetailSlot->SetZOrder(2);
	SlotDetailPanel->SetBrush(FSlateColorBrush(FLinearColor(0.05f, 0.05f, 0.1f, 0.9f)));
	SlotDetailPanel->SetVisibility(ESlateVisibility::Hidden);

	UVerticalBox* DetailContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	SlotDetailPanel->SetContent(DetailContent);

	SlotDetailText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	SlotDetailText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SlotDetailText->SetAutoWrapText(true);
	FSlateFontInfo DetailFont = SlotDetailText->GetFont();
	DetailFont.Size = 14;
	SlotDetailText->SetFont(DetailFont);
	UVerticalBoxSlot* DetailTextSlot = DetailContent->AddChildToVerticalBox(SlotDetailText);
	DetailTextSlot->SetPadding(FMargin(10, 10, 10, 20));

	auto CreateDetailButton = [&](const FString& Label) -> UButton*
	{
		UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		FButtonStyle BtnStyle = Btn->GetStyle();
		BtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.2f, 0.8f));
		BtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.2f, 0.4f, 0.9f));
		Btn->SetStyle(BtnStyle);
		UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Txt->SetText(FText::FromString(Label));
		Txt->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo Font = Txt->GetFont();
		Font.Size = 16;
		Txt->SetFont(Font);
		Txt->SetJustification(ETextJustify::Center);
		Btn->AddChild(Txt);
		UVerticalBoxSlot* BtnSlot = DetailContent->AddChildToVerticalBox(Btn);
		BtnSlot->SetPadding(FMargin(10, 5));
		BtnSlot->SetHorizontalAlignment(HAlign_Center);
		return Btn;
	};

	ContinueButton = CreateDetailButton(TEXT("继续游戏"));
	BranchButton = CreateDetailButton(TEXT("查看分支"));
	StoryTextButton = CreateDetailButton(TEXT("欣赏剧情"));
	DeleteButton = CreateDetailButton(TEXT("删除存档"));

	BackButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle BackBtnStyle = BackButton->GetStyle();
	BackBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.15f, 0.1f, 0.1f, 0.8f));
	BackBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.3f, 0.15f, 0.15f, 0.9f));
	BackButton->SetStyle(BackBtnStyle);
	UTextBlock* BackText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	BackText->SetText(FText::FromString(TEXT("返回")));
	BackText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo BackFont = BackText->GetFont();
	BackFont.Size = 18;
	BackText->SetFont(BackFont);
	BackText->SetJustification(ETextJustify::Center);
	BackButton->AddChild(BackText);
	UCanvasPanelSlot* BackSlot = RootCanvas->AddChildToCanvas(BackButton);
	BackSlot->SetAnchors(FAnchors(0.4f, 0.88f, 0.6f, 0.95f));
	BackSlot->SetOffsets(FMargin(0));
	BackSlot->SetZOrder(3);
}

void UVNSaveMenuWidget::SetSaveMenuCallbacks(AVNGameMode* InGameMode)
{
	GameModePtr = InGameMode;
}

void UVNSaveMenuWidget::RefreshSaveSlots()
{
	if (!SaveSlotContainer) return;

	for (UButton* Btn : SlotButtons)
	{
		if (Btn) Btn->RemoveFromParent();
	}
	SlotButtons.Empty();

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	UVNSaveManager* SM = GI->GetSaveManager();
	if (!SM) return;

	SlotInfos = SM->GetAllSaveSlots();

	for (int32 i = 0; i < SlotInfos.Num(); i++)
	{
		const FVNSaveSlotInfo& Info = SlotInfos[i];

		UButton* SlotBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		FButtonStyle SlotBtnStyle = SlotBtn->GetStyle();
		SlotBtnStyle.Normal.TintColor = FSlateColor(
			Info.bHasSave ? FLinearColor(0.08f, 0.08f, 0.15f, 0.9f) : FLinearColor(0.04f, 0.04f, 0.08f, 0.6f));
		SlotBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.15f, 0.15f, 0.3f, 0.9f));
		SlotBtn->SetStyle(SlotBtnStyle);

		UTextBlock* SlotText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		FString DisplayStr;
		if (Info.bHasSave)
		{
			DisplayStr = FString::Printf(TEXT("Slot %d | %s | Choices: %d"),
				Info.SlotIndex + 1, *Info.Timestamp, Info.ChoiceCount);
		}
		else
		{
			DisplayStr = FString::Printf(TEXT("Slot %d - Empty"), Info.SlotIndex + 1);
		}
		SlotText->SetText(FText::FromString(DisplayStr));
		SlotText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo Font = SlotText->GetFont();
		Font.Size = 16;
		SlotText->SetFont(Font);
		SlotBtn->AddChild(SlotText);

		UVerticalBoxSlot* BtnSlot = SaveSlotContainer->AddChildToVerticalBox(SlotBtn);
		BtnSlot->SetPadding(FMargin(5, 3));

		SlotBtn->OnClicked.AddDynamic(this, &UVNSaveMenuWidget::OnSlotButtonClicked);

		SlotButtons.Add(SlotBtn);
		SlotButtonIndexMap.Add(i);
	}
}

void UVNSaveMenuWidget::OnSlotButtonClicked()
{
	for (int32 i = 0; i < SlotButtons.Num(); i++)
	{
		if (SlotButtons[i] && SlotButtons[i]->IsHovered())
		{
			OnSlotClicked(SlotButtonIndexMap[i]);
			return;
		}
	}
}

void UVNSaveMenuWidget::OnSlotClicked(int32 SlotIndex)
{
	SelectedSlot = SlotIndex;
	ShowSlotDetail(SlotIndex);
}

void UVNSaveMenuWidget::OnBackClicked()
{
	HideSlotDetail();
	if (GameModePtr.IsValid())
	{
		GameModePtr->ShowMainMenu();
	}
}

void UVNSaveMenuWidget::OnContinueClicked()
{
	if (SelectedSlot >= 0 && GameModePtr.IsValid())
	{
		HideSlotDetail();
		GameModePtr->OnLoadGame(SelectedSlot);
	}
}

void UVNSaveMenuWidget::OnBranchClicked()
{
	if (SelectedSlot < 0) return;

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	UVNSaveManager* SM = GI->GetSaveManager();
	FVNSaveData SaveData;
	if (!SM->LoadGame(SelectedSlot, SaveData)) return;

	if (!BranchViewWidget)
	{
		BranchViewWidget = CreateWidget<UVNBranchViewWidget>(GetWorld(), UVNBranchViewWidget::StaticClass());
	}

	if (BranchViewWidget)
	{
		UVNStoryEngine* TempEngine = NewObject<UVNStoryEngine>(this);
		TempEngine->LoadStory(SaveData.StoryId);
		BranchViewWidget->ShowBranchView(TempEngine, SaveData);
		BranchViewWidget->AddToViewport(100);
	}
}

void UVNSaveMenuWidget::OnStoryTextClicked()
{
	if (SelectedSlot < 0) return;

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	UVNSaveManager* SM = GI->GetSaveManager();
	FVNSaveData SaveData;
	if (!SM->LoadGame(SelectedSlot, SaveData)) return;

	if (!StoryTextViewWidget)
	{
		StoryTextViewWidget = CreateWidget<UVNStoryTextViewWidget>(GetWorld(), UVNStoryTextViewWidget::StaticClass());
	}

	if (StoryTextViewWidget)
	{
		UVNStoryEngine* TempEngine = NewObject<UVNStoryEngine>(this);
		TempEngine->LoadStory(SaveData.StoryId);
		StoryTextViewWidget->ShowStoryText(TempEngine, SaveData);
		StoryTextViewWidget->AddToViewport(100);
	}
}

void UVNSaveMenuWidget::OnDeleteClicked()
{
	if (SelectedSlot < 0) return;

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (!GI) return;

	UVNSaveManager* SM = GI->GetSaveManager();
	SM->DeleteSave(SelectedSlot);
	HideSlotDetail();
	RefreshSaveSlots();
}

void UVNSaveMenuWidget::ShowSlotDetail(int32 SlotIndex)
{
	if (!SlotInfos.IsValidIndex(SlotIndex) || !SlotInfos[SlotIndex].bHasSave)
	{
		HideSlotDetail();
		return;
	}

	const FVNSaveSlotInfo& Info = SlotInfos[SlotIndex];
	FString DetailStr = FString::Printf(TEXT("Slot %d\nTime: %s\nChoices: %d"),
		Info.SlotIndex + 1, *Info.Timestamp, Info.ChoiceCount);
	if (!Info.LastChoiceText.IsEmpty())
	{
		DetailStr += TEXT("\nLast Choice: ") + Info.LastChoiceText;
	}
	SlotDetailText->SetText(FText::FromString(DetailStr));
	SlotDetailPanel->SetVisibility(ESlateVisibility::Visible);
}

void UVNSaveMenuWidget::HideSlotDetail()
{
	SlotDetailPanel->SetVisibility(ESlateVisibility::Hidden);
	SelectedSlot = -1;
}
