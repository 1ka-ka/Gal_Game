#include "UI/VNBranchViewWidget.h"
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

void UVNBranchViewWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BuildWidgetTree();
}

void UVNBranchViewWidget::NativeConstruct()
{
	if (!bWidgetTreeBuilt)
	{
		BuildWidgetTree();
	}

	Super::NativeConstruct();

	if (CloseButton)
	{
		CloseButton->OnClicked.Clear();
		CloseButton->OnClicked.AddDynamic(this, &UVNBranchViewWidget::OnCloseClicked);
	}
}

void UVNBranchViewWidget::BuildWidgetTree()
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
	TitleText->SetText(FText::FromString(TEXT("Branch View")));
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
	FSlateChildSize ScrollSize;
	ScrollSize.SizeRule = ESlateSizeRule::Fill;
	ScrollSize.Value = 0.0f;
	ScrollSlot->SetSize(ScrollSize);

	BranchContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	ScrollBox->AddChild(BranchContainer);

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

void UVNBranchViewWidget::ShowBranchView(UVNStoryEngine* InStoryEngine, const FVNSaveData& SaveData)
{
	if (!BranchContainer || !InStoryEngine) return;

	BranchContainer->ClearChildren();

	const FVNStoryData& StoryData = InStoryEngine->GetStoryData();
	TArray<FString> AllChoiceNodes = StoryData.GetAllChoiceNodeIds();
	TArray<FString> VisitedChoiceNodes = SaveData.GetVisitedChoiceNodeIds();

	if (AllChoiceNodes.Num() == 0)
	{
		UTextBlock* EmptyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		EmptyText->SetText(FText::FromString(TEXT("No branch data")));
		EmptyText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)));
		FSlateFontInfo EmptyFont = EmptyText->GetFont();
		EmptyFont.Size = 16;
		EmptyText->SetFont(EmptyFont);
		EmptyText->SetJustification(ETextJustify::Center);
		UVerticalBoxSlot* EmptySlot = BranchContainer->AddChildToVerticalBox(EmptyText);
		EmptySlot->SetPadding(FMargin(0, 20));
		EmptySlot->SetHorizontalAlignment(HAlign_Center);
		return;
	}

	TMap<FString, FString> SelectedChoices;
	for (const FVNChoiceRecord& Record : SaveData.ChoiceHistory)
	{
		SelectedChoices.Add(Record.ChoiceNodeId, Record.SelectedChoiceText);
	}

	for (const FString& ChoiceNodeId : AllChoiceNodes)
	{
		const FVNStoryNode* Node = StoryData.GetNode(ChoiceNodeId);
		if (!Node) continue;

		bool bVisited = VisitedChoiceNodes.Contains(ChoiceNodeId);

		UBorder* NodeBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		NodeBorder->SetBrush(FSlateColorBrush(bVisited
			? FLinearColor(0.1f, 0.15f, 0.25f, 0.9f)
			: FLinearColor(0.05f, 0.05f, 0.08f, 0.7f)));
		NodeBorder->SetPadding(FMargin(10, 8));

		UVerticalBox* NodeContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
		NodeBorder->SetContent(NodeContent);

		FString NodeLabel = bVisited ? TEXT("[Visited] ") : TEXT("[Unvisited] ");
		if (!Node->Text.IsEmpty())
		{
			NodeLabel += Node->Text;
		}
		else
		{
			NodeLabel += TEXT("Choice: ") + ChoiceNodeId;
		}

		UTextBlock* NodeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		NodeText->SetText(FText::FromString(NodeLabel));
		NodeText->SetColorAndOpacity(FSlateColor(bVisited
			? FLinearColor(0.6f, 0.8f, 1.0f)
			: FLinearColor(0.4f, 0.4f, 0.4f)));
		FSlateFontInfo Font = NodeText->GetFont();
		Font.Size = 16;
		NodeText->SetFont(Font);
		UVerticalBoxSlot* NodeTextSlot = NodeContent->AddChildToVerticalBox(NodeText);
		NodeTextSlot->SetPadding(FMargin(0, 0, 0, 5));

		for (const FVNChoice& Choice : Node->Choices)
		{
			bool bSelected = SelectedChoices.Contains(ChoiceNodeId) &&
				SelectedChoices[ChoiceNodeId] == Choice.Text;

			FString ChoiceLabel = TEXT("  -> ");
			if (bSelected) ChoiceLabel = TEXT("  *> ");
			ChoiceLabel += Choice.Text;
			if (bSelected) ChoiceLabel += TEXT(" (Selected)");

			UTextBlock* ChoiceText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			ChoiceText->SetText(FText::FromString(ChoiceLabel));
			ChoiceText->SetColorAndOpacity(FSlateColor(bSelected
				? FLinearColor(0.3f, 1.0f, 0.5f)
				: FLinearColor(0.5f, 0.5f, 0.5f)));
			FSlateFontInfo CFont = ChoiceText->GetFont();
			CFont.Size = 14;
			ChoiceText->SetFont(CFont);
			UVerticalBoxSlot* CSlot = NodeContent->AddChildToVerticalBox(ChoiceText);
			CSlot->SetPadding(FMargin(20, 2, 0, 2));
		}

		UVerticalBoxSlot* BorderSlot = BranchContainer->AddChildToVerticalBox(NodeBorder);
		BorderSlot->SetPadding(FMargin(0, 5));
	}
}

void UVNBranchViewWidget::OnCloseClicked()
{
	RemoveFromParent();
}
