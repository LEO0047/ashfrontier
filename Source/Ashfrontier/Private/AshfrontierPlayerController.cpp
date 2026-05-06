#include "AshfrontierPlayerController.h"

#include "AshfrontierCameraControllerComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierOrderSystemComponent.h"
#include "AshfrontierSquadManagerComponent.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"

AAshfrontierPlayerController::AAshfrontierPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    SquadManager = CreateDefaultSubobject<UAshfrontierSquadManagerComponent>(TEXT("SquadManager"));
    OrderSystem = CreateDefaultSubobject<UAshfrontierOrderSystemComponent>(TEXT("OrderSystem"));
    CameraController = CreateDefaultSubobject<UAshfrontierCameraControllerComponent>(TEXT("CameraController"));
}

void AAshfrontierPlayerController::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);

    if (SquadManager)
    {
        SquadManager->EnsureStartingSquad(this, AAshfrontierCharacter::StaticClass(), 2);
    }
    if (CameraController)
    {
        CameraController->FocusCharacter(this, CurrentViewCharacter());
    }
}

void AAshfrontierPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!InputComponent)
    {
        return;
    }

    InputComponent->BindAction(TEXT("Select"), IE_Pressed, this, &AAshfrontierPlayerController::HandleSelectPressed);
    InputComponent->BindAction(TEXT("IssueMove"), IE_Pressed, this, &AAshfrontierPlayerController::HandleIssueMovePressed);
    InputComponent->BindAction(TEXT("FollowOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleFollowPressed);
    InputComponent->BindAction(TEXT("HoldOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleHoldPressed);
    InputComponent->BindAction(TEXT("SelectAllSquad"), IE_Pressed, this, &AAshfrontierPlayerController::HandleSelectAllPressed);
    InputComponent->BindAction(TEXT("SelectNextSquad"), IE_Pressed, this, &AAshfrontierPlayerController::HandleSelectNextPressed);
    InputComponent->BindAction(TEXT("ToggleTacticalCamera"), IE_Pressed, this, &AAshfrontierPlayerController::HandleToggleTacticalCamera);
    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AAshfrontierPlayerController::SelectSlot1);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AAshfrontierPlayerController::SelectSlot2);
    InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AAshfrontierPlayerController::SelectSlot3);
    InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AAshfrontierPlayerController::SelectSlot4);
}

UAshfrontierSquadManagerComponent* AAshfrontierPlayerController::GetSquadManager() const
{
    return SquadManager;
}

UAshfrontierOrderSystemComponent* AAshfrontierPlayerController::GetOrderSystem() const
{
    return OrderSystem;
}

UAshfrontierCameraControllerComponent* AAshfrontierPlayerController::GetCameraController() const
{
    return CameraController;
}

void AAshfrontierPlayerController::HandleSelectPressed()
{
    if (!SquadManager)
    {
        return;
    }

    FHitResult Hit;
    if (TraceCursor(Hit, ECC_Pawn))
    {
        if (AAshfrontierCharacter* HitCharacter = Cast<AAshfrontierCharacter>(Hit.GetActor()))
        {
            const bool bAppend = IsInputKeyDown(EKeys::LeftShift) || IsInputKeyDown(EKeys::RightShift);
            SquadManager->SelectMember(HitCharacter, bAppend);
            if (CameraController)
            {
                CameraController->FocusCharacter(this, SquadManager->GetPrimarySelected());
            }
        }
    }
}

void AAshfrontierPlayerController::HandleIssueMovePressed()
{
    if (!SquadManager || !OrderSystem)
    {
        return;
    }

    FHitResult Hit;
    FVector Destination = FVector::ZeroVector;
    if (TraceCursor(Hit, ECC_Visibility))
    {
        Destination = Hit.ImpactPoint;
    }
    else if (AAshfrontierCharacter* ViewCharacter = CurrentViewCharacter())
    {
        Destination = ViewCharacter->GetActorLocation() + ViewCharacter->GetActorForwardVector() * 600.0f;
    }

    OrderSystem->IssueMove(SquadManager->GetSelectedMembers(), Destination);
}

void AAshfrontierPlayerController::HandleFollowPressed()
{
    if (!SquadManager || !OrderSystem)
    {
        return;
    }

    TArray<AAshfrontierCharacter*> Members = SquadManager->GetSelectedMembers();
    AAshfrontierCharacter* Target = SquadManager->GetLeader();
    if (Members.Num() == 1 && Members[0] == Target)
    {
        Members = SquadManager->GetSquadMembers();
    }
    OrderSystem->IssueFollow(Members, Target);
}

void AAshfrontierPlayerController::HandleHoldPressed()
{
    if (SquadManager && OrderSystem)
    {
        OrderSystem->IssueHold(SquadManager->GetSelectedMembers());
    }
}

void AAshfrontierPlayerController::HandleSelectAllPressed()
{
    if (SquadManager)
    {
        SquadManager->SelectAll();
    }
}

void AAshfrontierPlayerController::HandleSelectNextPressed()
{
    if (SquadManager)
    {
        SquadManager->SelectNext();
        if (CameraController)
        {
            CameraController->FocusCharacter(this, SquadManager->GetPrimarySelected());
        }
    }
}

void AAshfrontierPlayerController::HandleToggleTacticalCamera()
{
    if (CameraController)
    {
        CameraController->ToggleTacticalCamera(this, CurrentViewCharacter());
    }
}

void AAshfrontierPlayerController::SelectSlot1()
{
    SelectSlot(0);
}

void AAshfrontierPlayerController::SelectSlot2()
{
    SelectSlot(1);
}

void AAshfrontierPlayerController::SelectSlot3()
{
    SelectSlot(2);
}

void AAshfrontierPlayerController::SelectSlot4()
{
    SelectSlot(3);
}

void AAshfrontierPlayerController::SelectSlot(int32 ZeroBasedIndex)
{
    if (SquadManager)
    {
        SquadManager->SelectMemberAtIndex(ZeroBasedIndex, false);
        if (CameraController)
        {
            CameraController->FocusCharacter(this, SquadManager->GetPrimarySelected());
        }
    }
}

bool AAshfrontierPlayerController::TraceCursor(FHitResult& OutHit, ECollisionChannel Channel) const
{
    return GetHitResultUnderCursor(Channel, false, OutHit) && OutHit.bBlockingHit;
}

AAshfrontierCharacter* AAshfrontierPlayerController::CurrentViewCharacter() const
{
    return SquadManager ? SquadManager->GetPrimarySelected() : Cast<AAshfrontierCharacter>(GetPawn());
}
