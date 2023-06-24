#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
	char binary_file_name[256];

	cout << "Process Receiver 开始工作。Receiver процесса начал работу." << endl << endl;

	cout << "输入二进制文件名 Введите имя двоичного файла  : ";
	//cin.getline(binary_file_name, 256);
	cin >> binary_file_name;
	int records_num = 0;
	cout << "输入该文件中的记录数：Введите количество записей в этом файле: ";
	cin >> records_num;

	char binary_file_path[1000];
	sprintf_s(binary_file_path, "%s", binary_file_name);
	ofstream f(binary_file_path, ios::binary);
	f.close();

	int senders_amount = 0;
	cout << "输入进程数发件人：Введите количество процессов Sender: ";
	cin >> senders_amount;

	cin.ignore();

	HANDLE SenderSemaphore = CreateSemaphore(
		NULL,
		0,
		records_num,
		TEXT("sender"));



	HANDLE* EventsFromSender;

	EventsFromSender = new HANDLE[senders_amount];
	for (int i = 0; i < senders_amount; i++)
	{
		char EventNum[256];
		sprintf_s(EventNum, "过程事件Событие процесса %d", (i + 1));
		EventsFromSender[i] = CreateEvent(
			NULL,
			TRUE,
			FALSE,
			TEXT(EventNum)
		);
	}

	


	STARTUPINFO* Sender = new STARTUPINFO[senders_amount];//结构体
	PROCESS_INFORMATION* Sender_Info = new PROCESS_INFORMATION[senders_amount];

	for (int i = 0; i < senders_amount; i++)
	{
		ZeroMemory(&(Sender[i]), sizeof(Sender[i]));//SDK中的一个宏。其作用是用0来填充一块内存区域。
		(Sender[i]).cb = sizeof(Sender[i]);
		ZeroMemory(&(Sender_Info[i]), sizeof(Sender_Info[i]));

		char buffer[1000];

		sprintf_s(buffer, "%s %s %d", "sender.exe", binary_file_name, (i + 1));

		CreateProcess(
			NULL,
			buffer,
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&(Sender[i]),
			&(Sender_Info[i])
		);
	}
	cout << endl;
	cout << "等待所有 Sender 进程为下一部分工作做好准备...Waiting all Sender processes to be ready for next part of work..." << endl;

	WaitForMultipleObjects(senders_amount, EventsFromSender, TRUE, 1000);
	//这两个函数的优点是它们在等待的过程中会进入一个非常高效沉睡状态，只占用极少的CPU时间片。
	//（这两个函数都是在内核状态下等待内核对象，不切换到用户模式下，因而效率很高）

	cout << "所有发送方进程都准备好恢复工作......Все процессы-отправители готовы возобновить работу..." << endl << endl;
	cout << "二进制文件现在是空的。 等待来自 Sender 进程的消息..Двоичный файл сейчас пуст. Ожидание сообщений от процессов Sender..." << endl;

	int current_records_num = 0;
	
	while (true)
	{
		HANDLE SenderSemaphore = CreateSemaphore(
			NULL,
			current_records_num,
			0,
			TEXT("sender"));

		if (current_records_num >= records_num)
		{
			cout << endl << "达到记录限制，进程退出读取模式...Достигнут предел записи, процесс выходит из режима чтения..." << endl;
			break;
		}
		ifstream fin(binary_file_path, ios::binary);

		fin.close();

		if (fin.peek() == EOF)
		{
			cout << endl;
			while (true)
			{
				ifstream fin(binary_file_path, ios::binary);
				if (fin.peek() != EOF)
					break;
				fin.close();
			}
		}
		cout << "有来自发送方进程之一的一些新消息...Есть какое-то новое сообщение от одного из процессов Sender..." << endl;
		cout << "Process Receiver：写入“exit”以完成工作或其他任何操作，以便从文件中读取更多消息：процесс Receiver: Напишите \"exit\", чтобы завершить работу или что-нибудь еще, чтобы прочитать еще какое-то сообщение из файла.:" << endl;
		char answer[256];
		cin.getline(answer, 256);
		if (!strncmp(answer, "exit", 4))
			break;
		else
		{
		
			char message[20];
			ifstream fin(binary_file_path, ios::binary);
			while (fin.peek() != EOF)
			{
				message[0] = '\0';
				fin.getline(message, 20);
				if (message[0] == '\0')
					cout << "没有来自 Sender 进程的任何新消息...Нет новых сообщений от процессов Sender..." << endl;
				else
				{
					current_records_num++;

					if (current_records_num > records_num)
					{
						//for(int i=0;i< senders_amount;i++)
						//PostThreadMessage(GetThreadId(EventsFromSender[i]), true, 0, 0);
						break;
					}
					cout << "阅读留言Чтение сообщения " << current_records_num << "/" << records_num << " из бинарника：from binary file:" << endl;
					
					cout << message << endl;
				}
			}
			fin.close();
			if (current_records_num <= records_num)
				current_records_num = 0;
			
		}
		fin.close();
		ofstream f(binary_file_path, ios::binary);
		f.close();
		cout << endl;
	}

	cout << "Receiver 完成的工作。Процесс Receiver завершил работу." << endl;

	for (int i = 0; i < senders_amount; i++)
	{
		CloseHandle(Sender_Info[i].hProcess);
		CloseHandle(Sender_Info[i].hThread);
	}

	CloseHandle(SenderSemaphore);

	for (int i = 0; i < senders_amount; i++)
		CloseHandle(EventsFromSender[i]);

	delete[] Sender;
	delete[] Sender_Info;
	delete[] EventsFromSender;

	return 0;
}