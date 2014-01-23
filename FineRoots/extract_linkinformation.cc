//This program reads output text files from WinRhizo files and
//extracts from them the information concerning root segments (links)
//and writes it to one file kaikki.dat. The input files are given in
//file files.txt.

//The WinRhizo text files are Windows files and need to be transformed
//to Unix (otherwise this program does not recognize end of a
//line). This succeeded (on Risto's Mac OS X 10.9.1) with Perl script
//perl -pe 's/\r\n|\n|\r/\n/g' WinRhizo_file |cut -f 1-20 | tr ' ' _ >
//output_file

//The outline of program is simple: 1) read a line from
//WinRhizo_file, replace some characters in it, 2) Extract data, if
//line concerns a LINK , and write to kaikki.dat

//This program compiles with g++ extract_linkinformation.cc -o extract


#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>
#include<vector>

using namespace std;

void replace_space(string& str) {
  string::size_type index = str.find_first_of(" ");
  while(index != string::npos) {
    str[index] = '_';
    index = str.find_first_of(" ");
  } 
}

string replace_tab(string str) {
  string::size_type len = str.size();
  string s2;
  for(string::size_type i = 0; i < len; i++) {
    s2 += str[i];
    if( (str[i] == '\t' && str[i+1] == '\t') || 
	(i == len - 1 && str[i] == '\t')){
      s2 += "-1";
    }
  }
  return s2;
}

void replace_umlaut(string& str) {
  for(string::size_type i = 0; i < str.size(); i++) {
    if(int(str[i]) == -28) str[i] = 'a';              //Why -28 ???
//     if(int(str[i]) == 142) str[i] = 'A';
//     if(int(str[i]) == 148) str[i] = 'o';
//     if(int(str[i]) == 153) str[i] = 'O';   
  }
}
    


int main(int argc, char** argv)
{

  //Ensiksi luettavat tiedostot tiedostosta
  ifstream file_file("files.txt");
  if (!file_file) {
    cout << "Eipa ole inputtiedostoa files.txt" << endl;
    exit(-1);
  }
  string line;
  list<string> input_files;
  for(;;) {
    getline(file_file,line);
    if(file_file.eof())
      break;
    input_files.push_back(line);
  }
  file_file.close();

  cout << "There are  " << (int)input_files.size()  << "  input files" << endl;

  //Sitten prosessoidaan tiedostot

  ofstream sum_file("kaikki.dat",ofstream::trunc);
  sum_file << "SampleId Part LinkNo SeedlingNo Length ProjArea SurfArea AvgDiam Ind Angle" <<
    " Magnitude Altitude Order Father Brother1 Brother2 Baby1 Baby2 Baby3" << endl;

  list<string>::iterator I;
  for(I = input_files.begin(); I != input_files.end(); I++) {

    ifstream data_file((*I));
    if (!data_file) {
      cout << "Eipa onnistunut " << *I << endl;
      exit(-1);
    }
    cout << "Processing " << *I << endl;

    string::size_type l1 = (*I).size();
    string::size_type p1 = (*I).find_last_of('/');
    string title;
    if(p1 != string::npos)
      title = "#" + (*I).substr(p1+1, l1 - p1 - 1);
    else
      title = "#???????";

    replace_space(title);

    sum_file << title << endl;

    vector<string> fields(19);
    while(!data_file.eof()){
      //   getline(inp_file,line,'\r');
      getline(data_file,line);
   
      replace_space(line);

      line = replace_tab(line);

      istringstream l(line);
      string s1,s2;
      l >> s1 >> s2;

      if(s2 == "LINK" && s1 != "SampleId") {

	//The identification label s1 is of form xxxA, xxxB, ...
	//The labels A, B, ... are taken to own variable
	fields[0] = s1.substr(0,s1.size()-1);
	fields[1] = s1.substr(s1.size()-1,1);
	l >> fields[2];
	l >> fields[3];
	l >> fields[4];           //Skip fourth column of data_file
	for(int i = 4; i < 19; i++) {
	  l >> fields[i];
	}
     
	for(int i = 0; i < 19; i++) {
	  if(fields[i].size() == 0)
	    fields[i] = "-1";
	}

	for(int i = 0; i < 19; i++) {
	  replace_umlaut(fields[i]);
	}

	for(int i = 0; i < 19; i++) {
	  sum_file << fields[i] << " ";
	}
	sum_file << endl;
      }

    }
    data_file.close();
  }
  sum_file.close();
  exit(0);
}


