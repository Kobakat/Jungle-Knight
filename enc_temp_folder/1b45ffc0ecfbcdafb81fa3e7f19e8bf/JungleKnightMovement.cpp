#include "JungleKnightMovement.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "ClimbableSurface.h"


void UJungleKnightMovement::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UJungleKnightMovement::OverlapBegin);
	GetOwner()->OnActorEndOverlap.AddDynamic(this, &UJungleKnightMovement::OverlapEnd);

	Controller = GetCharacterOwner()->GetController();
}

void UJungleKnightMovement::OverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA<AClimbableSurface>())
	{
		bInClimbArea = true;
		ClimbSurface = Cast<AClimbableSurface>(OtherActor);
	}
}

void UJungleKnightMovement::OverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->IsA<AClimbableSurface>())
	{
		bInClimbArea = false;
		ClimbSurface = nullptr;
	}
}

void UJungleKnightMovement::SetKnightState(EKnightState NewState)
{
	MoveState = NewState;

	if (NewState == EKS_Climbing)
	{
		SetMovementMode(MOVE_Flying);
		bOrientRotationToMovement = false;
	}

	else
	{
		SetMovementMode(MOVE_Walking);
		bOrientRotationToMovement = true;
	}
}

void UJungleKnightMovement::BeginClimb()
{
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	const FVector Start = GetActorLocation();
	const FVector End = ClimbSurface->GetActorLocation();

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, QueryParams))
	{
		//Face the wall
		GetOwner()->SetActorRotation((-Hit.ImpactNormal).ToOrientationQuat());

		//Start "climbing"
		SetKnightState(EKS_Climbing);		
	}
}


void UJungleKnightMovement::EndClimb()
{
	SetKnightState(EKS_Walking);
	bInClimbArea = false;
	ClimbSurface = nullptr;
	const FQuat Rotation = ((-LastClimbNormal).GetSafeNormal2D()).ToOrientationQuat();
	GetOwner()->SetActorRotation(Rotation);
}




/// INPUT

void UJungleKnightMovement::ReceiveForwardInput(float Value)
{
	if (Controller == nullptr || Value == 0.0f)
	{
		return;
	}

	switch (MoveState)
	{
	case EKS_Walking:
		WalkForward(Value);
		break;
	case EKS_Climbing:
		Climb(Value);
		break;
	}
}

void UJungleKnightMovement::ReceiveRightInput(float Value)
{
	if (Controller == nullptr || Value == 0.0f)
	{
		return;
	}

	switch (MoveState)
	{
	case EKS_Walking:
		WalkRight(Value);
		break;
	}
}

void UJungleKnightMovement::ReceiveJumpStart()
{
	switch (MoveState)
	{
	case EKS_Walking:
		if (bInClimbArea)
		{
			BeginClimb();
		}
		else
		{
			GetCharacterOwner()->Jump();
		}
		break;
	case EKS_Climbing:
		JumpFromClimb();
		break;
	}
}

void UJungleKnightMovement::ReceiveJumpEnd()
{
	switch (MoveState)
	{
	case EKS_Walking:
		GetCharacterOwner()->StopJumping();
		break;
	}
}


///INPUT RESPONSES

void UJungleKnightMovement::WalkForward(float Value)
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddInputVector(Direction * Value);
}

void UJungleKnightMovement::WalkRight(float Value)
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddInputVector(Direction * Value);
}

void UJungleKnightMovement::JumpFromClimb()
{
	if (bOnClimbLedge)
	{
		OnFinishClimb.Broadcast();
	}

	else
	{
		OnJumpFromClimb.Broadcast();
	}	
}

void UJungleKnightMovement::ReceiveClimbAnimationComplete()
{	
	EndClimb();
}

void UJungleKnightMovement::Climb(float Value)
{
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	//Good enough
	const float Epsilon = 5.f;

	float HalfHeight, Radius;
	GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleSize(Radius, HalfHeight);

	const FVector Start = GetActorLocation() + FVector(0, 0, HalfHeight);
	const FVector End = Start + GetOwner()->GetActorForwardVector() * (Radius + Epsilon);
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, QueryParams);

	if (!bHit)
	{
		bOnClimbLedge = true;

		if (Value <= 0)
		{
			AddInputVector(FVector::UpVector * Value);
		}
	}

	else
	{
		LastClimbNormal = Hit.ImpactNormal;
		AddInputVector(FVector::UpVector * Value);
	}
}
