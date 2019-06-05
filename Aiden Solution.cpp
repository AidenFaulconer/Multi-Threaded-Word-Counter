#include "pch.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <future>
#include <unordered_map>
#include <windows.h>
using namespace std;
string replaceFirstOccurrence(
	string& s,
	const string& toReplace,
	const string& replaceWith)
{
	size_t pos = s.find(toReplace);
	if (pos == string::npos) return s;
	return s.replace(pos, toReplace.length(), replaceWith);
}

string exePath() {
	string path;
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	path = string(buffer).substr(0, pos)/*+"\\system.exe"*/;
	return path;
}

int main(int argc, char* argv[])
{
	vector<string> fileName;
    string path = exePath() + "\\";
	string DIR = replaceFirstOccurrence(path, "WordCounter.exe", "");//name of the executable hardcoded, if built this must match up correctly
	cout << DIR << endl;

	fileName = { "data_1.txt","data_2.txt","data_3.txt","data_4.txt","data_5.txt","data_6.txt","data_7.txt","data_8.txt","data_9.txt","data_10.txt" };//hardcoded text file names to search, this can scale upwards or downwards
	static vector<pair<int, string>> finalWordCount;
	static vector<thread> workers;//store threads so we can check for thread termination before continuing the main thread
	mutex lock_finalWordCount;
	for (string& item : fileName)
	{
		string currentFile = DIR + item;
		cout << currentFile << endl; 
		workers.push_back(thread([currentFile, &lock_finalWordCount]{
			unordered_map<string, int>thisWordCount;
			ifstream filestream;
			string word;
			filestream.open(currentFile);
			if (!filestream) {
				cout << "Unable to open file, change directory path or file names" << endl;
				exit(1);
			}
			else if (filestream.is_open()) {
				cout << "Starting thread" << "\n";
				while (filestream >> word)//concurrently read through text files and only sequentially access a reasource when the heavy work finishes
				{
					++thisWordCount[word];
				}
			}
			filestream.close();
			lock_finalWordCount.lock();//protect reasource to ensure only one thread has access to it at a time
			for (auto item : thisWordCount) {
				if (item.second > 1) { finalWordCount.push_back(pair<int,string>(item.second, item.first)); }
			}
			lock_finalWordCount.unlock();
		}));

	}

	for (auto& thread : workers) {//ensure safe termination of threads
		thread.join();
	}

	sort(finalWordCount.begin(), finalWordCount.end(), [](const pair<int, string> &a, const pair<int, string> &b){
		return (a.first > b.first); 
	});

	cout << flush;
	system("CLS");
	cout << "____________________" << endl;
	for (const auto &it : finalWordCount) {
		cout << it.first << "\t" << it.second << "\n";
	}
	cout << "____________________" << endl;
	Sleep(10000);
	return 0;
}