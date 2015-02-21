#include <iostream>
#include <thread>
#include <array>
#include <vector>
#include <mutex>
#include <string>
#include <random>
#include <condition_variable>
#include <queue>

using namespace std;

int waitingConsumers = 0;
bool onChair = false;
mutex m;
condition_variable cv_sleepingBarber, cv_waitingConsumers, cv_chair;
queue<int> buffer;

const int WAITING_ROOM_SIZE = 5;

int random(int min = 500, int max = 800)
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

void sleepingBarber()
{
	while (true)
	{
		unique_lock<mutex> lk(m);

		if (waitingConsumers == 0)
		{
			cout << "-----Barber sleeps" << endl;
			cv_sleepingBarber.wait(lk, []{ return waitingConsumers > 0;  });
			cout << "-----Barber wakes up" << endl;
		}
		else
		{
			cout << "-----Barber brings a customer to the chair" << endl;	
			cv_waitingConsumers.notify_all();
			cv_chair.wait(lk, []{ return onChair; });
			--waitingConsumers;

			cout << "-----Barber cuts customer's hair." << endl;
			this_thread::sleep_for(chrono::milliseconds(random()));
			onChair = false;			
		}

		lk.unlock();
		this_thread::sleep_for(chrono::milliseconds(random()));
	}
}

void consumer(int id)
{
	while (true)
	{
		unique_lock<mutex> lk(m);

		if (waitingConsumers >= WAITING_ROOM_SIZE)
		{
			cout << "Consumer " << id << " doesn't enter - no place." << endl;
		}		
		else
		{
			++waitingConsumers;
			cout << "Consumer " << id << " waits, number of consumers: " << waitingConsumers << endl;
			cv_sleepingBarber.notify_one();

			cv_waitingConsumers.wait(lk, []{ return !onChair; });
			cout << "Consumer " << id << " sits down on the chair" << endl;
			onChair = true;
			cv_chair.notify_one();
		}					

		lk.unlock();
		this_thread::sleep_for(chrono::milliseconds(random(10000, 40000)));
	}
}



int main()
{
	const int consumers_count = 15;
	vector<thread> consumers;
	thread sleepingBarberThread(sleepingBarber);

	for (int i = 0; i < consumers_count; i++)
		consumers.push_back(thread(consumer, i + 1));

	sleepingBarberThread.join();

	return 0;
}