#include "UI/VNStoryTextViewWidget.h"
#include "Core/VNStoryEngine.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Border.h"
#include "Components/ScrollBox.h"
#include "Blueprint/WidgetTree.h"
#include "Brushes/SlateColorBrush.h"

void UVNStoryTextViewWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BuildWidgetTree();
}

void UVNStoryTextViewWidget::NativeConstruct()
{
	if (!bWidgetTreeBuilt)
	{
		BuildWidgetTree();
	}

	Super::NativeConstruct();

	if (CloseButton)
	{
		CloseButton->OnClicked.Clear();
		CloseButton->OnClicked.AddDynamic(this, &UVNStoryTextViewWidget::OnCloseClicked);
	}
}

void UVNStoryTextViewWidget::BuildWidgetTree()
{
	if (bWidgetTreeBuilt) return;
	bWidgetTreeBuilt = true;

	if (!WidgetTree) return;

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = RootCanvas;

	UBorder* PanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(PanelBorder);
	PanelSlot->SetAnchors(FAnchors(0.1f, 0.05f, 0.9f, 0.95f));
	PanelSlot->SetOffsets(FMargin(0));
	PanelSlot->SetZOrder(0);
	PanelBorder->SetBrush(FSlateColorBrush(FLinearColor(0.03f, 0.03f, 0.08f, 0.98f)));
	PanelBorder->SetPadding(FMargin(20));

	UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	PanelBorder->SetContent(Content);

	UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TitleText->SetText(FText::FromString(TEXT("Story Review")));
	FSlateFontInfo TitleFont = TitleText->GetFont();
	TitleFont.Size = 24;
	TitleFont.TypefaceFontName = FName(TEXT("Bold"));
	TitleText->SetFont(TitleFont);
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	TitleText->SetJustification(ETextJustify::Center);
	UVerticalBoxSlot* TitleSlot = Content->AddChildToVerticalBox(TitleText);
	TitleSlot->SetPadding(FMargin(0, 0, 0, 15));
	TitleSlot->SetHorizontalAlignment(HAlign_Center);

	UScrollBox* ScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
	UVerticalBoxSlot* ScrollSlot = Content->AddChildToVerticalBox(ScrollBox);
	ScrollSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	StoryTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	StoryTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.9f, 1.0f)));
	FSlateFontInfo Font = StoryTextBlock->GetFont();
	Font.Size = 16;
	StoryTextBlock->SetFont(Font);
	StoryTextBlock->SetAutoWrapText(true);
	ScrollBox->AddChild(StoryTextBlock);

	CloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle CloseBtnStyle = CloseButton->GetStyle();
	CloseBtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.15f, 0.1f, 0.1f, 0.8f));
	CloseBtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.3f, 0.15f, 0.15f, 0.9f));
	CloseButton->SetStyle(CloseBtnStyle);
	UTextBlock* CloseText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	CloseText->SetText(FText::FromString(TEXT("Close")));
	CloseText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	CloseButton->AddChild(CloseText);
	UVerticalBoxSlot* CloseSlot = Content->AddChildToVerticalBox(CloseButton);
	CloseSlot->SetPadding(FMargin(0, 15, 0, 0));
	CloseSlot->SetHorizontalAlignment(HAlign_Center);
}

void UVNStoryTextViewWidget::ShowStoryText(UVNStoryEngine* InStoryEngine, const FVNSaveData& SaveData)
{
	if (!StoryTextBlock || !InStoryEngine) return;

	FString FullText = InStoryEngine->GetStoryTextForNodes(SaveData.VisitedNodeIds);

	if (FullText.IsEmpty())
	{
		FullText = TEXT("No story records yet");
	}

	StoryTextBlock->SetText(FText::FromString(FullText));
}

void UVNStoryTextViewWidget::OnCloseClicked()
{
	RemoveFromParent();
}
