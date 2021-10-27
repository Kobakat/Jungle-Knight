#include "KnightAnimInstance.h"
#include "JungleKnightCharacter.h"
#include "JungleKnightMovement.h"

void UKnightAnimInstance::SetAnalogWeight()
{
	if (Movement != nullptr)
	{
		AnalogWeight = Movement->GetAnalogInputModifier();	
	}	
}

void UKnightAnimInstance::SetAnalogDirection()
{
	if (Movement != nullptr)
	{
		const FVector MeshForward = GetSkelMeshComponent()->GetRightVector();
		const FVector InputForward = Movement->GetLastInputVector().GetSafeNormal();

		float Sign = (MeshForward ^ InputForward).Z;

		if (Sign > 0) Sign = 1;
		else if (Sign < 0) Sign = -1;
		else Sign = 0;

		const float Dot = MeshForward | InputForward;
		const float Angle = ((1 - Dot) * 90) * Sign;

		AnalogDirection = Angle;
	}
}

void UKnightAnimInstance::SetClimbDirection()
{
	if (Movement != nullptr)
	{
		const FVector Input = Movement->GetLastInputVector();

		AnalogX = Input.X;
		AnalogZ = Input.Z;
	}
}

void UKnightAnimInstance::NativeBeginPlay()
{
	Character = Cast<AJungleKnightCharacter>(TryGetPawnOwner());
	Movement = Character->GetKnightMovement();

	Movement->OnJumpFromClimb.AddDynamic(this, &UKnightAnimInstance::PlayClimbJump);
	Movement->OnFinishClimb.AddDynamic(this, &UKnightAnimInstance::PlayClimbOver);
	OnMontageEnded.AddDynamic(this, &UKnightAnimInstance::MontageEnded);
}

void UKnightAnimInstance::PlayClimbJump()
{
	Montage_Play(ClimbJump);
}

void UKnightAnimInstance::PlayClimbOver()
{
	Montage_Play(ClimbOver);
}

void UKnightAnimInstance::MontageEnded(UAnimMontage* Montage, bool bInteruppted)
{
	//Todo check assets
	Movement->ReceiveClimbAnimationComplete();
}