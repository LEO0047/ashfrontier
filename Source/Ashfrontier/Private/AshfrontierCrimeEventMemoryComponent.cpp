#include "AshfrontierCrimeEventMemoryComponent.h"

UAshfrontierCrimeEventMemoryComponent::UAshfrontierCrimeEventMemoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAshfrontierCrimeEventMemoryComponent::RecordEvent(const FAshfrontierCrimeEventRecord& EventRecord)
{
    if (EventRecord.RemainingMemorySeconds <= 0.0f)
    {
        return;
    }

    Events.Add(EventRecord);
}

void UAshfrontierCrimeEventMemoryComponent::TickMemory(float DeltaSeconds)
{
    if (DeltaSeconds <= 0.0f)
    {
        return;
    }

    for (FAshfrontierCrimeEventRecord& Event : Events)
    {
        Event.RemainingMemorySeconds -= DeltaSeconds;
    }

    Events.RemoveAll([](const FAshfrontierCrimeEventRecord& Event)
    {
        return Event.RemainingMemorySeconds <= 0.0f;
    });
}

bool UAshfrontierCrimeEventMemoryComponent::HasMemoryFor(const FName& OffenderFactionId, EAshfrontierLegalEventType EventType) const
{
    return Events.ContainsByPredicate([OffenderFactionId, EventType](const FAshfrontierCrimeEventRecord& Event)
    {
        return Event.OffenderFactionId == OffenderFactionId && Event.EventType == EventType;
    });
}

int32 UAshfrontierCrimeEventMemoryComponent::GetMemoryCount() const
{
    return Events.Num();
}

const TArray<FAshfrontierCrimeEventRecord>& UAshfrontierCrimeEventMemoryComponent::GetEvents() const
{
    return Events;
}
