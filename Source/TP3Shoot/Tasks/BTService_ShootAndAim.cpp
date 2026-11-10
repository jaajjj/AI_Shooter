#include "BTService_ShootAndAim.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../TP3ShootCharacter.h"

UBTService_ShootAndAim::UBTService_ShootAndAim()
{
	NodeName = TEXT("Shoot and Aim");
	Interval = 0.6f;
	RandomDeviation = 0.1f;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTService_ShootAndAim::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB) return;

	if (AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)))
	{
		AIC->SetFocus(Target);
	}
}

void UBTService_ShootAndAim::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (Target)
	{
		AIC->SetFocus(Target);

		if (ATP3ShootCharacter* Char = Cast<ATP3ShootCharacter>(AIC->GetPawn()))
		{
			Char->Fire(); 
		}
	}
}

void UBTService_ShootAndAim::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}
}