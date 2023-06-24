#pragma once

struct QueueNode
{
	char* msg = new char[20];
	QueueNode* next_node = nullptr;
};

struct Queue
{
	QueueNode* first = new QueueNode;
	QueueNode* last = first;
	int size = 0;
};

void AddToQueue(Queue* Q, char* new_msg)
{
	Q->last->next_node = new QueueNode;
	for (int i = 0; i < 20; i++)
		Q->last->msg[i] = new_msg[i];
	Q->last = Q->last->next_node;
	Q->size++;
}

void DeleteFromQueue(Queue * Q)
{
	QueueNode* link_to_next = Q->first->next_node;
	delete Q->first;
	Q->first = link_to_next;
	Q->size--;
}

bool QueueIsEmpty(Queue * Q)
{
	return !(Q->size);
}