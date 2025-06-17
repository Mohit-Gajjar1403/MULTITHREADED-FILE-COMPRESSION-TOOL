#include <bits/stdc++.h>
using namespace std;
string read(string filename)
{
    ifstream file (filename);
    if(!file)
    {
        throw runtime_error("Not able to access file : "+ filename);
    }
    stringstream content;
    content<<file.rdbuf();
    return content.str();
}
void write(string content, string filename)
{
    ofstream file(filename);
    if(!file)
    {
        throw runtime_error("Not able to access file : "+ filename);
    }
    file<<content;
}
string compress(string content)
{
    if (content.empty()) 
        return "";
    stringstream str;
    int c=1;
    for(int i=1;i<=content.length();i++)
    {
        if(i<content.length()&&content[i]==content[i-1])
        {
            c++;
        }
        else
        {
            str<<c<<content[i-1];
            c=1;
        }
    }
    return str.str();
}
string mcompress(string content,int tcount)
{
    /*
        for multithreading we'll use this
        we will create a vector of threads and assign a chunk of content to each thread
        then we will compress the content to optimize time.
    */
   vector<thread> thr;
   vector<string> str(tcount);
   int csize=content.length()/tcount;

   //lamda function to put chunks in str vector and then compress it alongide with threads
   auto letscompress=[&](int x,int start, int end)
   {
        string s=content.substr(start,end-start);
        str[x]=compress(s);
   };

   for(int i=0;i<tcount;i++)
   {
        int st=i*csize;
        int end=(i==tcount)?content.length():csize*(i+1);
        thr.push_back(thread(letscompress,i,st,end));
   }
   
   for(auto& t:thr)
        t.join();
    
    string result=str[0];
    for(int i=1;i<tcount;i++)
    {
        result+=str[i];
    }
    return result;
}
string decompress(string content)
{
    if (!isdigit(content[0])) {
        throw runtime_error("Invalid compressed file format.");
    }
    stringstream str;
    int c=0;
    for(char ch:content)
    {
        if(isdigit(ch)){
             if (c > 1e7) throw runtime_error("Invalid compressed format: too large repeat count");
            c=c*10+(ch-'0');
        }
        else{
            str<<string(c,ch);
            c=0;
        }
    }
    return str.str();
}
int main(int argc, char* arg[])
{
    if(argc<4){
    if(argc==2&&arg[1]=="--info"){
    cout<<"Hey this is my tool to compress and Decompress the text file this program is CLI based : \n"<<"The format of writing the command is :";
    cout<<" \n   ./myproject compress <input_file> <output_file> [--single|--multi|--time] [--time]\n";
    }
    else{
        cout<<"Write commands Properly."<<endl;
        return 1;
    }
    }
    string cd=arg[1], input=arg[2], output=arg[3];

    bool multi=false, time=false;
    // if no other arguments than we have to use single thread and not have to display time

    for(int i=4;i<argc;i++)
    {
        string cond=arg[i];
        if(cond=="--multi")
            multi=true;
        if(cond=="--single")
            multi=false;
        if(cond=="--time")
            time=true;
    }

    try{
        auto st=chrono::high_resolution_clock::now();
        if(cd=="compress")
        {
            string content=read(input);
            string compressed;
            if(multi)
            {
               compressed=mcompress(content,5);
            }
            else{
                compressed=compress(content);
            }
            write(compressed,output);
        }
        else if(cd=="decompress")
        {
            if (input == output) {
                throw runtime_error("Input and output files must be different.");
            }
            string content=read(input);
            string decompressed=decompress(content);
            write(decompressed,output);
        }
        else
        {
            cout<<" Write Compress or Decompress only"<<endl;
        }
        auto fn=chrono::high_resolution_clock::now();
        if(time)
        {
            cout<<"Seccessfully Done!\n";
            chrono::duration<double> duration = fn-st;
            cout<<"Execution time : "<<duration.count()<<" seconds "<<endl;
        }
    }
    catch(exception& e)
    {
        cerr<<"Error : "<<e.what()<<endl;
    }
    return 0;
}