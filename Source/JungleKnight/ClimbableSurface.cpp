#include "ClimbableSurface.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AClimbableSurface::AClimbableSurface()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	BoxTrigger = CreateDefaultSubobject<UBoxComponent>("Trigger");
	BoxTrigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BoxTrigger->SetCollisionProfileName("OverlapOnlyPawn");
}