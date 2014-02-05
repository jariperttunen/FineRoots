#ifndef MAKEFINEROOTS_H
#define MAKEFINEROOTS_H
#include <Lignum.h>
#include <Turtle.h>

using namespace std;

class FineRootBud;

class FineRootSegment : public CfTreeSegment<FineRootSegment,FineRootBud>
{
 public:
  FineRootSegment(Tree<FineRootSegment,FineRootBud>* t) 
    :CfTreeSegment<FineRootSegment,FineRootBud>(t){}

  int getNumber() {return number;}
  void setNumber(const int& nu) {number = nu;}

 private:
  int number;      
 };

class FineRootBud:public Bud<FineRootSegment,FineRootBud>{
public:
  FineRootBud(Tree<FineRootSegment,FineRootBud>* t):Bud<FineRootSegment,FineRootBud>(t){}
};

class SetLink {
 public:
 SetLink(const int f_num, const int m_num, const int m_o) : father_num(f_num), 
    my_num(m_num), my_order(m_o) {}

  TreeCompartment<FineRootSegment,FineRootBud>* operator()
    (TreeCompartment<FineRootSegment,FineRootBud>* tc)const {
    if(Axis<FineRootSegment,FineRootBud>* ax = 
       dynamic_cast<Axis<FineRootSegment,FineRootBud>*>(tc)){
      list<TreeCompartment<FineRootSegment,FineRootBud>*>& c_list =
	GetTreeCompartmentList(*ax);
      list<TreeCompartment<FineRootSegment,FineRootBud>*>::iterator Ic;
      bool found = false;
      FineRootSegment* frs;
      for(Ic = c_list.begin(); Ic != c_list.end(); Ic++) {
	if((frs =  dynamic_cast<FineRootSegment*>(*Ic)))
	   if(frs->getNumber() == father_num) {
	    found = true;
	    break;             //father found
	  }
      }
      if(!found)
	return tc;

      Tree<FineRootSegment,FineRootBud>& t = GetTree(*frs);
      
      if(static_cast<int>(GetValue(*frs,LGAomega) == my_order)) {
	FineRootSegment* seg = new FineRootSegment(&t);  //into the same axis
	seg->setNumber(my_num);
	SetValue(*seg,LGAomega,static_cast<double>(my_order));
	BranchingPoint<FineRootSegment,FineRootBud> *bp =
	  new BranchingPoint<FineRootSegment,FineRootBud>(&t);
	Ic++;  Ic++;    //points now past next BranchingPoint
	c_list.insert(Ic, seg);
	c_list.insert(Ic, bp);
      }
      else {    //forking from the next BranchingPoint
	FineRootBud *new_bud = new FineRootBud(&t);
	FineRootSegment* new_seg = new FineRootSegment(&t);
	new_seg->setNumber(my_num);
	SetValue(*new_seg,LGAomega,static_cast<double>(my_order));
	BranchingPoint<FineRootSegment,FineRootBud> *new_bp =
	  new BranchingPoint<FineRootSegment,FineRootBud>(&t);
	Axis<FineRootSegment,FineRootBud>* new_ax =
	  new Axis<FineRootSegment,FineRootBud>(&t);
	InsertTreeCompartment(*new_ax, new_bud);
	InsertTreeCompartmentSecondLast(*new_ax,new_seg);
	InsertTreeCompartmentSecondLast(*new_ax,new_bp);
	Ic++;        //points now to next BranchingPoint
	BranchingPoint<FineRootSegment,FineRootBud> *bp =
	  dynamic_cast<BranchingPoint<FineRootSegment,FineRootBud>*>(*Ic);
	InsertAxis(*bp, new_ax);
      }
    }   
 
    return tc;
  }
 private:
  int father_num;
  int my_num;
  int my_order;
};

void add_link(Tree<FineRootSegment,FineRootBud>& fr_tree, const int num,
	      const int father_num, const int ord)  {
  if(num == 0)  { //first segment, tree is empty
      Axis<FineRootSegment,FineRootBud>& main_ax = GetAxis(fr_tree);
      FineRootBud *bud0 = new  FineRootBud(&fr_tree);
      FineRootSegment* seg0 = new FineRootSegment(&fr_tree);
      BranchingPoint<FineRootSegment,FineRootBud> *bp0 = 
	new BranchingPoint<FineRootSegment,FineRootBud>(&fr_tree);
      InsertTreeCompartment(main_ax,bud0);
      InsertTreeCompartmentSecondLast(main_ax,seg0);
      InsertTreeCompartmentSecondLast(main_ax,bp0);
      seg0->setNumber(num);
      SetValue(*seg0,LGAomega,static_cast<double>(ord));
      //      cout << "nolla " << num << endl;
  }
  else {
    SetLink sl(father_num, num, ord);
    ForEach(fr_tree, sl);
  }

  return;
}

class PrintN {
 public:
  TreeCompartment<FineRootSegment,FineRootBud>* operator()
    (TreeCompartment<FineRootSegment,FineRootBud>* tc)const {
    if(FineRootSegment* frs = dynamic_cast<FineRootSegment*>(tc)){
      cout << frs->getNumber() << endl;
    }
    return tc;
  }
};

struct SAInfo {
  Point point;
  PositionVector direction;
};

class SetArchitecture {
 public:
 SetArchitecture(vector<vector<double> >& dat) : data(dat) {} 
  SAInfo& operator()
    (SAInfo& from_base, TreeCompartment<FineRootSegment,FineRootBud>* tc)const {
    if(FineRootSegment* frs = dynamic_cast<FineRootSegment*>(tc)){
      int number = frs->getNumber();
      double len = (data[number])[0];
      double diam = (data[number])[1];
      double angle = (data[number])[2];
      extern int ran3_seed;
      if(ran3(&ran3_seed) < 0.5)
	angle = -angle;

      SetValue(*frs,LGAR, diam/2.0);
      SetValue(*frs,LGAL, len);

      SetPoint(*frs, from_base.point);

      if(number == 0) {   //base segment
	SetDirection(*frs, from_base.direction);
      }
      else {
	Turtle t;
	SetHeading(t, from_base.direction);
	SetUp(t, PositionVector(0.0,1.0,0.0));
	SetLeft(t,Cross(PositionVector(0.0,1.0,0.0),from_base.direction)); //L = U x H
       
	t.turn(angle*PI_VALUE/180.0);
	SetDirection(*frs, GetHeading(t));
      }

      from_base.direction = GetDirection(*frs);
      from_base.point = Point(PositionVector(GetPoint(*frs))+
			      len*GetDirection(*frs));
    }
    else if(FineRootBud *bu = dynamic_cast<FineRootBud*>(tc)){
      SetPoint(*bu, from_base.point);
      SetDirection(*bu, from_base.direction);
    }
    else if(BranchingPoint<FineRootSegment,FineRootBud> *bp = 
	    dynamic_cast<BranchingPoint<FineRootSegment,FineRootBud>*>(tc)){
      SetPoint(*bp, from_base.point);
      SetDirection(*bp, from_base.direction);
    }
    else {}

    return from_base;
  }
 private:
  vector<vector<double> > data;

};

 
#endif

