#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "JungleKnightMovement.generated.h"

class AClimbableSurface;
class AController;

UENUM()
enum EKnightState { EKS_Walking, EKS_Climbing };

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRootMotionEvent);

UCLASS()
class JUNGLEKNIGHT_API UJungleKnightMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	FRootMotionEvent OnJumpFromClimb;
	FRootMotionEvent OnFinishClimb;

	//Input
	void ReceiveForwardInput(float Value);

	void ReceiveRightInput(float Value);

	void ReceiveJumpStart();

	void ReceiveJumpEnd();

	void ReceiveClimbAnimationComplete();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsClimbing() const { return MoveState == EKS_Climbing; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EKnightState GetState() const { return MoveState; }

protected:

	uint8 bInClimbArea : 1;

	uint8 bOnClimbLedge : 1;

	void BeginPlay() override;

	UFUNCTION()
	void OverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

	void BeginClimb();

	void EndClimb();

	void SetKnightState(EKnightState State);

	
	void WalkForward(float Value);
	void WalkRight(float Value);

	void JumpFromClimb();

	void ClimbUp(float Value);

	void ClimbDown(float Value);

	void ClimbSide(float Value);

private:

	AClimbableSurface* ClimbSurface;

	TEnumAsByte<EKnightState> MoveState;

	AController* Controller;

	FVector LastClimbNormal;
};
