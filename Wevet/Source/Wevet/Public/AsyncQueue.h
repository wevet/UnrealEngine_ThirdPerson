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
 * The example bellow will output:
 *
 * START
 * Starting Long Task ASYNC
 * //10 seconds later
 * Starting Short Task ASYNC
 * //1 second later
 * Doing Instant Task SYNC
 * Starting Longest Parallel ASYNC
 * Starting Shortest Parallel ASYNC
 * Starting Medium Parallel ASYNC
 * //1 second later
 * Finished Shortest Parallel ASYNC
 * //1 second later (2 seconds from parallel tasks started)
 * Finished Medium Parallel
 * //8 seconds later (10 seconds from parallel tasks started)
 * Finished Longest Parallel
 * DONESKIES
 *
 * The example itself:
 *
 *  // Don't store the Queue on the stack or it will get destroyed before it finishes
 *  // You can't use "new", only a factory method "FAsyncQueue::Create()" which always returns `TSharedRef<FAsyncQueue, ESPMode::ThreadSafe>`
 *	Queue = FAsyncQueue::Create();
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
 *	TArray<FAsyncDelegate> ShortestParallel;
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
 *	TArray<FAsyncDelegate> MediumParallel;
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
#if WITH_HOT_RELOAD
	/*
	* WITH_HOT_RELOAD_CTORS 
	*/
	friend class TSharedRef<FAsyncQueue, ESPMode::ThreadSafe>;
	friend class TSharedPtr<FAsyncQueue, ESPMode::ThreadSafe>;
	friend class TWeakPtr<FAsyncQueue, ESPMode::ThreadSafe>;
#endif

public:
	static FAsyncDelegate MakeSequence(const TArray<FAsyncDelegate>& SequenceDelegates);
	static FAsyncDelegate MakeParallel(const TArray<FAsyncDelegate>& ParallelDelegates);
	static FAsyncDelegate MakeSync(const FCallbackDelegate& SyncDelegates);

	static TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> Create();

	void Add(const FAsyncDelegate& AsyncDelegate);
	void AddSync(const FCallbackDelegate& SyncDelegate) 
	{ 
		Add(FAsyncQueue::MakeSync(SyncDelegate)); 
	}

	void AddParallel(const TArray<FAsyncDelegate>& ParallelDelegates) 
	{
		Add(FAsyncQueue::MakeParallel(ParallelDelegates)); 
	}
	
	void StoreHardReferenceToSelf(TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> NewHardReferenceToSelf);
	void ReleaseHardReferenceToSelf();
	void Execute(const FCallbackDelegate& Callback);
	void Execute();
	void Empty();
	void RemoveAllCallbacks();

	bool IsExecuting()
	{
		return CurrentDelegate.IsBound(); 
	}
	
	bool IsEmpty()
	{
		return Queue.IsEmpty(); 
	}

private:
	FAsyncQueue();

	TQueue<FAsyncDelegate> Queue;
	TArray<FCallbackDelegate> CompleteCallbacks;
	FCallbackDelegate OnAsyncDelegateFinishedDelegate;
	FAsyncDelegate CurrentDelegate;

	TSharedPtr<FAsyncQueue, ESPMode::ThreadSafe> HardReferenceToSelf;

	void ExecuteNextInQueue();
	void OnAsyncDelegateFinished();
};
