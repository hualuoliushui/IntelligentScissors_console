#pragma once

#include <stack>
#include <fstream>
#include <time.h>

#include <Windows.h>

using namespace std;

class Log{
private:
	stack<double> ts;
public:
	ofstream cout;
	Log(){
		cout = ofstream("log.txt", std::ios_base::app);
		cout << "# # # # # # # # # # # # # # # # # \n";
	}

	~Log(){
		cout << "\n##################################\n";
		cout.close();
	}

	void print_time_self(){
		time_t rawtime;
		time(&rawtime);

		//����ʱ��
		struct tm* timeinfo;
		timeinfo = localtime(&rawtime);

		//���ʱ��
		cout << "\n" << ctime(&rawtime) << endl;
	}

	void init_time(){
		ts.push((double)GetTickCount());
		cout << "\n== == == == == == == == == == == == == == == ==\n";
	}

	void end_time(){
		cout << "\n=============\n";
		double t = (double)GetTickCount() - ts.top();
		cout << "��ʱ" << t << "ms\n";
		cout << "==============================================\n";

		ts.pop();
	}
};