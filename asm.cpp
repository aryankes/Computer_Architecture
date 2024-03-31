#include <bits/stdc++.h>
using namespace std;

typedef vector<string> vs;
typedef vector<pair<int, string>> vpis;
typedef set<string> sets;
typedef set<int> seti;

// Structure For a Mnemonic
struct Mnemonic
{

    string opcode; // Opcode in Binary
    int mnemonic_type;
    /*  Type:
        Case 0 - No Operands
        Case 1 - Argument is Given
        Case 2 - Offset is Given
    */
};

// Structure For Labels And Symbols
struct symb
{

    int address; // 32- Bit Address

    int type_of_symb;

    /*
        Type:
        Case 0 means Data or Variable is Declared
        Case 1Lab means Label or Block name is Declared

    */
};
struct part_in_3
{

    string Label;
    string Instruction,Operand;
    
    part_in_3(string &LABEL, string &INSTRUCTION, string &OPERAND)
    {
        Label = LABEL;
        Instruction = INSTRUCTION;
        Operand = OPERAND;
    }
};
typedef vector<pair<int, part_in_3>> vpip3;
typedef map<string, symb> mss;
mss Declared_Label; // For Storing Declared Labels 
mss Used_Label;     // For Storing Used Labels
// Class For Storing OPCODES
class OPTABLE
{

public:
    map<string, Mnemonic> opta; // Map for storing opcodes
    // Function For Filling OPTABLE
    void Initialise_optable()
    {

        int typ=1;
        opta["data"]={"",typ} ; //data has no opcode
        opta["ldc"] = {"00", typ} ; //loads A
        opta["adc"] = {"01", typ} ; //incerases A
        opta["adj"]={"0A",typ} ; // increases SP
        opta["set"] = {"", typ} ; //has no opcode
        //Filling the OPMAP by the instruction manual
        //Opcode is 8-bit which is 2-byte Hexadecimal
        //For 1 operand  argument ones we have type=1
        //For offset type =2
        typ=2;
        opta["ldl"] = {"02", typ} ; //loading in memory sp+offset
        opta["stl"] = {"03", typ} ; //storing in memory sp+offset
        opta["ldnl"] = {"04", typ} ; // non -local loading from A=mem[A+offset]
        opta["stnl"] = {"05", typ} ; //non local storing in B mem[A+offset]=B
        opta["call"] = {"0D", typ} ; //B := A ;  A := PC ;  PC := PC + offset ;  
        opta["brz"] = {"0F", typ} ; //If accumulator is zero, branch to specified offset
        opta["brlz"] = {"10", typ} ; //if A < 0 then PC := PC + offset ; 
        opta["br"] = {"11", typ} ; //PC := PC + offset ;  

        //For no operand type=0
        typ=0;
        opta["add"] = {"06", typ} ; //A := B + A ;  
        opta["sub"] = {"07", typ} ; //A=B-A
        opta["shl"] = {"08", typ} ; //A := B << A
        opta["shr"] = {"09", typ} ; //A := B >> A ;  
        opta["a2sp"] = {"0B", typ} ; //transfer a to sp//sp=a a=b
        opta["sp2a"] = {"0C", typ} ; //Transfer SP to A 
        opta["return"] = {"0E", typ} ; //Return from procedure PC=A A+B
        opta["halt"] = {"12", typ} ; //like li v0 10 exit or stopping the emulator
    }
};

OPTABLE OPtable;

vpip3 Data_Seg; // Pair of 32- Bit addres and Label, Instruction, Operand For Data Segment
// Declaring Variables For All required Data Variables

int pc = 0; // Program Counter
int pcc=0;//Extra temporary pc counter

bool Halt_Declared = false; // Flag to check if Halt Function is called
bool Set_Ins_called=0;//check whether set ins present or not
// Declaring The ouput Files Data Types
vs MachineCode; // Final Machine Code
vs Log_code;//final Log code
vs ListingCode; // Final Listing Code

// Table_of_Warnings and Table_of_Errors

vpis Table_of_Warnings;
vpis Table_of_Errors;

// Clean Output for 1st- Pass

vpip3 Clean_Instructions; // 32-Bit address with Label,Instruction,Operand for 1st pass without comments and spaces

// input For 2nd- Pass
string MachineCodeFileName;

vpip3 Input_Instructions; // 32-Bit address with Label,Instruction,Operand for 2nd Pass after error Checking

// Input Filenmae,LogFile,MachineCode,ListingCode
string LogFileName;

sets Declared_Data; //  set of Labels with Data 
sets Set_Data;      //  set of Labels with Set
string ListingFileName;
bool getLIO_in_or_not=0;
class GetLIO
{

public:
    void getLIO(string &LabelName, string &Instruction, string &Operand, int &LineNumber, string Cleansed_Input)
    {
        getLIO_in_or_not=1;
        int word = 0;

        // Checking If a label is present
        bool if_label=0;
        for (word=0; word < Cleansed_Input.size(); word++)
        {

            if (Cleansed_Input[word] == ':')
            {
                if_label=1;
                for (int i = 0; i < word; i++)
                {
                    LabelName += Cleansed_Input[i];
                }
                word++;
                break;
            }
        }
        if (Cleansed_Input[word - 1] != ':'){
            word = 0;
        }
            
        if(if_label){
            // cout<<"YES"<<endl;
            if_label=0;
        }
        while (word<Cleansed_Input.size()&&(Cleansed_Input[word] == ' ' || Cleansed_Input[word] == '\t'))
        {
            word++;
        }
        while (word<Cleansed_Input.size()&&!(Cleansed_Input[word] == ' ' || Cleansed_Input[word] == '\t'))
        {
            Instruction += Cleansed_Input[word];
            word++;
        }

        while (word<Cleansed_Input.size()&&(Cleansed_Input[word] == ' ' || Cleansed_Input[word] == '\t'))
        {
            word++;
        }
        
        while (word<Cleansed_Input.size()&&!(Cleansed_Input[word] == ' ' || Cleansed_Input[word] == '\t'))
        {
            Operand += Cleansed_Input[word];

            word++;
        }
        // cout<<Instruction<<" "<<Operand<<endl;
        // Table_of_Errors :

        while (word<Cleansed_Input.size()&&(Cleansed_Input[word] == ' ' || Cleansed_Input[word] == '\t'))
        {
            word++;
        }
        if (word != Cleansed_Input.size())
        {
            string error = "There is a Extra Operand! ";
            Table_of_Errors.push_back({LineNumber, error});
        }
    }
};
GetLIO GEtLIO;
class TypeCheck
{
public:
    bool ifNumber(char x)
    {
        return (x >= '0' && x <= '9') ;
    }
    bool ifChar(char c)
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');;
    }

    bool ifHexaDecimal(string s)
    {
        bool flag1=s.compare(0, 2, "0x") == 0 ;
        bool flag2=s.size() > 2;
        bool flag3=s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
        return flag1 && flag2 && flag3;
    }
    // Function to check if Octal
    bool ifOctal(string s)
    {

        bool flag1=s.compare(0, 1, "0") == 0 ;
        bool flag2=s.size() > 1;
        bool flag3=s.find_first_not_of("01234567", 1) == std::string::npos;
        return flag1 && flag2 && flag3;
    }

    int Hex_2_Dec(string numb)
    {

        int len = numb.size();

        // Initializing base value to 1, i.e 16^0
        int x = 1;

        int ans = 0;

        for (int i = len - 1; i >= 2; i--)
        {
            if (ifNumber(numb[i]))
            {
                ans += (int(numb[i]) - '0') * x;
                x = x * 16;
            }
            else if (numb[i] >= 'A' && numb[i] <= 'F')
            {
                ans += (int(numb[i]) - 'A'+10) * x;

                x = x * 16;
            }
        }
        return ans;
    }
    // Function to convert Octal to Decimal
    int Oct_2_Dec(string numb)
    {
        int n = numb.size();
        int x = 1;
        int ans = 0;
        for (int i = n - 1; i >= 1; i--)
        {
            if (numb[i] >= '0' && numb[i] <= '7')
            {
                ans += (int(numb[i]) - '0') * x;
                x = x * 8;
            }
        }
        return ans;
    }
    // Function To check If its a valid Label Name
    bool is_valid_LabelName(string &Label_Name)
    {
        if (Label_Name.empty())
            return false;
        bool temp = 1;
        // First should be Only Char
        bool flag= (ifChar(Label_Name[0]));
        for (int i = 1; i < Label_Name.size(); i++)
        {
            temp &= (ifChar(Label_Name[i])||ifNumber(Label_Name[i])||Label_Name[i] == '_');
        }
        temp&=flag;
        return temp;
    }
};
int Operand_pro=0;
string OperandProcess(string s, int *a, int LineNum)
{
    Operand_pro++;
    if (TypeCheck().is_valid_LabelName(s))
    {
        Used_Label[s] = {LineNum, 5};
        return s;
    }

    int sign = 1;
    bool flag=s[0] == '-';
    if (flag)
        sign *= -1;

    flag = (s[0] == '+');
    flag=(flag||s[0] == '-');

    if (flag)
        s = s.substr(1);

    if (TypeCheck().ifOctal(s)){
        string st=to_string(sign*(TypeCheck().Oct_2_Dec(s)));
        return st;
    }
    if (TypeCheck().ifHexaDecimal(s)){
        string st=to_string(sign * (TypeCheck().Hex_2_Dec(s)));
        return st;
    }
        
    bool flag1 = 1;
    
    for (int i = 0;; i++)
    {
        if(i>=s.size()){
            break;
        }
        flag1 &= TypeCheck().ifNumber(s[i]);
        
    }
    *a=0;
    if (flag1){
        string st=to_string(sign * stoi(s));
        return st;
    }
    *a = 1;
    return s;
}

// Function To remove comments and trailing Spaces , Needed before 1st Pass
bool Clean_raw_in=0;
string CleanRawInput(string s)
{
    Clean_raw_in=1;
    string new_str;
    int itr = 0;

    // Clearing Starting Spaces
    while (itr<s.size()&&(s[itr] == ' '))
    {
        itr++;
    }

    int i = itr;
    /*Instruction Till Comment ';'*/
    for (i=itr; i < s.size(); i++)
    {
        char temp = s[i];

        if (s[i] == ';')
            break;
        /* Comment Case*/

        /* all lower Alphabets*/
        if ((temp >= 'A' && temp <= 'Z'))
            temp = tolower(temp);
        new_str.push_back(temp);
    }

    return new_str;
}
// Function to Fill Label Names declared By Data And Set
bool Seperating_d_s=0;
void Seperating_Data_and_Set_labels()
{
    Seperating_d_s=1;
    vpip3 tempInstruction;
    for (auto &Line : Clean_Instructions){
        //reporting whether set ins or not
        if ((Line.second.Instruction == "set"))
        {
            Set_Data.insert(Line.second.Label);
        }
    }

    /* Now we will go through  Names Declared in Data */
    for (auto &Line : Data_Seg)
    {
        if(Line.second.Instruction=="data"){
            // cout<<"YES"<<endl;
        }
        Declared_Data.insert(Line.second.Label);
    }
    /* For More than one Declaration Of same Label  */
    //Now Dataseg contains data and set labels and temp containe remaning ins
    int count=0;
    for (auto &Line : Clean_Instructions){//clean instructions don't contain initially data segment lines
        bool inData = false;
        if ((Line.second.Instruction == "set")){
            if (Declared_Data.count(Line.second.Label) == 0)
            {
                if(Declared_Data.find(Line.second.Label)!=Declared_Data.end()){
					// cout<<"finded"<<endl;
				}
                count++;
                Line.second.Instruction = "data";
                Declared_Data.insert(Line.second.Label);
                inData = true;
                Data_Seg.push_back(Line);
            }
        }
        if (!inData){//only those instruction stored which don't contain set instructions
            /* Store in Temp Instruction */
            if(Line.second.Instruction=="Data"){
                // cout<<"error Clean_instructions contains data segment lines"
            }
            tempInstruction.push_back(Line);
        }
    }
    /* Since Tempory Instruction Stored we can clear The instructions for FInal Instructions */
    Clean_Instructions.clear();

    /* Reconstruction Instruction with data at the end of instruction */
    int cnt1=0,cnt2=0;
    for (auto &Line : tempInstruction)
    {
        Clean_Instructions.push_back(Line);
        cnt1++;
    }
    for (auto &Line : Data_Seg)
    {
        Clean_Instructions.push_back(Line);
        cnt2++;
    }
    //finally we have all the set and data labels in declared_data & all set labels also in set_data
    //also all clean instructions have data labels in the bottom and rest in above
    // Clearing Extra Memory
    if(cnt2==count){
    //     cout<<"only originally set instr present"
    }
    tempInstruction.clear();
    Data_Seg.clear();
    
    // Final Cleaned Instructions for Pass 1 are ready
}
bool Ins_setted=0;
bool setInstruction(string &LabelName, string &val, int LineNum, int to_add)
{
    Ins_setted=1;
    /* Checking For Table_of_Errors*/

    if (LabelName.empty())
    {
        string error = "No Label Name Given - Set Instruction! ";
        Ins_setted=0;
        Table_of_Errors.push_back({LineNum, error});
        return false;
    }
    if (val.empty())
    {
        string error = "No Operand Given - Set Instruction! ";
        Table_of_Errors.push_back({LineNum, error});
        Ins_setted=0;
        return false;
    }
    int check = 0;
    /* Convertin Operand into int*/

    string nop = OperandProcess(val, &check, LineNum);
    if (TypeCheck().ifChar(val[0])||check)
    {
        string error = "Unexpected Number - Set Instruction! ";
        Table_of_Errors.push_back({LineNum, error});
        Ins_setted=0;
        return false;
    }
    
    string ins;

    if (Declared_Data.count(LabelName)&&Declared_Label.count(LabelName))
    {
        return true;
    }
    else
    {
        string op;
        string label;

        /* Increase Stack Size*/

        ins = "adj";op = "2";
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="";
        pc++;
        /* PUSH A*/

        ins = "stl";op = "-1";
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="";
        pc++;
        /* PUSH B */

        ins = "stl";op = "0";
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="";
        pc++;
        /* Load Value to Set */

        ins = "ldc";op = val;
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="ldc";
        pc++;
        /* Load Location Of Pointer */

        ins = "ldc";op = LabelName;
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="adc";
        pc++;
        /* Set the Value in Array */

        ins = "adc";op = to_string(to_add);
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="";
        pc++;
        /* Store the value To Set in memory */

        ins = "stnl";op = "0";
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="";
        pc++;
        /*  Load Back B */

        ins = "ldl";op = "0";
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="";
        pc++;

        /* Load Back A */

        ins = "ldl";op = "0";
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="";
        pc++;

        /* Decrease Stack Size */

        ins = "adj";op = "-2";
        nop = OperandProcess(op, &check, LineNum);
        Input_Instructions.push_back({pc, part_in_3(label, ins, nop)});pcc++;ins="";
        pc++;
    }
    return false;
}
// Function To Get Code from File
bool Get_file=0;
string InputName;
void GetFile()
{
    Get_file=1;
    ifstream reading_file;
    reading_file.open(InputName);
    bool fllag=reading_file.fail();
    if (fllag)
    {
        cout << "Error Occured - Can't Open Input File\n";
        exit(0);
    }
    int LineNumber = 0;
    string LineInput;
    // Taking Line By line Input From File and Removing trailing and extra Spaces
    while (getline(reading_file, LineInput))
    {
        // cout<<LineInput<<endl;
        LineNumber++;
        string Cleansed_Input = CleanRawInput(LineInput);
        // cout<<Cleansed_Input<<endl;
        if (Cleansed_Input.empty())
            continue;
        string LabelName,Instruction,Operand;
        /* Getting The Label Name ,Instruction ,Operand with Line and also checking For Table_of_Errors */
        GEtLIO.getLIO(LabelName, Instruction, Operand, LineNumber, Cleansed_Input);

        /* Checking For Data Instruction */
        int data_ins=0,clecount=0;
        if ((!Instruction.empty())&&Instruction == "data")
        {
            /* Storing Only Data Label name in Data Segment, Required For Branch Operation */

            if ( Clean_Instructions.size() > 0)
            {
                auto lastInstruction = Clean_Instructions.back();
                /*checking if last line of code just had Label Name */
                if (LabelName.empty()&&lastInstruction.second.Instruction.empty())
                {
                    /* Label Name Should be stored In data for Branch operation, Needed in 2nd Pass*/
                    Data_Seg.push_back(lastInstruction);
                    Clean_Instructions.pop_back();
                }
            }
            /* Storing Current Line in data */
            data_ins++;
            Data_Seg.push_back({LineNumber, part_in_3{LabelName, Instruction, Operand}});
        }
        else
        {
            clecount++;
            Clean_Instructions.push_back({LineNumber, part_in_3{LabelName, Instruction, Operand}});
        }
        // cout<<data_ins<<endl;
        // for(auto i:Clean_Instructions){
        //     cout<<i.first<<i.second.Instruction<<" "<<i.second.Label<<" "<<i.second.Operand<<endl;
        // }
    }
    // for(auto i:Data_Seg){
    //     cout<<i.first<<" "<<i.second.Instruction<<" "<<i.second.Label<<" "<<i.second.Operand<<endl;
    // }
}

/* -------------------------ASSEMBLER PASS NO 1 ----------------------------------*/
bool Assem_pas1=0;
void Assembler_Pass_1()
{
    Assem_pas1=1;
    /* First We Have To GEt Input File */

    GetFile();
    /* Checking if declared by set or data */
    // for(auto i:Clean_Instructions){
    //     cout<<i.first<<" "<<i.second.Instruction<<" "<<i.second.Label<<" "<<i.second.Operand<<endl;
    // }
    // cout<<endl;
    // cout<<"Data_seg Begins here"<<endl;
    // for(auto i:Data_Seg){
    //     cout<<i.first<<" "<<i.second.Instruction<<" "<<i.second.Label<<" "<<i.second.Operand<<endl;
    // }
    Seperating_Data_and_Set_labels();
    // for(auto i:Clean_Instructions){
    //     cout<<i.first<<" "<<i.second.Label<<" "<<i.second.Instruction<<" "<<i.second.Operand<<endl;
    // }
    string prev;
    int count = 0;
    /* Iterating through cleaned pass 1 instruction */

    for (auto &Line : Clean_Instructions)
    {
        // cout<<"YES"<<endl;
        int LineNum = Line.first;
        string Operand,LabelName,Instruction;
        Operand = Line.second.Operand;
        LabelName = Line.second.Label;
        Instruction = Line.second.Instruction;
        bool toDeclare = false;
        int toIncrease = 0;
        if (!LabelName.empty())
        {
            /* Checking For A Valid Label Name */
            if (TypeCheck().is_valid_LabelName(LabelName))
            {
                /* Declare the Label Name */

                count = 0;
                toDeclare = 1;
                prev = LabelName;
            }
            else
            {
                string error = "Incorrect Label Name!";
                Table_of_Errors.push_back({LineNum, error});
                
            }
        }
        else
        {
            /* Already in label name or No Label Name */
            count++;
        }
        bool needOperand = false;
        bool flag = false;
        int use=0;
        int minus = 1;
        // Checking For Instruction
        // cout<<Instruction<<endl;
        if (!Instruction.empty())
        {
            /* Cheking for valid Instruction name */
            if (!(OPtable.opta.count(Instruction) > 0))
            {
                // cout << Instruction << endl;
                string error = "Invalid Mnemonic! ";
                Table_of_Errors.push_back({LineNum, error});
            }
            else
            {
                /* Need Operand for Mnemic Type>0 */
                needOperand |= (OPtable.opta[Instruction].mnemonic_type > 0);
                /* IF instruction is set */

                if ((Instruction == "set"))
                {
                    flag = setInstruction(prev, Operand, LineNum, count);
                }
                if (flag)
                {
                    Instruction = "data";
                }
                toIncrease++;
                bool fllag=toDeclare;fllag&=(Instruction == "data");
                if (fllag)
                {
                    use = 1;
                }
            }
        }
        bool setIns=(Instruction == "set");
        if (!flag && setIns)
        {
            continue;
        }
        if (toDeclare)
        {
            if (Declared_Label.count(LabelName) > 0)
            {
                string error = "Duplicate Label Found - \"" + LabelName + "\"";
                Table_of_Errors.push_back({LineNum, error});
            }
            else
            {
                Declared_Label[LabelName] = {pc, use};//use=0 means normal label else use=1 means data label
            }
        }
        string nop;
        if (!Operand.empty())
        {
            if (!needOperand)
            {
                string error = "Unexpected Operand Found ! ";
                Table_of_Errors.push_back({LineNum, error});
            }
            else
            {
                /* Checking iF it is Label Name or Number */
                int a = 0;
                nop = OperandProcess(Operand, &a, LineNum);
                if (a == 1)
                {
                    string error = "Valid Number or label Name not Found! ";
                    Table_of_Errors.push_back({LineNum, error});
                }
            }
        }
        else
        {
            if (needOperand)
            {
                string error = "Operand Missing! ";
                Table_of_Errors.push_back({LineNum, error});
            }
        }
        Input_Instructions.push_back({pc, part_in_3(LabelName, Instruction, nop)});
        pc += toIncrease;
    }
    // for(auto i:Declared_Label){
    //     cout<<i.first<<" "<<i.second.address<<endl;
    // }
    // cout<<"Clean instr are: "<<endl;
    // for(auto i:Input_Instructions){
    //     cout<<i.first<<" "<<i.second.Label<<" "<<i.second.Instruction<<" "<<i.second.Operand<<endl;
    // }
    auto it=Used_Label.begin();
    while(it!=Used_Label.end())
    {
        int addr = it->second.address;
        bool check1 = Declared_Label.count(it->first) == 0;
        if (check1)
        {
            string error = "No  Label as \"" + it->first + "\" has been Declared! ";
            Table_of_Errors.push_back({addr, error});
        }
        it++;
    }
    auto itrr=Declared_Label.begin();
    while(itrr!=Declared_Label.end())
    {
        bool check1 = Used_Label.count(itrr->first) == 0;
        if (check1)
        {
            string error = "Label with Name \"" + itrr->first + "\" not used but Declared! ";
            Table_of_Warnings.push_back({itrr->second.address, error});
        }
        itrr++;
    }
}
// Pass 2
int inGen_hex=0;
//generates hexa decimal code
string Gen_Hexacode(int num, bool is24)
{
    
    inGen_hex++;
    map<int, char> m;
    vector<char> vec(16);
    char dig = '0';
    char c = 'A';
    string ans="00000000";
    for (int i = 0; i <= 15; i++)
    {
        if (i >= 10)
        {
            vec[i] = c++;
            m[i]=vec[i];
        }
        else
        {
            vec[i] = dig++;
            m[i]=vec[i];
        }
    }
    string res = "";
    if (!num)
    {
        ans;
        res = "0";
    }
    if (num > 0)
    {
        ans=to_string(num);
        while (num)
        {
            res = m[num % 16] + res;
            num /= 16;
        }
    }
    else
    {
        ans=to_string(num);
        unsigned int n = num;
        while (n)
        {
            res = m[n % 16] + res;
            n /= 16;
        }
    }

    int temp = res.size();
    for (int itr = 1; itr <= 8 - temp; itr++)
    {
        ans[itr-1]=res[itr];
        res = '0' + res;
    }
    if (is24)
    {
        if(ans.size()==8){
            // cout<<ans<<endl;
        }
        return res.substr(2);
    }
    return res;
}
bool Assem2=0;
void Assembler_Pass_2()
{
    Assem2=1;
    
    for (auto &Line : Input_Instructions)
    {
        bool isDataValue = false;
        string Instruction,InstructionValue,Operand,LabelName;
        LabelName = Line.second.Label;
        Operand = Line.second.Operand;
        Instruction = Line.second.Instruction;
        int counter=0;
        int OperandValue;
        bool needOffset = false;
        int Location = Line.first;
        string Final;
        bool fllag=0;
        bool is_halt=0;
        if(Instruction=="halt"){
            is_halt=1;
        }
        if (!Instruction.empty())
        {   
            bool checkt = OPtable.opta[Instruction].mnemonic_type == 2;
            InstructionValue = OPtable.opta[Instruction].opcode;
            
            if (is_halt)
                Halt_Declared = true;
            if (checkt)
            {
                fllag=1;
                needOffset = 1;
            }
            if (Instruction == "data")
                isDataValue = true;
        }
        if (!Operand.empty())
        {
            if (Declared_Label.count(Operand) == 0)
            {
                /* Convert The operand to Int*/
                OperandValue = stoi(Operand);
            }
            else
            {
                OperandValue = Declared_Label[Operand].address;
            }
            bool temp4 = needOffset;
            temp4 &= Declared_Label.count(Operand) != 0;
            if (temp4)
            {
                OperandValue -= Location + 1;
                // Take care when operand should be 0 but it comes to -1
            }
            if (isDataValue)
            {
                /* Need to Convert to 32-Bit Address */
                Final= Gen_Hexacode(OperandValue, false);
            }
            else
            {
                /* Convert to 24 Bit Address i.e 6 place hexadecimal */
                string tempk = Gen_Hexacode(OperandValue, true);
                tempk += InstructionValue;
                Final = tempk;
            }
        }
        else
        {
            Final = "000000" + InstructionValue;
        }
        string ProgramCounter,Mac_Code, Lis_Code;
        ProgramCounter = Gen_Hexacode(Location, false);
        Lis_Code = ProgramCounter;
        if (!Instruction.empty())
        {
            Mac_Code += Final;
            Lis_Code += " " + Final;
            counter++;
        }
        else
        {
            Lis_Code += "         ";
            counter++;
        }
        if (!LabelName.empty())
        {
            Lis_Code += " " + LabelName + ":";
            counter++;
        }
        if (!Instruction.empty())
        {
            Lis_Code += " " + Instruction;
            counter++;

        }
        if (!Operand.empty())
        {
            Lis_Code += " " + Operand;
            counter++;

        }

        if (!Mac_Code.empty()){
            MachineCode.push_back(Mac_Code);counter++;
        }

        if (!Lis_Code.empty()){
            ListingCode.push_back(Lis_Code);counter++;
        }
        if(counter>5){
            // cout<<counter<<endl;
        }
    }
    if (!Halt_Declared)
    {
        Table_of_Warnings.push_back({-1, "HALT Instruction Not Found! "});
    }
}
char *Filename;
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "error: Please pass only *.asm File ";
        return 0;
    }
    Filename = argv[1];
    // Checking if File is of .asm extension
    int len=strlen(Filename);
    if (strlen(Filename) <= 3 or strcmp("asm", Filename + len - 3))
    {
        cout << "Error: Incorrect file type\n";
        cout << "please Enter a .asm file ";
        return 0;
    }
    string FilenameWithoutASM;
    int file_name_len=0;
    for (int i = 0; argv[1][i] != '.'; i++)
    {
        file_name_len++;
        FilenameWithoutASM += argv[1][i];
    }

    InputName = argv[1];
    // initialising OPTABLE
    OPtable.Initialise_optable();

    Assembler_Pass_1();
    // for(auto i:Input_Instructions){
    //     cout<<i.first<<" "<<i.second.Instruction<<" "<<i.second.Label<<" "<<i.second.Operand<<endl;
    // }
    LogFileName = FilenameWithoutASM + ".log";

    if (Table_of_Errors.size() > 0)
    {

        /* Reading From Table_of_Errors and priting */
        cout << "-------------------Process Failed! Please Correct Your Code----------------------\n";
        cout << "-------------------------------ERRORS FOUND!---------------------------\n";
        sort(Table_of_Errors.begin(), Table_of_Errors.end());
        //sorts on the basis of line number
        int cnt=0;
        ofstream Lgout(LogFileName);

        Lgout << "Failed To Assemble\n";
        Lgout << "errors are :- \n";
        for (auto &i : Table_of_Errors)
        {
            int line_num=i.first;
            cout << "Error at Line " << line_num << " : " << i.second << endl;
            cnt++;
        }
        for (auto &i : Table_of_Errors)
        {
            int line_num=i.first;
            Lgout << "Error at Line " << line_num << " : " << i.second << endl;
            cnt--;
        }
        if(cnt==0){
            // cout<<"YES error=log"
        }
        Lgout.close();
        exit(0);
    }

    Assembler_Pass_2();
    if (Table_of_Warnings.size() > 0)
    {
        /* Reading From Warning and priting */

        sort(Table_of_Warnings.begin(), Table_of_Warnings.end());
        int cnt=0;

        ofstream Lgout(LogFileName);
        cout << "---------------------------Table_of_Warnings--------------------------------- \n";
        Lgout << "Table_of_Warnings -- \n";
        for (auto &i : Table_of_Warnings)
        {
            cnt++;
            int line_num=i.first;
            cout << "Warning at Line " << line_num << " : " << i.second << endl;
        }
        for (auto &i : Table_of_Warnings)
        {
            cnt--;
            int line_num=i.first;
            Lgout << "Warning at Line " << line_num<< " : " << i.second << endl;
        }
        if(cnt==0){
            // cout<<"YES printed lines=loglines"<<endl;
        }
        Lgout.close();
    }
    ListingFileName = FilenameWithoutASM + ".l";
    ofstream Lout(ListingFileName);
    for (auto i : ListingCode)
    {
        Lout << i << endl;
    }
    Lout.close();
    ofstream Objout;
    MachineCodeFileName = FilenameWithoutASM + ".o";
    Objout.open(MachineCodeFileName, ios::binary | ios::out);
    for (auto &i : MachineCode)
    {
        unsigned int y;
        stringstream ss;
        ss << hex << i;
        ss >> y;
        static_cast<int>(y);
        Objout.write((const char *)&y, sizeof(unsigned));
    }
    Objout.close();
    return 0;
}