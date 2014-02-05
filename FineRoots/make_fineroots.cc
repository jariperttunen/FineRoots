//This program collects link information from the file kaikki.dat about
//scanned fine roots. Only roots that are listed in the file kuvat.txt
//are processed (versions A, B and C).
//The program makes a "root tree" of each root given in kuvat.txt and
// writes its content to file <root name>.xml

//The program reads link data from kaikki.dat and when all link information
//of a root has been read, makes the root tree.
//First the topology of the root tree is built (function add_link) and
//then the architecture (length, diameter & orientation) of links is constructed.

//The progran relies heavily to the property of the links that they are
//numbered from 0 to #links-1 and if a link is acropetally from the subject link,
//its number is larger.

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <math.h>
#include <XMLTree.h>

#include <MakeFineRoots.h>

int ran3_seed = -9648383; //-23797843;

using namespace std;

void process_line(string& line, vector<string>& items) {
  istringstream l(line);
  for(int i = 0; i < 20; i++) {
    l >> items[i];
  }
}

int main()
{
  //luetaan tiedosto, jossa on luettelo juurista, jotka prosessoidaan
  ifstream juuri_file("kuvat.txt");
  if (!juuri_file) {
    cout << "Eipa ole juuritiedostoa kuvat.txt" << endl;
    exit(-1);
  }

  string line;
  set<string> kuvat;
  
  for(;;) {
    getline(juuri_file,line);
    if(juuri_file.eof())
      break;
    kuvat.insert(line);
  }

  //tiedosto, jossa koko data on
  ifstream input_file("kaikki.dat");
  if (!input_file) {
    cout << "Eipa ole inputtiedostoa kaikki.dat" << endl;
    exit(-1);
  }

  vector<string> items(20);
  list<vector<string> > root_links;

  getline(input_file,line);                 //header

  getline(input_file,line);       //first segment to start 
  process_line(line, items);
  root_links.push_back(items);

  string current_label1 = items[0];
  string current_label2 = items[1];

  bool last = false;
  for(;;) {
    getline(input_file,line);
    if(input_file.eof()) {
      last = true;
    }
    process_line(line, items);
    
    if(items[1] != current_label2 || items[0] != current_label1 || last) { //make root
      if(kuvat.find(current_label1) != kuvat.end()) {
      
	int n_links = static_cast<int>(root_links.size());
	cout << current_label1 << " " << current_label2 <<  " links: " << n_links << endl;
      
	Tree<FineRootSegment,FineRootBud> froot(Point(0,0,0), PositionVector(0,0,-1.0));

	list<vector<string> >::iterator rI;
	for(rI = root_links.begin(); rI != root_links.end(); rI++) {
	  int link_num = atoi(((*rI)[2]).c_str());
	  int father_num = atoi(((*rI)[14]).c_str());
	  int order = atoi(((*rI)[12]).c_str());

	  add_link(froot, link_num, father_num, order);
	}

	vector<vector<double> > struct_data;  //length, diam, angle by segment number
	for(rI = root_links.begin(); rI != root_links.end(); rI++) {
	  double len = atof(((*rI)[4]).c_str())/1000.0;
	  double diam = atof(((*rI)[7]).c_str())/1000.0;
	  double angle = atof(((*rI)[9]).c_str());

	  vector<double> v(3);
	  v[0] = len; v[1] = diam; v[2] = angle;
	  struct_data.push_back(v);
	}

	SetArchitecture SA(struct_data);
	SAInfo sai;
	sai.point = Point(0.0,0.0,0.0);
	sai.direction = PositionVector(0.0,0.0,-1.0); //Fine root goes downwards
	PropagateUp(froot, sai, SA);

	XMLDomTreeWriter<FineRootSegment,FineRootBud> writer;
	writer.writeTreeToXML(froot,current_label1+current_label2+".xml");
      }   //if(kuvat.find(current_label1) != ...

      if(last)
	break;

      root_links.clear();
      current_label2 = items[1];
      current_label1 = items[0];
    }
    root_links.push_back(items);
  }

  exit(0);
}


