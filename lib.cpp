#include <iostream>
#include <map>
#include <chrono>

int factarial(int x){
    int res = 1;
    for(int i = 1; i <= x; i++){
        res *= i;
    }
    return res;
}