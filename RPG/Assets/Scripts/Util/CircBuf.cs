using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CircBuf<T> where T : class
{
    protected int maxQueueSize;
    protected int front, rear;

    protected T[] queue;

    public void Init(int _queueSize, T _initParam)
    {
        maxQueueSize = _queueSize;
        queue = new T[maxQueueSize];

        front = rear = 0;

        for (int i = 0; i < maxQueueSize; i++)
            queue[i] = _initParam;
    }

    protected bool IsEmpty()
    {
        return front == rear ? true : false;
    }

    protected bool IsFull()
    {
        return (rear + 1) % maxQueueSize == front ? true : false;
    }

    public void Enqueue(T _data)
    {
        if (IsFull())
            Dequeue();

        rear = (++rear) % maxQueueSize;
        queue[rear] = _data;

    }

    public void Dequeue()
    {
        if (IsEmpty())
            return;

        front = (++front) % maxQueueSize;
    }

    public T[] GetQueue()
    {
        return queue;
    }

    public T GetQueueParameter(int _index)
    {
        int index = (front + _index) % maxQueueSize;

        return queue[index];
    }

    //나중에 추출할때 front~rear만 읽으면 되는데
}