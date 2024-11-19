#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>
using namespace std;

mutex m;				//mutual exclusion
mutex m_cout;			//mutual exclusion for couts
condition_variable cv; 	//for signaling

bool bDone=false;
int numDone = 0;			//used by producer to indicate we are done
int  gCount = 0;			//how many produced

void report(string s){
	lock_guard<mutex> lck(m_cout);
	cout<<s<<endl;
}

void producer(int numbcounts) {
	for(int i=0;i<numbcounts;i++){
		{
			unique_lock<mutex> lck(m);
			//produce one
			gCount++;
			report("Produced one, gCount="+to_string(gCount));
		}
		cv.notify_all();
	}

	{
		unique_lock<mutex> lck(m);
		//signal that we are done
		bDone=true;
		report("Producer DONE, gCount="+to_string(gCount));
	}
	cv.notify_all();
}
void consumer(int id) {
	while(true){
		unique_lock<mutex> lck(m);
		while(gCount==0 && bDone==false)
			cv.wait(lck);

		//was 1 produced?
		if(gCount>0){
			gCount--;
			report("     Consumer "+to_string(id)+" consumed, gCount="+to_string(gCount));
		}

		//is it time to leave?
		if(gCount ==0 && bDone==true)
			break;
	}

	report("     Consumer "+to_string(id)+" DONE, gCount="+to_string(gCount));
}

int main() {
	report("The initial value of gCount is " + to_string(gCount) );

	thread t_producer(producer, 1000);
	thread t_consumer1(consumer, 1);

	t_producer.join();
	t_consumer1.join();

	report("The final value of gCount is " +to_string(gCount)); //

	return 0;
}
