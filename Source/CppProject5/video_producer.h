#pragma once

struct video_producer
{
	typedef TSharedRef<FArchive, ESPMode::ThreadSafe> archive_ref;

	virtual ~video_producer() = default;

	virtual archive_ref get_archive() = 0;
	virtual void append(uint8_t const *data, uint32_t size) = 0;
};

typedef TSharedPtr<video_producer, ESPMode::ThreadSafe> video_producer_ptr;

video_producer_ptr create_video_producer();