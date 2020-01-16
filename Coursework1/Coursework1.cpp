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

void PrintItem(ITEM10* item, int i) {
	cout << i << ") " << item->pID << ' ' << item->Code << ' ';
	DATE3 *date = &(item->Date);
	cout << date->Day << ' ' << date->pMonth << ' ' << date->Year << endl;
}

void PrintDataStructure(HEADER_E *p)
{
	int i = 1;
	void **ppItems;

	while (p) {
		ppItems = p->ppItems;

		for (int letter = 0; letter < 26; letter++) {
			ITEM10 *item = (ITEM10*) *(ppItems + letter);

			while (item) {
				PrintItem(item, i++);
				item = item->pNext;
			}
		}

		p = p->pNext;
	}
}



bool IsAlphabetic(string s)
{
	return find_if(s.begin(), s.end(),
			[](char c) { return !isalpha(c); }) != s.end();
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
	if (!CheckID(pNewItemID)) throw "ID is not valid!";
	ITEM10 *item = (ITEM10 *)GetItem(1, pNewItemID); // ??????????????????????????????????????? mingi exception catchida wtf
	item->Date = *GetCurrentDate();
	item->pNext = NULL;

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
		if (lastItem->pID == pNewItemID) throw "ID already exists!";
		while (lastItem->pNext) {
			lastItem = lastItem->pNext;
			if (lastItem->pID == pNewItemID) throw "ID already exists!";
		}
		lastItem->pNext = item;
	}

	// Go to first position
	while (p->pPrior) p = p->pPrior;
	return p;
}

int main()
{
	int ITEM_NR = 10;
	HEADER_E *p = GetStruct5(ITEM_NR, 100);
	ITEM10 *pNewItem = (ITEM10 *)GetItem(10);

	// #1 Print
	// PrintDataStructure(p);

	// #2 Add item
	// string s = "Red Blue";
	// string s = "Red Delta";
	string s = "Kori Blue";
	char *ID = &s[0];
	p = InsertItem(p, ID);
	
	// #3 Remove item

	PrintDataStructure(p);
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
