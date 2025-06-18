#include <bits/stdc++.h>
using namespace std;

// Function to read the entire content of a file into a string
string read(string filename)
{
    ifstream file(filename);
    if (!file)
    {
        throw runtime_error("Not able to access file : " + filename);
    }
    stringstream content;
    content << file.rdbuf(); // Read the file buffer into stringstream
    return content.str();
}

// Function to write a string content to a file
void write(string content, string filename)
{
    ofstream file(filename);
    if (!file)
    {
        throw runtime_error("Not able to access file : " + filename);
    }
    file << content;
}

// Function to compress a string using basic RLE (Run-Length Encoding) technique (single-threaded)
string compress(string content)
{
    if (content.empty())
        return "";

    stringstream str;
    int c = 1;
    for (int i = 1; i <= content.length(); i++)
    {
        if (i < content.length() && content[i] == content[i - 1])
        {
            c++; // Count repeating characters
        }
        else
        {
            str << c << content[i - 1]; // Append count and character
            c = 1;
        }
    }
    return str.str(); // Return compressed string
}

// Function to compress string using multiple threads for speed optimization
string mcompress(string content, int tcount)
{
    /*
        This function splits the content into 'tcount' chunks and compresses them in parallel using threads.
        Each thread processes a portion of the content and stores its result in a shared vector.
    */

    vector<thread> thr;           // Thread vector to manage multiple threads
    vector<string> str(tcount);   // Stores results from each thread
    int csize = content.length() / tcount; // Size of each chunk

    // Lambda function that compresses a chunk of the string
    auto letscompress = [&](int x, int start, int end)
    {
        string s = content.substr(start, end - start);
        str[x] = compress(s);
    };

    // Create threads to compress each chunk
    for (int i = 0; i < tcount; i++)
    {
        int st = i * csize;
        int end = (i == tcount) ? content.length() : csize * (i + 1);
        thr.push_back(thread(letscompress, i, st, end));
    }

    // Wait for all threads to finish
    for (auto &t : thr)
        t.join();

    // Combine results from all threads
    string result = str[0];
    for (int i = 1; i < tcount; i++)
    {
        result += str[i];
    }
    return result;
}

// Function to decompress RLE-compressed string back to original
string decompress(string content)
{
    if (!isdigit(content[0])) {
        throw runtime_error("Invalid compressed file format.");
    }

    stringstream str;
    int c = 0;

    for (char ch : content)
    {
        if (isdigit(ch)) {
            if (c > 1e7) throw runtime_error("Invalid compressed format: too large repeat count");
            c = c * 10 + (ch - '0'); // Parse number from digits
        }
        else {
            str << string(c, ch); // Append character 'c' times
            c = 0;
        }
    }
    return str.str();
}

/*
Function Overview:
read()        → Reads file content as a string.
write()       → Writes a string to a file.
compress()    → Compresses using Run-Length Encoding (RLE) in a single thread.
mcompress()   → Compresses using multiple threads to improve performance.
decompress()  → Decompresses an RLE-compressed string.
*/

int main(int argc, char* arg[])
{
    // If not enough arguments are provided
    if (argc < 4) {
        if (argc == 2 && arg[1] == "--info") {
            // Provide usage info
            cout << "Hey this is my tool to compress and Decompress the text file. This program is CLI based.\n";
            cout << "The format of writing the command is:\n";
            cout << "   ./myproject compress <input_file> <output_file> [--single|--multi|--time] [--time]\n";
            return 0;
        }
        else {
            cout << "Write commands properly." << endl;
            return 1;
        }
    }

    string cd = arg[1], input = arg[2], output = arg[3];
    bool multi = false, time = false;

    // Parse optional arguments
    for (int i = 4; i < argc; i++)
    {
        string cond = arg[i];
        if (cond == "--multi")
            multi = true;
        if (cond == "--single")
            multi = false;
        if (cond == "--time")
            time = true;
    }

    try {
        // Measure time if requested
        auto st = chrono::high_resolution_clock::now();

        if (cd == "compress")
        {
            string content = read(input);
            string compressed;

            if (multi)
            {
                compressed = mcompress(content, 5); // Use 5 threads
            }
            else {
                compressed = compress(content);     // Use single thread
            }

            write(compressed, output);             // Write to output file
        }
        else if (cd == "decompress")
        {
            if (input == output) {
                throw runtime_error("Input and output files must be different.");
            }

            string content = read(input);
            string decompressed = decompress(content);
            write(decompressed, output);
        }
        else
        {
            cout << " Write 'compress' or 'decompress' only" << endl;
        }

        auto fn = chrono::high_resolution_clock::now();

        // Show execution time if requested
        if (time)
        {
            cout << "Successfully Done!\n";
            chrono::duration<double> duration = fn - st;
            cout << "Execution time : " << duration.count() << " seconds " << endl;
        }
    }
    catch (exception &e)
    {
        cerr << "Error : " << e.what() << endl; // Display error message if any exception occurs
    }

    return 0;
}
