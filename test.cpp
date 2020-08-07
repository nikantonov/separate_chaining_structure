//
//  main.cpp
//  ADS_Projekt_2
//
//  Created by Никита on 14.05.18.
//  Copyright (c) 2018 Никита. All rights reserved.
//

#include <iostream>
#include "ADS_set.h"
#include<vector>

using namespace std;

int main() {
    vector<int> v{7, 1, 5, 0};
    vector<int> r;
    ADS_set<int> a{v.begin(), v.end()};
    //a.erase(7);
    a.erase(8);
    //cout << a.y()<<endl;
    cout << a.a()<<endl;
    a.dump();
    
    
    
    
    //cout << b.x()<<endl;
    //cout << *i << endl;
    
    //cout <<a.y()<< endl;
    
    return 0;
}
