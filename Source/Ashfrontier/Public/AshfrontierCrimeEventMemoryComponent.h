#pragma once

#include "CoreMinimal.h"
#include "AshfrontierLegalTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierCrimeEventMemoryComponent.generated.h"

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierCrimeEventMemoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierCrimeEventMemoryComponent();

    void RecordEvent(const FAshfrontierCrimeEventRecord& EventRecord);
    void TickMemory(float DeltaSeconds);
    bool HasMemoryFor(const FName& OffenderFactionId, EAshfrontierLegalEventType EventType) const;
    int32 GetMemoryCount() const;
    const TArray<FAshfrontierCrimeEventRecord>& GetEvents() const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Legal")
    TArray<FAshfrontierCrimeEventRecord> Events;
};
