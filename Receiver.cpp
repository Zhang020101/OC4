#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
	char binary_file_name[256];

	cout << "Process Receiver ��ʼ������Receiver �����֧��� �ߧѧ�ѧ� ��ѧҧ���." << endl << endl;

	cout << "����������ļ��� ���ӧ֧էڧ�� �ڧާ� �էӧ�ڧ�ߧ�ԧ� ��ѧۧݧ�  : ";
	//cin.getline(binary_file_name, 256);
	cin >> binary_file_name;
	int records_num = 0;
	cout << "������ļ��еļ�¼�������ӧ֧էڧ�� �ܧ�ݧڧ�֧��ӧ� �٧ѧ�ڧ�֧� �� ����� ��ѧۧݧ�: ";
	cin >> records_num;

	char binary_file_path[1000];
	sprintf_s(binary_file_path, "%s", binary_file_name);
	ofstream f(binary_file_path, ios::binary);
	f.close();

	int senders_amount = 0;
	cout << "��������������ˣ����ӧ֧էڧ�� �ܧ�ݧڧ�֧��ӧ� �����֧���� Sender: ";
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
		sprintf_s(EventNum, "�����¼�����ҧ��ڧ� �����֧��� %d", (i + 1));
		EventsFromSender[i] = CreateEvent(
			NULL,
			TRUE,
			FALSE,
			TEXT(EventNum)
		);
	}

	


	STARTUPINFO* Sender = new STARTUPINFO[senders_amount];//�ṹ��
	PROCESS_INFORMATION* Sender_Info = new PROCESS_INFORMATION[senders_amount];

	for (int i = 0; i < senders_amount; i++)
	{
		ZeroMemory(&(Sender[i]), sizeof(Sender[i]));//SDK�е�һ���ꡣ����������0�����һ���ڴ�����
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
	cout << "�ȴ����� Sender ����Ϊ��һ���ֹ�������׼��...Waiting all Sender processes to be ready for next part of work..." << endl;

	WaitForMultipleObjects(senders_amount, EventsFromSender, TRUE, 1000);
	//�������������ŵ��������ڵȴ��Ĺ����л����һ���ǳ���Ч��˯״̬��ֻռ�ü��ٵ�CPUʱ��Ƭ��
	//�������������������ں�״̬�µȴ��ں˶��󣬲��л����û�ģʽ�£����Ч�ʺܸߣ�

	cout << "���з��ͷ����̶�׼���ûָ�����......����� �����֧���-�����ѧӧڧ�֧ݧ� �ԧ���ӧ� �ӧ�٧�ҧߧ�ӧڧ�� ��ѧҧ���..." << endl << endl;
	cout << "�������ļ������ǿյġ� �ȴ����� Sender ���̵���Ϣ..���ӧ�ڧ�ߧ�� ��ѧۧ� ��֧ۧ�ѧ� �����. ���اڧէѧߧڧ� ����ҧ�֧ߧڧ� ��� �����֧���� Sender..." << endl;

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
			cout << endl << "�ﵽ��¼���ƣ������˳���ȡģʽ...������ڧԧߧ�� ���֧է֧� �٧ѧ�ڧ��, �����֧�� �ӧ���էڧ� �ڧ� ��֧اڧާ� ���֧ߧڧ�..." << endl;
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
		cout << "�����Է��ͷ�����֮һ��һЩ����Ϣ...������ �ܧѧܧ��-��� �ߧ�ӧ�� ����ҧ�֧ߧڧ� ��� ��էߧ�ԧ� �ڧ� �����֧���� Sender..." << endl;
		cout << "Process Receiver��д�롰exit������ɹ����������κβ������Ա���ļ��ж�ȡ������Ϣ�������֧�� Receiver: ���ѧ�ڧ�ڧ�� \"exit\", ����ҧ� �٧ѧӧ֧��ڧ�� ��ѧҧ��� �ڧݧ� ����-�ߧڧҧ�է� �֧��, ����ҧ� �����ڧ�ѧ�� �֧�� �ܧѧܧ��-��� ����ҧ�֧ߧڧ� �ڧ� ��ѧۧݧ�.:" << endl;
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
					cout << "û������ Sender ���̵��κ�����Ϣ...���֧� �ߧ�ӧ�� ����ҧ�֧ߧڧ� ��� �����֧���� Sender..." << endl;
				else
				{
					current_records_num++;

					if (current_records_num > records_num)
					{
						//for(int i=0;i< senders_amount;i++)
						//PostThreadMessage(GetThreadId(EventsFromSender[i]), true, 0, 0);
						break;
					}
					cout << "�Ķ����ԧ���֧ߧڧ� ����ҧ�֧ߧڧ� " << current_records_num << "/" << records_num << " �ڧ� �ҧڧߧѧ�ߧڧܧѣ�from binary file:" << endl;
					
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

	cout << "Receiver ��ɵĹ�����������֧�� Receiver �٧ѧӧ֧��ڧ� ��ѧҧ���." << endl;

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