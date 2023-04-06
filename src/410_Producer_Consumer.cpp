#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>
using namespace std;

bool done_producing = false;			//used by producer to indicate we are done
int  gCount = 0;			//how many produced

void report(string s){
	cout<<s<<endl;
}

void producer(int numbcounts) {
	//produce numbcounts widgets

	//when done set done_producing to true to leave

	//usefull
	report("Produced one, gCount="+to_string(gCount));
	report("Producer DONE, gCount="+to_string(gCount));
}
void consumer(int id) {
	//if 1 produced then consume it
	//keep consuming until time to leave

	//usefull
	report("     Consumer "+to_string(id)+" consumed, gCount="+to_string(gCount));
	report("     Consumer "+to_string(id)+" DONE, gCount="+to_string(gCount));
}

int main() {
	report("The initial value of gCount is " + to_string(gCount) );

	thread t_producer(producer, 1000);
	thread t_producer1(producer, 1000);
	thread t_consumer1(consumer, 1);
	thread t_consumer2(consumer, 2);
	thread t_consumer3(consumer, 3);
	thread t_consumer4(consumer, 4);

	t_producer.join();
	t_consumer1.join();
	t_consumer2.join();
	t_consumer3.join();
	t_consumer4.join();
	t_producer1.join();

	report("The final value of gCount is " +to_string(gCount)); //

	return 0;
}
