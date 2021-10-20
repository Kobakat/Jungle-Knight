#include "BalanceBeam.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

ABalanceBeam::ABalanceBeam()
{
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	SetRootComponent(Spline);
}

void ABalanceBeam::BuildMesh()
{
	if (Spline != nullptr)
	{
		for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 2; i++)
		{		
			USplineMeshComponent* Comp = CreateDefaultSubobject<USplineMeshComponent>("Segment");
			Comp->SetStaticMesh(SplineMesh);
			Comp->SetForwardAxis(ESplineMeshAxis::X);		
			FVector StartPoint, StartTangent, EndPoint, EndTangent;
			
			Spline->GetLocationAndTangentAtSplinePoint(i, StartPoint, StartTangent, ESplineCoordinateSpace::Local);
			Spline->GetLocationAndTangentAtSplinePoint(i, EndPoint, EndTangent, ESplineCoordinateSpace::Local);

			Comp->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent);
		}
	}
}