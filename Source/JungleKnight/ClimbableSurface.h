#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClimbableSurface.generated.h"

class UBoxComponent;

UCLASS()
class JUNGLEKNIGHT_API AClimbableSurface : public AActor
{
	GENERATED_BODY()
	
public:	
	AClimbableSurface();

	FORCEINLINE UStaticMeshComponent* GetMesh() { return Mesh; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxTrigger;
};
