#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <utility>

#include "startup_config.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "message_queue.hpp"
#include <csignal>
#include <cstdlib>

typedef MsgSafeQueue<std::string> inputQueue;
typedef std::pair<Expression, std::string> output;
typedef MsgSafeQueue<output> outputQueue;


/*// This global is needed for communication between the signal handler
// and the rest of the code. This atomic integer counts the number of times
// Cntl-C has been pressed by not reset by the REPL code.
volatile sig_atomic_t global_status_flag = 0;

// *****************************************************************************
// install a signal handler for Cntl-C on Windows
// *****************************************************************************
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>

// this function is called when a signal is sent to the process
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {

  switch (fdwCtrlType) {
  case CTRL_C_EVENT: // handle Cnrtl-C
    // if not reset since last call, exit
    if (global_status_flag > 0) { 
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
    return TRUE;

  default:
    return FALSE;
  }
}

// install the signal handler
inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }
// *****************************************************************************

// *****************************************************************************
// install a signal handler for Cntl-C on Unix/Posix
// *****************************************************************************
#elif defined(__APPLE__) || defined(__linux) || defined(__unix) ||             \
    defined(__posix)
#include <unistd.h>

// this function is called when a signal is sent to the process
void interrupt_handler(int signal_num) {

  if(signal_num == SIGINT){ // handle Cnrtl-C
    // if not reset since last call, exit
    if (global_status_flag > 0) {
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
  }
}

// install the signal handler
inline void install_handler() {

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = interrupt_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif
// *****************************************************************************
*/

class Consumer {
public:
	Consumer(inputQueue *messageQueueIn, outputQueue *messageQueueOut) {
		mqi = messageQueueIn;
		mqo = messageQueueOut;
	}

	void operator()(Interpreter interp) const {
		while (1) {
			std::string temp;
			Expression exp;
			mqi->wait_and_pop(temp);
			if (temp.empty()) {
				return;
			}
			std::string error;
			std::istringstream expression(temp);
			if (!interp.parseStream(expression)) {
				error = "Invalid Expression. Could not parse.";
			}
			else {
				try {
					exp = interp.evaluate();
				}
				catch (const SemanticError & ex) {
					error = ex.what();
				}
			}
			output out = std::make_pair(exp, error);
			mqo->push(out);
		}
	}

private:
	inputQueue * mqi;
	outputQueue * mqo;
};

void prompt(){
  std::cout << "\nplotscript> ";
}

std::string readline(){
  std::string line;
  std::getline(std::cin, line);

  return line;
}

void error(const std::string & err_str){
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str){
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, Interpreter interp){
  if(!interp.parseStream(stream)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
      std::cout << exp << std::endl;
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }	
  }

  return EXIT_SUCCESS;
}

int eval_from_file(std::string filename, Interpreter interp){
      
  std::ifstream ifs(filename);
  if(!ifs){
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }
  
  return eval_from_stream(ifs, interp);
}

int eval_from_command(std::string argexp, Interpreter interp){

  std::istringstream expression(argexp);

  return eval_from_stream(expression, interp);
}

// A REPL is a repeated read-eval-print loop
void repl(Interpreter interp){
	bool threadRun = true;
	inputQueue *iq = new inputQueue;
	outputQueue *oq = new outputQueue;
	output out;

	Interpreter newInterp = interp;
	Consumer cons(iq, oq);
	std::thread t1(cons, interp);
	while(!std::cin.eof()){
    
		prompt();
		std::string line = readline();

		if (line == "%start") {
			if (!threadRun) {
				threadRun = true;
				t1 = std::thread(cons, interp);
			}
			else {
				error("Could not start the thread, already a thread running.");
			}
			continue;
		}
		
		if (line == "%stop") {
			if (threadRun) {
				threadRun = false;
				std::string str;
				iq->push(str);
				t1.join();
				iq->try_pop(str);
			}
			else {
				error("Could not stop the thread, already no thread running");
			}
			continue;
		}
		
		if (line == "%reset") {
			// If not stopped
			if (threadRun) {
				// Stop the code
				std::string str;
				iq->push(str);
				t1.join();
				iq->try_pop(str);
			}
			threadRun = true;
			// Start the code
			t1 = std::thread(cons, interp);
			interp = newInterp;
			continue;
		}

		if (line == "%exit") {
			if (threadRun) {
				threadRun = false;
				std::string str;
				iq->push(str);
				t1.join();
				iq->try_pop(str);
			}
			exit(EXIT_SUCCESS);
		}

		if (line.empty()) continue;
		if (!threadRun) {
			error("interpreter kernel not running");
			continue;
		}
		iq->push(line);
		oq->wait_and_pop(out);

		if (out.second.empty()) {
			std::cout << out.first << " ";
		}
		else {
			std::cout << out.second << " ";
		}
	}

	t1.join();
	delete iq;
	delete oq;
}

int main(int argc, char *argv[])
{
	//install_handler();
	Interpreter interp;
	std::ifstream ifs(STARTUP_FILE);
	if (!interp.parseStream(ifs)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	}
	else {
		try {
			Expression exp = interp.evaluate();
		}
		catch (const SemanticError & ex) {
			std::cerr << ex.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	if(argc == 2){
		return eval_from_file(argv[1], interp);
	}
	else if(argc == 3){
		if(std::string(argv[1]) == "-e"){
			return eval_from_command(argv[2], interp);
		}
		else{
			error("Incorrect number of command line arguments.");
		}
	}
	else{
		repl(interp);
	}
    
	return EXIT_SUCCESS;
}
