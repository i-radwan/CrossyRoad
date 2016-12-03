#include <vector>
using namespace std;


struct lane{
    float startPos;
    int type;
    bool drawnBefore = 0;
    lane(){
        drawnBefore = 0;
    }
};


void generateLaneData(int laneData[]){
    int count[2] = {0,1};
    int randomNumber;
    int maxAllowed[2] = {2,5};
    laneData[0] = 1;
    count[1]++;
    int previousLane = 1;
    int i = 1;
    for(; i < 48; i++){
        randomNumber = rand();
        int randomLane = randomNumber%2;
        if(randomLane == previousLane && count[randomLane] < maxAllowed[randomLane]){
            count[randomLane]++;
            laneData[i] = randomLane;
            count[1-randomLane] = 0;
        }
        else if( randomLane == previousLane && count[randomLane] > maxAllowed[randomLane]){
            laneData[i] = 1 - randomLane;//Swap
            count[randomLane] = 0;
            count[1-randomLane] = 1;
            previousLane = 1- randomLane;
        }
        else if( randomLane == previousLane && count[randomLane] == maxAllowed[randomLane]){
            count[randomLane] = 0;
            laneData[i] =1 -randomLane;
            previousLane = randomLane;
            count[1-randomLane] = 1;
        }
        else{
            count[randomLane] = 1;
            laneData[i] = randomLane;
            count[1-randomLane] = 0;
            previousLane = randomLane;
        }
    }
}

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
    
    static void generateLanesAlgorithm(vector<lane>& lanesArray) {
        int lanesData[] = {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 ,1 ,1,  1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0}; // 0 safe lane, 1 normal lane
        for(int i = 0 ;i < 52; i++){
            lane s;
            s.type = lanesData[i];
            s.startPos = 0;
            s.drawnBefore = 0;
            lanesArray.push_back(s);
        }
    }
    static void addMoreLanes(vector<lane>& lanesArray,float &newstart, bool& firstLanesSet, float* carsXPosition) {
        int lanesData[50];
        //autogeneration
        generateLaneData(lanesData);
        vector<lane> newLanesArray;
        for(int j = 34, i = 0 ; j < 52; j++, i++){
            newLanesArray.push_back(lanesArray[j]);
            carsXPosition[i] = carsXPosition[j];
        }
        for(int i = 0, j = (52-34); i < 34; i++, j++){
            lane s;
            s.startPos = 0;
            s.type = lanesData[i];
            s.drawnBefore = 0;
            newLanesArray.push_back(s);
            carsXPosition[j] = 12;
        }
        lanesArray.clear();
        lanesArray = newLanesArray;
        newstart = lanesArray[0].startPos+1.5;
        firstLanesSet= false;
    }
    
};
