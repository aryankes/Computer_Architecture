/*
    author_name= Aryan Kesharwani
    Roll_No= 2201CS19

    Declaration of Authorship
    This cpp file, asm.cpp, is part of the miniproject of CS209/CS210 at the 
    department of Computer Science and Engg, IIT Patna . 
    
*/
#include <bits/stdc++.h>
using namespace std;
// Declaring 32 - bit register
#define ms map<int,string>
#define vec vector<int>
#define pb push_back
#define pr printf
int A, B, SP;
// Program Counter
int PC, Current_PC,pcc;
int Cnt{};
const int N = 10000; // MAX MEMORY
vec Instructions;
int PrintSt = 0;
ofstream Trace_out;
string outpfile1="";
int Memory[N];
string TraceFileName = "test2.trace ";
string outpfile2="";
int Max_N = 0;
set<int> type = {6, 7, 8, 9, 11, 12, 14, 18};
string head="Opcode Mnemonic Operand";
ms OPTABLE = {{0, "ldc"}, {1, "adc"}, {2, "ldl"}, {3, "stl"}, {4, "ldnl"}, {5, "stnl"}, {6, "add"}, {7, "sub"}, {8, "shl"}, {9, "shr"}, {10, "adj"}, {11, "a2sp"}, {12, "sp2a"}, {13, "call"}, {14, "return"}, {15, "brz"}, {16, "brlz"}, {17, "br"}, {18, "halt"}, {1000, "data"}, {100, "set"}},extra;
int printed_instructions=0;
int perform_num=0;
string Conv_to_hex(int i)
{
    stringstream stream;
    stream << setfill('0') << setw(8)<< hex << i;
    return stream.str();
}
bool is_memory_dumped=0;

void Memory_Dump(bool ifBefore)
{
    is_memory_dumped=1;
    cout << "----Dumping From Memory!-------";
    Trace_out << "Dumping From Memory!";
    int temp = Current_PC - 1;
    int count=0;;
    if (!ifBefore)
        temp = Max_N + 2;
    int cnt1=0,cnt2=0;
    for (int i = 0; i < temp - 1; i++)
    {
        count++;
        if ((i % 4) == 0)
        {
            pr("\n%08x \t %08x ", i, Memory[i]);
            cnt1++;
            Trace_out << endl<< Conv_to_hex(i) << " \t " << Conv_to_hex(Memory[i]) << " ";
        }
        else
        {
            cnt2++;
            pr("%08x ", Memory[i]);
            Trace_out << Conv_to_hex(Memory[i]) << " ";
            cnt2+=2;
        }
        // cout<<count<<endl;
    }
    cout << endl<< endl;Trace_out << endl;
    count++; 
    // cout<<count<<" "<<cnt1<<" "<<cnt2<<endl;
    // Trace_out.close();
}
void ldc(int n)
{
    B = A;
    A = n;
    pcc++;
}
void adc(int n)
{
    pcc++;
    A = A + n;
}
bool more_mem_req=1;
void ldl(int n)
{
    pcc++;
    B = A;
    if (SP + n >= N || SP + n < 0)
    {
        
        cerr << "ERROR, OUT OF RANGE MEMORY ACCESS\n";
        exit(0);
    }
    more_mem_req=0;
    A = Memory[SP + n];
    Max_N = max(Max_N, SP + n);
}
void stl(int n)
{
    pcc++;
    if (A + n >= N || A + n < 0)
    {
        cerr << "ERROR, OUT OF RANGE MEMORY ACCESS\n";
        exit(0);
    }
    more_mem_req=0;
    Memory[SP + n] = A;
    Max_N = max(Max_N, SP + n);
    A = B;
}
void ldnl(int n)
{
    pcc++;
    if (A + n >= N || A + n < 0)
    {
        cerr << "ERROR, OUT OF RANGE MEMORY ACCESS\n";
        exit(0);
    }
    more_mem_req=0;
    A = Memory[A + n];
    Max_N = max(Max_N, A + n);
}
void stnl(int n)
{
    pcc++;
    if (A + n >= N || A + n < 0)
    {
        cerr << "ERROR, OUT OF RANGE MEMORY ACCESS\n";
        exit(0);
    }
    more_mem_req=0;
    Memory[A + n] = B;
    Max_N = max(Max_N, A + n);
}
void add(int n)
{
    pcc++;
    A = B + A;
}
void sub(int n)
{
    pcc++;
    A = B - A;
}
void shl(int n)
{
    pcc++;
    A = B << A;
}
void shr(int n)
{
    pcc++;
    A = B >> A;
}
void adj(int n)
{
    pcc++;

    SP = SP + n;
}
void a2sp(int n)
{
    pcc++;
    SP = A;
    A = B;
}
void sp2a(int n)
{
    pcc++;
    B = A;
    A = SP;
}
void call(int n)
{
    pcc++;
    B = A;
    A = PC;
    PC += n;
    if (n == -1)
    {
        cout << "Infinite Loop Detected!\n";
        exit(0);
    }
}
void return1(int n)
{
    pcc++;
    PC = A;
    A = B;

    if (PC == A && A == B)
    {
        cout << "Infinite Loop Detected!\n";
        exit(0);
    }
}
void brz(int n)
{
    pcc++;
    if (A == 0)
    {
        if (n == -1)
        {
            cout << "Infinite Loop Detected!\n";
            exit(0);
        }
        PC = PC + n;
    }
}
void br(int n)
{
    pcc++;
    if (n == -1)
    {
        cout << "Infinite Loop Detected!\n";
        exit(0);
    }

    PC = PC + n;
}
void brlz(int n)
{
    pcc++;
    if (A < 0)
    {
        if (n == -1)
        {
            cout << "Infinite Loop Detected!\n";
            exit(0);
        }
        PC = PC + n;
    }
}

int fun=0;
void (*fun_arr[])(int) = {ldc, adc, ldl, stl, ldnl, stnl, add, sub, shl, shr, adj, a2sp, sp2a, call, return1, brz, brlz, br};
int print_states=0;
bool Printed_ins=0;
void Print_Ins(int n)
{
    printed_instructions++;
    unsigned int nn = n;
    // ofstream Trace_out(TraceFileName);
    Printed_ins=1;
    Trace_out << OPTABLE[nn % 256] << " ";
    cout << OPTABLE[nn % 256] << " ";
    unsigned int offset = nn / 256;
    bool flag2=0;
    if ((offset >> 23) & 1)
    {
        offset |= (255 << 24);
        flag2=1;
    }
    int ofs = (int)offset;
    if(flag2){
        // cout<<"YES offset is greater than 24"<<endl;
    }
    bool flag1=0;
    if (type.find(nn % 256) != type.end())
    {
        flag1=1;
        Trace_out << " " << Conv_to_hex(ofs);
        pr(" %08x", ofs);
    }
    else
    {
        flag1=0;
        Trace_out << ofs;
        pr("%d", ofs);
    }
    if(flag1){
        // cout<<"YES"<<endl;
    }
    Trace_out << "\n\n";
    pr("\n");
    pr("\n");
    // Trace_out.close();
}
bool print_stated=0;
// bool performed_or_not=0;
void Execute()
{
    perform_num++;
    Cnt++;
    unsigned int n = Instructions[PC];
    unsigned int offset = n / 256;
    unsigned int opcode = n % 256;
    bool flag3=0;
    if (OPTABLE.find(opcode) == OPTABLE.end())
    {
        flag3=1;
        cerr << "Error, Invalid Mnemonic Found !\n";
        exit(0);
    }
    if(flag3){
        // cout << "not a valid op" << opcode << "\n";
    }
    if ((offset >> 23) & 1)
    {
        offset |= (255 << 24);
    }
    int ofs = (int)offset;
    int offs=offset;
    // int *fl=((int)offs);
    bool halt1=0;
    if (opcode == 18)
    {
        halt1=1;
        Trace_out << "PC=" << Conv_to_hex(PC) << " \t SP=" << Conv_to_hex(SP) << " \t A=" << Conv_to_hex(A) << " \t B=" << Conv_to_hex(B) << " ";
        pr("PC=%08X \t SP=%08X \t A=%08X \t B=%08X ", PC, SP, A, B);
        Trace_out << "HALT ";
        offs=18;
        cout << "HALT ";
        Trace_out << "0";
        cout<<0;
        Trace_out << "\n\n";
        cout<<"\n"<<"\n";
        cout << Cnt << " number of instructions executed!\n\n";
        PC = Current_PC - 1;
        return;
    }
    (*fun_arr[opcode])(ofs);
    if(pcc==5){
        // cout<<"YES working"<<endl;
    }

    /////////////Printing all the current states

    print_stated=1;
    // ofstream Trace_out(TraceFileName);
    Trace_out << "PC=" << Conv_to_hex(PC) << " \t SP=" << Conv_to_hex(SP) << " \t A=" << Conv_to_hex(A) << " \t B=" << Conv_to_hex(B) << " ";
    print_states++;
    pr("PC=%08X \t SP=%08X \t A=%08X \t B=%08X ", PC, SP, A, B);
    /////////////////////////////////////////////////

    //cout<<pcc<<endl;
    Print_Ins(Instructions[PC]);
    PC++;
    printed_instructions--;
    // cout<<printed_instructions<<endl;
    print_states--;
    // cout<<print_states<<endl;
    // cout << Cnt << endl;
}

int main(int argc, char *argv[])
{
    // Reseting registers
    A = 0;
    B = 0;
    PC = 0;
    pcc=0;
    SP = 9999;
    Current_PC = 0;
    
    if (argc < 3)
    {
        cout << "Usage: ./emu.exe [option] file.o" << "\n";
        cout << "Options:" << "\n";
        cout << "\t-trace\tshow instruction trace" << "\n";
        cout << "\t-before\tshow memory dump before execution" << "\n";
        cout << "\t-after\tshow memory dump after execution" << "\n";
        cout << "\t-isa\tdisplay ISA" << "\n";
        return (0);
    }

    // reading file

    ifstream infile;
    infile.open(argv[2], ios::in | ios::binary);
    int read;
    int doubleread;
    while (!infile.eof())
    {
        infile.read((char *)&read, sizeof(int));
        Memory[Current_PC] = read;
        doubleread=read;
        if(doubleread==18){
            // cout<<"YES Halt"<<endl;
        }
        Instructions.pb(read);
        Current_PC++;
    }
    // for(auto i:Memory){
    //     cout<<i<<" ";
    // }
    // cout<<endl;
    // cout<<"Instructions are: "<<endl;
    // for(auto i:Instructions){
    //     cout<<i<<endl;
    // }
    Max_N = max(Current_PC - 1, Max_N);
    string input;
    string word;
    string fna="";
    int num = 0;
    int flag=0;
    string in_file=argv[2];
    TraceFileName="";
    int itr=0;
    string fnaa=fna;
    //cout << in_file << endl;
    while (in_file[itr]!='.')
    {
        
        TraceFileName+=in_file[itr];
        fna=TraceFileName;
        itr++;
    }
    TraceFileName += ".trace";
    Trace_out.open(TraceFileName);
    fna+="fna is ";
    // cout << "Total Code " << Current_PC << endl;
    if(TraceFileName.size()>10){
        // cout<<fna<<endl;
    }
    string read_f=(string)argv[1];
    if (read_f== "-trace")
    {
        while (PC < Current_PC - 1)
        {
            Execute();
        }
       ;
        Trace_out.close();
    }
    else if (read_f== "-before")
    {

        Memory_Dump(true);
        Trace_out.close();
    }
    else if (read_f== "-after")
    {
        while (PC < Current_PC - 1)
        {
            Execute();
        }
        Memory_Dump(false);
        Trace_out.close();
    }
    else if (read_f== "-isa")
    {
        cout <<head<<"\n";
        cout << "       data     value"<<endl;
        cout << "0      ldc      value"<<endl;
        cout << "1      adc      value"<<endl;
        cout << "2      ldl      value"<<endl;
        cout << "3      stl      value"<<endl;
        cout << "4      ldnl     value"<<endl;
        cout << "5      stnl     value"<<endl;
        cout << "6      add"<<endl;
        cout << "7      sub"<<endl;
        cout << "8      shl"<<endl;
        cout << "9      shr"<<endl;
        cout << "10     adj      value"<<endl;
        cout << "11     a2sp"<<endl;
        cout << "12     sp2a"<<endl;
        cout << "13     call     offset"<<endl;
        cout << "14     return"<<endl;
        cout << "15     brz      offset"<<endl;
        cout << "16     brlz     offset"<<endl;
        cout << "17     br       offset"<<endl;
        cout << "18     HALT"<<endl;
        cout << "       SET      value"<<endl;
        Trace_out.close();
    }
    else
    {
        Trace_out.close();
        // promt();
        cout << "Invalid Argument!\n";
        cout << "Usage: ./emu.exe [option] file.o" << "\n";
        cout << "Options:" << "\n";
        cout << "\t-trace\tshow instruction trace" << "\n";
        cout << "\t-before\tshow memory dump before execution" << "\n";
        cout << "\t-after\tshow memory dump after execution" << "\n";
        cout << "\t-isa\tdisplay ISA" << "\n";
    }
    return 0;
}