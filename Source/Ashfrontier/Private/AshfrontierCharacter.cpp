#include "AshfrontierCharacter.h"

#include "AshfrontierDamageModelComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

AAshfrontierCharacter::AAshfrontierCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->bOrientRotationToMovement = true;
    Movement->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    Movement->JumpZVelocity = 500.0f;
    Movement->AirControl = 0.35f;
    Movement->MaxWalkSpeed = 480.0f;
    Movement->MinAnalogWalkSpeed = 20.0f;
    Movement->BrakingDecelerationWalking = 1800.0f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 420.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    DamageModel = CreateDefaultSubobject<UAshfrontierDamageModelComponent>(TEXT("DamageModel"));
    Inventory = CreateDefaultSubobject<UAshfrontierInventoryComponent>(TEXT("Inventory"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

    PlaceholderBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderBody"));
    PlaceholderBody->SetupAttachment(RootComponent);
    PlaceholderBody->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.8f));
    PlaceholderBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (CubeMesh.Succeeded())
    {
        PlaceholderBody->SetStaticMesh(CubeMesh.Object);
    }

    SelectionMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionMarker"));
    SelectionMarker->SetupAttachment(RootComponent);
    SelectionMarker->SetRelativeLocation(FVector(0.0f, 0.0f, -92.0f));
    SelectionMarker->SetRelativeScale3D(FVector(1.35f, 1.35f, 0.04f));
    SelectionMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SelectionMarker->SetVisibility(false);
    if (CubeMesh.Succeeded())
    {
        SelectionMarker->SetStaticMesh(CubeMesh.Object);
    }

    CommandDestination = GetActorLocation();
    SquadDisplayName = TEXT("未編隊隊員");
}

void AAshfrontierCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UpdateSquadOrder(DeltaSeconds);
    UpdateCarriedPose();
}

void AAshfrontierCharacter::SetSquadIndex(int32 NewIndex)
{
    SquadIndex = NewIndex;
}

int32 AAshfrontierCharacter::GetSquadIndex() const
{
    return SquadIndex;
}

void AAshfrontierCharacter::SetSquadDisplayName(const FString& NewName)
{
    SquadDisplayName = NewName;
}

const FString& AAshfrontierCharacter::GetSquadDisplayName() const
{
    return SquadDisplayName;
}

void AAshfrontierCharacter::SetSelected(bool bNewSelected)
{
    bSelected = bNewSelected;
    if (SelectionMarker)
    {
        SelectionMarker->SetVisibility(bSelected);
    }
}

bool AAshfrontierCharacter::IsSelected() const
{
    return bSelected;
}

void AAshfrontierCharacter::CommandMoveTo(const FVector& Destination)
{
    CommandDestination = FVector(Destination.X, Destination.Y, GetActorLocation().Z);
    FollowTarget.Reset();
    CurrentOrder = EAshfrontierSquadOrder::Moving;
}

void AAshfrontierCharacter::CommandFollow(AAshfrontierCharacter* TargetCharacter)
{
    if (TargetCharacter == nullptr || TargetCharacter == this)
    {
        CommandHold();
        return;
    }

    FollowTarget = TargetCharacter;
    CurrentOrder = EAshfrontierSquadOrder::Following;
}

void AAshfrontierCharacter::CommandHold()
{
    FollowTarget.Reset();
    CommandDestination = GetActorLocation();
    CurrentOrder = EAshfrontierSquadOrder::Holding;
}

EAshfrontierSquadOrder AAshfrontierCharacter::GetCurrentOrder() const
{
    return CurrentOrder;
}

FVector AAshfrontierCharacter::GetCommandDestination() const
{
    return CommandDestination;
}

AAshfrontierCharacter* AAshfrontierCharacter::GetFollowTarget() const
{
    return FollowTarget.Get();
}

FString AAshfrontierCharacter::GetOrderLabelZh() const
{
    switch (CurrentOrder)
    {
    case EAshfrontierSquadOrder::Moving:
        return TEXT("移動");
    case EAshfrontierSquadOrder::Following:
        return TEXT("跟隨");
    case EAshfrontierSquadOrder::Holding:
        return TEXT("待命");
    case EAshfrontierSquadOrder::Idle:
    default:
        return TEXT("閒置");
    }
}

void AAshfrontierCharacter::SetCameraArmLength(float NewArmLength)
{
    if (CameraBoom)
    {
        CameraBoom->TargetArmLength = NewArmLength;
    }
}

float AAshfrontierCharacter::GetCameraArmLength() const
{
    return CameraBoom ? CameraBoom->TargetArmLength : 0.0f;
}

UAshfrontierDamageModelComponent* AAshfrontierCharacter::GetDamageModel() const
{
    return DamageModel;
}

UAshfrontierInventoryComponent* AAshfrontierCharacter::GetInventory() const
{
    return Inventory;
}

void AAshfrontierCharacter::SetCharacterTeam(EAshfrontierCharacterTeam NewTeam)
{
    CharacterTeam = NewTeam;
}

EAshfrontierCharacterTeam AAshfrontierCharacter::GetCharacterTeam() const
{
    return CharacterTeam;
}

bool AAshfrontierCharacter::IsHostileToPlayer() const
{
    return CharacterTeam == EAshfrontierCharacterTeam::Hostile;
}

void AAshfrontierCharacter::SetFactionId(const FName& NewFactionId)
{
    FactionId = NewFactionId;
}

FName AAshfrontierCharacter::GetFactionId() const
{
    return FactionId;
}

void AAshfrontierCharacter::SetCarriedTarget(AAshfrontierCharacter* Target)
{
    CarriedTarget = Target;
}

AAshfrontierCharacter* AAshfrontierCharacter::GetCarriedTarget() const
{
    return CarriedTarget.Get();
}

void AAshfrontierCharacter::SetCarrier(AAshfrontierCharacter* NewCarrier)
{
    Carrier = NewCarrier;
}

AAshfrontierCharacter* AAshfrontierCharacter::GetCarrier() const
{
    return Carrier.Get();
}

bool AAshfrontierCharacter::IsBeingCarried() const
{
    return Carrier.IsValid();
}

void AAshfrontierCharacter::SetRecruitable(bool bNewRecruitable, int32 NewRecruitCost)
{
    bRecruitable = bNewRecruitable;
    RecruitCost = FMath::Max(0, NewRecruitCost);
}

bool AAshfrontierCharacter::IsRecruitable() const
{
    return bRecruitable;
}

int32 AAshfrontierCharacter::GetRecruitCost() const
{
    return RecruitCost;
}

void AAshfrontierCharacter::SetShopkeeper(bool bNewShopkeeper)
{
    bShopkeeper = bNewShopkeeper;
}

bool AAshfrontierCharacter::IsShopkeeper() const
{
    return bShopkeeper;
}

void AAshfrontierCharacter::UpdateSquadOrder(float DeltaSeconds)
{
    (void)DeltaSeconds;

    if (IsBeingCarried() || (DamageModel && DamageModel->IsDownedOrUnconscious()))
    {
        return;
    }

    if (CurrentOrder == EAshfrontierSquadOrder::Moving)
    {
        const FVector CurrentLocation = GetActorLocation();
        const FVector DeltaToTarget = CommandDestination - CurrentLocation;
        if (FVector::Dist2D(CurrentLocation, CommandDestination) <= ArrivalRadius)
        {
            CurrentOrder = EAshfrontierSquadOrder::Idle;
            return;
        }

        AddMovementInput(DeltaToTarget.GetSafeNormal2D(), 1.0f);
        return;
    }

    if (CurrentOrder == EAshfrontierSquadOrder::Following)
    {
        AAshfrontierCharacter* TargetCharacter = FollowTarget.Get();
        if (!IsValid(TargetCharacter))
        {
            CurrentOrder = EAshfrontierSquadOrder::Idle;
            return;
        }

        const FVector CurrentLocation = GetActorLocation();
        const FVector TargetLocation = TargetCharacter->GetActorLocation();
        const float Distance = FVector::Dist2D(CurrentLocation, TargetLocation);
        if (Distance > FollowDistance)
        {
            AddMovementInput((TargetLocation - CurrentLocation).GetSafeNormal2D(), 1.0f);
        }
    }
}

void AAshfrontierCharacter::UpdateCarriedPose()
{
    AAshfrontierCharacter* CurrentCarrier = Carrier.Get();
    if (!IsValid(CurrentCarrier))
    {
        return;
    }

    const FVector CarryOffset = CurrentCarrier->GetActorForwardVector() * -90.0f + FVector(0.0f, 0.0f, 85.0f);
    SetActorLocation(CurrentCarrier->GetActorLocation() + CarryOffset);
    SetActorRotation(CurrentCarrier->GetActorRotation());
}
