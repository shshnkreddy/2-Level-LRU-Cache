#include<bits/stdc++.h>
using namespace std;

const int CACHE1SIZE = 4096;
const int CACHE2SIZE = 4194304;
const int MAINMEMORYSIZE = 1024*1024*1024;
int currTime = 0;
double l1Hit = 0;
double l2Hit = 0;
double tot = 0;
const int inputLines = 482824;

int res = 0;

struct unit
{
    int time = -1;
    int data;
    bool validBit = false;
    int tag;
};

struct line1
{
    unit l[2];

    void LRU(int data, int tag)
    {
        int minIndex;
        if(l[0].time < l[1].time) minIndex = 0;

        else minIndex = 1;

        l[minIndex].data = data;
        l[minIndex].tag = tag;
        l[minIndex].time = currTime;
        l[minIndex].validBit = true;
    }
};

struct line2
{
    unit l[4];

    void LRU(int data, int tag)
    {
        int minIndex = 0;
        for(int i = 0; i < 4; i++)
        {
            if(l[minIndex].time > l[i].time)
                minIndex = i;
        }
        l[minIndex].data = data;
        l[minIndex].tag = tag;
        l[minIndex].time = currTime;
        l[minIndex].validBit = true;
    }


};

int *mainMemory;
int *mainMemory2;
line1 l1Cache[CACHE1SIZE];
line2 l2Cache[CACHE2SIZE];
int p = 0;



int getTag1(int x)
{
    return x >> 12;
}

int getTag2(int x)
{
    return x >> 10;
}

bool outOfLimits(int address)
{
    if(address < 0 || address > MAINMEMORYSIZE)
        return true;
    return false;
}


int load(unsigned int address)
{
    bool memFlag = false;
 
    currTime++;
    int lineNumber = address % CACHE1SIZE;
    int tag = getTag1(address);
    for(int i = 0; i < 2; i++)
    {
        if(l1Cache[lineNumber].l[i].tag == tag && l1Cache[lineNumber].l[i].validBit)
        {
            l1Hit++;
            l1Cache[lineNumber].l[i].time = currTime;
            //cout << "FOUND IN LEVEL1-CACHE" << "\n";
            return l1Cache[lineNumber].l[i].data;
        }
    }

    //Missed in first cache, checks second cache
    lineNumber = address % CACHE2SIZE;
    for(int i = 0; i < 4; i++)
    {
        if(l2Cache[lineNumber].l[i].tag == tag && l2Cache[lineNumber].l[i].validBit)
        {
            l2Hit++;
            int data = l2Cache[lineNumber].l[i].data;
            l1Cache[address % CACHE1SIZE].LRU(data, getTag1(address));
            //cout << "FOUND IN LEVEL2-CACHE" << "\n"; 
            return l2Cache[lineNumber].l[i].data;
        }
    }

    if(outOfLimits(address))
    {
        address %= MAINMEMORYSIZE;
        memFlag = true;
        //return 0;
    }

    //Missed in both caches, using main memory
    int data = 0;
    if(!memFlag) data = mainMemory[address];
    else data = mainMemory2[address];

    p++;
    l1Cache[address % CACHE1SIZE].LRU(data, getTag1(address));
    l2Cache[address % CACHE2SIZE].LRU(data, getTag2(address));
    //cout << "FOUND IN MAIN-MEMORY" << "\n";        
    return data;
}

int hexadecimalToDecimal(string hexVal) 
{    
    transform(hexVal.begin(), hexVal.end(), hexVal.begin(), ::toupper);
    int len = hexVal.length();
    int base = 1;       
    int dec_val = 0; 

    for (int i = len-1; i >= 0; i--) 
    {    
        if (hexVal[i] >= '0' && hexVal[i] <= '9') 
        {
            dec_val += (hexVal[i] - 48)*base; 
            base = base * 16; 
        } 
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') 
        { 
            dec_val += (hexVal[i] - 55)*base; 
            base = base*16; 
        } 
    } 
    return abs(dec_val); 
}

void store(unsigned int address, int data)
{
    bool memFlag = false;
   
    currTime++;
    int lineNumber = address % CACHE1SIZE;
    int tag = getTag1(address);
    bool flag1 = false;
    for(int i = 0; i < 2; i++)
    {
        if(l1Cache[lineNumber].l[i].tag == tag && l1Cache[lineNumber].l[i].validBit)
        {
            l1Hit++;
            l1Cache[lineNumber].l[i].data = data;
            flag1 = true;
            l1Cache[lineNumber].l[i].time = currTime;
            break;
            //cout << "UPDATED LEVEL-1 CACHE" << "\n";
        }
    }
    bool flag2 = false;
    lineNumber = address % CACHE2SIZE;
    for(int i = 0; i < 4; i++)
    {
        if(flag1)
            break;
        if(l2Cache[lineNumber].l[i].tag == tag && l2Cache[lineNumber].l[i].validBit)
        {
            l2Hit++;
            l2Cache[lineNumber].l[i].data = data;
            flag2 = true;
            l2Cache[lineNumber].l[i].time = currTime;
            //cout << "UPDATED LEVEL-2 CACHE" << "\n";
            break;
        }
    }
    if(!flag1 && flag2)
    {
        l1Cache[address % CACHE1SIZE].LRU(data, getTag1(address));
        //cout << "NOT FOUND IN L1 BUT FOUND IN L2 : UPDATED LEVEL-1 CACHE" << "\n";
    }
    if(!flag1 && !flag2)
    {
        l1Cache[address % CACHE1SIZE].LRU(data, getTag1(address));
        l2Cache[address % CACHE2SIZE].LRU(data, getTag2(address));
        //cout << "NOT FOUND IN EITHER CACHE : ADDED TO CACHES" << "\n";
    }

     if(outOfLimits(address))
    {
        address %= MAINMEMORYSIZE;
        memFlag = true;
        //return;
    }
    if(!flag1 && !flag2)
        p++;

    if(!memFlag) mainMemory[address] = data;

    else mainMemory2[address] = data;
}



int32_t main()
{
    freopen("twolf.trace", "r", stdin);
    //freopen("output.txt", "w", stdout);
    mainMemory = new int[MAINMEMORYSIZE+1];
    mainMemory2 = new int[MAINMEMORYSIZE+1];
    for(int i = 0; i < inputLines; i++)
    {
        int memoryInterval;
        string type;
        string hexAddress;
        cin >> type;
        if(type == "X")
            break;
        cin >> hexAddress >> memoryInterval;
        if(hexAddress.size() <= 2)
        {
            res++; 
            continue;
        }
        unsigned int address;
        stringstream ss;
        ss << hex << hexAddress.substr(2, hexAddress.length() - 2);
        ss >> address;
        //int address = hexadecimalToDecimal(hexAddress.substr(2, hexAddress.length() - 2));
        srand(time(0));
        if(type == "s")
        {
            store(address, rand() % 1000);
        }
        else if(type == "l")
        {
            load(address);
            //cout << load(address) << "\n";
        }
        ++tot;
    }
    cout << "Hit rate is: " << (l1Hit + l2Hit)*100 / tot << "%\n";
    cout << "L1-Hit rate: " << (l1Hit*100)/tot << "%\tL2-Hit rate: " << (l2Hit*100)/(tot-l1Hit) << "%\n";
    return 0;
}