// Coursework1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <time.h>

#include "DateTime.h"
#include "Items.h"
#include "Headers.h"
#include "ICS0017DataSource.h"
#include "DataStructure.h"


using namespace std;


vector<string> split(const string& str)
{
	vector<string> cont;
	istringstream iss(str);
	copy(istream_iterator<string>(iss),
		istream_iterator<string>(),
		back_inserter(cont));
	return cont;
}

void PrintItem(ITEM10* item) {
	if (!item) return;
	cout << item->pID << ' ' << item->Code << ' ';
	DATE3 *date = &(item->Date);
	cout << date->Day << ' ' << date->pMonth << ' ' << date->Year << endl;
}

void PrintDataStructure(HEADER_E *h)
{
	int i = 1;
	void **ppItems;
	HEADER_E *p = h;
	HEADER_E *pPrior = nullptr;

	while (p) {
		p->pPrior = pPrior;
		ppItems = p->ppItems;

		for (int letter = 0; letter < 26; letter++) {
			ITEM10 *item = (ITEM10*) *(ppItems + letter);
			// cout << letter << " " << ppItems + letter << " " << *(ppItems + letter) << endl;
			while (item) {
				cout << i++ << ") ";
				PrintItem(item);
				item = item->pNext;
			}
		}

		pPrior = p;
		p = p->pNext;
	}
	p = pPrior;
	while (p->pPrior) p = p->pPrior;
}



bool IsAlphabetic(string s)
{
	char *str = &s[0];
	while (*str) {
		if (!isalpha(*str)) return false;
		str++;
	}
	return true;
}

bool CheckID(char *ID)
{
	vector<string> splitted = split(ID);
	if (splitted.size() == 2) {
		string firstWord = splitted.at(0);
		string secondWord = splitted.at(1);

		if (IsAlphabetic(firstWord) && IsAlphabetic(secondWord)) {

			if (isupper(firstWord[0]) && isupper(secondWord[0])) {
				return true;
			}
		}
	}
	return false;
}

DATE3* GetCurrentDate() {
	time_t now = time(0);
	tm ltm;
	localtime_s(&ltm, &now);

	static char months[12][10]{
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	};

	DATE3 *date = new DATE3();
	date->Day = ltm.tm_mday;
	date->pMonth = months[ltm.tm_mon];
	date->Year = 1900 + ltm.tm_year;

	return date;
}

HEADER_E* InsertItem(HEADER_E *p, char *pNewItemID)
{
	cout << "Inserting item " << pNewItemID << endl;
	if (!CheckID(pNewItemID)) throw "ID is not valid!";

	ITEM10 *item = (ITEM10 *)GetItem(1, pNewItemID);
	if (!item) throw "Not a valid item!";

	item->Date = *GetCurrentDate();
	item->pNext = nullptr;

	char c1 = pNewItemID[0];
	char c2 = split(pNewItemID)[1][0];

	// Find first initial position
	HEADER_E *pPrior;
	while (p->cBegin < c1) {
		pPrior = p;
		p = p->pNext;
		p->pPrior = pPrior;
	}

	if (p->cBegin != c1) {
		// First initial not in header, adding new header
		HEADER_E *newHeader = new HEADER_E();
		newHeader->cBegin = c1;

		newHeader->pPrior = p->pPrior;
		p->pPrior->pNext = newHeader;

		newHeader->pNext = p;
		p->pPrior = newHeader;

		newHeader->ppItems = new void*[26];
		for (int i = 0; i < 26; i++) *(newHeader->ppItems + i) = NULL;

		p = newHeader;
	}


	// Find second initial position
	void **ppItems = p->ppItems;
	int letterNr = int(c2) - int('A');
	ppItems += letterNr;
	ITEM10 *lastItem = (ITEM10 *) *ppItems; // 26 slots - for every letter
	
	if (!lastItem) {
		// No items with same first and last initial
		*ppItems = item;
	}
	else {
		// There exist items with the same first and second initial
		if ((string) lastItem->pID == (string) pNewItemID) throw "ID already exists!";
		while (lastItem->pNext) {
			lastItem = lastItem->pNext;
			if ((string) lastItem->pID == (string) pNewItemID) throw "ID already exists!";
		}
		lastItem->pNext = item;
	}

	// Go to first position
	while (p->pPrior) p = p->pPrior;
	return p;
}


HEADER_E* RemoveItem(HEADER_E *p, char *pItemID)
{
	cout << "Removing item " << pItemID << endl;
	if (!CheckID(pItemID)) {
		cout << "ID is not valid!" << endl;
		throw "ID is not valid!";
		return 0;
	}

	char c1 = pItemID[0];
	char c2 = split(pItemID)[1][0];

	// Find first initial position
	HEADER_E *pPrior;
	while (p->cBegin < c1) {
		pPrior = p;
		p = p->pNext;
		p->pPrior = pPrior;
	}

	// First initial not in header
	if (p->cBegin != c1) {
		cout << "ID does not exist!" << endl;
		throw "ID does not exist!";
		return 0;
	}


	// Find second initial position
	void **ppItems = p->ppItems;
	int letterNr = int(c2) - int('A');
	ppItems += letterNr;
	ITEM10 *item = (ITEM10 *)*(ppItems); // 26 slots - for every letter

	// No items with same first and last initial
	if (!item) {
		cout << "ID does not exist!" << endl;
		throw "ID does not exist!";
		return 0;
	}

	// There exist items with the same first and second initial
	ITEM10 *lastItem = NULL;
	while (item->pNext) {
		if ((string) item->pID == (string) pItemID) break;
		lastItem = item;
		item = item->pNext;
	}

	
	// Remove item
	if (item->pNext) {
		// cout << "Removable items next item is " << item->pNext->pID << endl;
		if (lastItem && lastItem != item) {
			// cout << "Last item was " << lastItem->pID << endl;
			lastItem->pNext = item->pNext;
		}
		else {
			// cout << "First item is now " << item->pNext->pID << endl;
			*(ppItems) = item->pNext;
		}
	}
	else if (!lastItem) *(ppItems) = nullptr;
	else lastItem->pNext = nullptr;

	delete[] item->pID;
	delete item; // TODO: make this work


	ppItems -= letterNr;
	// Check if header empty
	bool isEmpty = true;
	for (int letter = 0; letter < 26; letter++) {
		// cout << letter << " " << ppItems + letter << " " << *(ppItems + letter) << endl;
		if (*(ppItems + letter)) {
			isEmpty = false;
			// cout << "Not empty at letter " << char(int('A') + letter)  << endl;
			break;
		}
	}
	if (isEmpty) {
		// Remove header if empty
		// cout << "delete header" << endl;
		HEADER_E *removableHeader = p;
		p->pPrior->pNext = p->pNext;
		p = p->pPrior;
		delete removableHeader;
	}

	// Go to first position
	while (p->pPrior) p = p->pPrior;
	return p;
}


void PrintInfo() {
	printf(
		"\n\nTo execute a method, on the CLI type the name of the method followed by space separated values as paramaters according to their order (NB! parameters must match their type).\n\n"
	);
	printf("Methods:\n");
	printf("-- Print\n");
	printf("-- Insert [char* pID]\n");
	printf("-- Remove [char* pID]\n");
	printf("Press ENTER to see info.\n");
}



int main1()
{
	int ITEM_NR = 10;
	HEADER_E *p = GetStruct5(ITEM_NR, 100);
	cout << "Constructed datastructure 5 of 100 item nr 10." << endl;

	// #1 Print
	//PrintDataStructure(p);

	// #2 Add item
	// string s = "Red Blue";
	// string s = "Red Delta";
	//string s = "Kori Blue";
	//char *ID = &s[0];
	//p = InsertItem(p, ID);
	//PrintDataStructure(p);
	// #3 Remove item
	//p = RemoveItem(p, (char*) "Ultramarine Blue");
	//p = RemoveItem(p, (char*) "Tiffany Blue");
	//p = RemoveItem(p, (char*) "Piggy Pink");

	// PrintDataStructure(p);

	string input, func, s1;
	vector<string> para;
	PrintInfo();
	while (true) {
		getline(cin, input);

		if (input == "")  PrintInfo();
		else {
			para = split(input);
			func = para.at(0);
			if (func == "Print") PrintDataStructure(p);
			else {
				if (para.size() != 3) cout << "Invalid number of parameters! Try again.." << endl;
				else {
					s1 = para.at(1);
					s1.append(" ");
					s1.append(para.at(2));

					if (func == "Remove") RemoveItem(p, &s1[0]);
					else if (func == "Insert") InsertItem(p, &s1[0]);
					else cout << "Invalid function! Try again..";
				}
			}
		}
	}
	return 0;
}

// COURSEWORK 2

void PrintInfo2() {
	printf(
		"\n\nTo execute a method, on the CLI type the name of the method followed by space separated values as paramaters according to their order (NB! parameters must match their type).\n\n"
	);
	printf("Methods:\n");
	printf("-- Print\n");
	printf("-- GetItemNumber\n");
	printf("-- GetItemByID [char* pID]\n");
	printf("-- GetItemByCode [long int code]\n");
	printf("-- GetItemByDate [int day] [char* month] [int year]\n");
	printf("-- AddItem [char* pID]\n");
	printf("-- RemoveItem [char* pID]\n");
	printf("-- GenerateNew\n");
	// printf("-- Compare\n");
	printf("-- Write [char* filename]");
	printf("Press ENTER to see info.\n");
}


int main2()
{
	DataStructure *d = new DataStructure();
	DataStructure *d2 = new DataStructure();
	*d2 = *d;
	PrintInfo2();

	string input, func, s1;
	vector<string> para;
	DATE3 *date;
	while (true) {
		getline(cin, input);
		// PrintDataStructure(d2->h);

		if (input == "")  PrintInfo2();
		else {
			para = split(input);
			func = para.at(0);
			if (func == "Print") PrintDataStructure(d->h);
			else if (func == "GetItemNumber") cout << d->GetItemsNumber() << endl;
			else if (func == "GetItemByCode") PrintItem(d->GetItem1(strtoul(&para.at(1)[0], NULL, 0)));
			else if (func == "GetItemByDate") {
				date = new DATE3;
				date->Day = stoi(para.at(1));
				date->pMonth = &para.at(2)[0];
				date->Year = stoi(para.at(3));
				PrintItem(d->GetItem1(*date));
			}
			else if (func == "GenerateNew") d = new DataStructure();
			else if (func == "Compare") cout << (d == d2) << endl;
			else if (func == "Write") d->Write(&para.at(1)[0]);
			else if (func == "Read") d = new DataStructure(&para.at(1)[0]);
			else {
				s1 = para.at(1);
				s1.append(" ");
				s1.append(para.at(2));
				if (func == "GetItemByID") PrintItem(d->GetItem1(&s1[0]));
				else if (func == "AddItem") *d += (ITEM10*)GetItem(10, &s1[0]);
				else if (func == "RemoveItem") *d -= &s1[0];
			}
		}
	}
	return 0;
}

int main() {

	//main1();
	main2();

	return 0;
}