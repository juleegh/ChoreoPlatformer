#include "TimelineCreatorComponent.h"
#include "GameFramework/Character.h"
#include "Curves/CurveBase.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Paper2D/Classes/PaperSpriteComponent.h"
#include "Engine/DataTable.h"
#include "Math/UnrealMathUtility.h"

UTimelineCreatorComponent::UTimelineCreatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

UMovementTimelineComponent::UMovementTimelineComponent()
{
	static ConstructorHelpers::FObjectFinder<UCurveFloat>Line(TEXT("/Game/Curves/C_TangentHalf"));
	if (Line.Succeeded())
	{
		TimelineCurve = Line.Object;
	}
}

UColorTimelineComponent::UColorTimelineComponent()
{
	static ConstructorHelpers::FObjectFinder<UCurveFloat>Curve(TEXT("/Game/Curves/C_UpDownCurve"));
	if (Curve.Succeeded())
	{
		TimelineCurve = Curve.Object;
	}
}

UColorFadeTimelineComponent::UColorFadeTimelineComponent()
{
	static ConstructorHelpers::FObjectFinder<UCurveFloat>Curve(TEXT("/Game/Curves/C_Upwards"));
	if (Curve.Succeeded())
	{
		TimelineCurve = Curve.Object;
	}
}

USpritesTimelineComponent::USpritesTimelineComponent()
{
	static ConstructorHelpers::FObjectFinder<UCurveFloat>Curve(TEXT("/Game/Curves/C_UpDownCurve"));
	if (Curve.Succeeded())
	{
		TimelineCurve = Curve.Object;
	}
}

UProjectileTimelineComponent::UProjectileTimelineComponent()
{
	static ConstructorHelpers::FObjectFinder<UCurveFloat>Curve(TEXT("/Game/Curves/C_ProjectileCurve"));
	if (Curve.Succeeded())
	{
		TimelineCurve = Curve.Object;
	}
}

UScaleUpTimelineComponent::UScaleUpTimelineComponent()
{
	static ConstructorHelpers::FObjectFinder<UCurveFloat>Curve(TEXT("/Game/Curves/C_Upwards"));
	if (Curve.Succeeded())
	{
		TimelineCurve = Curve.Object;
	}
}

void UTimelineCreatorComponent::Initialize()
{
	FOnTimelineEventStatic onTimelineFinishedCallback;
	FOnTimelineFloat onTimelineCallback;

	if (TimelineCurve != NULL)
	{
		MyTimeline = NewObject<UTimelineComponent>(GetOwner());
		MyTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript; // Indicate it comes from a blueprint so it gets cleared when we rerun construction scripts
		GetOwner()->BlueprintCreatedComponents.Add(MyTimeline); // Add to array so it gets saved
		MyTimeline->SetNetAddressable();    // This component has a stable name that can be referenced for replication

		MyTimeline->SetPropertySetObject(this); // Set which object the timeline should drive properties on
		MyTimeline->SetDirectionPropertyName(FName("TimelineDirection"));

		MyTimeline->SetLooping(false);
		MyTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);

		MyTimeline->SetPlaybackPosition(0.0f, false);

		//Add the float curve to the timeline and connect it to your timelines's interpolation function
		onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
		MyTimeline->AddInterpFloat(TimelineCurve, onTimelineCallback, "ElapsedTime");
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
		MyTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);

		MyTimeline->RegisterComponent();
		TimelineTarget = GetOwner();
	}
}

void UTimelineCreatorComponent::TimelineFinishedCallback()
{
	if (TimelineEnded.IsBound())
	{
		TimelineEnded.Broadcast();
	}
}

void UTimelineCreatorComponent::Stop(bool bForceFinishAction)
{
	if (MyTimeline != NULL)
	{
		MyTimeline->Stop();
	}
	if (bForceFinishAction)
	{
		ForceStopTimeline();
	}
}

bool UTimelineCreatorComponent::IsRunning() const
{
	if (MyTimeline != NULL)
	{
		return MyTimeline->GetPlaybackPosition() < MyTimeline->GetTimelineLength() && MyTimeline->GetPlaybackPosition() > 0;
	}
	return false;
}

bool UTimelineCreatorComponent::ReachedHalfwayPoint() const
{
	if (MyTimeline != NULL)
	{
		if (MyTimeline->GetPlaybackPosition() <= 0)
			return false;

		float RemainingPercentage = (MyTimeline->GetTimelineLength() - MyTimeline->GetPlaybackPosition()) / MyTimeline->GetTimelineLength();
		return  RemainingPercentage < 0.05f;
	}
	return false;
}

void UTimelineCreatorComponent::PlayTimeline()
{
	if (MyTimeline != NULL)
	{
		MyTimeline->PlayFromStart();
	}
}

void UTimelineCreatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (MyTimeline != NULL)
	{
		MyTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}


void UTimelineCreatorComponent::TimelineCallback(float interpolatedVal)
{
	if (TimelineTick.IsBound())
	{
		TimelineTick.Execute(interpolatedVal);
	}
}

void UMovementTimelineComponent::MoveToPosition(FVector TargetPosition, float TimelineLength)
{
	OriginLocation = TimelineTarget->GetActorLocation();
	TargetLocation = TargetPosition;
	MyTimeline->SetTimelineLength(TimelineLength);
	TimelineTick.BindDynamic(this, &UMovementTimelineComponent::MoveCallback);
	PlayTimeline();
}

void UMovementTimelineComponent::MoveCallback(float interpolatedVal)
{
	TimelineTarget->SetActorLocation(UKismetMathLibrary::VLerp(OriginLocation, TargetLocation, MyTimeline->GetPlaybackPosition() / MyTimeline->GetTimelineLength()));
}

void UMovementTimelineComponent::RotateToPosition(FRotator TargetPosition, float TimelineLength)
{
	OriginRotation = TimelineTarget->GetActorRotation();
	TargetRotation = TargetPosition;
	MyTimeline->SetTimelineLength(TimelineLength);
	TimelineTick.BindDynamic(this, &UMovementTimelineComponent::RotateCallback);
	PlayTimeline();
}

void UMovementTimelineComponent::RotateCallback(float interpolatedVal)
{
	TimelineTarget->SetActorRotation(UKismetMathLibrary::RLerp(OriginRotation, TargetRotation, MyTimeline->GetPlaybackPosition() / MyTimeline->GetTimelineLength(), true));
}

void UMovementTimelineComponent::Reset()
{
	OriginLocation = TimelineTarget->GetActorLocation();
	TargetLocation = TimelineTarget->GetActorLocation();
}

void UMovementTimelineComponent::ForceStopTimeline()
{
	if (TargetLocation != OriginLocation)
	{
		TimelineTarget->SetActorLocation(TargetLocation);
	}
	if (TargetRotation != OriginRotation)
	{
		TimelineTarget->SetActorRotation(TargetRotation);
	}
}

void UColorTimelineComponent::AddMesh(UMeshComponent* Mesh)
{
	ShineMat = Mesh->CreateDynamicMaterialInstance(0, Mesh->GetMaterial(0));
}

void UColorTimelineComponent::Blink()
{
	OriginBrightness = 0;
	TimelineTick.BindDynamic(this, &UColorTimelineComponent::BlinkCallback);
	PlayTimeline();
}

void UColorTimelineComponent::ChangeColor(FColor newColor)
{
	ShineMat->SetVectorParameterValue(FName("LightColor"), newColor);
}

void UColorTimelineComponent::BlinkCallback(float interpolatedVal)
{
	ShineMat->SetScalarParameterValue(FName("LightMultiplier"), interpolatedVal * TargetBrightness);
}

void UColorTimelineComponent::Reset()
{
	ShineMat->SetScalarParameterValue(FName("LightMultiplier"), 0);
}

void UColorTimelineComponent::ForceStopTimeline()
{
	ShineMat->SetScalarParameterValue(FName("LightMultiplier"), 0);
}

void UColorFadeTimelineComponent::AddMesh(UMeshComponent* Mesh)
{
	ShineMat = Mesh->CreateDynamicMaterialInstance(0, Mesh->GetMaterial(0));
}

void UColorFadeTimelineComponent::FadeInDirection(bool Direction)
{
	OriginBrightness = FMath::Abs(Direction - 1);
	TargetBrightness = Direction;
	MyTimeline->SetTimelineLength(0.75f);
	TimelineTick.BindDynamic(this, &UColorFadeTimelineComponent::FadeCallback);
	PlayTimeline();
}

void UColorFadeTimelineComponent::FadeCallback(float interpolatedVal)
{
	float progress = MyTimeline->GetPlaybackPosition() / MyTimeline->GetTimelineLength();
	ShineMat->SetScalarParameterValue(FName("LightMultiplier"), (1 - progress) * OriginBrightness + progress * TargetBrightness);
}

void UColorFadeTimelineComponent::Reset()
{
	ShineMat->SetScalarParameterValue(FName("LightMultiplier"), 0);
}

void UColorFadeTimelineComponent::ForceStopTimeline()
{
	ShineMat->SetScalarParameterValue(FName("LightMultiplier"), 0);
}

void UProjectileTimelineComponent::Throw(FVector OriginLocation, FVector TargetLocation, float TimelineLength)
{
	OriginPosition = OriginLocation;
	TargetPosition = TargetLocation;
	MyTimeline->SetTimelineLength(TimelineLength);
	TimelineTick.BindDynamic(this, &UProjectileTimelineComponent::LandedCallback);
	PlayTimeline();
}

void UProjectileTimelineComponent::ForceStopTimeline()
{
	TimelineTarget->SetActorLocation(TargetPosition);
}

void UProjectileTimelineComponent::LandedCallback(float interpolatedVal)
{
	FVector MiddlePosition = UKismetMathLibrary::VLerp(OriginPosition, TargetPosition, MyTimeline->GetPlaybackPosition() / MyTimeline->GetTimelineLength());
	MiddlePosition.Z += interpolatedVal * 200;
	TimelineTarget->SetActorLocation(MiddlePosition);
}


void USpritesTimelineComponent::SetSprites(TArray<UPaperSpriteComponent*> NewSprites)
{
	Sprites = NewSprites;
	ShineMat = Sprites[0]->CreateDynamicMaterialInstance(0, Sprites[0]->GetMaterial(0));
	for (auto Sprite : Sprites)
	{
		Sprite->SetMaterial(0, ShineMat);
	}
}

void USpritesTimelineComponent::Blink()
{
	OriginOpacity = 0;
	TimelineTick.BindDynamic(this, &USpritesTimelineComponent::OpacityCallback);
	PlayTimeline();
}

void USpritesTimelineComponent::ChangeType(const FTileSpriteInfo& SpriteType)
{
	for (auto Sprite : Sprites)
	{
		Sprite->SetSpriteColor(SpriteType.Color);
		Sprite->SetSprite(SpriteType.Icon);
	}
}

void USpritesTimelineComponent::ChangeBrightness(float Brightness)
{
	ShineMat->SetScalarParameterValue(FName("Brightness"), Brightness);
}

void USpritesTimelineComponent::OpacityCallback(float interpolatedVal)
{
	auto TempColor = Sprites[0]->GetSpriteColor();
	TempColor.A = interpolatedVal * TargetOpacity;
	for (auto Sprite : Sprites)
	{
		Sprite->SetSpriteColor(TempColor);
	}
}

void USpritesTimelineComponent::Reset()
{
	for (auto Sprite : Sprites)
	{
		Sprite->SetSpriteColor(FLinearColor(0,0,0,0));
	}
}

void UScaleUpTimelineComponent::ScaleUp(FVector Origin, FVector Target, float TimelineLength)
{
	OriginScale = Origin;
	TargetScale = Target;
	MyTimeline->SetTimelineLength(TimelineLength);
	TimelineTick.BindDynamic(this, &UScaleUpTimelineComponent::ScaleCallback);
	PlayTimeline();
}

void UScaleUpTimelineComponent::ForceStopTimeline()
{
	ScalingActor->SetWorldScale3D(TargetScale);
}

void UScaleUpTimelineComponent::ScaleCallback(float interpolatedVal)
{
	FVector MiddlePosition = UKismetMathLibrary::VLerp(OriginScale, TargetScale, MyTimeline->GetPlaybackPosition() / MyTimeline->GetTimelineLength());
	ScalingActor->SetWorldScale3D(MiddlePosition);
}