#include "CppProject5.h"
#include "video_producer.h"

namespace
{

struct memory_archive_impl
	: FArchive
{
	memory_archive_impl(uint32_t safety_span)
		: safety_span_(safety_span)
	{
		UE_LOG(LogTemp, Display, TEXT("memory_archive_impl ctor"));
	}

	~memory_archive_impl()
	{
		UE_LOG(LogTemp, Display, TEXT("memory_archive_impl dtor"));
	}
	
	void append(uint8_t const* data, uint32_t size)
	{
		FScopeLock lock(&cs_);

		buffer_.Append(data, size);
	}

	void shutdown()
	{
		UE_LOG(LogTemp, Display, TEXT("memory_archive_impl shutdown"));

		FScopeLock lock(&cs_);

		shutdown_ = true;
	}

	FString GetArchiveName() const override 
	{ 
		return TEXT("memory_archive_impl"); 
	}

	int64 TotalSize() override
	{
		FScopeLock lock(&cs_);

		if (shutdown_)
			return offset_;

		return buffer_.Num();
	}

	void Seek(int64 initial_pos) final
	{
		if (initial_pos < 0)
			return;
		
		const int64_t pos = FMath::Min<int64_t>(buffer_.Num(), initial_pos);

		offset_ = pos;

//		while (true)
//		{
//			{
//				FScopeLock lock(&cs_);
//
//				const int64_t reported_size = buffer_.Num() - safety_span_;
//
//				if (shutdown_ || pos <= reported_size)
//				{
//					offset_ = pos;
//					break;
//				}
//			}
//
//			sleep();
//		}
	}
	
	int64 Tell() final
	{
		FScopeLock lock(&cs_);

		return offset_;
	}

	void Serialize(void* data, int64 initial_num) override
	{
		if (initial_num <= 0)
			return;

		const int64_t num = FMath::Min(buffer_.Num() - offset_, initial_num);

		while (true)
		{
			int64_t reported_size;
			int64_t desired_pos;

			{
				FScopeLock lock(&cs_);

				reported_size = buffer_.Num() - safety_span_;
				desired_pos = offset_ + num;


				if (shutdown_ || desired_pos <= reported_size)
				{
					FMemory::Memcpy(data, &buffer_[offset_], num);
					offset_ += num;
				
					break;
				}
			}

			UE_LOG(LogTemp, Display, TEXT("memory_archive_impl starving: %d < %d"), reported_size, desired_pos);
			sleep();
		}
	}

private:

	void sleep() const
	{
		FPlatformProcess::Sleep(1.f);
	}

private:
	TArray<uint8> buffer_;
	int64_t safety_span_ = 0;
	int64_t offset_ = 0;
	FCriticalSection cs_;
	bool shutdown_ = false;
};


struct video_producer_impl
	: video_producer
{
	explicit video_producer_impl(uint32_t safety_span)
		: archive_(new memory_archive_impl(safety_span))
	{
		int aaa = 5;
	}

	~video_producer_impl()
	{
		archive_->shutdown();
	}


	archive_ref get_archive() override
	{
		return archive_;
	}

	void append(uint8_t const* data, uint32_t size) override
	{
		archive_->append(data, size);
	}

private:
	TSharedRef<memory_archive_impl, ESPMode::ThreadSafe> archive_;
};



} // namespace

video_producer_ptr create_video_producer(uint32_t safety_span)
{
	auto ptr = new video_producer_impl(safety_span);
	return video_producer_ptr(ptr);
}
