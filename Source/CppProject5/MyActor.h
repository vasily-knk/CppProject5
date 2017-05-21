// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/MediaAssets/Public/MediaPlayer.h"

#include "MyActor.generated.h"


UCLASS()
class CPPPROJECT5_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    void NotifyAboutPlayer() const;

private:
    TArray<uint8> video_bytes_;
    TSharedPtr<IMediaPlayer> player_;
    IMediaTextureSink *texture_sink_ = nullptr;
};
