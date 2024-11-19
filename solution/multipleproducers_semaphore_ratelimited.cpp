#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include "Semaphore.h"
using namespace std;


int  gCount = 0;			//how many produced
mutex m;
mutex mcout;
condition_variable cv;
condition_variable cv1;
//bool bDone=false; //indicates that producer has not left
int numProducers=3;
const int CAPACITY=5;
Semaphore s(CAPACITY);

void report(string s){
	unique_lock<mutex> lck(mcout);
	cout<<s<<endl;
}

void producer(int numbcounts, int id) {
	report("     Producer "+to_string(id)+" starting");
	//produce numbcounts widgets
	while(numbcounts>0){
		{
			unique_lock<mutex> lck(m);
			gCount++;		//producing a widget
			report("     Producer "+to_string(id)+" produced, gCount="+to_string(gCount));
		}
		cv.notify_one();
		numbcounts--;

		//now wait for consumer to say its consumed the widget
//		unique_lock<mutex> lck(m);
//		while(gCount>=CAPACITY)	//if >0 then wait until consumer consumes
//			cv1.wait(lck);
		s.wait();
	}
	{
		unique_lock<mutex> lck(m);
		numProducers--;
	}
	cv.notify_all();

	report("     Producer "+to_string(id)+" exiting");
}
void consumer(int id) {
	report("Consumer "+to_string(id)+" starting");
	while(true){
		unique_lock<mutex> lck(m);
		while(gCount==0 && (numProducers>0))	//sleep while there is nothing to do
			cv.wait(lck);
		//why did I wake up?

		//or is it time to leave?
		if(gCount==0 && (numProducers==0))
			break;

		//is it because I have to consume?
		if(gCount>0){
			gCount--;

			report("Consumer "+to_string(id)+" consumed, gCount="+to_string(gCount));
//			lck.unlock();
//			cv1.notify_all();
			s.signal();
		}
	}

	report("Consumer "+to_string(id)+" exiting");
}

int main() {
	report("The initial value of gCount is " + to_string(gCount) );
	thread t_producer1(producer, 1000,1);
	thread t_producer2(producer, 2000,2);
	thread t_producer3(producer, 6000,3);
	thread t_consumer1(consumer, 1);
//	thread t_consumer2(consumer, 2);
//	thread t_consumer3(consumer, 3);
//	thread t_consumer4(consumer, 4);

	t_producer1.join();
	t_producer2.join();
	t_producer3.join();
	t_consumer1.join();
//	t_consumer2.join();
//	t_consumer3.join();
//	t_consumer4.join();
	report("The final value of gCount is " +to_string(gCount)); //
	return 0;
}
