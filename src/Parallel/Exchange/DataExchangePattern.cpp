#include "DataExchangePattern.h"
#include "CmfScreen.h"
#include "CmfGC.h"
#include <algorithm>
namespace cmf
{
    DataExchangePattern::DataExchangePattern(ParallelGroup* group_in)
    {
        group = group_in;
        int groupSize = group->Size();
        resizeOutBufferRequired.resize(groupSize, true);
        resizeInBufferRequired.resize(groupSize, true);
        sendBufferIsAllocated.resize(groupSize, false);
        receiveBufferIsAllocated.resize(groupSize, false);
        sendBuffer.resize(groupSize, NULL);
        receiveBuffer.resize(groupSize, NULL);
        requestHandles.resize(groupSize);
        sendSizes.resize(groupSize, 0);
        receiveSizes.resize(groupSize, 0);
    }
    
    DataExchangePattern::~DataExchangePattern(void)
    {
        for (int rank = 0; rank < group->Size(); rank++)
        {
            if (sendBufferIsAllocated[rank])
            {
                sendBufferIsAllocated[rank] = false;
                Cmf_Free(sendBuffer[rank]);
            }
            if (receiveBufferIsAllocated[rank])
            {
                receiveBufferIsAllocated[rank] = false;
                Cmf_Free(receiveBuffer[rank]);
            }
        }
        for (auto tr:transactions)
        {
            delete tr;
        }
    }
    
    void DataExchangePattern::SortByPriority(void)
    {
        auto sortRule = [](IDataTransaction* const& a, IDataTransaction* const& b) -> bool { return (a->Priority() > b->Priority()); };
        std::sort(transactions.begin(), transactions.end(), sortRule);
    }
    
    IDataTransaction* DataExchangePattern::Add(IDataTransaction* transaction)
    {
        return this->Add(transaction, -1);
    }
    
    IDataTransaction* DataExchangePattern::Add(IDataTransaction* transaction, int priorityLevel)
    {
        int sender = transaction->Sender();
        int receiver = transaction->Receiver();
        int currentRank = group->Rank();
        if ((sender == currentRank) || (receiver == currentRank))
        {
            transactions.push_back(transaction);
            if ((sender == currentRank) && (transaction->GetPackedSize() > 0))
            {
                resizeOutBufferRequired[receiver] = true;
                sendSizes[receiver] += transaction->GetPackedSize();
            }
            if ((receiver == currentRank) && (transaction->GetPackedSize() > 0))
            {
                resizeInBufferRequired[sender] = true;
                receiveSizes[sender] += transaction->GetPackedSize();
            }
            transaction->SetPriority(priorityLevel);
            return transaction;
        }
        else
        {
            CmfError("This is a temporary error: a data exchange pattern has been added where neither the sender nor the receiver are the current rank");
            // This might not be the best way to do this.
            // Delete if the transaction does not pertain to the current rank
            delete transaction;
            return NULL;
        }
    }
    
    void DataExchangePattern::Pack(void)
    {
        pointerIndices = sendBuffer;
        for (const auto tr:transactions)
        {
            int currentRank = group->Rank();
            if (tr->Sender() == currentRank)
            {
                tr->Pack(pointerIndices[tr->Receiver()]);
                pointerIndices[tr->Receiver()] += tr->GetPackedSize();
            }
        }
    }
    
    void DataExchangePattern::Unpack(void)
    {
        pointerIndices = receiveBuffer; //Vector deep-copy.
        
        //Note that self-to-self transactions are not copied between
        //send and receive buffers on the same rank. Why would they be? :-)
        pointerIndices[group->Rank()] = sendBuffer[group->Rank()];
        
        for (const auto tr:transactions)
        {
            int currentRank = group->Rank();
            if (tr->Receiver() == currentRank)
            {
                tr->Unpack(pointerIndices[tr->Sender()]);
                pointerIndices[tr->Sender()] += tr->GetPackedSize();
            }
        }
    }
    
    void DataExchangePattern::ExchangeData(void)
    {
        group->Synchronize();
        for (int rank = 0; rank < group->Size(); rank++)
        {
            if (resizeOutBufferRequired[rank] || !sendBufferIsAllocated[rank])    this->ResizeOutBuffer(rank);
            if (resizeInBufferRequired[rank]  || !receiveBufferIsAllocated[rank]) this->ResizeInBuffer(rank);
        }
        
        //Aggregate the data
        Pack();
        
        //Count the number of send requests
        int numReceivesTotal = 0;
        for (int rank = 0; rank < group->Size(); rank++)
        {
            if ((rank != group->Rank()) && (receiveSizes[rank] > 0))
            {
                numReceivesTotal++;
            }
        }
        
        // resize the status array if need be
        if (numReceivesTotal!=statusHandles.size())  statusHandles.resize(numReceivesTotal);
        if (numReceivesTotal!=requestHandles.size()) requestHandles.resize(numReceivesTotal);
        
        //Queue the asynchronous send requests
        int counter = 0;
        for (int rank = 0; rank < group->Size(); rank++)
        {
            // Note that self-to-self transactions are handled using memcpy() in the Unpack() function
            if ((rank != group->Rank()) && (receiveSizes[rank] > 0))
            {
                group->QueueReceive(receiveBuffer[rank], receiveSizes[rank], parallelChar, rank, &requestHandles[counter]);
                counter++;
            }
        }   
        
        for (int rank = 0; rank < group->Size(); rank++)
        {
            //Send the data
            if ((rank != group->Rank()) && (sendSizes[rank] > 0))
            {
                group->BlockingSynchronousSend(sendBuffer[rank], sendSizes[rank], parallelChar, rank);
            }
        }
        
        group->AwaitAllAsynchronousOperations(numReceivesTotal, requestHandles.data(), statusHandles.data());
        
        //Distribute the data
        Unpack();
        group->Synchronize();
    }
    
    //Considering using a predicate here...
    void DataExchangePattern::ResizeOutBuffer(int rank)
    {
        size_t totalSize = 0;
        for (const auto tr:transactions)
        {
            if (tr->Sender() == group->Rank()) totalSize += tr->GetPackedSize();
        }
        // Consider a wrapper for realloc() if downsizing, it is a lot faster!
        if (sendBufferIsAllocated[rank]) Cmf_Free(sendBuffer[rank]);
        sendBuffer[rank] = (char*)Cmf_Alloc(totalSize);
        resizeOutBufferRequired[rank] = false;
        sendBufferIsAllocated[rank] = true;
    }
    
    void DataExchangePattern::ResizeInBuffer(int rank)
    {
        size_t totalSize = 0;
        for (const auto tr:transactions)
        {
            if (tr->Receiver() == group->Rank()) totalSize += tr->GetPackedSize();
        }
        // Consider a wrapper for realloc() if downsizing, it is a lot faster!
        if (receiveBufferIsAllocated[rank]) Cmf_Free(receiveBuffer[rank]);
        receiveBuffer[rank] = (char*)Cmf_Alloc(totalSize);
        resizeInBufferRequired[rank] = false;
        receiveBufferIsAllocated[rank] = true;
    }
}