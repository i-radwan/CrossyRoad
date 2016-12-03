#include <vector>
using namespace std;
class Utilities{
public:
    /**
     @Samir: add your algo here
     * 
     *  ToDo: Algorithm to generate this array with the following conditions:
     *  • lanesData[]: random sequence of 0's and 1's while maintaining the following:
     *               - No more than 2 safe lands next to each other
     *               - No more than 5 normal lanes next to each other
     *               - Start with a safe lans
     */
    static void generateLanesAlgorithm(vector<int>& lanes) {
        int lanesData[] = {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 ,1 ,1,  1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0}; // 0 safe lane, 1 normal lane
        vector<int> tmpLanes (lanesData, lanesData + sizeof(lanesData) / sizeof(int) );
        lanes = tmpLanes;
    }
    static void addMoreLanes(vector<int>& lanes) {
        int lanesData[] = {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 ,1 ,1,  1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0}; // 0 safe lane, 1 normal lane
        for (int i = 1; i < 52; i++) {
            lanes.push_back(lanesData[i]);
        }
    }
    
};
