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

mutex m;
condition_variable producer_cv, consumer_cv;
queue<int> buffer;

const int BUFFER_SIZE = 10;

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

void consumer_thread(int id)
{
	while (true)
	{
		unique_lock<mutex> lk(m);

		if (!buffer.empty())
		{			
			cout << "Consumer " << id << " ate " << buffer.front() << endl;
			buffer.pop();			
			producer_cv.notify_all();					
		}
		else
		{
			cout << "*** Consumer " << id << " waits" << endl;
			consumer_cv.wait(lk, []{ return !buffer.empty(); });
		}
	
		lk.unlock();
		this_thread::sleep_for(chrono::milliseconds(random(800, 1200)));
	}
}

void producer_thread(int id)
{
	while (true)
	{
		unique_lock<mutex> lk(m);

		if (buffer.size() < BUFFER_SIZE)
		{
			int a = random();	
			cout << "Producer " << id << " produced: " << a << endl;

			buffer.push(a);
			consumer_cv.notify_all();				
		}
		else
		{
			cout << "### Producer " << id << " waits" << endl;
			producer_cv.wait(lk, []{ return buffer.size() < BUFFER_SIZE; });
		}
			
		lk.unlock();
		this_thread::sleep_for(chrono::milliseconds(random(400, 500)));
	}
}

int main()
{
	const int consumers_count = 5;
	const int producers_count = 3;

	vector<thread> producers;
	vector<thread> consumers;

	for (int i = 0; i < consumers_count; i++)
		consumers.push_back(thread(consumer_thread, i + 1));
	for (int i = 0; i < producers_count; i++)
		producers.push_back(thread(producer_thread, i + 1));

	for (int i = 0; i < consumers_count; i++)
		consumers[i].join();
	for (int i = 0; i < producers_count; i++)
		producers[i].join();

	system("pause");
	return 0;
}