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
	cout << "��������Process Sender " << process_num << " ��ʼ�����ˡ�started work." << endl;
	cout << endl;

	char EventNum[256];
	sprintf_s(EventNum, "�����¼�Event of the process %d", process_num);

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
		//cout << "��������Process Sender " << process_num << ": �ȴ�����...Waiting for the queue..." << endl;
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
		cout << "��������Process Sender " << process_num << ": ����Ϣд�� Receiver ��д \"exit\" ��ɹ�����: Write the message to the Receiver or write \"exit\" to complete work:" << endl;
		char messages[20];

			cin.getline(messages, 20);
			s++;
		if (!strncmp(messages, "exit", 4))
			break;
		if(sum>=s) {
			cout << "�ﵽ����" << endl;
		}
		else
		{
			Queue MessagesQueue; //��һ�ֽ��̼�ͨ�Ż�ͬһ���̵Ĳ�ͬ�̼߳��ͨ�ŷ�ʽ�������������������һϵ�е����룬ͨ���������û��� 
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
			cout << "���Խ��̷����ߵ���ϢMessage from Process Sender " << process_num << " successfully sent" << endl;
			fout.close();
		}
		cout << endl;

	}

	cout << "��������Process Sender " << process_num << " ��ɵĹ�����completed work." << endl;

	CloseHandle(EventFromSender);

	return 0;
}