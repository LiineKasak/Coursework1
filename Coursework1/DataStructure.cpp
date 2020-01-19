#include "pch.h"
#include "Datastructure.h"

using namespace std;

void DataStructure::PrintItem(ITEM10* item) {
	if (!item) return;
	cout << item->pID << ' ' << item->Code << ' ';
	DATE3 *date = &(item->Date);
	cout << date->Day << ' ' << date->pMonth << ' ' << date->Year << endl;
}

bool DataStructure::isEqual(DATE3 d1, DATE3 d2)
{
	return d1.Day == d2.Day && (string)d1.pMonth == (string)d2.pMonth && d1.Year == d2.Year;
}

DATE3* DataStructure::GetCurrentDate()
{
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

int DataStructure::ContainsSameItems(DataStructure & Other)
{
	HEADER_E *p = h;
	void **ppItems;
	ITEM10 *item;

	while (p) {
		ppItems = p->ppItems;

		for (int letter = 0; letter < 26; letter++) {
			item = (ITEM10*) *(ppItems + letter);
			while (item) {
				if (!Other.GetItem1(item->pID)) return 0;
				item = item->pNext;
			}
		}

		p = p->pNext;
	}
	return 1;
}

vector<string> DataStructure::split(const string& str)
{
	vector<string> cont;
	istringstream iss(str);
	copy(istream_iterator<string>(iss),
		istream_iterator<string>(),
		back_inserter(cont));
	return cont;
}

DataStructure::DataStructure()
{
	int ITEM_NR = 10;
	h = GetStruct5(ITEM_NR, 5); // todo: change to 0
}

DataStructure::DataStructure(char *pFilename)
{
	string fn = "C:\\Users\\Liine\\source\\repos\\Coursework1\\";
	fn.append(pFilename);
	cout << "Reading from file " << fn << endl;
	string text;
	ifstream f(fn);
	vector<string> i;
	this->h = nullptr;

	while (getline(f, text)) {
		i = split(text);
		string pID = i[0];
		pID.append(" ");
		pID.append(i[1]);

		DATE3 *date = new DATE3;
		date->Day = stoi(i[3]);
		date->pMonth = &i[4][0];
		date->Year = stoi(i[5]);

		ITEM10 *item = (ITEM10 *) GetItem(10, &pID[0]);
		PrintItem(item);
		item->Code = strtoul(&i[2][0], NULL, 0);
		item->pID = &pID[0];
		*this += item;
	}

	f.close();
}

DataStructure::~DataStructure()
{
	HEADER_E *p = h;
	void **ppItems;
	ITEM10 *item, *nextItem;
	HEADER_E *next;

	while (p) {
		next = p->pNext;
		ppItems = p->ppItems;

		for (int letter = 0; letter < 26; letter++) {
			item = (ITEM10*) *(ppItems + letter);
			while (item) {
				nextItem = item->pNext;
				delete[] item->pID;
				delete item;
				item = nextItem;
			}
		}
		delete[] p->ppItems;
		p->pPrior = nullptr;
		p->pNext = nullptr;
		delete p;

		p = next;
	}
}

DataStructure::DataStructure(const DataStructure &Original)
{
	*this = Original;
}

int DataStructure::GetItemsNumber()
{
	HEADER_E *p = h;
	int i = 0;
	void **ppItems;
	ITEM10 *item;

	while (p) {
		ppItems = p->ppItems;

		for (int letter = 0; letter < 26; letter++) {
			item = (ITEM10*) *(ppItems + letter);
			while (item) {
				i++;
				item = item->pNext;
			}
		}

		p = p->pNext;
	}
	return i;
}

ITEM10* DataStructure::GetItem1(char *pID)
{
	HEADER_E *p = h;
	int i = 0;
	char c1 = pID[i];
	while (pID[++i] != ' ') {}
	char c2 = pID[++i];

	// Find first initial position
	while (p->cBegin < c1) {
		p = p->pNext;
	}

	if (p->cBegin != c1) {
		// Item not in structure
		return 0;
	}


	// Find second initial position
	void **ppItems = p->ppItems;
	int letterNr = int(c2) - int('A');
	ppItems += letterNr;
	ITEM10 *item = (ITEM10 *)*ppItems; // 26 slots - for every letter

	if (!item) {
		// Item not in structure
		return 0;
	}
	else {
		while ((string)item->pID != (string)pID) {
			if (!item->pNext) return 0;
			item = item->pNext;
		}
	}

	return item;
}


ITEM10* DataStructure::GetItem1(long int code)
{
	HEADER_E *p = h;
	void **ppItems;
	ITEM10 *item;

	while (p) {
		ppItems = p->ppItems;

		for (int letter = 0; letter < 26; letter++) {
			item = (ITEM10*) *(ppItems + letter);
			while (item) {
				if (item->Code == code) return item;
				item = item->pNext;
			}
		}

		p = p->pNext;
	}
	return 0;
}

ITEM10* DataStructure::GetItem1(DATE3 date)
{
	HEADER_E *p = h;
	void **ppItems;
	ITEM10 *item;

	while (p) {
		ppItems = p->ppItems;

		for (int letter = 0; letter < 26; letter++) {
			item = (ITEM10*) *(ppItems + letter);
			while (item) {
				if (this->isEqual(item->Date, date)) return item;
				item = item->pNext;
			}
		}

		p = p->pNext;
	}
	return 0;
}

void DataStructure::operator+=(ITEM10 * item)
{
	if (!item) throw "Not a valid item!";

	if (!item->Date.pMonth) item->Date = *GetCurrentDate();
	item->pNext = nullptr;

	int i = 0;
	char c1 = item->pID[i];
	while (item->pID[++i] != ' ') {}
	char c2 = item->pID[++i];

	// Find first initial position
	HEADER_E *pPrior = h;
	if (!h) {
		h = new HEADER_E;
		h->cBegin = c1;
		h->pPrior = nullptr;
		h->pNext = nullptr;
		h->ppItems = new void*[26];
		for (int i = 0; i < 26; i++) *(h->ppItems + i) = nullptr;
	}
	else {
		while (h->cBegin < c1 && h->pNext) {
			pPrior = h;
			h = h->pNext;
			h->pPrior = pPrior;
		}
		if (h->cBegin < c1 && !h->pNext) {
			HEADER_E *newHeader = new HEADER_E;
			newHeader->cBegin = c1;

			newHeader->pPrior = pPrior;
			h->pNext = newHeader;

			newHeader->pNext = nullptr;
			newHeader->ppItems = new void*[26];
			for (int i = 0; i < 26; i++) *(newHeader->ppItems + i) = nullptr;
			h = newHeader;
		}
		else if (h->cBegin != c1) {
			// First initial not in header, adding new header
			HEADER_E *newHeader = new HEADER_E();
			newHeader->cBegin = c1;

			newHeader->pPrior = h->pPrior;
			h->pPrior->pNext = newHeader;

			newHeader->pNext = h;
			h->pPrior = newHeader;

			newHeader->ppItems = new void*[26];
			for (int i = 0; i < 26; i++) *(newHeader->ppItems + i) = NULL;

			h = newHeader;
		}
	}


	// Find second initial position
	void **ppItems = h->ppItems;
	int letterNr = int(c2) - int('A');
	ppItems += letterNr;
	ITEM10 *lastItem = (ITEM10 *)*ppItems; // 26 slots - for every letter

	if (!lastItem) {
		// No items with same first and last initial
		*ppItems = item;
	}
	else {
		// There exist items with the same first and second initial
		if ((string)lastItem->pID == (string)item->pID) throw "ID already exists!";
		while (lastItem->pNext) {
			lastItem = lastItem->pNext;
			if ((string)lastItem->pID == (string)item->pID) throw "ID already exists!";
		}
		lastItem->pNext = item;
	}

	// PrintItem(item);

	// Go to first position
	while (h->pPrior) h = h->pPrior;
}

void DataStructure::operator-=(char *pID)
{
	int i = 0;
	char c1 = pID[i];
	while (pID[++i] != ' ') {}
	char c2 = pID[++i];

	// Find first initial position
	HEADER_E *pPrior;
	while (h->cBegin < c1) {
		pPrior = h;
		h = h->pNext;
		h->pPrior = pPrior;
	}

	// First initial not in header
	if (h->cBegin != c1) {
		cout << "ID does not exist!" << endl;
		throw "ID does not exist!";
		return;
	}


	// Find second initial position
	void **ppItems = h->ppItems;
	int letterNr = int(c2) - int('A');
	ppItems += letterNr;
	ITEM10 *item = (ITEM10 *)*(ppItems); // 26 slots - for every letter

	// No items with same first and last initial
	if (!item) {
		cout << "ID does not exist!" << endl;
		throw "ID does not exist!";
		return;
	}

	// There exist items with the same first and second initial
	ITEM10 *lastItem = NULL;
	while (item->pNext) {
		if ((string)item->pID == (string)pID) break;
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
		HEADER_E *removableHeader = h;
		h->pPrior->pNext = h->pNext;
		h = h->pPrior;
		delete removableHeader;
	}

	// Go to first position
	while (h->pPrior) h = h->pPrior;
}

DataStructure& DataStructure::operator=(const DataStructure& Right)
{
	delete this;
	this->h = NULL;
	HEADER_E *p = Right.h, *pPrior = nullptr;
	void **ppItems;
	ITEM10 *item, *newItem, *last;
	last = nullptr;

	while (p) {
		ppItems = p->ppItems;
		p->pPrior = pPrior;

		for (int letter = 0; letter < 26; letter++) {
			item = (ITEM10*) *(ppItems + letter);
			while (item) {
				newItem = new ITEM10;
				newItem->pID = item->pID;
				newItem->Code = item->Code;
				newItem->Date = item->Date;
				newItem->pNext = last;
				*this += newItem;
				last = newItem;
				item = item->pNext;
			}
		}
		pPrior = p;
		p = p->pNext;
	}
	p = pPrior;
	while (p->pPrior) p = p->pPrior;
	return *this;
}

int DataStructure::operator==(DataStructure & Other)
{
	return this->ContainsSameItems(Other) && Other.ContainsSameItems(*this);
}

void DataStructure::Write(char * pFilename)
{
	string fn = "C:\\Users\\Liine\\source\\repos\\Coursework1\\";
	fn.append(pFilename);

	cout << "Writing to file " << fn << endl;
	ofstream f;
	f.open(fn);


	HEADER_E *p = h;
	int i = 0;
	void **ppItems;
	ITEM10 *item;

	while (p) {
		ppItems = p->ppItems;

		for (int letter = 0; letter < 26; letter++) {
			item = (ITEM10*) *(ppItems + letter);
			while (item) {
				f << item->pID << ' ' << item->Code << ' ';
				DATE3 *date = &(item->Date);
				f << date->Day << ' ' << date->pMonth << ' ' << date->Year << endl;
				item = item->pNext;
			}
		}

		p = p->pNext;
	}

	f.close();
}
