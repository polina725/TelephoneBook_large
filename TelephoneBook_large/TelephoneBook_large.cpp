#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <vector>
#include "Record.h"
using namespace std;

string getLine();
LPWSTR ConvertToLPWSTR(string);
vector<string> split(string, char=' ');
vector<Record*> loadDB();
vector<Record*> search(Record);
Record getNext();
void updatePointer(int);
bool paramsEqueal(string, string);
bool paramsEqueal(int, int);

int currentOffset = 0, totalBytesRead = 0;
const int frameSize = 65536;
LPSTR fileStart=NULL,currPointer=NULL;
DWORD fileSize;
HANDLE file, fileMapping;
SYSTEM_INFO sysinfo = { 0 };

int main()
{
    int offset = 0;
    GetSystemInfo(&sysinfo);
    LPCWSTR fileName = L"D:\\3course\\5sem\\OSaSP\\TelephoneBook_large\\addresses.txt";
    LPCWSTR name = L"TelephoneBook";
    file = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    fileSize = GetFileSize(file, NULL);
    fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, name);
    if (fileMapping == NULL) {
        return -1;
    }
    updatePointer(0);
    currPointer = fileStart;
    vector<Record*> records = loadDB();
    Record* r = new Record("80444488936", "", "", "", "", 0, 0, 0);//"Shuleyko", "Yaric", "Aleksandrovna", "Shirokaya", 72, 0, 40
    vector<Record*> recs = search(*r);
    for (Record* r : recs) {
        cout << r->surname << " " << r->phoneNumber << endl;
    }
    UnmapViewOfFile(fileStart);
    CloseHandle(fileMapping);
    CloseHandle(file);
}

string getLine()
{
    int i = 0, prevLength = 0;
    char* str = new char[500];
    while (fileStart[currentOffset+i]!= '\n'&&totalBytesRead<fileSize) {
        str[i] = fileStart[currentOffset + i];
        i++;
        totalBytesRead++;
        if ((currentOffset + i) % frameSize == 0) {
            updatePointer(currentOffset + i );//65536
            prevLength = i;
            i = 0;
            currentOffset = 0;
        }
    }
    str[prevLength + i - 1] = '\0';
    currentOffset = i + currentOffset + 1;
    return string(str);
}

LPWSTR ConvertToLPWSTR(string s)
{
    const char* str = s.c_str();
    wchar_t wtext[255];
    mbstowcs(wtext, str, s.size() + 1);
    LPWSTR ptr = wtext;
    return ptr;
}

vector<string> split(string str, char separator) {
    vector<string> substrings;
    int startPos = 0;
    int endPos = str.find(separator);
    while (endPos != -1) {
        substrings.push_back(str.substr(startPos, endPos - startPos));
        startPos = endPos + 1;
        endPos = str.find(separator, startPos);
    }
    substrings.push_back(str.substr(startPos, str.length() - startPos));
    return substrings;
}

vector<Record*> search(Record searchVals) {
    vector<Record*> v;
    while (v.size() < 15 && totalBytesRead<fileSize) {//||
        Record r = getNext();
        if (paramsEqueal(searchVals.blockNumber, r.blockNumber) ||
            paramsEqueal(searchVals.fathersName, r.fathersName) ||
            paramsEqueal(searchVals.name, r.name) ||
            paramsEqueal(searchVals.surname, r.surname) ||
            paramsEqueal(searchVals.phoneNumber, r.phoneNumber) ||
            paramsEqueal(searchVals.street, r.street) ||
            paramsEqueal(searchVals.flatNumber, r.flatNumber) ||
            paramsEqueal(searchVals.blockNumber, r.blockNumber) ||
            paramsEqueal(searchVals.houseNumber, r.houseNumber)) continue;

            Record* tmp = new Record(r);
            v.push_back(tmp);
        
    }
    return v;
}

bool paramsEqueal(string expected, string actual) {
    return !expected.empty() && actual != expected;
}

bool paramsEqueal(int expected, int actual) {
    return expected!=0 && expected == actual;
}
Record getNext()
{
    string str = getLine();
    vector<string> params = split(str);
    return Record(params[0],
        params[1],
        params[2],
        params[3],
        params[4],
        stoi(params[5]),
        stoi(params[6]),
        stoi(params[7])
    );
}

vector<Record*> loadDB() {
    vector<Record*> records;
    for (int i = 0; i < 15; i++) {
        Record* r = new Record(getNext());
        records.push_back(r);
    }
    return records;
}

void updatePointer(int offset) {
    if (fileStart != NULL)
        UnmapViewOfFile(fileStart);
    fileStart = (LPSTR)MapViewOfFile(fileMapping, FILE_MAP_READ, 0, offset, sysinfo.dwAllocationGranularity);
    if (fileStart == NULL) {
        fileStart = (LPSTR)MapViewOfFile(fileMapping, FILE_MAP_READ, 0, offset, 0);
    }
}

/*
wstring(chosenRecord.phoneNumber.begin(), chosenRecord.phoneNumber.end()).c_str()
to_wstring(chosenRecord.houseNumber).c_str()

                LPWSTR ptr = ConvertToLPWSTR("");
                GetWindowText(hEd, ptr, 255);
               // SendMessage(hEd, WM_GETTEXT, (WPARAM)255, (LPARAM)ptr);

                char buffer[500];

                wcstombs(buffer, ptr, 500);
                std::string param = std::string(buffer);
*/
