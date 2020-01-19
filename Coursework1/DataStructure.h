#pragma once

#include <string>
#include <iostream>
#include <ctime>
#include <time.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>

#include "DateTime.h"
#include "Items.h"
#include "Headers.h"
#include "ICS0017DataSource.h"

using namespace std;

class DataStructure {
private:
	//HEADER_E *h;
	void PrintItem(ITEM10 *item);
	bool isEqual(DATE3 d1, DATE3 d2);
	static DATE3* GetCurrentDate();
	int ContainsSameItems(DataStructure &Other);
	vector<string> split(const string& str);

public:
	HEADER_E *h = nullptr;
	DataStructure();
	DataStructure(char *pFilename);
	~DataStructure();
	DataStructure(const DataStructure &Original);

	int GetItemsNumber();	ITEM10* GetItem1(char *pID);
	ITEM10* GetItem1(long int code);
	ITEM10* GetItem1(DATE3 date);
	void operator+=(ITEM10 *item);	void operator-=(char *pID);
	DataStructure &operator=(const DataStructure &Right);
	int operator==(DataStructure &Other);
	void Write(char *pFilename);
};