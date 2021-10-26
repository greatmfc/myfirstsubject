#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <cctype>
#include "head.h"
using namespace std;
vector<int> via;
list<int> lia;
int ia[]={0,1,1,2,3,5,8,13,21,55,89};
int main()
{
	for (int i=0;i!=11;++i){
		via.push_back(ia[i]);
		lia.push_back(ia[i]);
	}
	for(auto &a:lia)
		cout<<a<<" ";
	cout<<endl;
	/*auto rvia=copy(begin(ia),end(ia),via);
	auto rlia=copy(begin(ia),end(ia),lia);*/
	auto bvia=via.begin();
	auto blia=lia.begin();
	while (bvia!=via.end()){
		if (*bvia%2)
			++bvia;
		else
			bvia=via.erase(bvia);
	}
	while (blia!=lia.end()){
		if (*blia%2)
			blia=lia.erase(blia);
		else
			++blia;
	}
	for(auto &c:via)
		cout<<c<<" ";
	cout<<endl;
	for(auto &b:lia)
		cout<<b<<" ";
	cout<<endl;
	return 0;
}
