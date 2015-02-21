#include <string>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <random>
#include <functional>
#include <atomic>
#include <mutex>
#include <chrono>
#include <array>

using namespace std;

mutex m;
array<mutex, 5> forks;

int random()
{
	std::array<int, std::mt19937::state_size> seed_data;
	std::random_device r;
	std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
	std::seed_seq seq(std::begin(seed_data), std::end(seed_data));

	std::uniform_int_distribution<int> distribiuton(500, 2000);
	std::mt19937 engine;
	engine.seed(seq);
	auto generator = std::bind(distribiuton, engine);

	return generator();
}

void philosopher(int id, int rightFork, int leftFork)
{
	//http://en.wikipedia.org/wiki/Dining_philosophers_problem

	//Each philosopher eats only once, so algorithm will finish.
	//It bases on philosophers problem, but it doesn't solve starvation case (if you replace "while (hungry)" with "while (true)").
	//It's just an example of synchronization.
	
	bool hungry = true;
	while (hungry)
	{
		forks[rightFork].lock();

		m.lock();
		cout << "Philosopher " << id + 1 << " picks up the right fork." << endl;
		m.unlock();

		if (forks[leftFork].try_lock())
		{
			m.lock();
			cout << "Philosopher " << id + 1 << " picks up the left fork and eats." << endl;
			m.unlock();

			this_thread::sleep_for(chrono::milliseconds(random()));

			m.lock();
			cout << "Philosopher " << id + 1 << " puts down both forks." << endl;
			m.unlock();

			forks[leftFork].unlock();
			forks[rightFork].unlock();
			hungry = false;
		}
		else
		{
			m.lock();
			cout << "Philosopher " << id + 1 << " puts down the right fork and thinks." << endl;
			m.unlock();

			forks[rightFork].unlock();
			this_thread::sleep_for(chrono::milliseconds(random()));
		}
	}		
}

int main()
{
	vector<thread> philosophers;

	for (int i = 0; i < 5; i++)
		philosophers.push_back(thread(philosopher, i, i, (i + 1) % 5));
	for (int i = 0; i < 5; i++)
		philosophers[i].join();
	
	cout << "No more spaghetti..." << endl;
	system("pause");
	return 0;
}
