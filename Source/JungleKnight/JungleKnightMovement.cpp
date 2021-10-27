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
		if (Value > 0)
		{
			ClimbUp(Value);
		}
		else
		{
			ClimbDown(Value);
		}
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
	case EKS_Climbing:
		ClimbSide(Value);
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

void UJungleKnightMovement::ClimbUp(float Value)
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
		bOnClimbLedge = false;
		LastClimbNormal = Hit.ImpactNormal;
		AddInputVector(FVector::UpVector * Value);
	}
}

void UJungleKnightMovement::ClimbDown(float Value)
{
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	//Good enough
	const float Epsilon = 5.f;

	const float HalfHeight = GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FVector Start = GetActorLocation() + FVector(0, 0, -HalfHeight);
	FVector End = Start + (FVector::DownVector * Epsilon);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, QueryParams);

	if (bHit)
	{
		//We climbed down into an object (like the ground)

		return;
	}

	const float Radius = GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius();
	End = Start + (GetOwner()->GetActorForwardVector() * Epsilon);

	bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, QueryParams);

	//There is still room to climb down, move down
	if(bHit)
	{
		LastClimbNormal = Hit.ImpactNormal;
		AddInputVector(FVector::DownVector * Value);
	}
}

void UJungleKnightMovement::ClimbSide(float Value)
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const float Actual = (Direction ^ GetOwner()->GetActorForwardVector()).Z;

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	//Good enough
	const float Epsilon = 5.f;
	FVector MoveDirection = GetOwner()->GetActorRightVector();

	if (Value < 0)
	{
		MoveDirection *= -1;
	}

	const float Radius = GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius();

	const FVector Start = GetActorLocation() + MoveDirection;
	const FVector End = Start + (GetOwner()->GetActorForwardVector() * (Radius + Epsilon));

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, QueryParams);

	//There is still room to climb sideways, move over
	if (bHit)
	{
		LastClimbNormal = Hit.ImpactNormal;
		AddInputVector(MoveDirection * Value);
	}
}
