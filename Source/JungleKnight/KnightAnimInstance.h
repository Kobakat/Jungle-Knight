#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KnightAnimInstance.generated.h"

class AJungleKnightCharacter;
class UJungleKnightMovement;

UCLASS()
class JUNGLEKNIGHT_API UKnightAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Parameters")
	AJungleKnightCharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Parameters")
	UJungleKnightMovement* Movement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ClimbJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ClimbOver;

	/** Magnitude of input vector (0 - 1)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Parameters")
	float AnalogWeight;
	
	/** Direction of input vector, relative to mesh forward (-180 to 180)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Parameters")
	float AnalogDirection;

	/** Value of input stick on left/right axis*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Parameters")
	float AnalogX;

	/** Value of input stick on up/down axis*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Parameters")
	float AnalogZ;

	UFUNCTION(BlueprintCallable)
	void SetAnalogWeight();

	UFUNCTION(BlueprintCallable)
	void SetAnalogDirection();

	UFUNCTION(BlueprintCallable)
	void SetClimbDirection();

	UFUNCTION()
	void MontageEnded(UAnimMontage* Montage, bool bInteruppted);

	UFUNCTION()
	void PlayClimbJump();

	UFUNCTION()
	void PlayClimbOver();

	void NativeBeginPlay() override;
};
