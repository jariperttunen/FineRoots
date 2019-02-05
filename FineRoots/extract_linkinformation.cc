//This program reads output text files of  WinRhizo  and
//extracts from them the informations concerning root segments (links)
//and writes them to the file kaikki.dat. The input files are given in
//file files.txt.

//The WinRhizo text files are Windows files and need to be transformed
//to Unix (otherwise this program does not recognize end of a
//line). This succeeded (on Risto's Mac OS X 10.9.1) with Perl script
//perl -pe 's/\r\n|\n|\r/\n/g' Winrhizo_file |cut -f 1-20 | tr ' ' _ >
//Output_file

//This program consists of two parts:
//First: 1) read a line from
//WinRhizo_file, replace some characters in it, 2) Extract data, if
//line concerns a LINK, and write to kaikki0.dat

//2) Second read information of one root from kaikki0.dat (defined by SampleId and Part,
//they are in the two first columns of kaikki0.dat), calculate root
//volume (mm3) as PI x (diam/2) x length (diam is transformed 1/10 mm
// to mm) and write the data to file kaikki.dat (otherwise the same
// as kaikki0.dat but also with root volume)

//3) Delete file kaikki0.dat

//This program compiles with g++ extract_linkinformation.cc -o extract


#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>
#include <vector>
#include <math.h>

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

void  output_previous_root (list<vector<string> >& r_links, ofstream& f_stream) {
  const double PI = 3.141593;

  list<vector<string> >::iterator rI;
  double root_volume = 0.0;
  for(rI = r_links.begin(); rI != r_links.end(); rI++) {
    double len = atof(((*rI)[4]).c_str());      //pituus on rivin 5's elementti 
    double d = atof(((*rI)[7]).c_str());        //lpm on rivin 8's elementti
    d /= 10.0;                                  //jonka laatu on 1/10 mm
    root_volume += PI*pow(d/2.0,2.0)*len;
    ostringstream oss;
    oss << setprecision(3) << d;
    (*rI)[7] = oss.str();             //nyt lpm on samaa laatua (mm) kun pituus
  }

  ostringstream oss;
  oss << setprecision(4) << root_volume;
  string root_volume_string = oss.str();

  for(rI = r_links.begin(); rI != r_links.end(); rI++) {
    for(int i = 0; i < 13; i++) {
      f_stream << (*rI)[i] << " ";
    }	 
    f_stream << 	 root_volume_string << " ";

    for(int i = 13; i < 19; i++) {
      f_stream << (*rI)[i] << " ";
    }
    f_stream << endl;

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

  bool ennen = false;
  ofstream sum_file("kaikki0.dat",ofstream::trunc); //Tama on valikaikainen
  //tulostustiedosto

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
    //    int count = 0;
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
	fields[0] = s1.substr(0,s1.size());
	//	fields[0] = s1.substr(0,s1.size()-1);
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


	if(atoi(fields[2].c_str()) <= atoi(fields[13].c_str())){
	  ennen = true;
	  cout << "!!!!";
	  for(int i = 0; i < 19; i++) {
	    cout << fields[i] << " ";
	  }
	  cout << endl;
	}
    
      }    // if(s2 == "LINK" && s1 != "SampleId") ...

    }
    data_file.close();
  }
  sum_file.close();

//   if(ennen) {
//       exit(0);
//   }

  //Part 2: read kaikki0.dat and add root volume

  ofstream  lopullinen("kaikki.dat", ofstream::trunc);
  lopullinen << "SampleId Part LinkNo SeedlingNo Length ProjArea SurfArea AvgDiam Ind Angle"
	     << " Magnitude Altitude Order RVolume Father Brother1 Brother2 Baby1 Baby2 Baby3" << endl;

  list<vector<string> > root_links;
  ifstream  uudestaan("kaikki0.dat");

  string current_label2 = "", current_label1 = "";
  vector<string> items(19);
  for(;;) {
    getline(uudestaan,line);
    if(uudestaan.eof())
      break;
    if(line[0] == '#') {
      continue;
    }

    istringstream l(line);
    for(int i = 0; i < 19; i++) {
      l >> items[i];
    }

    if(items[1] != current_label2 || items[0] != current_label1) {
      current_label2 = items[1];
      current_label1 = items[0];
    
      output_previous_root(root_links, lopullinen);
      root_links.clear();
    }

    root_links.push_back(items);
  }

  //tämä koska muuten viimeinen juuri jaa kasittelematta, kun
  //on tultu for(;;) loopista pois break : lla
  output_previous_root(root_links, lopullinen);

  lopullinen.close();

  // 3)
  if(remove ("kaikki0.dat") != 0) {
    cout << "kaikki0.dat tiedoston poisto epaonnistui!" << endl;
  }
  else {
    cout << "Valitulostiedosto kaikki0.dat poistettu" << endl;
  }

  exit(0);
}


