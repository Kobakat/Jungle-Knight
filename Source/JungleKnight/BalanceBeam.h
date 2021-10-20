#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BalanceBeam.generated.h"

class USplineComponent;
class UStaticMesh;

UCLASS()
class JUNGLEKNIGHT_API ABalanceBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	ABalanceBeam();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* SplineMesh;

	UFUNCTION(BlueprintCallable)
	void BuildMesh();
};
