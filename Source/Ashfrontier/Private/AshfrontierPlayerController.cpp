#include "AshfrontierPlayerController.h"

#include "AshfrontierCameraControllerComponent.h"
#include "AshfrontierCarrySystemComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierCombatResolverComponent.h"
#include "AshfrontierBuildingPlacementSystemComponent.h"
#include "AshfrontierConstructionSystemComponent.h"
#include "AshfrontierCrimeEventMemoryComponent.h"
#include "AshfrontierDamageModelComponent.h"
#include "AshfrontierFactionSystemComponent.h"
#include "AshfrontierGuardAIComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierLegalRuleSystemComponent.h"
#include "AshfrontierMedicalSystemComponent.h"
#include "AshfrontierOrderSystemComponent.h"
#include "AshfrontierPlacedBuilding.h"
#include "AshfrontierProductionSystemComponent.h"
#include "AshfrontierRecruitSystemComponent.h"
#include "AshfrontierResourceNodeSystemComponent.h"
#include "AshfrontierSaveGame.h"
#include "AshfrontierSaveGameSystemComponent.h"
#include "AshfrontierSquadManagerComponent.h"
#include "AshfrontierStorageSystemComponent.h"
#include "AshfrontierTradingSystemComponent.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"

AAshfrontierPlayerController::AAshfrontierPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;

    SquadManager = CreateDefaultSubobject<UAshfrontierSquadManagerComponent>(TEXT("SquadManager"));
    OrderSystem = CreateDefaultSubobject<UAshfrontierOrderSystemComponent>(TEXT("OrderSystem"));
    CameraController = CreateDefaultSubobject<UAshfrontierCameraControllerComponent>(TEXT("CameraController"));
    CombatResolver = CreateDefaultSubobject<UAshfrontierCombatResolverComponent>(TEXT("CombatResolver"));
    CarrySystem = CreateDefaultSubobject<UAshfrontierCarrySystemComponent>(TEXT("CarrySystem"));
    MedicalSystem = CreateDefaultSubobject<UAshfrontierMedicalSystemComponent>(TEXT("MedicalSystem"));
    RecruitSystem = CreateDefaultSubobject<UAshfrontierRecruitSystemComponent>(TEXT("RecruitSystem"));
    TradingSystem = CreateDefaultSubobject<UAshfrontierTradingSystemComponent>(TEXT("TradingSystem"));
    BuildingPlacementSystem = CreateDefaultSubobject<UAshfrontierBuildingPlacementSystemComponent>(TEXT("BuildingPlacementSystem"));
    ConstructionSystem = CreateDefaultSubobject<UAshfrontierConstructionSystemComponent>(TEXT("ConstructionSystem"));
    ResourceNodeSystem = CreateDefaultSubobject<UAshfrontierResourceNodeSystemComponent>(TEXT("ResourceNodeSystem"));
    ProductionSystem = CreateDefaultSubobject<UAshfrontierProductionSystemComponent>(TEXT("ProductionSystem"));
    StorageSystem = CreateDefaultSubobject<UAshfrontierStorageSystemComponent>(TEXT("StorageSystem"));
    FactionSystem = CreateDefaultSubobject<UAshfrontierFactionSystemComponent>(TEXT("FactionSystem"));
    LegalRuleSystem = CreateDefaultSubobject<UAshfrontierLegalRuleSystemComponent>(TEXT("LegalRuleSystem"));
    CrimeEventMemory = CreateDefaultSubobject<UAshfrontierCrimeEventMemoryComponent>(TEXT("CrimeEventMemory"));
    GuardAI = CreateDefaultSubobject<UAshfrontierGuardAIComponent>(TEXT("GuardAI"));
    SaveGameSystem = CreateDefaultSubobject<UAshfrontierSaveGameSystemComponent>(TEXT("SaveGameSystem"));
}

void AAshfrontierPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetControlRotation(FRotator(-48.0f, -35.0f, 0.0f));

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
    InputComponent->BindAction(TEXT("CarryOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleCarryPressed);
    InputComponent->BindAction(TEXT("MedicalOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleMedicalPressed);
    InputComponent->BindAction(TEXT("RecruitOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleRecruitPressed);
    InputComponent->BindAction(TEXT("TradeOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleTradePressed);
    InputComponent->BindAction(TEXT("BuildOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleBuildPressed);
    InputComponent->BindAction(TEXT("GatherOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleGatherPressed);
    InputComponent->BindAction(TEXT("ProduceOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleProducePressed);
    InputComponent->BindAction(TEXT("TheftOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleTheftPressed);
    InputComponent->BindAction(TEXT("AttackCrimeOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleAttackCrimePressed);
    InputComponent->BindAction(TEXT("SelfDefenseOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleSelfDefensePressed);
    InputComponent->BindAction(TEXT("RestrictedAreaOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleRestrictedAreaPressed);
    InputComponent->BindAction(TEXT("SaveOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleSavePressed);
    InputComponent->BindAction(TEXT("LoadOrder"), IE_Pressed, this, &AAshfrontierPlayerController::HandleLoadPressed);
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

UAshfrontierCombatResolverComponent* AAshfrontierPlayerController::GetCombatResolver() const
{
    return CombatResolver;
}

UAshfrontierCarrySystemComponent* AAshfrontierPlayerController::GetCarrySystem() const
{
    return CarrySystem;
}

UAshfrontierMedicalSystemComponent* AAshfrontierPlayerController::GetMedicalSystem() const
{
    return MedicalSystem;
}

UAshfrontierRecruitSystemComponent* AAshfrontierPlayerController::GetRecruitSystem() const
{
    return RecruitSystem;
}

UAshfrontierTradingSystemComponent* AAshfrontierPlayerController::GetTradingSystem() const
{
    return TradingSystem;
}

UAshfrontierBuildingPlacementSystemComponent* AAshfrontierPlayerController::GetBuildingPlacementSystem() const
{
    return BuildingPlacementSystem;
}

UAshfrontierConstructionSystemComponent* AAshfrontierPlayerController::GetConstructionSystem() const
{
    return ConstructionSystem;
}

UAshfrontierResourceNodeSystemComponent* AAshfrontierPlayerController::GetResourceNodeSystem() const
{
    return ResourceNodeSystem;
}

UAshfrontierProductionSystemComponent* AAshfrontierPlayerController::GetProductionSystem() const
{
    return ProductionSystem;
}

UAshfrontierStorageSystemComponent* AAshfrontierPlayerController::GetStorageSystem() const
{
    return StorageSystem;
}

UAshfrontierFactionSystemComponent* AAshfrontierPlayerController::GetFactionSystem() const
{
    return FactionSystem;
}

UAshfrontierLegalRuleSystemComponent* AAshfrontierPlayerController::GetLegalRuleSystem() const
{
    return LegalRuleSystem;
}

UAshfrontierCrimeEventMemoryComponent* AAshfrontierPlayerController::GetCrimeEventMemory() const
{
    return CrimeEventMemory;
}

UAshfrontierGuardAIComponent* AAshfrontierPlayerController::GetGuardAI() const
{
    return GuardAI;
}

UAshfrontierSaveGameSystemComponent* AAshfrontierPlayerController::GetSaveGameSystem() const
{
    return SaveGameSystem;
}

AAshfrontierPlacedBuilding* AAshfrontierPlayerController::GetLastPlacedBuilding() const
{
    return LastPlacedBuilding.Get();
}

TArray<AAshfrontierPlacedBuilding*> AAshfrontierPlayerController::GetPlacedBuildings() const
{
    TArray<AAshfrontierPlacedBuilding*> Result;
    for (AAshfrontierPlacedBuilding* Building : PlacedBuildings)
    {
        if (IsValid(Building))
        {
            Result.Add(Building);
        }
    }
    return Result;
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
        if (AAshfrontierCharacter* HitCharacter = Cast<AAshfrontierCharacter>(Hit.GetActor()))
        {
            if (HitCharacter->IsHostileToPlayer() && CombatResolver)
            {
                for (AAshfrontierCharacter* Attacker : SquadManager->GetSelectedMembers())
                {
                    CombatResolver->ResolveMeleeAttack(Attacker, HitCharacter, EAshfrontierBodyPart::Chest, 38.0f);
                }
                return;
            }
        }
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

void AAshfrontierPlayerController::HandleCarryPressed()
{
    if (!SquadManager || !CarrySystem)
    {
        return;
    }

    AAshfrontierCharacter* Carrier = SquadManager->GetPrimarySelected();
    if (!Carrier)
    {
        return;
    }

    FHitResult Hit;
    if (TraceCursor(Hit, ECC_Pawn))
    {
        if (AAshfrontierCharacter* Patient = Cast<AAshfrontierCharacter>(Hit.GetActor()))
        {
            CarrySystem->CarryDownedCharacter(Carrier, Patient);
        }
    }
}

void AAshfrontierPlayerController::HandleMedicalPressed()
{
    if (!SquadManager || !MedicalSystem)
    {
        return;
    }

    AAshfrontierCharacter* Medic = SquadManager->GetPrimarySelected();
    if (!Medic)
    {
        return;
    }

    AAshfrontierCharacter* Patient = Medic->GetCarriedTarget();
    FHitResult Hit;
    if (!Patient && TraceCursor(Hit, ECC_Pawn))
    {
        Patient = Cast<AAshfrontierCharacter>(Hit.GetActor());
    }

    if (Patient)
    {
        MedicalSystem->Bandage(Medic, Patient);
    }
}

void AAshfrontierPlayerController::HandleRecruitPressed()
{
    if (!SquadManager || !RecruitSystem)
    {
        return;
    }

    AAshfrontierCharacter* Recruiter = SquadManager->GetLeader();
    FHitResult Hit;
    if (Recruiter && TraceCursor(Hit, ECC_Pawn))
    {
        if (AAshfrontierCharacter* Candidate = Cast<AAshfrontierCharacter>(Hit.GetActor()))
        {
            RecruitSystem->Recruit(Recruiter, Candidate, SquadManager);
        }
    }
}

void AAshfrontierPlayerController::HandleTradePressed()
{
    if (!SquadManager || !TradingSystem)
    {
        return;
    }

    AAshfrontierCharacter* Buyer = SquadManager->GetLeader();
    FHitResult Hit;
    if (Buyer && TraceCursor(Hit, ECC_Pawn))
    {
        if (AAshfrontierCharacter* Seller = Cast<AAshfrontierCharacter>(Hit.GetActor()))
        {
            if (Seller->IsShopkeeper())
            {
                TradingSystem->BuyItem(Buyer, Seller, TEXT("item_field_bandage"), 1);
            }
        }
    }
}

void AAshfrontierPlayerController::HandleBuildPressed()
{
    if (!SquadManager || !BuildingPlacementSystem || !ConstructionSystem)
    {
        return;
    }

    AAshfrontierCharacter* Builder = SquadManager->GetLeader();
    if (!Builder)
    {
        return;
    }

    FVector BuildLocation(1600.0f, 1800.0f, 120.0f);
    FHitResult Hit;
    if (TraceCursor(Hit, ECC_Visibility))
    {
        BuildLocation = Hit.ImpactPoint;
    }

    if (AAshfrontierPlacedBuilding* Building = BuildingPlacementSystem->PlaceBuilding(Builder, TEXT("building_camp_kitchen"), BuildLocation, ConstructionSystem))
    {
        LastPlacedBuilding = Building;
        PlacedBuildings.Add(Building);
    }
}

void AAshfrontierPlayerController::HandleGatherPressed()
{
    if (!SquadManager || !ResourceNodeSystem)
    {
        return;
    }

    AAshfrontierCharacter* Gatherer = SquadManager->GetLeader();
    if (!Gatherer || !Gatherer->GetInventory())
    {
        return;
    }

    FName ResourceId = TEXT("resource_brackish_water");
    if (Gatherer->GetInventory()->GetItemCount(TEXT("item_salvaged_plank")) < 2)
    {
        ResourceId = TEXT("resource_salvage_wood");
    }
    else if (Gatherer->GetInventory()->GetItemCount(TEXT("item_scrap_ore")) < 1)
    {
        ResourceId = TEXT("resource_scrap_ore");
    }
    else if (Gatherer->GetInventory()->GetItemCount(TEXT("item_raw_grain")) < 2)
    {
        ResourceId = TEXT("resource_dry_grain");
    }

    ResourceNodeSystem->GatherResource(Gatherer, ResourceId, 1);
}

void AAshfrontierPlayerController::HandleProducePressed()
{
    if (!SquadManager || !ProductionSystem)
    {
        return;
    }

    AAshfrontierCharacter* Worker = SquadManager->GetLeader();
    AAshfrontierPlacedBuilding* Building = LastPlacedBuilding.Get();
    if (!Worker || !Building)
    {
        return;
    }

    if (ProductionSystem->EnqueueRecipe(Building, Worker, TEXT("recipe_grain_to_ration")))
    {
        ProductionSystem->CompleteNextProductionImmediately(Building, Worker);
    }
}

void AAshfrontierPlayerController::HandleTheftPressed()
{
    HandleLegalEvent(EAshfrontierLegalEventType::Theft);
}

void AAshfrontierPlayerController::HandleAttackCrimePressed()
{
    HandleLegalEvent(EAshfrontierLegalEventType::Attack);
}

void AAshfrontierPlayerController::HandleSelfDefensePressed()
{
    HandleLegalEvent(EAshfrontierLegalEventType::SelfDefense);
}

void AAshfrontierPlayerController::HandleRestrictedAreaPressed()
{
    HandleLegalEvent(EAshfrontierLegalEventType::RestrictedArea);
}

void AAshfrontierPlayerController::HandleLegalEvent(EAshfrontierLegalEventType EventType)
{
    if (!SquadManager || !GuardAI || !LegalRuleSystem || !FactionSystem || !CrimeEventMemory)
    {
        return;
    }

    AAshfrontierCharacter* Offender = SquadManager->GetLeader();
    if (!Offender)
    {
        return;
    }

    FHitResult Hit;
    if (TraceCursor(Hit, ECC_Pawn))
    {
        if (AAshfrontierCharacter* Guard = Cast<AAshfrontierCharacter>(Hit.GetActor()))
        {
            if (Guard != Offender)
            {
                GuardAI->HandleObservedEvent(Guard, Offender, nullptr, EventType, TEXT("legal_saltwardens_city"), LegalRuleSystem, FactionSystem, CrimeEventMemory);
            }
        }
    }
}

void AAshfrontierPlayerController::HandleSavePressed()
{
    if (!SquadManager || !SaveGameSystem)
    {
        return;
    }

    SaveGameSystem->SaveToSlot(TEXT("AshfrontierPrototype"), 0, SquadManager->GetSquadMembers(), GetPlacedBuildings(), FactionSystem, CrimeEventMemory);
}

void AAshfrontierPlayerController::HandleLoadPressed()
{
    if (!SquadManager || !SaveGameSystem)
    {
        return;
    }

    UAshfrontierSaveGame* SaveGame = SaveGameSystem->LoadFromSlot(TEXT("AshfrontierPrototype"), 0);
    TArray<AAshfrontierCharacter*> Characters = SquadManager->GetSquadMembers();
    TArray<AAshfrontierPlacedBuilding*> Buildings = GetPlacedBuildings();
    if (!SaveGameSystem->ApplyState(SaveGame, Characters, Buildings, FactionSystem, CrimeEventMemory))
    {
        return;
    }

    for (AAshfrontierCharacter* Character : Characters)
    {
        if (Character && Character->GetCharacterTeam() == EAshfrontierCharacterTeam::PlayerSquad)
        {
            SquadManager->AddExistingMember(Character);
        }
    }

    PlacedBuildings.Reset();
    for (AAshfrontierPlacedBuilding* Building : Buildings)
    {
        if (IsValid(Building))
        {
            PlacedBuildings.Add(Building);
        }
    }
    LastPlacedBuilding = Buildings.Num() > 0 ? Buildings[0] : nullptr;
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
