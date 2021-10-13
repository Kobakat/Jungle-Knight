#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KnightAnimInstance.generated.h"

UCLASS()
class JUNGLEKNIGHT_API UKnightAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UKnightAnimInstance() { }

protected:

	/** Magnitude of input vector (0 - 1)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Parameters")
	float AnalogWeight;
	
	/** Direction of input vector, relative to mesh forward (-1 - 1)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Parameters")
	float AnalogDirection;
};
