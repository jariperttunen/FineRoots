#ifndef FUNCTORS_H
#define FUNCTORS_H
//#include <Lignum.h>
//#include <Turtle.h>
#include <MakeFineRoots.h>


using namespace std;

class pioneer_class_info{
 public:
 pioneer_class_info() : no_long_branches(0), no_tips(0), class_freq(vector<int>(5,0)),
    class_area(vector<double>(5,0.0)), in_chunk(vector<int>(5,0)){;}
  pioneer_class_info& operator += (const pioneer_class_info& pi) {
    no_long_branches += pi.no_long_branches;
    no_tips += pi.no_tips;
    for(int i = 0; i < 5; i++) {
      class_freq[i] += pi.class_freq[i];
    }
    for(int i = 0; i < 5; i++) {
      class_area[i] += pi.class_area[i];
    }
    for(int i = 0; i < 5; i++) {
      in_chunk[i] += pi.in_chunk[i];
    }

    return *this;
  }

  int no_long_branches;
  int no_tips;
  vector<int> class_freq;
  vector<double> class_area;
  vector<int> in_chunk;
};


// Sanna-Kajsa Velmala, ohje 11/2017: 
// 0-luokka: 1-order root is shorter than 15 mm <0.015
// 4-luokka: more than 2 branches(2-order) longer than 5 mm >0.005
// 3-luokka: at least one 2-order branch longer than 5 mm >0.005
// Luokat 1 ja 2:  2) more than 2 root tips, 1) 0-2 root tips (root tip 0-5mm pitka linkki)





class FindPioneerClass {
 public:
 FindPioneerClass() : pioneer_limit(0.015), tip_limit(0.005), root("") {;}
 FindPioneerClass(const double pl, const double tl, const string r) : pioneer_limit(pl), tip_limit(tl),
    root(r) {;}
  //pioneer_limit defines if the branch (order == 1) is studied as pionee at all
  //tip_limit separates higher oder branches between long branches and root tips

  pioneer_class_info& operator()
    (pioneer_class_info& info, TreeCompartment<FineRootSegment,FineRootBud>* tc)const {
    if(Axis<FineRootSegment,FineRootBud>* axis = dynamic_cast<Axis<FineRootSegment,FineRootBud>*>(tc)){
      TreeSegment<FineRootSegment,FineRootBud>* ls  = GetLastTreeSegment(*axis);
      if(ls == NULL) {
	return info;
      }
      int order = static_cast<int>(GetValue(*ls, LGAomega));
      if(order < 1) {
	return info;    //main root axis, do nothing
      }

      if(order >= 2) {   //sideside branch or higher
	list<TreeCompartment<FineRootSegment,FineRootBud>*>& tc_list = GetTreeCompartmentList(*axis);
	double length = 0.0;
	list<TreeCompartment<FineRootSegment,FineRootBud>*>::iterator I;
	for(I = tc_list.begin(); I !=  tc_list.end(); I++) {
	  if(TreeSegment<FineRootSegment,FineRootBud>* ts = 
	     dynamic_cast<TreeSegment<FineRootSegment,FineRootBud>*>(*I)) {
	    length += GetValue(*ts, LGAL);
	  }
	}
	if(length > tip_limit) {
	  info.no_long_branches++;
	} else  {
	  info.no_tips++;
	}
	return info;
      } // if(order >= 2) ...

      // order == 1, the analyses are made for these root branches
      // check first if order == 1 total length >= pioneer_limit
      list<TreeCompartment<FineRootSegment,FineRootBud>*>& tc_list = GetTreeCompartmentList(*axis);
      double length = 0.0;
      double chunk_length = 0.0;
      list<TreeCompartment<FineRootSegment,FineRootBud>*>::iterator I;
      for(I = tc_list.begin(); I !=  tc_list.end(); I++) {
	if(TreeSegment<FineRootSegment,FineRootBud>* ts = 
	   dynamic_cast<TreeSegment<FineRootSegment,FineRootBud>*>(*I)) {
	  length += GetValue(*ts, LGAL);
	  chunk_length += (dynamic_cast<FineRootSegment*>(ts))->getInChunk();
	}
      }
      
      if(length < pioneer_limit) {
	info.class_freq[0] += 1;
	if(chunk_length > 0.0) {
	  info.in_chunk[0] += 1;
	}
	return info;
      }
      
      if(info.no_long_branches > 2) {
	info.class_freq[4] += 1;
	if(chunk_length > 0.0) {
	  info.in_chunk[4] += 1;
	}

	info.class_area[4] += 1.0e4 * calculate_area(*axis);  //m2 --> cm2
	return info;
      }

      if(info.no_long_branches == 1 || info.no_long_branches == 2 ) {
	info.class_freq[3] += 1;
	if(chunk_length > 0.0) {
	  info.in_chunk[3] += 1;
	}

	info.class_area[3] += 1.0e4 * calculate_area(*axis);  //m2 --> cm2
	return info;
      }

      if(info.no_long_branches == 0) {
	if(info.no_tips > 2) {
	  info.class_freq[2] += 1;
	  if(chunk_length > 0.0) {
	    info.in_chunk[2] += 1;
	  }
	  info.class_area[2] += 1.0e4 * calculate_area(*axis);  //m2 --> cm2
	}
	else {
	  info.class_freq[1] += 1;
	  if(chunk_length > 0.0) {
	    info.in_chunk[1] += 1;
	  } 

	  info.class_area[1] += 1.0e4 * calculate_area(*axis);  //m2 --> cm2
	}

	return info;
      }

    }  //if(Axis ...)
  }  //operator() ....
 private:
  double pioneer_limit;
  double tip_limit;
  string root;
};


#endif
