#include <iostream>
#include <stdexcept>
#include "Query.h"
#include "TextQuery.h"
#include "QueryResult.h"
using namespace std;

int main(int argc, char **argv)
{
  ifstream infile(argv[1]);
  if (!infile) { cerr << "No input file!" << endl; exit(1); }
  TextQuery tq(infile);
  tq.display_map();
  while (true)
  {
    cout << "Enter one or two words to look for, or q to quit:" << endl;
   // cout<<"1"<<endl;
    string line;
   // cout<<"2"<<endl;
    getline(cin, line);
   // cout<<"3"<<endl;
    if (line.empty() || line == "q") break;
  //  cout<<"4"<<endl;
	shared_ptr<QueryBase> q;
 // cout<<"5"<<endl;
	try {
      q = QueryBase::factory(line);
 //     cout<<"6"<<endl;
	}
	catch( const invalid_argument& e ) {
      cout << e.what() << endl;
  //    cout<<"7"<<endl;
	  continue;
	}
    print(cout, q->eval(tq)) << endl;
  //  cout<<"8"<<endl;
  }
  exit(0);
}
