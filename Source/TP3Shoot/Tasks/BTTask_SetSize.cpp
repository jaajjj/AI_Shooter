#include "BTTask_SetSize.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../TP3ShootCharacter.h"

UBTTask_SetSize::UBTTask_SetSize()
{
	NodeName = TEXT("Set Size");
}

EBTNodeResult::Type UBTTask_SetSize::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC) return EBTNodeResult::Failed;

	ATP3ShootCharacter* Char = Cast<ATP3ShootCharacter>(AIC->GetPawn());
	if (!Char) return EBTNodeResult::Failed;

	//si l'IA n'a pas déjà la bonne taille, on bascule
	if (Char->bIsGrand != bTargetIsGrand)
	{
		Char->ChangeSize();
	}

	//update le Blackboard
	if (BB)
	{
		BB->SetValueAsBool(IsGrandKey.SelectedKeyName, Char->bIsGrand);
	}

	return EBTNodeResult::Succeeded;
}