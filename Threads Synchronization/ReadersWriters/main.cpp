#include <iostream>
#include <thread>
#include <array>
#include <vector>
#include <mutex>
#include <string>
#include <random>

using namespace std;

int writers_counter = 0;
int readers_counter = 0;

mutex writers_counter_mtx, readers_counter_mtx, cout_mtx, no_entrance_mtx;

int random(int min = 500, int max = 2000)
{
	std::array<int, std::mt19937::state_size> seed_data;
	std::random_device r;
	std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));

	std::uniform_int_distribution<int> distribiuton(min, max);
	std::mt19937 engine;
	engine.seed(seq);
	auto generator = std::bind(distribiuton, engine);

	return generator();
}

bool checkIfAreReaders()
{
	bool areReaders = false;

	readers_counter_mtx.lock();
	if (readers_counter > 0)
	{
		no_entrance_mtx.try_lock();

		cout_mtx.lock();
		cout << "There are readers, writer doesn't enter, number of readers: " << readers_counter << endl;
		cout_mtx.unlock();
		areReaders = true;
	}
	readers_counter_mtx.unlock();

	if (areReaders)
		this_thread::sleep_for(chrono::milliseconds(random()));

	return areReaders;
}

void writers_foo()
{
	while (true)
	{
		if (checkIfAreReaders())
			continue;

		writers_counter_mtx.lock();
		++writers_counter;
		writers_counter_mtx.unlock();

		cout_mtx.lock();
		cout << "------Writer enters, number of writers: " << writers_counter << " number of readers: " << readers_counter << endl;
		cout_mtx.unlock();

		this_thread::sleep_for(chrono::milliseconds(random()));

		writers_counter_mtx.lock();
		--writers_counter;
		if (writers_counter == 0)
			no_entrance_mtx.unlock();
		writers_counter_mtx.unlock();

		cout_mtx.lock();
		cout << "------Writer leaves, number of readers: " << writers_counter << endl;
		cout_mtx.unlock();

		this_thread::sleep_for(chrono::milliseconds(random(3000, 4000)));
	}
}

bool checkIfAreWriters()
{
	bool areWriters = false;

	writers_counter_mtx.lock();
	if (writers_counter > 0)
	{
		cout_mtx.lock();
		cout << "There are writers, reader doesn't enter, number of writers: " << writers_counter << endl;
		cout_mtx.unlock();
		areWriters = true;
	}
	writers_counter_mtx.unlock();

	if (areWriters)
		this_thread::sleep_for(chrono::milliseconds(random()));
		
	return areWriters;
}

void readers_foo()
{
	while (true)
	{
		if (checkIfAreWriters())
			continue;		

		if (!no_entrance_mtx.try_lock())
		{
			this_thread::sleep_for(chrono::milliseconds(random()));
			continue;
		}			
		else
			no_entrance_mtx.unlock();

		readers_counter_mtx.lock();
		++readers_counter;
		readers_counter_mtx.unlock();

		cout_mtx.lock();
		cout << "Reader enters, number of readers: " << readers_counter << endl;
		cout_mtx.unlock();			

		this_thread::sleep_for(chrono::milliseconds(random()));

		readers_counter_mtx.lock();
		--readers_counter;
		readers_counter_mtx.unlock();

		cout_mtx.lock();
		cout << "Reader leaves, number of readers: " << readers_counter << endl;
		cout_mtx.unlock();

		this_thread::sleep_for(chrono::milliseconds(random(300, 800)));
	}
}

int main()
{
	vector<thread> readers, writers; 
	const int readers_count = 6;
	const int writers_count = 1;

	for (int i = 0; i < readers_count; i++)
		readers.push_back(thread(readers_foo));
	for (int i = 0; i < writers_count; i++)
		writers.push_back(thread(writers_foo));

	readers[0].join();

	return 0;
}