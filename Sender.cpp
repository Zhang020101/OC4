#include <windows.h>
#include <iostream>
#include <fstream>
using namespace std;

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
void DeleteFromQueue(Queue* Q)
{
	QueueNode* link_to_next = Q->first->next_node;
	delete Q->first;
	Q->first = link_to_next;
	Q->size--;
}
bool QueueIsEmpty(Queue* Q)
{
	return !(Q->size);
}

int main(int argc, char* argv[])
{
	int process_num = atoi(argv[2]);
	int sum = argc;
	cout << "处理发件人Process Sender " << process_num << " 开始工作了。started work." << endl;
	cout << endl;

	char EventNum[256];
	sprintf_s(EventNum, "过程事件Event of the process %d", process_num);

	char binary_file_path[1000];
	sprintf_s(binary_file_path, "%s", argv[1]);

	HANDLE EventFromSender = OpenEvent(
		NULL,
		TRUE,
		TEXT(EventNum)
	);

	SetEvent(EventFromSender);
	int s = 0;

	while (true)
	{
		//cout << "处理发件人Process Sender " << process_num << ": 等待队列...Waiting for the queue..." << endl;
		HANDLE Semaphore = OpenSemaphore(
			NULL,
			TRUE,
			TEXT("Sender")
		);

		ReleaseSemaphore(
			Semaphore,
			1,
			NULL
		);
		cout << "处理发件人Process Sender " << process_num << ": 将消息写给 Receiver 或写 \"exit\" 完成工作：: Write the message to the Receiver or write \"exit\" to complete work:" << endl;
		char messages[20];

			cin.getline(messages, 20);
			s++;
		if (!strncmp(messages, "exit", 4))
			break;
		if(sum>=s) {
			cout << "达到限制" << endl;
		}
		else
		{
			Queue MessagesQueue; //是一种进程间通信或同一进程的不同线程间的通信方式，软件的贮列用来处理一系列的输入，通常是来自用户。 
			ifstream fin(binary_file_path);
			while (fin.peek() != EOF)
			{
				char* temp = new char[20];
				fin.getline(temp, 20);
				AddToQueue(&MessagesQueue, temp);
				delete[] temp;
			}
			fin.close();
			ofstream fout(binary_file_path);
			while (!QueueIsEmpty(&MessagesQueue))
			{
				fout << MessagesQueue.first->msg << endl;
				DeleteFromQueue(&MessagesQueue);
			}
			fout << messages << endl;
			cout << "来自进程发送者的消息Message from Process Sender " << process_num << " successfully sent" << endl;
			fout.close();
		}
		cout << endl;

	}

	cout << "处理发件人Process Sender " << process_num << " 完成的工作。completed work." << endl;

	CloseHandle(EventFromSender);

	return 0;
}