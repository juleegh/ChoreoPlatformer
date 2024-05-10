#include "PlayerBTHelpers.h"
#include "ComponentGetters.h"
#include "DanceUtilsFunctionLibrary.h"
#include "SongTempoComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DanceCharacter.h"
#include "ChoreoPlayerController.h"
#include "DanceCharacter.h"
#include "Enemy.h"

FVector UPlayerBTHelpersFunctionLibrary::GetPlayerInput(AActor* Player)
{
	auto DanceCharacter = Cast<ADanceCharacter>(Player);
	return DanceCharacter->GetCurrentInput();
}

bool UPlayerBTHelpersFunctionLibrary::CanMove(AActor* Player)
{
	auto PlayerController = ComponentGetters::GetController(Player->GetWorld());
	return PlayerController->CanMove();
}

bool UPlayerBTHelpersFunctionLibrary::IsOnTempo(AActor* Player)
{
	auto PlayerController = ComponentGetters::GetController(Player->GetWorld());
	return PlayerController->IsOnTempo();
}

bool UPlayerBTHelpersFunctionLibrary::ActorIsOnTempo(AActor* Actor)
{
	FTileInfo CurrentTile = UDanceUtilsFunctionLibrary::CheckPosition({ Actor }, Actor->GetActorLocation());
	auto SongTempo = ComponentGetters::GetSongTempoComponent(Actor->GetWorld());
	bool bIsOnTempo = SongTempo->IsOnTempo(CurrentTile.TargetTempo, 0.05, true);
	return bIsOnTempo;
}

AChoreoPlayerController* UPlayerBTHelpersFunctionLibrary::GetChoreoPlayerController(AActor* WorldContextActor)
{
	return ComponentGetters::GetController(WorldContextActor->GetWorld());
}

ADanceCharacter* UPlayerBTHelpersFunctionLibrary::GetDanceCharacter(AActor* WorldContextActor)
{
	return ComponentGetters::GetDanceCharacter(WorldContextActor->GetWorld());
}

UCalibrationComponent* UPlayerBTHelpersFunctionLibrary::GetCalibrationComponent(AActor* WorldContextActor)
{
	return ComponentGetters::GetCalibrationComponent(WorldContextActor->GetWorld());
}

UGameCameraComponent* UPlayerBTHelpersFunctionLibrary::GetGameCameraComponent(AActor* WorldContextActor)
{
	return ComponentGetters::GetGameCameraComponent(WorldContextActor->GetWorld());
}

bool UPlayerBTHelpersFunctionLibrary::EnemyCanMove(AActor* EnemyActor)
{
	auto Enemy = Cast<AEnemy>(EnemyActor);
	return Enemy->CurrentlyActive() && Enemy->CanMove();
}