#include <iostream>
#include "apriori.h"

using namespace std;

int main (int argc, char ** argv) {
    if(argc!=4) {
        cout << "error : The number of parameters must be 3";
        return 0;
    }
    string minSupport(argv[1]);
    string inputFileName(argv[2]);
    string outputFileName(argv[3]);
    
    /*
    vector<vector<int> > transactions = {
        {1, 2, 5},
        {2,4},
        {2,3},
        {1, 2, 4},
        {1, 3},
        {2, 3},
        {1, 3},
        {1, 2, 3, 5},
        {1, 2, 3}
    };
    */
    
    InputReader inputReader(inputFileName);
    vector<vector<int> > transactions = inputReader.getTransactions();
    Apriori apriori(transactions, stold(minSupport));
    apriori.process();
    OutputPrinter outputPrinter(outputFileName, apriori.getAssociationRules());
    
    /*
    for test
    Checker checker("output5.txt", "outputRsupport5.txt");
    checker.compare();
    */

    
    return 0;
}
