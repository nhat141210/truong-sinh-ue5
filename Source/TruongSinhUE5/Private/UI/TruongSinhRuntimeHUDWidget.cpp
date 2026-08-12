#include "UI/TruongSinhRuntimeHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Simulation/TruongSinhGameSimulation.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "UObject/ConstructorHelpers.h"

namespace TruongSinhHUD
{
const FLinearColor Ink(0.014f, 0.024f, 0.026f, 0.92f);
const FLinearColor DeepJade(0.025f, 0.110f, 0.095f, 0.94f);
const FLinearColor Jade(0.16f, 0.72f, 0.54f, 1.0f);
const FLinearColor PaleJade(0.72f, 0.93f, 0.82f, 1.0f);
const FLinearColor Gold(0.91f, 0.72f, 0.31f, 1.0f);
const FLinearColor Ivory(0.93f, 0.92f, 0.84f, 1.0f);
const FLinearColor Muted(0.57f, 0.65f, 0.61f, 1.0f);
const FLinearColor Failure(0.86f, 0.31f, 0.24f, 1.0f);

FSlateFontInfo Font(const int32 Size, const bool bBold = false)
{
    return FCoreStyle::GetDefaultFontStyle(bBold ? TEXT("Bold") : TEXT("Regular"), Size);
}

UTextBlock* Text(UWidgetTree* Tree, const TCHAR* Name, const FText& Value, const int32 Size,
    const FLinearColor& Color, const bool bBold = false)
{
    UTextBlock* Label = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
    Label->SetText(Value);
    Label->SetFont(Font(Size, bBold));
    Label->SetColorAndOpacity(FSlateColor(Color));
    Label->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.75f));
    Label->SetShadowOffset(FVector2D(1.0f, 1.0f));
    return Label;
}

UBorder* Panel(UWidgetTree* Tree, const TCHAR* Name, const FLinearColor& Color, const FMargin& Padding)
{
    UBorder* Border = Tree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(Name));
    FSlateBrush Brush;
    Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
    Brush.OutlineSettings = FSlateBrushOutlineSettings(
        8.0f, FSlateColor(FLinearColor(0.52f, 0.72f, 0.61f, 0.16f)), 1.0f);
    Border->SetBrush(Brush);
    Border->SetBrushColor(Color);
    Border->SetPadding(Padding);
    return Border;
}

UImage* Frame(UWidgetTree* Tree, const TCHAR* Name, UTexture2D* Texture, const FVector2D& DesiredSize)
{
    if (!Texture)
    {
        return nullptr;
    }

    UImage* Image = Tree->ConstructWidget<UImage>(UImage::StaticClass(), FName(Name));
    FSlateBrush Brush;
    Brush.SetResourceObject(Texture);
    Brush.DrawAs = ESlateBrushDrawType::Box;
    Brush.Margin = FMargin(0.145f);
    Brush.ImageSize = DesiredSize;
    Image->SetBrush(Brush);
    Image->SetVisibility(ESlateVisibility::HitTestInvisible);
    return Image;
}

void FillOverlay(UOverlay* Parent, UWidget* Child)
{
    if (UOverlaySlot* Slot = Parent->AddChildToOverlay(Child))
    {
        Slot->SetHorizontalAlignment(HAlign_Fill);
        Slot->SetVerticalAlignment(VAlign_Fill);
    }
}

void AddVertical(UVerticalBox* Parent, UWidget* Child, const FMargin& Padding = FMargin(0.0f))
{
    if (UVerticalBoxSlot* Slot = Parent->AddChildToVerticalBox(Child))
    {
        Slot->SetPadding(Padding);
        Slot->SetHorizontalAlignment(HAlign_Fill);
    }
}

void AddHorizontal(UHorizontalBox* Parent, UWidget* Child, const FMargin& Padding = FMargin(0.0f),
    const float Fill = 0.0f)
{
    if (UHorizontalBoxSlot* Slot = Parent->AddChildToHorizontalBox(Child))
    {
        Slot->SetPadding(Padding);
        Slot->SetVerticalAlignment(VAlign_Center);
        if (Fill > 0.0f)
        {
            FSlateChildSize ChildSize(ESlateSizeRule::Fill);
            ChildSize.Value = Fill;
            Slot->SetSize(ChildSize);
        }
    }
}
}

UTruongSinhRuntimeHUDWidget::UTruongSinhRuntimeHUDWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UTexture2D> FrameTexture(
        TEXT("/Game/UI/Generated/T_UI_JadeFrame.T_UI_JadeFrame"));
    OrnateFrameTexture = FrameTexture.Object;
}

TSharedRef<SWidget> UTruongSinhRuntimeHUDWidget::RebuildWidget()
{
    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return Super::RebuildWidget();
    }

    UOverlay* Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("HUDRoot"));
    WidgetTree->RootWidget = Root;

    // Top-left identity and progression panel.
    UBorder* StatusPanel = TruongSinhHUD::Panel(WidgetTree, TEXT("StatusPanel"),
        FLinearColor(TruongSinhHUD::Ink.R, TruongSinhHUD::Ink.G, TruongSinhHUD::Ink.B, 0.68f),
        FMargin(70.0f, 47.0f, 58.0f, 43.0f));
    UVerticalBox* StatusStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StatusStack"));
    StatusPanel->SetContent(StatusStack);

    UTextBlock* GameTitle = TruongSinhHUD::Text(WidgetTree, TEXT("GameTitle"),
        NSLOCTEXT("TruongSinhHUD", "GameTitle", "TRƯỜNG SINH"), 25, TruongSinhHUD::Gold, true);
    TruongSinhHUD::AddVertical(StatusStack, GameTitle);

    UTextBlock* Chapter = TruongSinhHUD::Text(WidgetTree, TEXT("Chapter"),
        NSLOCTEXT("TruongSinhHUD", "Chapter", "Hạ Giới  ·  Vô tận tiên lộ"), 11, TruongSinhHUD::Muted);
    TruongSinhHUD::AddVertical(StatusStack, Chapter, FMargin(1.0f, 2.0f, 0.0f, 12.0f));

    UBorder* GoldRule = TruongSinhHUD::Panel(WidgetTree, TEXT("GoldRule"), TruongSinhHUD::Gold,
        FMargin(0.0f));
    USizeBox* RuleSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("GoldRuleSize"));
    RuleSize->SetHeightOverride(1.0f);
    RuleSize->SetContent(GoldRule);
    TruongSinhHUD::AddVertical(StatusStack, RuleSize, FMargin(0.0f, 0.0f, 0.0f, 13.0f));

    RealmText = TruongSinhHUD::Text(WidgetTree, TEXT("RealmText"), FText::GetEmpty(), 18, TruongSinhHUD::Ivory, true);
    TruongSinhHUD::AddVertical(StatusStack, RealmText);
    CultivationText = TruongSinhHUD::Text(WidgetTree, TEXT("CultivationText"), FText::GetEmpty(), 12, TruongSinhHUD::PaleJade);
    TruongSinhHUD::AddVertical(StatusStack, CultivationText, FMargin(0.0f, 3.0f, 0.0f, 0.0f));

    UOverlay* StatusFrameRoot = WidgetTree->ConstructWidget<UOverlay>(
        UOverlay::StaticClass(), TEXT("StatusFrameRoot"));
    TruongSinhHUD::FillOverlay(StatusFrameRoot, StatusPanel);
    if (UImage* StatusFrame = TruongSinhHUD::Frame(
        WidgetTree, TEXT("StatusOrnateFrame"), OrnateFrameTexture, FVector2D(360.0f, 218.0f)))
    {
        TruongSinhHUD::FillOverlay(StatusFrameRoot, StatusFrame);
    }

    USizeBox* StatusFrameSize = WidgetTree->ConstructWidget<USizeBox>(
        USizeBox::StaticClass(), TEXT("StatusFrameSize"));
    StatusFrameSize->SetWidthOverride(360.0f);
    StatusFrameSize->SetHeightOverride(218.0f);
    StatusFrameSize->SetContent(StatusFrameRoot);

    UOverlaySlot* StatusSlot = Root->AddChildToOverlay(StatusFrameSize);
    StatusSlot->SetHorizontalAlignment(HAlign_Left);
    StatusSlot->SetVerticalAlignment(VAlign_Top);
    StatusSlot->SetPadding(FMargin(34.0f, 30.0f, 0.0f, 0.0f));

    // Top-right world clock. It intentionally shows canonical simulation time, never wall-clock time.
    UBorder* ClockPanel = TruongSinhHUD::Panel(WidgetTree, TEXT("ClockPanel"), TruongSinhHUD::Ink,
        FMargin(20.0f, 14.0f));
    UVerticalBox* ClockStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ClockStack"));
    ClockPanel->SetContent(ClockStack);
    TimeText = TruongSinhHUD::Text(WidgetTree, TEXT("TimeText"), FText::GetEmpty(), 15, TruongSinhHUD::Ivory, true);
    TimeText->SetJustification(ETextJustify::Right);
    TruongSinhHUD::AddVertical(ClockStack, TimeText);
    RevisionText = TruongSinhHUD::Text(WidgetTree, TEXT("RevisionText"), FText::GetEmpty(), 10, TruongSinhHUD::Muted);
    RevisionText->SetJustification(ETextJustify::Right);
    TruongSinhHUD::AddVertical(ClockStack, RevisionText, FMargin(0.0f, 3.0f, 0.0f, 0.0f));

    UOverlaySlot* ClockSlot = Root->AddChildToOverlay(ClockPanel);
    ClockSlot->SetHorizontalAlignment(HAlign_Right);
    ClockSlot->SetVerticalAlignment(VAlign_Top);
    ClockSlot->SetPadding(FMargin(0.0f, 30.0f, 34.0f, 0.0f));

    // Context-sensitive interaction prompt.
    InteractionPanel = TruongSinhHUD::Panel(WidgetTree, TEXT("InteractionPanel"), TruongSinhHUD::DeepJade,
        FMargin(17.0f, 11.0f, 21.0f, 11.0f));
    UHorizontalBox* PromptRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("PromptRow"));
    InteractionPanel->SetContent(PromptRow);
    UBorder* KeyCap = TruongSinhHUD::Panel(WidgetTree, TEXT("InteractKeyCap"), TruongSinhHUD::Gold,
        FMargin(10.0f, 4.0f));
    UTextBlock* KeyText = TruongSinhHUD::Text(WidgetTree, TEXT("InteractKey"), FText::FromString(TEXT("E")),
        13, TruongSinhHUD::Ink, true);
    KeyCap->SetContent(KeyText);
    TruongSinhHUD::AddHorizontal(PromptRow, KeyCap, FMargin(0.0f, 0.0f, 12.0f, 0.0f));
    InteractionText = TruongSinhHUD::Text(WidgetTree, TEXT("InteractionText"),
        NSLOCTEXT("TruongSinhHUD", "DefaultInteract", "Cảm ứng linh khí"), 14, TruongSinhHUD::Ivory);
    TruongSinhHUD::AddHorizontal(PromptRow, InteractionText);

    UOverlaySlot* PromptSlot = Root->AddChildToOverlay(InteractionPanel);
    PromptSlot->SetHorizontalAlignment(HAlign_Center);
    PromptSlot->SetVerticalAlignment(VAlign_Bottom);
    PromptSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 94.0f));
    InteractionPanel->SetVisibility(ESlateVisibility::Collapsed);

    // Persistent input legend kept deliberately quiet so the world remains dominant.
    UBorder* HintPanel = TruongSinhHUD::Panel(WidgetTree, TEXT("HintPanel"), FLinearColor(0.01f, 0.02f, 0.02f, 0.74f),
        FMargin(18.0f, 9.0f));
    UTextBlock* Hint = TruongSinhHUD::Text(WidgetTree, TEXT("ControlHint"),
        NSLOCTEXT("TruongSinhHUD", "ControlHint", "WASD  DI CHUYỂN     ·     CHUỘT  QUAN SÁT     ·     SPACE  NHẢY     ·     ESC  TẠM DỪNG"),
        10, TruongSinhHUD::Muted);
    HintPanel->SetContent(Hint);
    UOverlaySlot* HintSlot = Root->AddChildToOverlay(HintPanel);
    HintSlot->SetHorizontalAlignment(HAlign_Center);
    HintSlot->SetVerticalAlignment(VAlign_Bottom);
    HintSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 28.0f));

    // Resolution card: Plan -> Resolve -> Commit -> Present, with no reroll affordance.
    ResultPanel = TruongSinhHUD::Panel(WidgetTree, TEXT("ResultPanel"), TruongSinhHUD::Ink,
        FMargin(0.0f));
    UHorizontalBox* ResultRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ResultRow"));
    ResultPanel->SetContent(ResultRow);
    ResultAccent = TruongSinhHUD::Panel(WidgetTree, TEXT("ResultAccent"), TruongSinhHUD::Jade, FMargin(0.0f));
    USizeBox* AccentSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ResultAccentSize"));
    AccentSize->SetWidthOverride(4.0f);
    AccentSize->SetContent(ResultAccent);
    TruongSinhHUD::AddHorizontal(ResultRow, AccentSize);
    UVerticalBox* ResultStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ResultStack"));
    TruongSinhHUD::AddHorizontal(ResultRow, ResultStack, FMargin(21.0f, 16.0f, 24.0f, 17.0f));
    ResultTitleText = TruongSinhHUD::Text(WidgetTree, TEXT("ResultTitleText"), FText::GetEmpty(), 19,
        TruongSinhHUD::Gold, true);
    TruongSinhHUD::AddVertical(ResultStack, ResultTitleText);
    ResultDetailsText = TruongSinhHUD::Text(WidgetTree, TEXT("ResultDetailsText"), FText::GetEmpty(), 12,
        TruongSinhHUD::PaleJade);
    ResultDetailsText->SetAutoWrapText(true);
    TruongSinhHUD::AddVertical(ResultStack, ResultDetailsText, FMargin(0.0f, 7.0f, 0.0f, 0.0f));
    ResultFrameRoot = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("ResultFrameRoot"));
    if (UImage* ResultFrame = TruongSinhHUD::Frame(
        WidgetTree, TEXT("ResultOrnateFrame"), OrnateFrameTexture, FVector2D(430.0f, 184.0f)))
    {
        TruongSinhHUD::FillOverlay(ResultFrameRoot, ResultFrame);
    }
    TruongSinhHUD::FillOverlay(ResultFrameRoot, ResultPanel);
    USizeBox* ResultFrameSize = WidgetTree->ConstructWidget<USizeBox>(
        USizeBox::StaticClass(), TEXT("ResultFrameSize"));
    ResultFrameSize->SetWidthOverride(430.0f);
    ResultFrameSize->SetHeightOverride(184.0f);
    ResultFrameSize->SetContent(ResultFrameRoot);
    UOverlaySlot* ResultSlot = Root->AddChildToOverlay(ResultFrameSize);
    ResultSlot->SetHorizontalAlignment(HAlign_Right);
    ResultSlot->SetVerticalAlignment(VAlign_Center);
    ResultSlot->SetPadding(FMargin(0.0f, 0.0f, 34.0f, 10.0f));
    ResultFrameRoot->SetVisibility(ESlateVisibility::Collapsed);

    // Full-screen pause treatment. Input ownership remains in the player controller.
    PauseOverlay = TruongSinhHUD::Panel(WidgetTree, TEXT("PauseOverlay"), FLinearColor(0.004f, 0.008f, 0.009f, 0.82f),
        FMargin(0.0f));
    UOverlay* PauseRoot = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("PauseRoot"));
    PauseOverlay->SetContent(PauseRoot);
    UBorder* PauseCard = TruongSinhHUD::Panel(WidgetTree, TEXT("PauseCard"), TruongSinhHUD::DeepJade,
        FMargin(54.0f, 39.0f, 54.0f, 42.0f));
    UVerticalBox* PauseStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PauseStack"));
    PauseCard->SetContent(PauseStack);
    UTextBlock* PauseEyebrow = TruongSinhHUD::Text(WidgetTree, TEXT("PauseEyebrow"),
        NSLOCTEXT("TruongSinhHUD", "PauseEyebrow", "TĨNH TÂM"), 11, TruongSinhHUD::Gold, true);
    PauseEyebrow->SetJustification(ETextJustify::Center);
    TruongSinhHUD::AddVertical(PauseStack, PauseEyebrow);
    UTextBlock* PauseTitle = TruongSinhHUD::Text(WidgetTree, TEXT("PauseTitle"),
        NSLOCTEXT("TruongSinhHUD", "PauseTitle", "Thời gian đã ngưng đọng"), 29, TruongSinhHUD::Ivory, true);
    PauseTitle->SetJustification(ETextJustify::Center);
    TruongSinhHUD::AddVertical(PauseStack, PauseTitle, FMargin(0.0f, 10.0f, 0.0f, 7.0f));
    UTextBlock* PauseHint = TruongSinhHUD::Text(WidgetTree, TEXT("PauseHint"),
        NSLOCTEXT("TruongSinhHUD", "PauseHint", "Nhấn  ESC  để tiếp tục tiên lộ"), 12, TruongSinhHUD::Muted);
    PauseHint->SetJustification(ETextJustify::Center);
    TruongSinhHUD::AddVertical(PauseStack, PauseHint);
    UOverlay* PauseCardFrameRoot = WidgetTree->ConstructWidget<UOverlay>(
        UOverlay::StaticClass(), TEXT("PauseCardFrameRoot"));
    if (UImage* PauseFrame = TruongSinhHUD::Frame(
        WidgetTree, TEXT("PauseOrnateFrame"), OrnateFrameTexture, FVector2D(720.0f, 330.0f)))
    {
        TruongSinhHUD::FillOverlay(PauseCardFrameRoot, PauseFrame);
    }
    TruongSinhHUD::FillOverlay(PauseCardFrameRoot, PauseCard);
    USizeBox* PauseFrameSize = WidgetTree->ConstructWidget<USizeBox>(
        USizeBox::StaticClass(), TEXT("PauseFrameSize"));
    PauseFrameSize->SetWidthOverride(720.0f);
    PauseFrameSize->SetHeightOverride(330.0f);
    PauseFrameSize->SetContent(PauseCardFrameRoot);
    UOverlaySlot* PauseCardSlot = PauseRoot->AddChildToOverlay(PauseFrameSize);
    PauseCardSlot->SetHorizontalAlignment(HAlign_Center);
    PauseCardSlot->SetVerticalAlignment(VAlign_Center);
    UOverlaySlot* PauseSlot = Root->AddChildToOverlay(PauseOverlay);
    PauseSlot->SetHorizontalAlignment(HAlign_Fill);
    PauseSlot->SetVerticalAlignment(VAlign_Fill);
    PauseOverlay->SetVisibility(ESlateVisibility::Collapsed);

    return Super::RebuildWidget();
}

void UTruongSinhRuntimeHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshState();
}

void UTruongSinhRuntimeHUDWidget::SetInteractionPrompt(const FText& Prompt, const bool bVisible)
{
    if (InteractionText)
    {
        InteractionText->SetText(Prompt.IsEmpty() ?
            NSLOCTEXT("TruongSinhHUD", "DefaultInteract", "Cảm ứng linh khí") : Prompt);
    }
    if (InteractionPanel)
    {
        InteractionPanel->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
    }
}

void UTruongSinhRuntimeHUDWidget::ShowActivityResult(const FText& ResultTitle, const FText& ResultDetails,
    const bool bSuccess)
{
    if (ResultTitleText)
    {
        ResultTitleText->SetText(ResultTitle);
        ResultTitleText->SetColorAndOpacity(FSlateColor(bSuccess ? TruongSinhHUD::Gold : TruongSinhHUD::Failure));
    }
    if (ResultDetailsText)
    {
        ResultDetailsText->SetText(ResultDetails);
    }
    if (ResultAccent)
    {
        ResultAccent->SetBrushColor(bSuccess ? TruongSinhHUD::Jade : TruongSinhHUD::Failure);
    }
    if (ResultFrameRoot)
    {
        ResultFrameRoot->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    RefreshState();
}

void UTruongSinhRuntimeHUDWidget::SetPaused(const bool bPaused)
{
    if (PauseOverlay)
    {
        PauseOverlay->SetVisibility(bPaused ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UTruongSinhRuntimeHUDWidget::RefreshState()
{
    const UGameInstance* GameInstance = GetGameInstance();
    const UTruongSinhGameSimulationFacade* Simulation = GameInstance ?
        GameInstance->GetSubsystem<UTruongSinhGameSimulationFacade>() : nullptr;
    if (!Simulation)
    {
        return;
    }

    const FTruongSinhSimulationState State = Simulation->GetState();
    FString Realm = TEXT("PHÀM NHÂN");
    if (!State.CurrentVessel.RealmId.Value.IsEmpty())
    {
        const FString& RealmId = State.CurrentVessel.RealmId.Value;
        if (RealmId != TEXT("realm.mortal"))
        {
            Realm = RealmId.Replace(TEXT("realm."), TEXT("")).Replace(TEXT("_"), TEXT(" ")).ToUpper();
        }
    }
    if (RealmText)
    {
        RealmText->SetText(FText::FromString(Realm));
    }
    if (CultivationText)
    {
        CultivationText->SetText(FText::Format(
            NSLOCTEXT("TruongSinhHUD", "CultivationFormat", "Tu vi  {0}  ·  Thọ nguyên  {1} ngày"),
            FText::AsNumber(State.CurrentVessel.CultivationUnits),
            FText::AsNumber(State.CurrentVessel.Lifespan.RemainingLifespanDays())));
    }

    const int64 Day = State.ElapsedMinutes / 1440 + 1;
    const int64 MinuteOfDay = State.ElapsedMinutes % 1440;
    const int64 Hour = MinuteOfDay / 60;
    const int64 Minute = MinuteOfDay % 60;
    if (TimeText)
    {
        TimeText->SetText(FText::Format(NSLOCTEXT("TruongSinhHUD", "TimeFormat", "Ngày {0}  ·  {1}:{2}"),
            FText::AsNumber(Day),
            FText::FromString(FString::Printf(TEXT("%02lld"), static_cast<long long>(Hour))),
            FText::FromString(FString::Printf(TEXT("%02lld"), static_cast<long long>(Minute)))));
    }
    if (RevisionText)
    {
        RevisionText->SetText(FText::Format(NSLOCTEXT("TruongSinhHUD", "RevisionFormat", "Thiên đạo lưu chuyển  ·  #{0}"),
            FText::AsNumber(State.WorldRevision)));
    }
}
