// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/MediaAssets/Public/MediaPlayer.h"
#include "video_producer.h"


#include "MyActor.generated.h"


UCLASS()
class CPPPROJECT5_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();
    ~AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void init_player_if_needed();

private:
    typedef TSharedPtr<IMediaPlayer> player_ptr;

private:
    TArray<uint8> video_bytes_;
	int32_t video_bytes_offset_ = 0;
	float sum_deltas_ = 0.f;

	video_producer_ptr video_producer_;

    player_ptr player_;
    IMediaTextureSink *texture_sink_ = nullptr;
};
