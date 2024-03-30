// Лабораторная 1. Сетевое планирование и управление.
// Кураева А.В. 22ПМ

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

bool toLocalize = false;

class Task
{
public:
	int weight;
	vector <int> outgoing;
	vector <int> incoming;
};

struct NetworkPlanning
{
public:
	void read(istream& in)
	{
		string str;
		int temp;


		Network = {{0}};

		while (true)
		{
			stringstream ss;
			Task t;
			if (!(in >> t.weight))
				break;
			if (t.weight < 0)
			{
				if (toLocalize)
					cout << "Неверные входные данные\nДлинна задачи не может быть меньше нуля" << endl;
				else
					cout << "Invalid input data\nLength of a task can't be less then zero" << endl;
				return;
			}

			getline(in, str);

			ss << str;
			while (ss >> temp)
				t.outgoing.push_back(temp);
			Network.push_back(t);
		}

		Network.push_back({ 0 });
		
		size = Network.size();


		for (int i = 1; i < size - 1; i++)
			for (int v : Network[i].outgoing)
				if (!(0 <= v && v < size)) // if (!(1 <= v && v < size))
				{
					if (toLocalize)
						cout << "Неверные входные данные\nОтсутствует работа с номером ";
					else
						cout << "Invalid input data\nCouldn't find task with number ";
					cout << v << endl;
					exit(1);
				}

		for (int i = 0; i < size; i++)
		{
			for (int j : Network[i].outgoing)
				Network[j].incoming.push_back(i);
		}

		for (int i = 1; i < size - 1; i++)
		{
			if (Network[i].outgoing.empty())
			{
				Network[i].outgoing = { size - 1 };
				Network.back().incoming.push_back(i);
			}

			if (Network[i].incoming.empty())
			{
				Network[i].incoming = { 0 };
				Network.front().outgoing.push_back(i);
			}
		}
	}

	void run()
	{
		vector <int> sortV = topological();
		vector<int> time = timeReserve(earlyStart(sortV), earlyEnd(sortV));

		print(time);
	}

private:
	vector <Task> Network;
	int size;

	vector <int> topological()
	{
		vector <int> visited(size);
		vector <int> sortV;
		sortV.reserve(size);
		topSort(visited, sortV, size - 1);

		return sortV;
	}
	void topSort(vector<int>& visited, vector <int>& sortV, int currV)
	{
		visited[currV]++;
		for (int v : Network[currV].incoming)
		{
			switch (visited[v])
			{
				case 0:
					topSort(visited, sortV, v);
					break;
				case 1:
					if (toLocalize)
						cout << "Задача " << v << " не может быть начата, присутствует цикл" << endl;
					else
						cout << "Task " << v << " can't be started, found a loop" << endl;
					exit(1);
			}

		}
		visited[currV]++;
		sortV.push_back(currV);
	}

	vector<int> earlyStart(const vector<int>& topSort)
	{
		vector <int> start(size, 0);

		for (int v : topSort)
			for (int incomingV : Network[v].incoming)
				start[v] = max(start[v], Network[incomingV].weight + start[incomingV]);

		return start;
	}

	vector<int> earlyEnd(const vector<int>& topSort)
	{
		vector <int> end(size, 0);

		for (int i = size - 1; i >= 0; i--)
		{
			int v = topSort[i];
			for (int outgoingV : Network[v].outgoing)
				end[v] = max(end[v], Network[outgoingV].weight + end[outgoingV]);
		}
		return end;
	}

	vector<int> timeReserve(const vector <int>& start, const vector <int>& end)
	{
		vector <int> time(size);
		int maxCritPathLength = end.front();

		for (int i = 0; i < size; i++)
		{
			int critPathLength = Network[i].weight + start[i] + end[i];
			time[i] = maxCritPathLength - critPathLength;
		}

		return time;
	}

	void print(const vector<int>& time)
	{
		if (toLocalize)
			cout << "Номер задачи | Максимальное время отсрочки без срыва сроков\n";
		else
			cout << "Task number  | Maximum delay time without missed deadlines\n";
		cout << "-------------+---------------------------------------------\n";

		for (int i = 1; i < size - 1; i++)
			cout << setw(12) << right << i << " | " << time[i] << endl;

	}
};

int main()
{
	setlocale(LC_ALL, "ru-RU");

	cout << "0 - En, 1 - Ru" << endl;
	cin >> toLocalize;

	ifstream fin("fin1.txt");
	if (!fin)
	{
		if (toLocalize)
			cout << "Файл не найден" << endl;
		else
			cout << "Couldn't find the file" << endl;
		return 0;
	}

	NetworkPlanning np;

	np.read(fin);
	np.run();
}
