#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <utility>

#include "startup_config.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "message_queue.hpp"

typedef MsgSafeQueue<std::string> inputQueue;
typedef std::pair<Expression, std::string> output;
typedef MsgSafeQueue<output> outputQueue;


class Consumer {
public:
	Consumer(inputQueue *messageQueueIn, outputQueue *messageQueueOut) {
		mqi = messageQueueIn;
		mqo = messageQueueOut;
	}

	void operator()(Interpreter interp) const {
		std::string temp;
		Expression exp;
		mqi->wait_and_pop(temp);
		std::string error;
		std::istringstream expression(temp);
		if (!interp.parseStream(expression)) {
			error = "Invalid Expression. Could not parse.";
		}
		else {
			try {
				exp = interp.evaluate();
				//std::cout << exp << std::endl;
			}
			catch (const SemanticError & ex) {
				error = ex.what();
			}
		}
		output out = std::make_pair(exp, error);
		mqo->push(out);
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

		if (line.empty()) continue;
		std::cout << "" << std::endl;
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
	t1.~thread();
	delete iq;
	delete oq;
}

int main(int argc, char *argv[])
{
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
