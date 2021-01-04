#pragma once

#include "Templates/SharedPointer.h"
#include "Containers/Queue.h"
#include "CoreMinimal.h"
//#include "AsyncQueue.generated.h"

/**
 * FAsyncQueue can be used to run asynchronous delegates in sequence, parallel and combinations of the above
 *
 * Use Add() to enqueue delegates matching FAsyncDelegate signature:
 * a void function that accepts a single argument of another void function with no arguments.
 *
 * Static factories MakeSync, MakeSequence and MakeParallel can be used to wrap different type of delegates and
 * delegate collections into a single FAsyncDelegate which can be enqueued with Add().
 *
 * Execute() accepts a callback and can be called multiple times. If queue is already running, Execute does nothing
 * except storing a callback.
 *
 *
 * The example itself:
 *
 *	TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> Queue = FAsyncQueue::Create();
 *	Queue->Add(FAsyncDelegate::CreateLambda([this](const FCallbackDelegate& Callback)
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Starting Long Task ASYNC"));
 *		FTimerHandle FooBar;
 *		this->GetWorldTimerManager().SetTimer(FooBar, Callback, 10, false);
 *	}));
 *	Queue->Add(FAsyncDelegate::CreateLambda([this](const FCallbackDelegate& Callback)
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Starting Short Task ASYNC"));
 *		FTimerHandle FooBar;
 *		this->GetWorldTimerManager().SetTimer(FooBar, Callback, 1, false);
 *	}));
 *	Queue->Add(FAsyncQueue::MakeSync(FCallbackDelegate::CreateLambda([]()
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Doing Instant Task SYNC"));
 *	})));
 *
 *	TArray<FAsyncDelegate> ParallelTasks;
 *	TArray<FAsyncDelegate> LongestParallel;
 *	TArray<FAsyncDelegate> ShortestParallel;
 *	TArray<FAsyncDelegate> MediumParallel;
 *
 *	LongestParallel.Add(FAsyncDelegate::CreateLambda([this](const FCallbackDelegate& Callback)
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Starting Longest Parallel ASYNC"));
 *		FTimerHandle FooBar;
 *		this->GetWorldTimerManager().SetTimer(FooBar, Callback, 10, false);
 *	}));
 *	LongestParallel.Add(FAsyncQueue::MakeSync(FCallbackDelegate::CreateLambda([]()
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Finished Longest Parallel"));
 *	})));
 *	ParallelTasks.Add(FAsyncQueue::MakeSequence(LongestParallel));
 *
 *
 *	ShortestParallel.Add(FAsyncDelegate::CreateLambda([this](const FCallbackDelegate& Callback)
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Starting Shortest Parallel ASYNC"));
 *		FTimerHandle FooBar;
 *		this->GetWorldTimerManager().SetTimer(FooBar, Callback, 1, false);
 *	}));
 *	ShortestParallel.Add(FAsyncQueue::MakeSync(FCallbackDelegate::CreateLambda([]()
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Finished Shortest Parallel"));
 *	})));
 *	ParallelTasks.Add(FAsyncQueue::MakeSequence(ShortestParallel));
 *
 *
 *	MediumParallel.Add(FAsyncDelegate::CreateLambda([this](const FCallbackDelegate& Callback)
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Starting Medium Parallel ASYNC"));
 *		FTimerHandle FooBar;
 *		this->GetWorldTimerManager().SetTimer(FooBar, Callback, 2, false);
 *	}));
 *	MediumParallel.Add(FAsyncQueue::MakeSync(FCallbackDelegate::CreateLambda([]()
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("Finished Medium Parallel"));
 *	})));
 *	ParallelTasks.Add(FAsyncQueue::MakeSequence(MediumParallel));
 *
 *	Queue->Add(FAsyncQueue::MakeParallel(ParallelTasks));
 *
 *	UE_LOG(LogTemp, Warning, TEXT("START"));
 *	Queue->Execute(FCallbackDelegate::CreateLambda([]()
 *	{
 *		UE_LOG(LogTemp, Warning, TEXT("DONESKIES"));
 *	}));
 */

DECLARE_DELEGATE(FCallbackDelegate);
DECLARE_DELEGATE_OneParam(FAsyncDelegate, FCallbackDelegate);

class WEVET_API FAsyncQueue : public TSharedFromThis<FAsyncQueue, ESPMode::ThreadSafe>
{
// Pointers need to create instances of a used class when compiled with WITH_HOT_RELOAD_CTORS
#if WITH_HOT_RELOAD
	friend class TSharedRef<FAsyncQueue, ESPMode::ThreadSafe>;
	friend class TSharedPtr<FAsyncQueue, ESPMode::ThreadSafe>;
	friend class TWeakPtr<FAsyncQueue, ESPMode::ThreadSafe>;
#endif

public:
	static FAsyncDelegate MakeParallel(const TArray<FAsyncDelegate>& ParallelDelegates)
	{
		return FAsyncDelegate::CreateLambda([ParallelDelegates](const FCallbackDelegate& Callback)
		{
			TSharedPtr<int32> ParallelCallbacksCounter = MakeShareable(new int32(ParallelDelegates.Num()));
			FCallbackDelegate ParallelCallback = FCallbackDelegate::CreateLambda([ParallelCallbacksCounter, Callback]()
			{
				--(*ParallelCallbacksCounter);
				if (*ParallelCallbacksCounter <= 0)
				{
					Callback.Execute();
				}
			});

			for (auto& Delegate : ParallelDelegates)
			{
				check(Delegate.IsBound());
				Delegate.Execute(ParallelCallback);
			}
		});
	}

	static FAsyncDelegate MakeSequence(const TArray<FAsyncDelegate>& SequenceDelegates)
	{
		return FAsyncDelegate::CreateLambda([SequenceDelegates](const FCallbackDelegate& Callback)
		{
			TSharedPtr<int32> SequenceCallbackCounter = MakeShareable(new int32(0));
			TSharedPtr<FCallbackDelegate> SequenceCallback(new FCallbackDelegate());
			SequenceCallback->BindLambda([SequenceCallback, SequenceCallbackCounter, &SequenceDelegates, Callback]()
			{
				int32 Index = (*SequenceCallbackCounter)++;
				if (Index < SequenceDelegates.Num())
				{
					SequenceDelegates[Index].ExecuteIfBound(*SequenceCallback);
				}
				else
				{
					Callback.ExecuteIfBound();
				}
			});
			SequenceCallback->Execute();
		});
	}

	static FAsyncDelegate MakeSync(const FCallbackDelegate& SyncDelegates)
	{
		return FAsyncDelegate::CreateLambda([SyncDelegates](const FCallbackDelegate& Callback)
		{
			check(SyncDelegates.IsBound());
			SyncDelegates.Execute();
			Callback.Execute();
		});
	}

	static FORCEINLINE const bool ValidPtr(const FAsyncDelegate* InDelegatePtr)
	{
		if (!InDelegatePtr)
		{
			return false;
		}
		return (InDelegatePtr && InDelegatePtr->IsBound());
	}

	static TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> Create()
	{
		TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> Result(new FAsyncQueue());
		return Result;
	}

	void Add(const FAsyncDelegate& AsyncDelegate)
	{
		Queue.Enqueue(AsyncDelegate);
	}

	void AddSync(const FCallbackDelegate& SyncDelegate) 
	{ 
		Add(FAsyncQueue::MakeSync(SyncDelegate)); 
	}

	void AddParallel(const TArray<FAsyncDelegate>& ParallelDelegates) 
	{
		Add(FAsyncQueue::MakeParallel(ParallelDelegates)); 
	}
	
	void Execute(const FCallbackDelegate& Callback)
	{
		//Sometimes, for convenience, functions pass empty delegate to indicate the lack of callback
		if (Callback.IsBound())
		{
			CompleteCallbacks.Add(Callback);
			CompleteCallbacks.Shrink();
		}
		Execute();
	}

	void Execute()
	{
		if (!OnAsyncFinishedDelegate.IsBound())
		{
			//OnAsyncFinishedDelegate.Unbind();
			OnAsyncFinishedDelegate.BindThreadSafeSP(this, &FAsyncQueue::OnAsyncDelegateFinished);
		}

		if (!IsExecuting())
		{
			ExecuteNextInQueue();
		}
	}

	void Empty()
	{
		FAsyncDelegate Tmp;
		while (!Queue.IsEmpty())
		{
			Queue.Dequeue(Tmp);
		}
	}

	void RemoveAllCallbacks()
	{
		CompleteCallbacks.Empty();
	}

	bool IsExecuting()
	{
		return CurrentDelegate.IsBound(); 
	}
	
	bool IsEmpty()
	{
		return Queue.IsEmpty(); 
	}

	void StoreHardReferenceToSelf(TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> NewHardReferenceToSelf)
	{
		HardReferenceToSelf = TSharedPtr<FAsyncQueue, ESPMode::ThreadSafe>(NewHardReferenceToSelf);
	}

	void ReleaseHardReferenceToSelf()
	{
		check(HardReferenceToSelf.IsValid());
		HardReferenceToSelf.Reset();
	}

private:
	FAsyncQueue() : TSharedFromThis() {}

	void ExecuteNextInQueue()
	{
		// Make sure previous delegate has been finished
		check(!IsExecuting());

		if (Queue.IsEmpty())
		{
			auto Tmp = CompleteCallbacks;
			CompleteCallbacks.Empty();
			for (auto& Callback : Tmp)
			{
				Callback.Execute();
			}
			return;
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("CompleteCallbacks.Num : %d"), CompleteCallbacks.Num());
		}

		Queue.Dequeue(CurrentDelegate);
		check(IsExecuting());
		CurrentDelegate.Execute(OnAsyncFinishedDelegate);
	}

	void OnAsyncDelegateFinished()
	{
		CurrentDelegate.Unbind();
		ExecuteNextInQueue();
	}

private:
	TQueue<FAsyncDelegate> Queue;
	TArray<FCallbackDelegate> CompleteCallbacks;
	FCallbackDelegate OnAsyncFinishedDelegate;
	FAsyncDelegate CurrentDelegate;

	TSharedPtr<FAsyncQueue, ESPMode::ThreadSafe> HardReferenceToSelf;
};
