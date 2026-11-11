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

		//vise
		if (ATP3ShootCharacter* Char = Cast<ATP3ShootCharacter>(AIC->GetPawn()))
		{
			Char->Aim();
		}
	}
}

void UBTService_ShootAndAim::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target) return;

	//si mort, on skip la cible
	if (ATP3ShootCharacter* TargetChar = Cast<ATP3ShootCharacter>(Target))
	{
		if (TargetChar->bIsDead)
		{
			BB->ClearValue(TargetActorKey.SelectedKeyName);
			AIC->ClearFocus(EAIFocusPriority::Gameplay);
			return;
		}
	}

	//en vie
	AIC->SetFocus(Target);
	if (AIC->LineOfSightTo(Target))
	{
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

		//arrete de viser
		if (ATP3ShootCharacter* Char = Cast<ATP3ShootCharacter>(AIC->GetPawn()))
		{
			Char->StopAiming();
		}
	}
}