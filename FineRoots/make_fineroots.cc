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
#include <utility>
#include <math.h>
#include <XMLTree.h>
#include <ParseCommandLine.h>
#include <MakeFineRoots.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/ch_graham_andrew.h>
#include <CGAL/Polygon_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef CGAL::Polygon_2<K> Polygon_2;

// #include <list>

// typedef CGAL::Point_2<K> Point;
// typedef CGAL::Polygon_2<K> Polygon_2;
// typedef Polygon_2::Vertex_iterator VertexIterator;
// typedef Polygon_2::Edge_const_iterator EdgeIterator;


using namespace std;

int ran3_seed = -9648383; //-23797843;

void process_line(string& line, vector<string>& items) {
  istringstream l(line);
  for(int i = 0; i < 20; i++) {
    l >> items[i];
  }
}

int main(int argc, char* argv[])
{

  string clarg;
  if(ParseCommandLine(argc,argv,"-seed", clarg )) {
    ran3_seed = atoi(clarg.c_str());
    ran3_seed = -abs(ran3_seed);
    ran3(&ran3_seed);
  }

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

  //   if(CheckCommandLine(argc,argv,"-influenceArea")) {
  //     ofstream are("areas.dat", ofstream::app);
  //     are << "seedling root  area" << endl;
  //     are.close();
  //   }


  vector<string> items(21);
  list<vector<string> > root_links;

  getline(input_file,line);                 //header

  getline(input_file,line);       //first segment to start 
  process_line(line, items);
  items[20] = "false";
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
	cout << "root_links " << " " <<root_links.size() << endl;

	int added_links = 1;
	while(added_links > 0) {
	  added_links = 0;
	  for(rI = root_links.begin(); rI != root_links.end(); rI++) {
	    if((*rI)[20] == "false") {
	      int link_num = atoi(((*rI)[2]).c_str());
	      int father_num = atoi(((*rI)[14]).c_str());
	      int order = atoi(((*rI)[12]).c_str());

	      bool succ = add_link(froot, link_num, father_num, order);
	      if(succ) {
		(*rI)[20] = "true";
		added_links++;
	      }
	    }
	  }
	  //	  cout << "added_links " << added_links << endl;
	}

	vector<pair<int, vector<double> > >struct_data;  //length, diam, angle by segment number
	for(rI = root_links.begin(); rI != root_links.end(); rI++) {
	  double len = atof(((*rI)[4]).c_str())/1000.0;
	  double diam = atof(((*rI)[7]).c_str())/1000.0;
	  double angle = atof(((*rI)[9]).c_str());
	  int link_num = atoi(((*rI)[2]).c_str());

	  vector<double> v(3);
	  v[0] = len; v[1] = diam; v[2] = angle;
	  
	  pair<int, vector<double> > item(link_num, v);
	  struct_data.push_back(item);
	}

	//	ForEach(froot,PrintN());

	SetArchitecture SA(struct_data);
	SAInfo sai;
	sai.point = Point(0.0,0.0,0.0);
	sai.direction = PositionVector(0.0,0.0,-1.0); //Fine root goes downwards
	PropagateUp(froot, sai, SA);

	if(CheckCommandLine(argc,argv,"-influenceArea")) {
	  
	  vector<pair<double, double> > apexes;

	  apexes = Accumulate(froot, apexes,  CollectApexes());
	  //	  cout << apexes.size() << endl;

	  cout << current_label1+current_label2 << endl;
	  vector<Point_2> apexes2;
	  //	  vector<pair<double, double> >::iterator Ip;
	  //	  for(Ip = apexes.begin(), Ip != apexes.end(); Ip++) {
	  for(int i = 0; i < (int)apexes.size(); i++) {
	    apexes2.push_back(Point_2(apexes[i].first,apexes[i].second));
	    //	    cout << apexes[i].first << " " << apexes[i].second << endl;
	  }

	  vector<Point_2> hull(apexes2.size());

	  vector<Point_2>::iterator hull_end = 
	    CGAL::ch_graham_andrew( apexes2.begin(), apexes2.end(), hull.begin());

	  // 	  ofstream pisteet("pisteet.dat", ofstream::trunc);
	  // 	  for(vector<Point_2>::iterator I = apexes2.begin(); I != apexes2.end(); I++) {
	  // 	    pisteet << *I << endl;
	  // 	  }
	  // 	  pisteet.close();

	  // 	  ofstream hu("hull.dat", ofstream::trunc);
	  // 	  for(vector<Point_2>::iterator I = hull.begin(); I != hull_end; I++) {
	  // 	    hu << *I << endl;
	  // 	  }
	  // 	  hu.close();

	  Polygon_2 hull_pgon;
	  for(vector<Point_2>::iterator I = hull.begin(); I != hull_end; I++) {
	    hull_pgon.push_back(*I);
	  }

	  ofstream are("areas.dat", ofstream::app);
	  are << current_label1 << " " << current_label2 << " " << hull_pgon.area() << endl;
	  are.close();
	  
	} else if(CheckCommandLine(argc,argv,"-influenceArea2")) {
	  vector<vector<pair<double, double> > > branches;

	  branches = Accumulate(froot, branches,  CollectApexes2());

	  cout << current_label1+current_label2 << endl;

	  vector<vector<pair<double, double> > >::iterator bI;
	  double area = 0.0;

	  for(bI = branches.begin(); bI != branches.end(); bI++) {
	    vector<Point_2> apexes2;
	    //	  vector<pair<double, double> >::iterator Ip;
	    //	  for(Ip = apexes.begin(), Ip != apexes.end(); Ip++) {
	    for(int i = 0; i < (int)(*bI).size(); i++) {
	      apexes2.push_back(Point_2(((*bI)[i]).first,((*bI)[i]).second));
	      //	    cout << apexes[i].first << " " << apexes[i].second << endl;
	    }

	    vector<Point_2> hull(apexes2.size());

	    vector<Point_2>::iterator hull_end = 
	      CGAL::ch_graham_andrew( apexes2.begin(), apexes2.end(), hull.begin());

	    Polygon_2 hull_pgon;
	    for(vector<Point_2>::iterator I = hull.begin(); I != hull_end; I++) {
	      hull_pgon.push_back(*I);
	    }
	    area += hull_pgon.area();
	  }

	  Axis<FineRootSegment,FineRootBud>& axis = GetAxis(froot); //Add "area" of the last link of root
	  TreeSegment<FineRootSegment,FineRootBud>* ls = GetLastTreeSegment(axis);
	  if(!(ls == NULL)) {
	    double l = GetValue(*ls, LGAL);
	    area  += l*0.57*l;        //this is explained in 
	    ///Users/matrps/Riston-D/E/Hankkeet/LIGNUM/Erillishankkeet/Metlan-hankkeet/
	    //Hienot-juuret/Workbook.txt 
	  }

	  ofstream are("areas.dat", ofstream::app);
	  are << current_label1 << " " << current_label2 << " " << area << endl;
	  are.close();
	}
	else {
	  XMLDomTreeWriter<FineRootSegment,FineRootBud> writer;
	  writer.writeTreeToXML(froot,current_label1+current_label2+".xml");
	}

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


