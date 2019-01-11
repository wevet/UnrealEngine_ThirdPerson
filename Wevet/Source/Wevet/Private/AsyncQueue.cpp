#include "AsyncQueue.h"

FAsyncDelegate FAsyncQueue::MakeParallel(const TArray<FAsyncDelegate>& ParallelDelegates)
{
	return FAsyncDelegate::CreateLambda([ParallelDelegates](const FCallbackDelegate& Callback)
	{
		//We need to make shared integer that can be modified by multiple calls of ParallelCallback lambda
		TSharedPtr<int32> ParallelCallbacksCounter = MakeShareable(new int32(ParallelDelegates.Num()));
		FCallbackDelegate ParallelCallback = FCallbackDelegate::CreateLambda([ParallelCallbacksCounter, Callback]()
		{
			//Every delegate will decrement the counter, the last one will execute the callback
			--(*ParallelCallbacksCounter);
			if (*ParallelCallbacksCounter <= 0) Callback.Execute();
		});

		for (auto& Delegate : ParallelDelegates)
		{
			check(Delegate.IsBound());
			Delegate.Execute(ParallelCallback);
		}
	});
}

FAsyncDelegate FAsyncQueue::MakeSequence(const TArray<FAsyncDelegate>& SequencedDelegates)
{
	return FAsyncDelegate::CreateLambda([SequencedDelegates](const FCallbackDelegate& Callback)
	{
		//We need to make shared integer that can be modified by multiple calls of SequenceCallback lambda
		TSharedPtr<int32> SequenceCallbackCounter = MakeShareable(new int32(0));
		//We need to store lambda delegate on heap to allow it to call itself
		TSharedPtr<FCallbackDelegate> SequenceCallback(new FCallbackDelegate());
		SequenceCallback->BindLambda([SequenceCallback, SequenceCallbackCounter, &SequencedDelegates, Callback]()
		{
			int32 Index = (*SequenceCallbackCounter)++;
			//Each delegate executes the next one. Last one executes Callback.
			if (Index < SequencedDelegates.Num())
			{
				check(SequencedDelegates[Index].IsBound());
				SequencedDelegates[Index].Execute(*SequenceCallback);
			}
			else
			{
				check(Callback.IsBound());
				Callback.Execute();
			}
		});
		SequenceCallback->Execute();
	});
}

FAsyncDelegate FAsyncQueue::MakeSync(const FCallbackDelegate& SyncDelegate)
{
	return FAsyncDelegate::CreateLambda([SyncDelegate](const FCallbackDelegate& Callback)
	{
		check(SyncDelegate.IsBound());
		SyncDelegate.Execute();
		Callback.Execute();
	});
}

TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> FAsyncQueue::Create() {
	TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> Result(new FAsyncQueue());
	return Result;
}


FAsyncQueue::FAsyncQueue()
	: TSharedFromThis()
{
}

void FAsyncQueue::Add(const FAsyncDelegate& AsyncDelegate)
{
	Queue.Enqueue(AsyncDelegate);
}


void FAsyncQueue::StoreHardReferenceToSelf(TSharedRef<FAsyncQueue, ESPMode::ThreadSafe> HardReferenceToSelf)
{
	this->HardReferenceToSelf = TSharedPtr<FAsyncQueue, ESPMode::ThreadSafe>(HardReferenceToSelf);
}


void FAsyncQueue::ReleaseHardReferenceToSelf()
{
	check(HardReferenceToSelf.IsValid());
	HardReferenceToSelf.Reset();
}

void FAsyncQueue::Execute(const FCallbackDelegate& Callback)
{
	//Sometimes, for convenience, functions pass empty delegate to indicate the lack of callback
	if (Callback.IsBound()) CompleteCallbacks.Add(Callback);
	Execute();
}

void FAsyncQueue::Execute()
{
	if (!OnAsyncDelegateFinishedDelegate.IsBound())
	{
		OnAsyncDelegateFinishedDelegate.BindThreadSafeSP(this, &FAsyncQueue::OnAsyncDelegateFinished);
	}
	if (!IsExecuting()) ExecuteNextInQueue();
}

void FAsyncQueue::Empty()
{
	FAsyncDelegate Tmp;
	while (!Queue.IsEmpty()) Queue.Dequeue(Tmp);
}

void FAsyncQueue::RemoveAllCallbacks()
{
	CompleteCallbacks.Empty();
}

void FAsyncQueue::ExecuteNextInQueue()
{
	//Make sure previous delegate has been finished
	check(!CurrentDelegate.IsBound());
	
	if (Queue.IsEmpty())
	{
		auto Tmp = CompleteCallbacks;
		CompleteCallbacks.Empty();
		for (auto& Callback : Tmp) Callback.Execute();
		return;
	}

	Queue.Dequeue(CurrentDelegate);
	check(CurrentDelegate.IsBound());
	CurrentDelegate.Execute(OnAsyncDelegateFinishedDelegate);
}


void FAsyncQueue::OnAsyncDelegateFinished()
{
	CurrentDelegate.Unbind();
	ExecuteNextInQueue();
}
