#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>
using namespace std;

const int MAX_NUMB=5;
int  gCount = 0;			//how many produced
const int PRODUCE_THIS_MANY_THEN_WAIT=5;
mutex m;
mutex mcout;
condition_variable cv;
condition_variable cv1;
int numprod=2;

void report(string s){
	unique_lock<mutex> lck(mcout);
	cout<<s<<endl;
}

void producer(int numbcounts, int id) {
	report("     Starting producer" + to_string(id) );

	//produce until done (numbcounts==0)
	while(numbcounts>0){
		{
			lock_guard<mutex> lck(m);
			gCount++;
			report("     Producer "+ to_string(id)+",produced 1,gCount="+to_string(gCount) );
		}
		cv.notify_one();

		numbcounts--;

		if(gCount==MAX_NUMB){
			//wait until consumer says it has consumed the widget
			unique_lock<mutex> lck(m);
			while(gCount>0)
				cv1.wait(lck);
		}
	}
	{
		lock_guard<mutex> lck(m);
		//indicates producer finished
		numprod--;
	}
	cv.notify_all();
	report("     Producer "+ to_string(id)+" leaving" );
}
void consumer(int id) {
	report("Starting consumer "+to_string(id) );
	//stay in this loop until its time to leave
	while(true){
		unique_lock<mutex> lck(m);
		while(gCount==0  && (numprod>0))
			cv.wait(lck);

		//stay in loop until nothing else to consume and producer is done
		if(gCount==0 && (numprod==0))
			break;

		if(gCount>0){
			gCount--;
			report("Consumer "+to_string(id)+" consumed 1, gCount="+to_string(gCount) );
		}

		if(gCount==0){
			lck.unlock();
			cv1.notify_one();
		}
	}
	report("leaving consumer "+to_string(id) );
}

int main() {
	report("The initial value of gCount is " + to_string(gCount) );
	thread t_producer1(producer, 1000,1);
	thread t_producer2(producer, 2000,2);
	thread t_consumer1(consumer, 1);
	thread t_consumer2(consumer, 2);
	thread t_consumer3(consumer, 3);
	thread t_consumer4(consumer, 4);
	t_producer1.join();
	t_producer2.join();
	t_consumer1.join();
	t_consumer2.join();
	t_consumer3.join();
	t_consumer4.join();
	report("The final value of gCount is " +to_string(gCount)); //
	return 0;
}
