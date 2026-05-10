#include "UI/VNMainMenuWidget.h"
#include "Game/VNGameMode.h"
#include "Core/VNAssetLoader.h"
#include "Core/VNGameInstance.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Blueprint/WidgetTree.h"
#include "Brushes/SlateColorBrush.h"

UVNMainMenuWidget::UVNMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: Constructor called"));
}

void UVNMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget::NativeOnInitialized - Building widget tree"));

	BuildWidgetTree();
}

void UVNMainMenuWidget::NativeConstruct()
{
	if (!bWidgetTreeBuilt)
	{
		UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget::NativeConstruct - Widget tree NOT built yet, building now"));
		BuildWidgetTree();
	}

	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget::NativeConstruct - Binding events, RootWidget=%s"),
		(WidgetTree && WidgetTree->RootWidget) ? TEXT("VALID") : TEXT("NULL"));

	if (StartButton)
	{
		StartButton->OnClicked.Clear();
		StartButton->OnClicked.AddDynamic(this, &UVNMainMenuWidget::OnStartClicked);
		UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: StartButton bound"));
	}
	if (LoadButton)
	{
		LoadButton->OnClicked.Clear();
		LoadButton->OnClicked.AddDynamic(this, &UVNMainMenuWidget::OnLoadClicked);
		UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: LoadButton bound"));
	}
	if (SettingsButton)
	{
		SettingsButton->OnClicked.Clear();
		SettingsButton->OnClicked.AddDynamic(this, &UVNMainMenuWidget::OnSettingsClicked);
		UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: SettingsButton bound"));
	}
}

void UVNMainMenuWidget::BuildWidgetTree()
{
	if (bWidgetTreeBuilt) return;
	bWidgetTreeBuilt = true;

	if (!WidgetTree)
	{
		UE_LOG(LogTemp, Error, TEXT("VNMainMenuWidget: WidgetTree is NULL!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget::BuildWidgetTree - START"));

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = RootCanvas;

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: Root CanvasPanel created"));

	BackgroundImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UCanvasPanelSlot* BgSlot = RootCanvas->AddChildToCanvas(BackgroundImage);
	BgSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	BgSlot->SetOffsets(FMargin(0));
	BgSlot->SetZOrder(0);
	BackgroundImage->SetBrush(FSlateColorBrush(FLinearColor(0.05f, 0.05f, 0.1f, 1.0f)));
	BackgroundImage->SetVisibility(ESlateVisibility::Visible);

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: BackgroundImage created"));

	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (GI)
	{
		UVNAssetLoader* Loader = GI->GetAssetLoader();
		if (Loader)
		{
			UTexture2D* BgTex = Loader->LoadBackground(TEXT("main_menu"));
			if (BgTex)
			{
				FSlateBrush Brush;
				Brush.SetResourceObject(BgTex);
				Brush.DrawAs = ESlateBrushDrawType::Image;
				Brush.Tiling = ESlateBrushTileType::NoTile;
				BackgroundImage->SetBrush(Brush);
				UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: Background image loaded from file"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: No background image file, using solid color"));
			}
		}
	}

	UBorder* OverlayBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UCanvasPanelSlot* OverlaySlot = RootCanvas->AddChildToCanvas(OverlayBorder);
	OverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	OverlaySlot->SetOffsets(FMargin(0));
	OverlaySlot->SetZOrder(1);
	OverlayBorder->SetBrush(FSlateColorBrush(FLinearColor(0.0f, 0.0f, 0.0f, 0.4f)));

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: Overlay border created"));

	UVerticalBox* MenuBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UCanvasPanelSlot* MenuSlot = RootCanvas->AddChildToCanvas(MenuBox);
	MenuSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
	MenuSlot->SetOffsets(FMargin(-150.0f, -200.0f, 300.0f, 400.0f));
	MenuSlot->SetZOrder(2);

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: MenuBox created"));

	TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TitleText->SetText(FText::FromString(TEXT("Visual Novel")));
	FSlateFontInfo TitleFont = TitleText->GetFont();
	TitleFont.Size = 42;
	TitleFont.TypefaceFontName = FName(TEXT("Bold"));
	TitleText->SetFont(TitleFont);
	TitleText->SetJustification(ETextJustify::Center);
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	UVerticalBoxSlot* TitleSlot = MenuBox->AddChildToVerticalBox(TitleText);
	TitleSlot->SetPadding(FMargin(0, 0, 0, 60));
	TitleSlot->SetHorizontalAlignment(HAlign_Center);

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: TitleText created"));

	auto CreateMenuButton = [&](const FString& Label) -> UButton*
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		FButtonStyle BtnStyle = Button->GetStyle();
		BtnStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.2f, 0.8f));
		BtnStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.2f, 0.4f, 0.9f));
		BtnStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.3f, 0.3f, 0.5f, 1.0f));
		Button->SetStyle(BtnStyle);

		UTextBlock* BtnText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		BtnText->SetText(FText::FromString(Label));
		FSlateFontInfo BtnFont = BtnText->GetFont();
		BtnFont.Size = 22;
		BtnText->SetFont(BtnFont);
		BtnText->SetJustification(ETextJustify::Center);
		BtnText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Button->AddChild(BtnText);

		UVerticalBoxSlot* BtnSlot = MenuBox->AddChildToVerticalBox(Button);
		BtnSlot->SetPadding(FMargin(20, 5, 20, 5));
		BtnSlot->SetHorizontalAlignment(HAlign_Center);

		return Button;
	};

	StartButton = CreateMenuButton(TEXT("开启模拟"));
	LoadButton = CreateMenuButton(TEXT("选择存档"));
	SettingsButton = CreateMenuButton(TEXT("系统设置"));

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: All buttons created"));

	this->SetVisibility(ESlateVisibility::Visible);
	this->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));

	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget::BuildWidgetTree - END, RootWidget=%s, CanvasChildren=%d"),
		WidgetTree->RootWidget ? TEXT("VALID") : TEXT("NULL"),
		RootCanvas->GetChildrenCount());
}

void UVNMainMenuWidget::SetMainMenuCallbacks(AVNGameMode* InGameMode)
{
	GameModePtr = InGameMode;
	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: SetMainMenuCallbacks called, GameMode=%s"),
		InGameMode ? TEXT("VALID") : TEXT("NULL"));
}

void UVNMainMenuWidget::SetBackground(UTexture2D* Texture)
{
	if (BackgroundImage && Texture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		Brush.DrawAs = ESlateBrushDrawType::Image;
		BackgroundImage->SetBrush(Brush);
	}
}

void UVNMainMenuWidget::OnStartClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: OnStartClicked"));
	if (GameModePtr.IsValid())
	{
		GameModePtr->OnStartNewGame();
	}
}

void UVNMainMenuWidget::OnLoadClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: OnLoadClicked"));
	if (GameModePtr.IsValid())
	{
		GameModePtr->ShowSaveMenu();
	}
}

void UVNMainMenuWidget::OnSettingsClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("VNMainMenuWidget: OnSettingsClicked"));
	if (GameModePtr.IsValid())
	{
		GameModePtr->ShowSettings();
	}
}
