#include "CppProject5.h"
#include "video_producer.h"

namespace
{

struct memory_archive_impl
	: FArchive
{
	memory_archive_impl()
	{
		int aaa = 5;
	}
	
	void append(uint8_t const* data, uint32_t size)
	{
		FScopeLock lock(&cs_);

		buffer_.Append(data, size);
	}

	void shutdown()
	{
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

		return INT32_MAX;
	}

	void Seek(int64 pos) final
	{
		while (true)
		{
			{
				FScopeLock lock(&cs_);

				if (shutdown_)
					break;

				if (pos <= buffer_.Num())
				{
					offset_ = pos;
					break;
				}
			}

			FPlatformProcess::Sleep(0.1f);
		}
	}
	
	int64 Tell() final
	{
		FScopeLock lock(&cs_);

		return offset_;
	}

	void Serialize(void* data, int64 num) override
	{
		if (num <= 0)
			return;

		while (true)
		{
			{
				FScopeLock lock(&cs_);

				if (shutdown_)
					break;

				if (offset_ + num <= buffer_.Num())
				{
					FMemory::Memcpy(data, &buffer_[offset_], num);
					offset_ += num;
				
					break;
				}
			}

			FPlatformProcess::Sleep(0.1f);
		}
	}

private:
	TArray<uint8> buffer_;
	int64_t offset_ = 0;
	FCriticalSection cs_;
	bool shutdown_ = false;
};


struct video_producer_impl
	: video_producer
{
	video_producer_impl()
		: archive_(new memory_archive_impl())
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

video_producer_ptr create_video_producer()
{
	auto ptr = new video_producer_impl;
	return video_producer_ptr(ptr);
}
